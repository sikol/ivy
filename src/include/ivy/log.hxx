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

namespace ivy {

    namespace log {

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

            auto log_message(severity_code severity, std::string_view message)
                -> void;

            auto add_sink(std::unique_ptr<sink> &&) -> void;
        };

        auto get_global_logger() -> logger *;

    } // namespace log

    namespace detail {

        template <typename... Args>
        auto log_any(log::severity_code sev, Args&&...args)
        {
            auto logger = ::ivy::log::get_global_logger();
            auto msg = std::format(std::forward<Args>(args)...);
            logger->log_message(sev, msg);
        }

    } // namespace detail

    template<typename... Args>
    auto log_trace(Args &&...args)
    {
        detail::log_any(log::severity_code::trace, std::forward<Args>(args)...);
    }

    template <typename... Args>
    auto log_info(Args &&...args)
    {
        detail::log_any(log::severity_code::info, std::forward<Args>(args)...);
    }

    template <typename... Args>
    auto log_notice(Args &&...args)
    {
        detail::log_any(log::severity_code::notice, std::forward<Args>(args)...);
    }

    template <typename... Args>
    auto log_warning(Args &&...args)
    {
        detail::log_any(log::severity_code::warning, std::forward<Args>(args)...);
    }

    template <typename... Args>
    auto log_error(Args &&...args)
    {
        detail::log_any(log::severity_code::error, std::forward<Args>(args)...);
    }

    template <typename... Args>
    auto log_fatal(Args &&...args)
    {
        detail::log_any(log::severity_code::fatal, std::forward<Args>(args)...);
    }

} // namespace ivy

#endif // IVY_LOG_HXX_INCLUDED
