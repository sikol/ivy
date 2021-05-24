/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_HTTPSYS_REQUEST_HANDLER_HXX_INCLUDED
#define IVY_WIN32_HTTPSYS_REQUEST_HANDLER_HXX_INCLUDED

#include <cstddef>
#include <span>
#include <system_error>

#include <ivy/win32/windows.hxx>

#include <http.h>

#include <ivy/expected.hxx>
#include <ivy/http/request.hxx>
#include <ivy/http/response.hxx>
#include <ivy/win32/httpsys/request_queue.hxx>
#include <ivy/win32/httpsys/response_writer.hxx>

namespace ivy::win32::httpsys {

    class service;

    class request_controller final {
    private:
        service *_service;
        unique_http_request _httpsys_request;
        response_writer _writer;

        [[nodiscard]] auto send_internal_error() noexcept
            -> expected<void, std::error_code>;

        [[nodiscard]] auto send_response(http::http_request &req,
                                         http::http_response &resp) noexcept
            -> expected<void, error>;

    public:
        request_controller(service *service,
                           unique_http_request &&httpsys_request);

        auto run() noexcept -> void;
    };

} // namespace ivy::win32::httpsys

#endif // IVY_WIN32_HTTPSYS_REQUEST_HANDLER_HXX_INCLUDED
