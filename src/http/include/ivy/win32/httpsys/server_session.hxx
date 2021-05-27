/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_HTTPSYS_SERVER_SESSION_HXX_INCLUDED
#define IVY_WIN32_HTTPSYS_SERVER_SESSION_HXX_INCLUDED

#include <ivy/win32/windows.hxx>
#include <ivy/noncopyable.hxx>

#include <http.h>

#include <wil/resource.h>

namespace ivy::win32::httpsys {

    /*************************************************************************
     *
     * server_session
     *
     * Creating an server_session calls HttpCreateServerSession().
     * Destructing it calls HttpCloseServerSession().
     * 
     * server_session::server_session() may throw http_error if
     * HttpCreateServerSession() fails.
     */

    class server_session : public ivy::noncopyable {
        using handle_type =
            wil::unique_any<HTTP_SERVER_SESSION_ID,
                            decltype(::HttpCloseServerSession),
                            ::HttpCloseServerSession>;

        handle_type _session{};

    public:
        server_session();
        server_session(server_session &&) noexcept;
        ~server_session();

        auto operator=(server_session &&) noexcept -> server_session &;

        [[nodiscard]] auto get_session_id() const noexcept -> HTTP_SERVER_SESSION_ID;
    };

}

#endif // IVY_WIN32_HTTPSYS_SERVER_SESSION_HXX_INCLUDED
