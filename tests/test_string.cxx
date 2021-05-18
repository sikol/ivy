/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <cstring>

#include <catch2/catch.hpp>

#include <ivy/string/u8string.hxx>

TEST_CASE("ivy:u8string:comparison", "[ivy][string][u8string]")
{
    ivy::u8string foo1(u8"foo");
    ivy::u8string foo2(u8"foo");
    ivy::u8string bar(u8"bar");
    ivy::u8string quux(u8"quux");

    REQUIRE(foo1 == foo2);
    REQUIRE(foo1 != bar);
    REQUIRE(foo1 != quux);
}

TEST_CASE("ivy:u8string::op[]", "[ivy][string][u8string]")
{
    ivy::u8string foo(u8"foo");
    REQUIRE(foo[0] == u8'f');
    REQUIRE(foo[1] == u8'o');
    REQUIRE(foo[2] == u8'o');
}

TEST_CASE("ivy:u8string::at()", "[ivy][string][u8string]")
{
    ivy::u8string foo(u8"foo");
    REQUIRE(foo.at(0) == u8'f');
    REQUIRE(foo.at(1) == u8'o');
    REQUIRE(foo.at(2) == u8'o');
    REQUIRE_THROWS(foo.at(3));
}

TEST_CASE("ivy:u8string::size", "[ivy][string][u8string]")
{
    ivy::u8string foo(u8"foo");
    REQUIRE(foo.size() == 3);
}

TEST_CASE("ivy:u8string::c_str", "[ivy][string][u8string]")
{
    ivy::u8string foo(u8"foo");
    auto const *cstr = reinterpret_cast<char const *>(foo.c_str());
    REQUIRE(std::strlen(cstr) == 3);
    REQUIRE(std::strcmp(cstr, "foo") == 0);
}

TEST_CASE("ivy:u8string::data", "[ivy][string][u8string]")
{
    ivy::u8string foo(u8"foo");
    auto const *data = reinterpret_cast<char const *>(foo.data());
    REQUIRE(std::memcmp(data, "foo", 3) == 0);
}

TEST_CASE("ivy:u8string::op+", "[ivy][string][u8string]")
{
    ivy::u8string foo(u8"foo");
    ivy::u8string quux(u8"quux");
    auto c = foo + quux;
    REQUIRE(c == u8"fooquux");
}
