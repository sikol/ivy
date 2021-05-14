/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <sstream>

#include <catch2/catch.hpp>

#include <ivy/log.hxx>
#include <ivy/log/ostream_sink.hxx>

TEST_CASE("ivy:log:ostream_sink", "[ivy][log][ostream_sink]") {
    using Catch::Matchers::EndsWith;

    ivy::log::logger log;
    std::ostringstream strm;

    log.add_sink(ivy::log::make_ostream_sink(strm));
    log.log_message(ivy::log::severity_code::info,
                    "this is a test log message");

    auto str = strm.str();
    REQUIRE_THAT(str, EndsWith("this is a test log message\n"));
}
