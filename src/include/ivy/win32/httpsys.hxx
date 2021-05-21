/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_HTTPSYS_HXX
#define IVY_WIN32_HTTPSYS_HXX

#include <memory>

#include <ivy/http/service.hxx>
#include <ivy/expected.hxx>
#include <ivy/error.hxx>

namespace ivy::win32 {

    auto make_httpsys_service() -> expected<std::unique_ptr<http::service>, error>;

}

#endif
