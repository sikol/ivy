/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <string>
#include <tuple>
#include <vector>

#include <catch2/catch.hpp>

#include <ivy/algorithm/base64.hxx>

TEST_CASE("ivy:base64_encode", "[ivy][base64][base64_encode]")
{
    std::vector<std::tuple<std::string, std::string>> test_vectors{
        std::make_tuple("", ""),
        std::make_tuple("d", "ZA=="),
        std::make_tuple("f", "Zg=="),
        std::make_tuple("fo", "Zm8="),
        std::make_tuple("foo", "Zm9v"),
        std::make_tuple("foob", "Zm9vYg=="),
        std::make_tuple("fooba", "Zm9vYmE="),
        std::make_tuple("foobar", "Zm9vYmFy"),
    };

    for (auto const &[string, expected] : test_vectors) {
        auto b = ivy::base64_encode_to_string(string);
        REQUIRE(b == expected);

        auto p = ivy::base64_decode_to_string(expected);
        REQUIRE(p);
        REQUIRE(*p == string);
    }
}

TEST_CASE("base64::decode() failed on invalid inputs")
{
    using namespace std::string_literals;

    std::vector<std::string> invalid_inputs{
        "Zg=",
        "Zg",
        "Z",
        "Zm9vY",
        "Zm9vYg",
        "Zm9vYg=",
        "Zm9v\0g=="s,
        "Zm9v$g==",
        "Zm9v*g==",
        "Z=A=",
        "ZA==ZA==",
    };

    for (auto const &b64 : invalid_inputs) {
        auto p = ivy::base64_decode_to_string(b64);
        REQUIRE(!p);
        REQUIRE(p.error() == std::errc::invalid_argument);
    }
}
