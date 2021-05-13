/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <system_error>

#include <catch2/catch.hpp>

#include <ivy/expected.hxx>

TEST_CASE("ivy:expected: basic", "[ivy][expected]")
{
    auto okay_function = []() -> ivy::expected<int, std::error_code> {
        return 42;
    };

    auto fail_function = []() -> ivy::expected<int, std::error_code> {
        return ivy::make_unexpected(std::make_error_code(std::errc::invalid_argument));
    };

    auto ok = okay_function();
    REQUIRE(ok);
    REQUIRE(*ok == 42);

    auto fail = fail_function();
    REQUIRE(!fail);
    REQUIRE(fail.error() == std::errc::invalid_argument);
}
