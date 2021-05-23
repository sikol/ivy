/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <catch2/catch.hpp>

#include <ivy/io/textchannel.hxx>
#include <ivy/io/stringchannel.hxx>

TEST_CASE("ivy:textchannel: output", "[ivy][io][textchannel]") {
    {
        ivy::u8stringchannel chan;
        auto strm = ivy::make_textchannel(chan);
        strm << 42;
        REQUIRE(chan.str() == u8"42");
    }

    {
        ivy::u8stringchannel chan;
        auto strm = ivy::make_textchannel(chan);
        strm << -1024;
        REQUIRE(chan.str() == u8"-1024");
    }

    {
        ivy::u8stringchannel chan;
        auto strm = ivy::make_textchannel(chan);
        strm << u8'x';
        REQUIRE(chan.str() == u8"x");
    }

    {
        ivy::u8stringchannel chan;
        auto strm = ivy::make_textchannel(chan);
        strm << u8"foo bar";
        REQUIRE(chan.str() == u8"foo bar");
    }

    {
        ivy::u8stringchannel chan;
        auto strm = ivy::make_textchannel(chan);
        strm << 1 << u8" + " << 2 << u8" = " << (1 + 2);
        REQUIRE(chan.str() == u8"1 + 2 = 3");
    }
}
