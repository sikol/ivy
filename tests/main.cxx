/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

auto main(int argc, char **argv) -> int
{
    int result = Catch::Session().run(argc, argv);
    return result;
}
