/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <format>

#include <ivy/win32/error.hxx>
#include <ivy/http/service.hxx>
#include <ivy/win32/httpsys/server_session.hxx>

namespace ivy::win32::httpsys {

    server_session::server_session()
    {
        auto r = ::HttpCreateServerSession(HTTPAPI_VERSION_2, &_session, 0);

        if (r != NO_ERROR) {
            auto err = make_win32_error(r);
            throw http::http_error(std::format(
                "HttpCreateServerSession() failed: {}", err.message()));
        }
    }

    server_session::server_session(server_session &&) noexcept =
        default;
    server_session::~server_session() = default;

    auto server_session::operator=(server_session &&) noexcept
        -> server_session & = default;

    auto server_session::get_session_id() const noexcept -> HTTP_SERVER_SESSION_ID    {
        return _session.get();
    }

}
