/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_HTTPSYS_INITIALISE_HANDLE_HXX_INCLUDED
#define IVY_WIN32_HTTPSYS_INITIALISE_HANDLE_HXX_INCLUDED

#include <utility>

#include <ivy/win32/windows.hxx>

#include <ivy/noncopyable.hxx>

namespace ivy::win32::httpsys {

    /*************************************************************************
     *
     * initialise_handle.
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

    class initialise_handle : public ivy::noncopyable {
        ULONG _flags{};

    private:
    public:
        initialise_handle();
        initialise_handle(ULONG flags);
        initialise_handle(initialise_handle &&other) noexcept;
        ~initialise_handle();

        auto operator=(initialise_handle &&other) noexcept
            -> initialise_handle &;
    };

} // namespace ivy::win32

#endif // IVY_WIN32_HTTPSYS_INITIALISE_HANDLE_HXX_INCLUDED
