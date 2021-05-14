/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <catch2/catch.hpp>

#include <cstring>
#include <stdexcept>
#include <string>

#include <ivy/lazy.hxx>

namespace {
    int num_calls;
}

auto f() -> int
{
    ++num_calls;
    return 42;
}

TEST_CASE("lazy<> calls the producer once")
{
    ivy::lazy<int> lz(f);

    REQUIRE(lz.get() == 42);
    REQUIRE(lz.get() == 42);
    REQUIRE(lz.get() == 42);
    REQUIRE(lz.get() == 42);
    REQUIRE(lz.get() == 42);

    REQUIRE(num_calls == 1);
}
