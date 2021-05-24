/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <catch2/catch.hpp>

#include <ivy/csv.hxx>
#include <ivy/io/stringchannel.hxx>

TEST_CASE("ivy:csv: basic reading", "[ivy][csv]") {
    ivy::u8stringchannel chan(u8"foo,bar,baz\nquux,,xyzzy\n");

    ivy::csv_reader csv(chan);

    std::vector<ivy::u8csv_row> rows;
    while (auto r = csv.read())
        rows.push_back(*r);

    REQUIRE(rows.size() == 2);

    auto &r1 = rows[0];
    REQUIRE(r1.fields[0] == u8"foo");
    REQUIRE(r1.fields[1] == u8"bar");
    REQUIRE(r1.fields[2] == u8"baz");

    auto &r2 = rows[1];
    REQUIRE(r2.fields[0] == u8"quux");
    REQUIRE(r2.fields[1] == u8"");
    REQUIRE(r2.fields[2] == u8"xyzzy");
}

TEST_CASE("ivy:csv: empty file", "[ivy][csv]")
{
    ivy::u8stringchannel chan(u8"");

    ivy::csv_reader csv(chan);

    std::vector<ivy::u8csv_row> rows;
    while (auto r = csv.read())
        rows.push_back(*r);

    REQUIRE(rows.size() == 0);
}
