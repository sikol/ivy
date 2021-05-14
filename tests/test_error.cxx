/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <catch2/catch.hpp>

#include <ivy/error.hxx>

ivy::error return_runtime_error(std::string what)
{
    return std::runtime_error(what);
}

ivy::error return_success()
{
    return ivy::error();
}

TEST_CASE("ivy::error returns an exception", "[ivy][error]")
{
    auto err = return_runtime_error("this is a test error");
    REQUIRE(!err);
    REQUIRE(std::strcmp(err.what(), "this is a test error") == 0);

    REQUIRE(err.is<std::exception>());
    REQUIRE(err.is<std::runtime_error>());
    REQUIRE(std::strcmp(err.get<std::runtime_error>()->what(),
                        "this is a test error") == 0);

    REQUIRE(!err.is<std::domain_error>());
    REQUIRE(err.get<std::domain_error>() == nullptr);
}

TEST_CASE("ivy::error returns success", "[ivy][error]")
{
    auto err = return_success();
    REQUIRE(err);
    REQUIRE(std::strcmp(err.what(), "success") == 0);
    REQUIRE(!err.is<std::exception>());
    REQUIRE(err.get<std::exception>() == nullptr);
}