/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <format>
#include <memory>

#include <ivy/http/service.hxx>
#include <ivy/string.hxx>
#include <ivy/win32/error.hxx>
#include <ivy/win32/httpsys.hxx>
#include <ivy/win32/windows.hxx>

#include <http.h>
#include <wil/resource.h>

namespace ivy::win32 {

    /*************************************************************************
     *
     * http_handle: RAII guard for HttpInitialise().
     *
     */

    class http_handle {
        ULONG _flags{};

    private:
        http_handle(ULONG flags) : _flags(flags) {}

    public:
        http_handle() = default;

        http_handle(http_handle &&other) noexcept
            : _flags(std::exchange(other._flags, 0))
        {
        }

        auto operator=(http_handle &&other) noexcept -> http_handle &
        {
            if (&other != this) {
                if (_flags != 0)
                    ::HttpTerminate(_flags, nullptr);
                _flags = std::exchange(other._flags, 0);
            }

            return *this;
        }

        http_handle(http_handle const &) = delete;
        auto operator=(http_handle const &) -> http_handle & = delete;

        ~http_handle()
        {
            if (_flags)
                ::HttpTerminate(_flags, nullptr);
        }

        static auto open(HTTPAPI_VERSION version, ULONG flags)
            -> expected<http_handle, error>
        {
            if (flags == 0)
                return make_unexpected(http::http_error(
                    "invalid flags passed to http_handle::open"));

            auto ret = ::HttpInitialize(version, flags, nullptr);

            if (ret != NO_ERROR) {
                auto err = make_win32_error(ret);
                return make_unexpected(http::http_error(
                    std::format("HttpInitialize() failed: {}", err.message())));
            }

            return http_handle(flags);
        }
    };

    /*************************************************************************
     *
     * http_request_queue
     *
     */

    using unique_http_request_queue =
        wil::unique_any<HANDLE,
                        decltype(::HttpCloseRequestQueue),
                        ::HttpCloseRequestQueue>;

    class httpsys_request_queue {
        unique_http_request_queue _queue;

        httpsys_request_queue() noexcept = default;

    public:
        httpsys_request_queue(httpsys_request_queue &&) noexcept = default;
        auto operator=(httpsys_request_queue &&) noexcept
            -> httpsys_request_queue & = default;

        httpsys_request_queue(httpsys_request_queue const &) = delete;
        auto operator=(httpsys_request_queue const &) noexcept
            -> httpsys_request_queue & = delete;

        static auto create(std::optional<ivy::wstring> name,
                           SECURITY_ATTRIBUTES *security_attributes,
                           ULONG flags) noexcept
            -> expected<httpsys_request_queue, error>;
    };

    auto httpsys_request_queue::create(std::optional<ivy::wstring> name,
                                       SECURITY_ATTRIBUTES *security_attributes,
                                       ULONG flags) noexcept
        -> expected<httpsys_request_queue, error>
    {
        httpsys_request_queue ret;

        // Create the request queue.
        auto r = ::HttpCreateRequestQueue(HTTPAPI_VERSION_2,
                                          name ? name->c_str() : nullptr,
                                          security_attributes,
                                          flags,
                                          &ret._queue);
        if (r != NO_ERROR) {
            auto err = make_win32_error(r);
            return make_unexpected(http::http_error(
                std::format("HttpInitialize() failed: {}", err.message())));
        }

        return ret;
    }

    /*************************************************************************
     *
     * http_request_queue
     *
     */

    class httpsys_service : public http::service {
        http_handle _http_handle;
        httpsys_request_queue _request_queue;

    public:
        httpsys_service(http_handle &&handle,
                        httpsys_request_queue &&rq) noexcept
            : _http_handle(std::move(handle)), _request_queue(std::move(rq))
        {
        }

        httpsys_service(httpsys_service &&) noexcept = delete;
        auto operator=(httpsys_service &&) -> httpsys_service & = delete;

        httpsys_service(httpsys_service const &) = delete;
        auto operator=(httpsys_service const &) -> httpsys_service & = delete;
    };

    auto make_httpsys_service()
        -> expected<std::unique_ptr<http::service>, error>
    {
        // Open HTTP.sys handle.
        auto httphandle =
            http_handle::open(HTTPAPI_VERSION_2, HTTP_INITIALIZE_SERVER);

        if (!httphandle)
            return make_unexpected(httphandle.error());

        // Create the request queue.
        SECURITY_ATTRIBUTES attrs{};
        attrs.nLength = sizeof(attrs);
        auto rq = httpsys_request_queue::create(std::nullopt, &attrs, 0);
        if (!rq)
            return make_unexpected(rq.error());

        // Create the httpsys_service.
        auto p = new (std::nothrow) httpsys_service(std::move(*httphandle), std::move(*rq));
        if (!p)
            return make_unexpected(
                http::http_error("make_httpsys_service: out of memory"));

        return std::unique_ptr<http::service>(p);
    }

} // namespace ivy::win32
