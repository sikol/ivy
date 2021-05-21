/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <ivy/http/service.hxx>
#include <ivy/win32/httpsys.hxx>
#include <ivy/log.hxx>
#include <ivy/log/ostream_sink.hxx>

auto main(int, char **) -> int
{
    auto logger = ivy::log::get_global_logger();
    logger->add_sink(ivy::log::make_ostream_sink(std::cout));

    ivy::log_notice("HTTP server starting");

    auto service = ivy::win32::make_httpsys_service();
    if (!service) {
        ivy::log_fatal("Cannot create HTTP service: {}",
                       service.error().what());
        return 1;
    }

    return 0;
}