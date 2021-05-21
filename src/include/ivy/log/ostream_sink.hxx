/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_LOG_OSTREAM_SINK_HXX_INCLUDED
#define IVY_LOG_OSTREAM_SINK_HXX_INCLUDED

#include <iosfwd>

#include <ivy/log.hxx>

namespace ivy::log {

    class ostream_sink final : public sink {
        std::ostream &_stream;

    public:
        ostream_sink(std::ostream &strm);

        auto log_message(std::chrono::system_clock::time_point timestamp,
                         severity_code severity,
                         std::string_view message) -> void final;
    };

    auto make_ostream_sink(std::ostream &) -> std::unique_ptr<ostream_sink>;

} // namespace ivy::log

#endif // IVY_LOG_OSTREAM_SINK_HXX_INCLUDED
