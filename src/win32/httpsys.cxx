/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <format>
#include <memory>

#include <ivy/http/service.hxx>
#include <ivy/noncopyable.hxx>
#include <ivy/string.hxx>
#include <ivy/win32/error.hxx>
#include <ivy/win32/httpsys.hxx>
#include <ivy/win32/httpsys/handle.hxx>
#include <ivy/win32/httpsys/request_queue.hxx>
#include <ivy/win32/httpsys/server_session.hxx>
#include <ivy/win32/httpsys/url_group.hxx>
#include <ivy/win32/windows.hxx>

#include <http.h>
#include <wil/resource.h>

namespace ivy::win32 {

    /*************************************************************************
     *
     * http_handle: RAII guard for HttpInitialise().
     *
     */

    http_handle::http_handle() = default;

    http_handle::http_handle(http_handle &&other) noexcept
        : _flags(std::exchange(other._flags, 0))
    {
    }

    auto http_handle::operator=(http_handle &&other) noexcept -> http_handle &
    {
        if (&other != this) {
            if (_flags != 0)
                ::HttpTerminate(_flags, nullptr);

            _flags = std::exchange(other._flags, 0);
        }

        return *this;
    }

    http_handle::~http_handle()
    {
        if (_flags)
            ::HttpTerminate(_flags, nullptr);
    }

    http_handle::http_handle(ULONG flags)
        : _flags(flags)
    {
        auto ret = ::HttpInitialize(HTTPAPI_VERSION_2, flags, nullptr);

        if (ret != NO_ERROR) {
            auto err = make_win32_error(ret);
            throw http::http_error(
                std::format("HttpInitialize() failed: {}", err.message()));
        }
    }

    /*************************************************************************
     *
     * http_request_queue
     *
     */

    http_request_queue::http_request_queue(
        std::optional<wstring> name,
        SECURITY_ATTRIBUTES *security_attributes,
        ULONG flags)
    {
        // Create the request queue.
        auto r = ::HttpCreateRequestQueue(HTTPAPI_VERSION_2,
                                          name ? name->c_str() : nullptr,
                                          security_attributes,
                                          flags,
                                          &_queue);

        if (r != NO_ERROR) {
            auto err = make_win32_error(r);
            throw http::http_error(std::format(
                "HttpCreateRequestQueue() failed: {}", err.message()));
        }
    }

    http_request_queue::http_request_queue(
        http_request_queue &&other) noexcept = default;

    auto http_request_queue::operator=(http_request_queue &&other) noexcept
        -> http_request_queue & = default;

    http_request_queue::~http_request_queue() = default;

    /*************************************************************************
     *
     * http_server_session
     *
     */

    http_server_session::http_server_session()
    {
        auto r = ::HttpCreateServerSession(HTTPAPI_VERSION_2, &_session, 0);

        if (r != NO_ERROR) {
            auto err = make_win32_error(r);
            throw http::http_error(std::format(
                "HttpCreateServerSession() failed: {}", err.message()));
        }
    }

    http_server_session::http_server_session(http_server_session &&) noexcept =
        default;
    http_server_session::~http_server_session() = default;

    auto http_server_session::operator=(http_server_session &&) noexcept
        -> http_server_session & = default;

    auto http_server_session::operator*() noexcept
        -> unique_http_server_session &
    {
        return _session;
    }

    auto http_server_session::operator->() noexcept
        -> unique_http_server_session *
    {
        return std::addressof(_session);
    }

    /*************************************************************************
     *
     * http_url_group
     *
     */

    http_url_group::http_url_group(http_server_session &session)
    {
        auto r = ::HttpCreateUrlGroup(session->get(), &_handle, 0);

        if (r != NO_ERROR) {
            auto err = make_win32_error(r);
            throw http::http_error(
                std::format("HttpCreateUrlGroup() failed: {}", err.message()));
        }
    }

    http_url_group::http_url_group(http_url_group &&) noexcept = default;
    http_url_group::~http_url_group() = default;

    auto http_url_group::operator=(http_url_group &&) noexcept
        -> http_url_group & = default;

    auto http_url_group::operator*() noexcept -> unique_http_url_group &
    {
        return _handle;
    }

    auto http_url_group::operator->() noexcept -> unique_http_url_group *
    {
        return std::addressof(_handle);
    }

    auto http_url_group::add_url(wstring const &url, HTTP_URL_CONTEXT context)
        -> expected<void, std::error_code>
    {
        auto ret =
            ::HttpAddUrlToUrlGroup(_handle.get(), url.c_str(), context, 0);

        if (ret != NO_ERROR)
            return make_unexpected(make_win32_error(ret));

        return {};
    }

    /*************************************************************************
     *
     * httpsys_service
     *
     */

    class httpsys_service final : public http::service {
        http_handle _http_handle;
        http_request_queue _request_queue;
        http_server_session _server_session;
        http_url_group _url_group;
        std::vector<std::unique_ptr<http::http_listener>> _listeners;

    public:
        httpsys_service(std::optional<wstring> name,
                        SECURITY_ATTRIBUTES *security_attributes,
                        ULONG flags)
            : _http_handle(HTTP_INITIALIZE_SERVER)
            , _request_queue(name, security_attributes, flags)
            , _url_group(_server_session)
        {
        }

        httpsys_service(httpsys_service &&) noexcept = delete;
        auto operator=(httpsys_service &&) -> httpsys_service & = delete;

        httpsys_service(httpsys_service const &) = delete;
        auto operator=(httpsys_service const &) -> httpsys_service & = delete;

        auto add_listener(http::http_listener const &)
            -> expected<void, error> override;
    };

    auto make_httpsys_service()
        -> expected<std::unique_ptr<http::service>, error>
    {
        SECURITY_ATTRIBUTES attrs{};
        attrs.nLength = sizeof(attrs);

        try {
            return std::make_unique<httpsys_service>(std::nullopt, &attrs, 0);
        } catch (std::exception const &e) {
            return make_unexpected(e);
        }
    }

    auto httpsys_service::add_listener(http::http_listener const &lsn)
        -> expected<void, error>
    {
        auto lsnp = std::unique_ptr<http::http_listener>(
            new (std::nothrow) http::http_listener(lsn));

        if (!lsnp)
            return make_unexpected(
                std::system_error(std::make_error_code(std::errc::not_enough_memory)));

        return {};
    }

} // namespace ivy::win32
