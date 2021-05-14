/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_LOG_HXX_INCLUDED
#define IVY_LOG_HXX_INCLUDED

#include <chrono>
#include <cstdint>
#include <shared_mutex>
#include <string_view>
#include <vector>

namespace ivy::log {

    enum struct severity_code : std::uint8_t {
        none = 0,
        trace = 1,
        info = 2,
        notice = 3,
        warning = 4,
        error = 5,
        fatal = 6,
        last
    };

    auto str(severity_code sev) -> std::string_view;

    class sink {
    public:
        virtual ~sink();

        virtual auto
        log_message(std::chrono::system_clock::time_point timestamp,
                    severity_code severity,
                    std::string_view message) -> void = 0;
    };

    class logger final {
        std::shared_mutex _mutex;
        std::vector<std::unique_ptr<sink>> _loggers;

    public:
        logger();

        auto log_message(severity_code severity,
                         std::string_view message) -> void;

        auto add_sink(std::unique_ptr<sink> &&) -> void;
    };

    auto get_global_logger() -> logger *;

} // namespace ivy::log

#endif // IVY_LOG_HXX_INCLUDED
