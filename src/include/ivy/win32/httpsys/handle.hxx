/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_HTTPSYS_HANDLE_HXX_INCLUDED
#define IVY_WIN32_HTTPSYS_HANDLE_HXX_INCLUDED

#include <utility>

namespace ivy::win32 {

    /*************************************************************************
     *
     * http_handle.
     *
     * When http_handle is initialised with a "flags" value, it calls
     * HttpInitialise().  Destructing the handle calls HttpTerminate().  The
     * HTTP API refcounts initialise calls, so multiple instances of http_handle
     * can exist.
     *
     * The default constructor creates an empty http_handle and does not call
     * HttpInitialise().
     *
     * http_handle::http_handle() throws http_error if HttpInitialise() fails.
     */

    class http_handle : public ivy::noncopyable {
        ULONG _flags{};

    private:
    public:
        http_handle();
        http_handle(ULONG flags);
        http_handle(http_handle &&other) noexcept;
        ~http_handle();

        auto operator=(http_handle &&other) noexcept -> http_handle &;
    };

} // namespace ivy::win32

#endif // IVY_WIN32_HTTPSYS_HANDLE_HXX_INCLUDED
