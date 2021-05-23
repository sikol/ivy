/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_HTTPSYS_REQUEST_QUEUE_HXX_INCLUDED
#define IVY_WIN32_HTTPSYS_REQUEST_QUEUE_HXX_INCLUDED

#include <ivy/noncopyable.hxx>
#include <ivy/win32/windows.hxx>

#include <http.h>

#include <wil/resource.h>

namespace ivy::win32 {

    /*************************************************************************
     *
     * http_request_queue.
     *
     * Represents a request queue in the HTTP API.  Creating a request_queue
     * object calls HttpCreateRequestQueue().  Closing a request_queue calls
     * HttpCloseRequestQueue().  Dereferencing the request queue returns the
     * wil::unique_any representing the request queue's handle.
     *
     * http_request_queue's constructor can throw http_error if
     * HttpCreateRequestQueue fails.
     */

    // Unique handle for a request queue.
    using unique_http_request_queue =
        wil::unique_any<HANDLE,
                        decltype(::HttpCloseRequestQueue),
                        ::HttpCloseRequestQueue>;

    namespace detail {

        template <typename T>
        struct process_heap_free {
            auto operator()(T *p) const noexcept -> void
            {
                ::HeapFree(::GetProcessHeap(), 0, p);
            }
        };

    } // namespace detail

    using http_request_pointer =
        std::unique_ptr<HTTP_REQUEST, detail::process_heap_free<HTTP_REQUEST>>;

    class http_request_queue : public ivy::noncopyable {
        unique_http_request_queue _handle{};

    public:
        http_request_queue(std::optional<wstring> name,
                           SECURITY_ATTRIBUTES *security_attributes,
                           ULONG flags);
        ~http_request_queue();

        http_request_queue(http_request_queue &&) noexcept;
        auto operator=(http_request_queue &&) noexcept -> http_request_queue &;

        auto read_request() -> expected<http_request_pointer, error>;
        auto get_handle() -> HANDLE;
    };

} // namespace ivy::win32

#endif // IVY_WIN32_HTTPSYS_REQUEST_QUEUE_HXX_INCLUDED
