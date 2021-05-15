/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <string>
#include <tuple>
#include <vector>

#include <catch2/catch.hpp>

#include <ivy/algorithm/bintext.hxx>

TEST_CASE("ivy:bintext_encode: base64", "[ivy][bintext][base64]")
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
        std::make_tuple("foo\x80" "bar", "Zm9vgGJhcg=="),
        std::make_tuple("foo\xff" "bar", "Zm9v/2Jhcg=="),
    };

    for (auto const &[string, expected] : test_vectors) {
        auto b = ivy::bintext_encode_to_string<ivy::base64>(string);
        INFO(string);
        INFO(expected);
        REQUIRE(b == expected);

        auto p = ivy::bintext_decode_to_string<ivy::base64>(expected);
        REQUIRE(p);
        REQUIRE(*p == string);
    }
}

TEST_CASE("ivy:bintext_encode: base64 invalid", "[ivy][bintext][base64]")
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
        auto p = ivy::bintext_decode_to_string<ivy::base64>(b64);
        INFO(b64);
        REQUIRE(!p);
        REQUIRE(p.error() == std::errc::invalid_argument);
    }
}

TEST_CASE("ivy:bintext_encode: base32", "[ivy][bintext][base32]")
{
    std::vector<std::tuple<std::string, std::string>> test_vectors{
        std::make_tuple("", ""),
        std::make_tuple("f", "MY======"),
        std::make_tuple("fo", "MZXQ===="),
        std::make_tuple("foo", "MZXW6==="),
        std::make_tuple("foob", "MZXW6YQ="),
        std::make_tuple("fooba", "MZXW6YTB"),
        std::make_tuple("foobar", "MZXW6YTBOI======"),
    };

    for (auto const &[string, expected] : test_vectors) {
        auto b = ivy::bintext_encode_to_string<ivy::base32>(string);
        INFO(string);
        INFO(expected);
        REQUIRE(b == expected);

        auto p = ivy::bintext_decode_to_string<ivy::base32>(expected);
        REQUIRE(p);
        REQUIRE(*p == string);
    }
}

TEST_CASE("ivy:bintext_encode: base16", "[ivy][bintext][base16]")
{
    std::vector<std::tuple<std::string, std::string>> test_vectors{
        std::make_tuple("", ""),
        std::make_tuple("f", "66"),
        std::make_tuple("fo", "666F"),
        std::make_tuple("foo", "666F6F"),
        std::make_tuple("foob", "666F6F62"),
        std::make_tuple("fooba", "666F6F6261"),
        std::make_tuple("foobar", "666F6F626172"),
    };

    for (auto const &[string, expected] : test_vectors) {
        auto b = ivy::bintext_encode_to_string<ivy::hexchars>(string);
        INFO(string);
        INFO(expected);
        REQUIRE(b == expected);

        auto p = ivy::bintext_decode_to_string<ivy::hexchars>(expected);
        REQUIRE(p);
        REQUIRE(*p == string);
    }
}
