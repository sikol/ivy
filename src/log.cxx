/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <iostream>
#include <syncstream>

#include <ivy/check.hxx>
#include <ivy/log.hxx>
#include <ivy/log/ostream_sink.hxx>

namespace ivy::log {

    auto get_global_logger() -> logger *
    {
        static logger global_logger;
        return &global_logger;
    }

    logger::logger() = default;

    auto str(severity_code sev) -> std::string_view {
        using namespace std::string_view_literals;

        static std::array severity_names{
            "none"sv,
            "trace"sv,
            "info"sv,
            "notice"sv,
            "warning"sv,
            "error"sv,
            "fatal"sv,
        };

        IVY_CHECK(sev > severity_code::none &&
                  sev < severity_code::last, "log: invalid severity code");

        auto isev = static_cast<int>(sev);
        return severity_names[isev];
    }

    auto logger::log_message(severity_code severity,
                             std::string_view message) -> void
    {
        if (_loggers.empty()) {
            std::cerr << message << '\n';
            return;
        }

        auto timestamp = std::chrono::system_clock::now();

        for (auto &&logger : _loggers)
            logger->log_message(timestamp, severity, message);
    }

    auto logger::add_sink(std::unique_ptr<sink> &&s) -> void
    {
        _loggers.push_back(std::move(s));
    }

    sink::~sink() = default;

    /*************************************************************************
     *
     * ostream_sink
     */

    ostream_sink::ostream_sink(std::ostream &stream) : _stream(stream) {}

    auto
    ostream_sink::log_message(std::chrono::system_clock::time_point timestamp,
                              severity_code severity,
                              std::string_view message) -> void
    {
        auto text = std::format("{} | {} | {}", timestamp, str(severity), message);
        std::osyncstream(_stream) << text << std::endl;
    }

    auto make_ostream_sink(std::ostream &strm) -> std::unique_ptr<ostream_sink>
    {
        return std::make_unique<ostream_sink>(strm);
    }

    } // namespace ivy::log
