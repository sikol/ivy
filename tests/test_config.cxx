/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <string>
#include <string_view>

#include <catch2/catch.hpp>

#include <ivy/config/bind.hxx>
#include <ivy/config/parse.hxx>

TEST_CASE("ivy:config:parse: invalid", "[ivy][config]")
{
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

    auto &config = *c;
    REQUIRE(config.size() == 6);

    {
        auto const &i = config[0];
        REQUIRE(i.size() == 0);
        REQUIRE(i.name() == U"int-value");
        REQUIRE(i.value() == std::int64_t{42});
    }

    {
        auto const &s = config[1];
        REQUIRE(s.size() == 0);
        REQUIRE(s.name() == U"string-value");
        REQUIRE(s.value() == U"foo");
    }

    {
        auto const &s = config[2];
        REQUIRE(s.size() == 0);
        REQUIRE(s.name() == U"qstring-value");
        REQUIRE(s.value() == U"this is a qstring");
    }

    {
        auto const &s = config[3];
        REQUIRE(s.size() == 0);
        REQUIRE(s.name() == U"bool-value");
        REQUIRE(s.value() == true);
    }

    {
        auto const &block = config[4];
        REQUIRE(block.name() == U"block");
        REQUIRE(block.value() == U"test");

        REQUIRE(block.size() == 1);

        auto const &i = block[0];
        REQUIRE(i.name() == U"sub-int");
        REQUIRE(i.value() == std::int64_t{666});
    }

    {
        auto const &block = config[5];
        REQUIRE(block.name() == U"block");
        REQUIRE(block.value() == true);

        REQUIRE(block.size() == 1);

        auto const &i = block[0];
        REQUIRE(i.name() == U"other-int");
        REQUIRE(i.value() == std::int64_t{1024});
    }
}

TEST_CASE("ivy:config:bind")
{
    auto config_text = UR"(
string-option 'test';
int-option 42;

int-list 42;
int-list 666;
int-list 1024;

subitem {
    int-suboption 123;
};

subitems "one" {
    int-suboption 222;
};

subitems "two" {
    int-suboption 333;
};

named-subitem "three" {
    int-suboption 444;
};

named-subitem "four" {
    int-suboption 555;
};
)";

    struct test_anonymous_subitem {
        int int_option = 0;
        ivy::string string_option;
    };

    struct test_named_subitem {
        ivy::string subitem_name;
        int int_option = 0;
        ivy::string string_option;
    };

    struct test_config {
        int int_option = 0;
        std::vector<int> int_list;
        ivy::string string_option;

        test_anonymous_subitem subitem;
        std::vector<test_named_subitem> subitems;
        std::map<ivy::string, test_named_subitem> named_subitems;
    };

    auto anonymous_subitem_schema =
        ivy::config::block<test_anonymous_subitem>::create() //
            .add_option(U"int-suboption", &test_anonymous_subitem::int_option)
            .add_option(U"string-suboption",
                        &test_anonymous_subitem::string_option);

    auto named_subitem_schema =
        ivy::config::block<test_named_subitem>::create(
            &test_named_subitem::subitem_name) //
            .add_option(U"int-suboption", &test_named_subitem::int_option)
            .add_option(U"string-suboption",
                        &test_named_subitem::string_option);

    auto schema =
        ivy::config::block<test_config>::create()
            .add_option(U"string-option", &test_config::string_option)
            .add_option(U"int-option", &test_config::int_option)
            .add_option(U"int-list", &test_config::int_list)
            .add_block(
                U"subitem", &test_config::subitem, anonymous_subitem_schema)
            .add_block(
                U"subitems", &test_config::subitems, named_subitem_schema)
            .add_block(U"named-subitem",
                       &test_named_subitem::subitem_name,
                       &test_config::named_subitems,
                       named_subitem_schema);

    auto c = ivy::config::parse(config_text);
    REQUIRE(c);

    test_config conf;
    auto ok = schema.load(conf, *c);
    if (!ok) {
        INFO(ok.error().what());
        REQUIRE(ok);
    }

    REQUIRE(conf.string_option == U"test");

    REQUIRE(conf.int_option == 42);

    REQUIRE(conf.int_list.size() == 3);
    REQUIRE(conf.int_list[0] == 42);
    REQUIRE(conf.int_list[1] == 666);
    REQUIRE(conf.int_list[2] == 1024);

    REQUIRE(conf.subitem.int_option == 123);

    REQUIRE(conf.subitems.size() == 2);

    REQUIRE(conf.subitems[0].int_option == 222);
    REQUIRE(conf.subitems[0].subitem_name == U"one");

    REQUIRE(conf.subitems[1].int_option == 333);
    REQUIRE(conf.subitems[1].subitem_name == U"two");

    REQUIRE(conf.named_subitems.size() == 2);

    {
        auto it = conf.named_subitems.find(U"three");
        REQUIRE(it != conf.named_subitems.end());
        REQUIRE(it->second.int_option == 444);
    }

    {
        auto it = conf.named_subitems.find(U"four");
        REQUIRE(it != conf.named_subitems.end());
        REQUIRE(it->second.int_option == 555);
    }
}