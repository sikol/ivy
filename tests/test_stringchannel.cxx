/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <span>

#include <catch2/catch.hpp>

#include <ivy/io/stringchannel.hxx>

TEST_CASE("ivy:stringchannel: input", "[ivy][io][stringchannel]")
{
    ivy::u8string s(u8"foobar");

    {
        char8_t buf[32];
        ivy::u8stringchannel chnl(s);

        auto r = chnl.read(buf);
        REQUIRE(r);

        auto str = ivy::u8string(&buf[0], &buf[0] + 6);
        REQUIRE(str == u8"foobar");
    }

    {
        char8_t buf[32];
        ivy::u8stringchannel chnl(s);

        auto r = chnl.read(std::span(buf).subspan(0, 3));
        REQUIRE(r);

        r = chnl.read(std::span(buf).subspan(3, 3));
        REQUIRE(r);

        auto str = ivy::u8string(&buf[0], &buf[0] + 6);
        REQUIRE(str == u8"foobar");
    }
}

TEST_CASE("ivy:stringchannel: output", "[ivy][io][stringchannel]")
{
    {
        ivy::u8stringchannel chan;

        chan.write(std::span(u8"test", 4));

        auto s = chan.str();
        REQUIRE(s == u8"test");
    }

    {
        ivy::u8stringchannel chan;

        chan.write(std::span(u8"foo", 3));
        chan.write(std::span(u8"quux", 4));

        auto s = chan.str();
        REQUIRE(s == u8"fooquux");
    }
}
