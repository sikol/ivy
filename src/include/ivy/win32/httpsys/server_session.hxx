/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_HTTPSYS_SERVER_SESSION_HXX_INCLUDED
#define IVY_WIN32_HTTPSYS_SERVER_SESSION_HXX_INCLUDED

#include <ivy/win32/windows.hxx>

#include <http.h>

#include <wil/resource.h>

namespace ivy::win32 {

    /*************************************************************************
     *
     * http_server_session
     *
     * Creating an http_server_session calls HttpCreateServerSession().
     * Destructing it calls HttpCloseServerSession().
     * Dereferencing returns the handle to the server session.
     * 
     * http_server_session::http_server_session() may throw http_error if
     * HttpCreateServerSession() fails.
     */

    // RAII handle to the server session.
    using unique_http_server_session =
        wil::unique_any<HTTP_SERVER_SESSION_ID,
                        decltype(::HttpCloseServerSession),
                        ::HttpCloseServerSession>;

    class http_server_session : public ivy::noncopyable {
        unique_http_server_session _session{};

    public:
        http_server_session();
        http_server_session(http_server_session &&) noexcept;
        ~http_server_session();

        auto operator=(http_server_session &&) noexcept
            -> http_server_session &;

        auto operator*() noexcept -> unique_http_server_session &;
        auto operator->() noexcept -> unique_http_server_session *;
    };

}

#endif // IVY_WIN32_HTTPSYS_SERVER_SESSION_HXX_INCLUDED
