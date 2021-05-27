/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <system_error>

#include <catch2/catch.hpp>

#include <ivy/expected.hxx>
#include <ivy/error.hxx>

TEST_CASE("ivy:expected: basic", "[ivy][expected]")
{
    auto okay_function = []() -> ivy::expected<int, ivy::error> {
        return 42;
    };

    auto fail_function = []() -> ivy::expected<int, ivy::error> {
        return ivy::make_unexpected(ivy::make_error(std::errc::invalid_argument));
    };

    auto ok = okay_function();
    REQUIRE(ok);
    REQUIRE(*ok == 42);

    REQUIRE_NOTHROW(ok.or_throw());

    auto fail = fail_function();
    REQUIRE(!fail);
    REQUIRE(fail.error() == std::errc::invalid_argument);

    REQUIRE_THROWS_AS(fail.or_throw(), std::system_error);
}
