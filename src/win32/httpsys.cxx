/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <format>
#include <memory>

#include <ivy/http/service.hxx>
#include <ivy/log.hxx>
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
                                          &_handle);

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

    auto http_request_queue::get_handle() -> HANDLE
    {
        return _handle.get();
    }

    auto http_request_queue::read_request()
        -> expected<http_request_pointer, error>
    {
        // Find the size of the request.
        HTTP_REQUEST hr{};
        ULONG request_size{};

        auto r = ::HttpReceiveHttpRequest(
            _handle.get(), 0, 0, &hr, sizeof(hr), &request_size, nullptr);

        if (r != ERROR_MORE_DATA)
            return make_unexpected(http::http_error(
                std::format("HttpReceiveHttpRequest() failed: {}",
                            make_win32_error(r).message())));

        auto p = ::HeapAlloc(::GetProcessHeap(), 0, request_size);
        if (p == nullptr)
            return make_unexpected(http::http_error(
                "cannot allocate request structure: out of memory"));

        auto reqp = http_request_pointer(static_cast<HTTP_REQUEST *>(p));

        r = ::HttpReceiveHttpRequest(_handle.get(),
                                     hr.RequestId,
                                     0,
                                     reqp.get(),
                                     request_size,
                                     &request_size,
                                     nullptr);
        if (r != NO_ERROR)
            return make_unexpected(http::http_error(
                std::format("HttpReceiveHttpRequest() failed: {}",
                            make_win32_error(r).message())));

        return reqp;
    }

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
            IVY_TRACE("http_url_group: creation failed {}", r);

            auto err = make_win32_error(r);
            throw http::http_error(
                std::format("HttpCreateUrlGroup() failed: {}", err.message()));
        }

        IVY_TRACE("http_url_group: created {}", _handle.get());
    }

    http_url_group::http_url_group(http_url_group &&) noexcept = default;

    http_url_group::~http_url_group()
    {
        IVY_TRACE("http_url_group: destructing {}", _handle.get());
    }

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

    auto http_url_group::set_request_queue(http_request_queue &queue)
        -> expected<void, std::error_code>
    {
        HTTP_BINDING_INFO binding{};
        binding.Flags.Present = 1;
        binding.RequestQueueHandle = queue.get_handle();

        auto r = ::HttpSetUrlGroupProperty(_handle.get(),
                                           HttpServerBindingProperty,
                                           &binding,
                                           sizeof(binding));

        if (r != NO_ERROR)
            return make_unexpected(make_win32_error(r));

        return {};
    }

    /*************************************************************************
     *
     * httpsys_request_context
     *
     */

    class httpsys_request_context : public http::request_context {
    };

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

        auto run() -> expected<void, error> override;
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
            return make_unexpected(std::system_error(
                std::make_error_code(std::errc::not_enough_memory)));

        auto wstr = transcode<wstring>(str(lsn.prefix));
        if (!wstr)
            return make_unexpected(
                http::http_error("invalid listener URI: cannot transcode"));

        auto r = _url_group.add_url(
            *wstr, reinterpret_cast<HTTP_URL_CONTEXT>(lsnp.get()));
        if (!r)
            return make_unexpected(std::system_error(r.error()));

        _listeners.push_back(std::move(lsnp));

        return {};
    }

    auto make_http_request(HTTP_REQUEST const *req)
        -> expected<http::http_request, error>
    {
        auto raw_url_bytes = std::span<std::byte const>(
            reinterpret_cast<std::byte const *>(req->CookedUrl.pFullUrl),
            req->CookedUrl.FullUrlLength);

        auto uri16str = bytes_to_string<u16string>(raw_url_bytes);
        if (!uri16str)
            return make_unexpected(
                http::http_error("Invalid request URI: cannot decode bytes"));

        auto uristr = transcode<u8string>(*uri16str);
        if (!uristr)
            return make_unexpected(
                http::http_error("Invalid request URI: cannot transcode"));

        log_trace("HTTP: Parse request URI [{}]",         
            std::string_view(reinterpret_cast<char const *>(uristr->data()),
                           uristr->size()));

        auto uri = net::parse_uri(*uristr);
        if (!uri)
            return make_unexpected(
                http::http_error("Invalid request URI: cannot parse"));

        auto version = http::http_version(req->Version.MajorVersion,
                                          req->Version.MinorVersion);

        return http::http_request{
            std::move(*uri), {}, {}, version, {}, {}};
    }

    auto httpsys_service::run() -> expected<void, error>
    {
        if (auto r = _url_group.set_request_queue(_request_queue); !r) {
            return make_unexpected(http::http_error(
                std::format("cannot add URL group to request queue: {}",
                            r.error().message())));
        }

        for (;;) {
            auto req = _request_queue.read_request();

            if (!req)
                return make_unexpected(req.error());

            http::http_listener *lsnr =
                reinterpret_cast<http::http_listener *>((*req)->UrlContext);

            auto reqs = make_http_request(req->get());
            if (!reqs) {
                log_warning("HTTP: Failed to receive request: {}",
                            reqs.error().what());
                continue;
            }

            reqs->listener_address = lsnr->prefix;
            httpsys_request_context ctx;
            lsnr->handler(ctx, *reqs);
        }
    }

} // namespace ivy::win32
