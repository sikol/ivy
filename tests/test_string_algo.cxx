/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <string>
#include <vector>

#include <catch2/catch.hpp>

#include <ivy/charenc/utf32.hxx>
#include <ivy/regex.hxx>
#include <ivy/string/join.hxx>
#include <ivy/string/match.hxx>
#include <ivy/string/split.hxx>
#include <ivy/string/trim.hxx>

TEST_CASE("ivy:string:join 1-element vector<string> with iterators",
          "[ivy][string][join]")
{
    std::vector<std::string> strings{"foo"};

    std::string r1 = ivy::join(strings.begin(), strings.end(), ",");
    REQUIRE(r1 == "foo");
}

TEST_CASE("ivy:string:join 1-element vector<string> with range",
          "[ivy][string][join]")
{
    std::vector<std::string> strings{"foo"};

    std::string r1 = ivy::join(strings, ",");
    REQUIRE(r1 == "foo");
}

TEST_CASE("ivy:string:join 3-element vector<string> with iterators",
          "[ivy][string][join]")
{
    std::vector<std::string> strings{"foo", "bar", "quux"};

    std::string r1 = ivy::join(strings.begin(), strings.end(), ",");
    REQUIRE(r1 == "foo,bar,quux");
}

TEST_CASE("ivy:string:join 3-element vector<string> with range",
          "[ivy][string][join]")
{
    std::vector<std::string> strings{"foo", "bar", "quux"};

    std::string r1 = ivy::join(strings, ",");
    REQUIRE(r1 == "foo,bar,quux");
}

TEST_CASE("ivy:string:join 3-element vector<string_view> with range",
          "[ivy][string][join]")
{
    std::vector<std::string_view> strings{"foo", "bar", "quux"};

    std::string r1 = ivy::join(strings, ",");
    REQUIRE(r1 == "foo,bar,quux");
}
#if 0
TEST_CASE("ivy:string:match_regex: integer",
          "[ivy][string][match][match_regex]")
{
    ivy::string test_string = U"123foo";
    ivy::u32regex re(U"^[0-9]+");

    auto r = ivy::match_regex(test_string, re);

    REQUIRE(r.first.has_value());
    REQUIRE(r.first->size() == 1);
    auto i_match = (*r.first)[0];
    REQUIRE(i_match == U"123");

    REQUIRE(r.second == U"foo");
}

TEST_CASE("ivy:string:match_regex: no match",
          "[ivy][string][match][match_regex]")
{
    ivy::string test_string = U"foo123";
    ivy::regex re(U"^[0-9]+");

    auto r = ivy::match_regex(test_string, re);

    REQUIRE(!r.first.has_value());
    REQUIRE(r.second == U"foo123");
}
#endif

TEST_CASE("ivy:string:match_string: match",
          "[ivy][string][match][match_string]")
{
    using namespace std::string_view_literals;

    ivy::string test_string = U"foo123";
    auto [p, rest] = ivy::match_string(test_string, U"fo");
    REQUIRE(p);
    REQUIRE(*p == U"fo");
    REQUIRE(rest == U"o123");
}

TEST_CASE("ivy:string:match_string: no match",
          "[ivy][string][match][match_string]")
{
    using namespace std::string_view_literals;

    ivy::string test_string = U"foo123";
    auto [p, rest] = ivy::match_string(test_string, U"123");
    REQUIRE(!p);
    REQUIRE(rest == U"foo123");
}

TEST_CASE("ivy:string:match_int: positive decimal integer",
          "[ivy][string][match][match_int]")
{
    using namespace std::string_view_literals;

    ivy::string test_string = U"123foo";
    auto [i, rest] = ivy::match_int<std::int64_t>(test_string);
    REQUIRE(i);
    REQUIRE(*i == 123);
    REQUIRE(rest == U"foo");
}

TEST_CASE("ivy:string:match_int: negative decimal integer",
          "[ivy][string][match][match_int]")
{
    using namespace std::string_view_literals;

    ivy::string test_string = U"-123foo";
    auto [i, rest] = ivy::match_int<std::int64_t>(test_string);
    REQUIRE(i);
    REQUIRE(*i == -123);
    REQUIRE(rest == U"foo");
}

TEST_CASE("ivy:string:match_int: positive hex integer",
          "[ivy][string][match][match_int]")
{
    using namespace std::string_view_literals;

    ivy::string test_string = U"1024quux";
    auto [i, rest] = ivy::match_int<std::int64_t>(test_string, 16);
    REQUIRE(i);
    REQUIRE(*i == 0x1024);
    REQUIRE(rest == U"quux");
}

