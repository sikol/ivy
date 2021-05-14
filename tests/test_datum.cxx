/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <catch2/catch.hpp>

#include <ivy/datum.hxx>
#include <ivy/datum/integer.hxx>
#include <ivy/datum/string.hxx>
#include <ivy/datum/null.hxx>
#include <ivy/datum/boolean.hxx>

TEST_CASE("ivy:datum:str", "[ivy][datum]") {
    ivy::datum i = ivy::make_integer_datum(42);
    REQUIRE(str(i) == "42");

    ivy::datum s = ivy::make_string_datum("foo");
    REQUIRE(str(s) == "foo");

    ivy::datum n = ivy::make_null_datum();
    REQUIRE(str(n) == "null");

    ivy::datum b = ivy::make_boolean_datum(true);
    REQUIRE(str(b) == "true");
}

TEST_CASE("ivy:datum:operator==", "[ivy][datum]") {
    ivy::datum one = ivy::make_integer_datum(1);
    ivy::datum one_ = ivy::make_integer_datum(1);
    ivy::datum two = ivy::make_integer_datum(2);

    ivy::datum foo = ivy::make_string_datum("foo");
    ivy::datum foo_ = ivy::make_string_datum("foo");
    ivy::datum bar = ivy::make_string_datum("bar");

    ivy::datum null = ivy::make_null_datum();
    ivy::datum null_ = ivy::make_null_datum();

    ivy::datum btrue = ivy::make_boolean_datum(true);
    ivy::datum btrue_ = ivy::make_boolean_datum(true);
    ivy::datum bfalse = ivy::make_boolean_datum(false);

    REQUIRE(one == one_);
    REQUIRE(one != two);
    REQUIRE(one != foo);
    REQUIRE(one != null);

    REQUIRE(foo == foo_);
    REQUIRE(foo != bar);
    REQUIRE(foo != one);

    REQUIRE(null == null_);

    REQUIRE(btrue == btrue_);
    REQUIRE(btrue != bfalse);
}

TEST_CASE("ivy:datum:datum_cast", "[ivy][datum]")
{
    ivy::datum i = ivy::make_integer_datum(42);
    REQUIRE(is<ivy::integer_type>(i));
    REQUIRE(ivy::datum_cast<std::int64_t>(i) == 42);

    ivy::datum s = ivy::make_string_datum("foo");
    REQUIRE(is<ivy::string_type>(s));
    REQUIRE(ivy::datum_cast<std::string>(s) == "foo");

    ivy::datum b = ivy::make_boolean_datum(true);
    REQUIRE(is<ivy::boolean_type>(b));
    REQUIRE(ivy::datum_cast<bool>(b) == true);
}
