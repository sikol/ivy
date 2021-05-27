/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <catch2/catch.hpp>

#include <ivy/error.hxx>

#ifdef _MSC_VER
// Unreachable code; triggered by REQUIRE_THROWS_AS.
#    pragma warning(disable : 4702)
#endif

auto return_runtime_error(std::string const &what) -> ivy::error
{
    return ivy::make_error<std::runtime_error>(what);
}

auto return_invalid_argument() -> ivy::error
{
    return ivy::make_error(std::errc::invalid_argument);
}

auto return_success() -> ivy::error
{
    return ivy::error();
}

TEST_CASE("ivy:error: exception", "[ivy][error]")
{
    auto err = return_runtime_error("this is a test error");
    REQUIRE(!err);
    REQUIRE(err.what() == "this is a test error");

    REQUIRE(err.is<std::exception>());
    REQUIRE(err.is<std::runtime_error>());
    REQUIRE(std::strcmp(err.get<std::runtime_error>()->what(),
                        "this is a test error") == 0);

    REQUIRE(!err.is<std::domain_error>());
    REQUIRE(err.get<std::domain_error>() == nullptr);

    REQUIRE_THROWS_AS(err.rethrow(), std::runtime_error);
}

TEST_CASE("ivy:error: std::error_code", "[ivy][error]")
{
    auto err = return_invalid_argument();
    REQUIRE(!err);

    REQUIRE(!is<std::exception>(err));
    REQUIRE(!is<std::runtime_error>(err));
    REQUIRE(!is<std::system_error>(err));
    REQUIRE(is<std::error_code>(err));
    REQUIRE(err == std::errc::invalid_argument);
}

TEST_CASE("ivy:error returns success", "[ivy][error]")
{
    auto err = return_success();
    REQUIRE(err);
    REQUIRE(err.what() == "success");
    REQUIRE(!err.is<std::exception>());
    REQUIRE(err.get<std::exception>() == nullptr);
}