TEST_CASE("ivy:string:match_int: negative hex integer",
          "[ivy][string][match][match_int]")
{
    using namespace std::string_view_literals;

    ivy::string test_string = U"-1024quux";
    auto [i, rest] = ivy::match_int<std::int64_t>(test_string, 16);
    REQUIRE(i);
    REQUIRE(*i == -0x1024);
    REQUIRE(rest == U"quux");
}

TEST_CASE("ivy:string:match_whitespace: simple",
          "[ivy][string][match][match_whitespace]")
{
    using namespace std::string_view_literals;

    ivy::string test_string = U"   foo";
    auto [ws, rest] = ivy::match_whitespace(test_string);
    REQUIRE(ws);
    REQUIRE(*ws == U"   ");
    REQUIRE(rest == U"foo");
}

TEST_CASE("ivy:string:match_whitespace: trailing space",
          "[ivy][string][match][match_whitespace]")
{
    using namespace std::string_view_literals;

    ivy::string test_string = U"   foo  "sv;
    auto [ws, rest] = ivy::match_whitespace(test_string);
    REQUIRE(ws);
    REQUIRE(*ws == U"   ");
    REQUIRE(rest == U"foo  ");
}

TEST_CASE("ivy:string:match_whitespace: no match",
          "[ivy][string][match][match_whitespace]")
{
    using namespace std::string_view_literals;

    ivy::string test_string = U"foo";
    auto [ws, rest] = ivy::match_whitespace(test_string);
    REQUIRE(!ws);
    REQUIRE(rest == U"foo");
}

TEST_CASE("ivy:string:trim:triml: simple", "[ivy][string][trim][triml]")
{
    using namespace std::string_view_literals;

    ivy::string test_string = U"   foo  ";
    auto trimmed = ivy::triml(test_string);
    REQUIRE(trimmed == U"foo  ");
}

TEST_CASE("ivy:string:trim:triml: no whitespace", "[ivy][string][trim][triml]")
{
    using namespace std::string_view_literals;

    ivy::string test_string = U"foo";
    auto trimmed = ivy::triml(test_string);
    REQUIRE(trimmed == U"foo");
}

TEST_CASE("ivy:string:split: simple string_view", "[ivy][string][split]")
{
    using namespace std::string_view_literals;

    std::vector<ivy::string> bits;

    ivy::string test_1 = U"foo$bar$baz";
    ivy::split(test_1, '$', std::back_inserter(bits));
    REQUIRE(bits.size() == 3);
    REQUIRE(bits[0] == U"foo");
    REQUIRE(bits[1] == U"bar");
    REQUIRE(bits[2] == U"baz");
}

TEST_CASE("ivy:string:split: single element", "[ivy][string][split]")
{
    using namespace std::string_view_literals;

    std::vector<ivy::string> bits;

    ivy::string test_1 = U"foobar";
    ivy::split(test_1, '$', std::back_inserter(bits));
    REQUIRE(bits.size() == 1);
    REQUIRE(bits[0] == U"foobar");
}

TEST_CASE("ivy:string:split: empty first token", "[ivy][string][split]")
{
    using namespace std::string_view_literals;

    std::vector<ivy::string> bits;

    ivy::string test_1 = U"$foo$bar";
    ivy::split(test_1, '$', std::back_inserter(bits));
    REQUIRE(bits.size() == 3);
    REQUIRE(bits[0] == U"");
    REQUIRE(bits[1] == U"foo");
    REQUIRE(bits[2] == U"bar");
}

TEST_CASE("ivy:string:split: empty last token", "[ivy][string][split]")
{
    using namespace std::string_view_literals;

    std::vector<ivy::string> bits;

    ivy::string test_1 = U"foo$bar$";
    ivy::split(test_1, '$', std::back_inserter(bits));
    REQUIRE(bits.size() == 3);
    REQUIRE(bits[0] == U"foo");
    REQUIRE(bits[1] == U"bar");
    REQUIRE(bits[2] == U"");
}

TEST_CASE("ivy:string:split: simple string", "[ivy][string][split]")
{
    using namespace std::string_literals;

    std::vector<ivy::string> bits;

    ivy::string test_1 = U"foo$bar$baz";
    ivy::split(test_1, '$', std::back_inserter(bits));
    REQUIRE(bits.size() == 3);
    REQUIRE(bits[0] == U"foo");
    REQUIRE(bits[1] == U"bar");
    REQUIRE(bits[2] == U"baz");
}
