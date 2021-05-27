/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_HTTPSYS_HEADER_HXX_INCLUDED
#define IVY_WIN32_HTTPSYS_HEADER_HXX_INCLUDED

#include <optional>

#include <ivy/win32/windows.hxx>

#include <http.h>

#include <ivy/string.hxx>

namespace ivy::win32::httpsys {

    // Map between header strings and http.sys header id.

    auto header_id_to_name(HTTP_HEADER_ID) -> astring;
    auto header_name_to_id(astring name) -> std::optional<HTTP_HEADER_ID>;

} // namespace ivy::win32::httpsys

#endif // IVY_WIN32_HTTPSYS_HEADER_HXX_INCLUDED
