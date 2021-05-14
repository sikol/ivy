/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <sstream>

#include <catch2/catch.hpp>

#include <ivy/format.hxx>

TEST_CASE("ivy:format:print to ostream", "[ivy][format][print]") {
    std::ostringstream strm;

    ivy::fprint(strm, "{} + {} = {}", 1, 2, (1 + 2));
    auto result = strm.str();
    REQUIRE(result == "1 + 2 = 3");
}
