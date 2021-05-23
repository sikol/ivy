/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <catch2/catch.hpp>

#include <ivy/http/service.hxx>

TEST_CASE("ivy:http_header", "[ivy][http]")
{
    ivy::http::http_header h;

    auto b = h.add_field({"test-header", u8"foo"});
    REQUIRE(b == true);

    b = h.add_field({"test-header", u8"bar"});
    REQUIRE(b == false);

    auto hdr = h.get_field("test-header");
    REQUIRE(hdr);
    REQUIRE(hdr->value == u8"foo");

    b = h.add_or_replace_field({"test-header", u8"quux"});
    REQUIRE(b == true);

    hdr = h.get_field("test-header");
    REQUIRE(hdr);
    REQUIRE(hdr->value == u8"quux");

    b = h.add_or_replace_field({"another-header", u8"xyzzy"});
    REQUIRE(b == false);

    hdr = h.get_field("another-header");
    REQUIRE(hdr);
    REQUIRE(hdr->value == u8"xyzzy");
}

TEST_CASE("ivy:http_version", "[ivy][http]") {
    auto v10 = ivy::http::http_version(1, 0);

    REQUIRE(v10.major() == 1);
    REQUIRE(v10.minor() == 0);
    REQUIRE(v10 == ivy::http::http_version(1, 0));
    REQUIRE(v10 != ivy::http::http_version(1, 1));

    auto v11 = ivy::http::http_version(1, 1);
    REQUIRE(v11.major() == 1);
    REQUIRE(v11.minor() == 1);
    REQUIRE(v11 == ivy::http::http_version(1, 1));
    REQUIRE(v11 != ivy::http::http_version(1, 0));

    REQUIRE(v10 != v11);
    REQUIRE(v10 < v11);
    REQUIRE(v11 > v10);
    REQUIRE(v10 <= v10);
}
