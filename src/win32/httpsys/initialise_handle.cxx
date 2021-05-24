/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <ivy/win32/httpsys/initialise_handle.hxx>

#include <ivy/win32/windows.hxx>
#include <ivy/win32/error.hxx>
#include <ivy/http/service.hxx>

#include <http.h>

namespace ivy::win32::httpsys {

    initialise_handle::initialise_handle() = default;

    initialise_handle::initialise_handle(initialise_handle &&other) noexcept
        : _flags(std::exchange(other._flags, 0))
    {
    }

    auto initialise_handle::operator=(initialise_handle &&other) noexcept
        -> initialise_handle &
    {
        if (&other != this) {
            if (_flags != 0)
                ::HttpTerminate(_flags, nullptr);

            _flags = std::exchange(other._flags, 0);
        }

        return *this;
    }

    initialise_handle::~initialise_handle()
    {
        if (_flags)
            ::HttpTerminate(_flags, nullptr);
    }

    initialise_handle::initialise_handle(ULONG flags)
        : _flags(flags)
    {
        auto ret = ::HttpInitialize(HTTPAPI_VERSION_2, flags, nullptr);

        if (ret != NO_ERROR) {
            auto err = make_win32_error(ret);
            throw http::http_error(
                std::format("HttpInitialize() failed: {}", err.message()));
        }
    }

}
