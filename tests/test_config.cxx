/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <string>
#include <string_view>

#include <catch2/catch.hpp>

#include <ivy/config/parse.hxx>

TEST_CASE("ivy:config:parse: invalid", "[ivy][config]") {
    using namespace std::string_literals;

    auto config_text = UR"(
int-value 42 7;
)"s; 

    auto c = ivy::config::parse(config_text);
    REQUIRE(!c);
//    auto const &err = c.error();
}

TEST_CASE("ivy:config:parse: simple", "[ivy][config]")
{
    using namespace std::string_literals;

    auto config_text = UR"(
int-value 42;
string-value foo;
qstring-value "this is a qstring";
bool-value;

block test {
  sub-int 666;
};

block {
  other-int 1024;
};

)"s;

    auto c = ivy::config::parse(config_text);
    REQUIRE(c);

    REQUIRE(c->items.size() == 6);

    {
        auto const &i = c->items[0];
        REQUIRE(i.size() == 0);
        REQUIRE(i.name() == U"int-value");
        REQUIRE(i.value() == std::int64_t{42});
    }

    {
        auto const &s = c->items[1];
        REQUIRE(s.size() == 0);
        REQUIRE(s.name() == U"string-value");
        REQUIRE(s.value() == U"foo");
    }

    {
        auto const &s = c->items[2];
        REQUIRE(s.size() == 0);
        REQUIRE(s.name() == U"qstring-value");
        REQUIRE(s.value() == U"this is a qstring");
    }

    {
        auto const &s = c->items[3];
        REQUIRE(s.size() == 0);
        REQUIRE(s.name() == U"bool-value");
        REQUIRE(s.value() == true);
    }

    {
        auto const &block = c->items[4];
        REQUIRE(block.name() == U"block");
        REQUIRE(block.value() == U"test");

        REQUIRE(block.size() == 1);

        auto const &i = block[0];
        REQUIRE(i.name() == U"sub-int");
        REQUIRE(i.value() == std::int64_t{666});
    }

    {
        auto const &block = c->items[5];
        REQUIRE(block.name() == U"block");
        REQUIRE(block.value() == true);

        REQUIRE(block.size() == 1);

        auto const &i = block[0];
        REQUIRE(i.name() == U"other-int");
        REQUIRE(i.value() == std::int64_t{1024});
    }
}
