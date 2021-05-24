/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_HTTPSYS_RESPONSE_WRITER_HXX_INCLUDED
#define IVY_WIN32_HTTPSYS_RESPONSE_WRITER_HXX_INCLUDED

#include <system_error>
#include <span>

#include <ivy/win32/windows.hxx>

#include <http.h>

#include <ivy/noncopyable.hxx>
#include <ivy/expected.hxx>

namespace ivy::win32::httpsys {

    /*************************************************************************
     *
     * response_writer: write the HTTP response header and body to the client.
     * Calling start_response() sends the header.  If has_body is true, then
     * write_data() can be called zero or more times to send the response body.
     * Once the response is finished, finish_response() should be called.
     *
     * If response_writer is destructed after start_response() is called, but
     * before finish_response() is called, it will attempt to abort the
     * response in a way that indicates the error to the client (for example,
     * by finishing a chunked response without writing the final chunk).
     * This makes response_writer exception-safe while avoiding clients 
     * receiving a partial response or no response at all if the response is 
     * not finished.
     */

    class request_queue;

    struct response_configuration {
        bool has_body = false;
        bool chunked = false;
    };

    class response_writer : ivy::noncopyable {
        enum state_t {
            s_initial,
            s_write_body,
            s_done
        } _state = s_initial;

        response_configuration _config;
        request_queue *_request_queue;
        HTTP_REQUEST_ID _request_id;

    public:
        response_writer(request_queue &, HTTP_REQUEST_ID) noexcept;
        ~response_writer();

        response_writer(response_writer &&) noexcept;
        auto operator=(response_writer &&) -> response_writer & = delete;

        auto start_response(HTTP_RESPONSE *, response_configuration) noexcept
            -> expected<void, std::error_code>;

        auto write_data(std::span<std::byte const>) noexcept
            -> expected<void, std::error_code>;

        auto finish_response() noexcept -> expected<void, std::error_code>;
    };

} // namespace ivy::win32::httpsys

#endif // IVY_WIN32_HTTPSYS_RESPONSE_WRITER_HXX_INCLUDED
