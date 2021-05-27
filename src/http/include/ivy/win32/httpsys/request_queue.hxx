/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_HTTPSYS_REQUEST_QUEUE_HXX_INCLUDED
#define IVY_WIN32_HTTPSYS_REQUEST_QUEUE_HXX_INCLUDED

#include <optional>

#include <ivy/noncopyable.hxx>
#include <ivy/win32/windows.hxx>
#include <ivy/win32/heap.hxx>
#include <ivy/string.hxx>

#include <http.h>

#include <wil/resource.h>

namespace ivy::win32::httpsys {

    /*************************************************************************
     *
     * http_request_queue.
     *
     * Represents a request queue in the HTTP API.  Creating a request_queue
     * object calls HttpCreateRequestQueue().  Closing a request_queue calls
     * HttpCloseRequestQueue().  
     *
     * http_request_queue's constructor can throw http_error if
     * HttpCreateRequestQueue fails.
     */

    // Unique handle for a request queue.

    using unique_http_request =
        std::unique_ptr<HTTP_REQUEST, process_heap_deleter<HTTP_REQUEST>>;

    class request_queue : public ivy::noncopyable {
        using handle_type =
            wil::unique_any<HANDLE,
                            decltype(::HttpCloseRequestQueue),
                            ::HttpCloseRequestQueue>;

        handle_type _handle{};

    public:
        request_queue(std::optional<wstring> name,
                           SECURITY_ATTRIBUTES *security_attributes,
                           ULONG flags);
        ~request_queue();

        request_queue(request_queue &&) noexcept;
        auto operator=(request_queue &&) noexcept -> request_queue &;

        [[nodiscard]] auto read_request() -> expected<unique_http_request, error>;
        [[nodiscard]] auto get_handle() -> HANDLE;
    };

} // namespace ivy::win32

#endif // IVY_WIN32_HTTPSYS_REQUEST_QUEUE_HXX_INCLUDED
