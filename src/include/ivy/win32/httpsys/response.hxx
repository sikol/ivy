/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_HTTPSYS_RESPONSE_HXX_INCLUDED
#define IVY_WIN32_HTTPSYS_RESPONSE_HXX_INCLUDED

#include <ivy/win32/windows.hxx>

#include <http.h>

#include <ivy/http/header.hxx>
#include <ivy/win32/heap.hxx>

namespace ivy::win32 {

	auto make_httpsys_header(http::http_header const &)
        -> unique_heap_ptr<HTTP_RESPONSE_HEADERS>;

}

#endif // IVY_WIN32_HTTPSYS_RESPONSE_HXX_INCLUDED
