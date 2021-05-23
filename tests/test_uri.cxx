/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <catch2/catch.hpp>

#include <ivy/net/uri.hxx>

using ivy::net::parse_uri;
using ivy::net::uri_errors;
namespace uri_options = ivy::net::uri_options;

TEST_CASE("URI 1", "[uri]")
{
    auto u = parse_uri(u8"http://localhost");
    REQUIRE(u);
    REQUIRE(is_absolute(*u));
    REQUIRE(!is_protocol_relative(*u));
    REQUIRE(u->scheme == u8"http");
    REQUIRE(u->authority);
    REQUIRE(!u->authority->username);
    REQUIRE(!u->authority->password);
    REQUIRE(!u->authority->port);
    REQUIRE(u->authority->hostname == u8"localhost");
    REQUIRE(!u->path);
}

TEST_CASE("URI 2", "[uri]")
{
    auto u = parse_uri(u8"http://localhost:80");
    REQUIRE(u);
    REQUIRE(is_absolute(*u));
    REQUIRE(!is_protocol_relative(*u));
    REQUIRE(u->scheme == u8"http");
    REQUIRE(u->authority);
    REQUIRE(!u->authority->username);
    REQUIRE(!u->authority->password);
    REQUIRE(u->authority->port == 80);
    REQUIRE(u->authority->hostname == u8"localhost");
    REQUIRE(!u->path);
}

TEST_CASE("URI 3", "[uri]")
{
    auto u = parse_uri(u8"http://jdoe@localhost");
    REQUIRE(u);
    REQUIRE(is_absolute(*u));
    REQUIRE(!is_protocol_relative(*u));
    REQUIRE(u->scheme == u8"http");
    REQUIRE(u->authority);
    REQUIRE(u->authority->username == u8"jdoe");
    REQUIRE(!u->authority->password);
    REQUIRE(!u->authority->port);
    REQUIRE(u->authority->hostname == u8"localhost");
    REQUIRE(!u->path);
}

TEST_CASE("URI 4", "[uri]")
{
    auto u = parse_uri(u8"http://jdoe:hunter2@localhost");
    REQUIRE(u);
    REQUIRE(is_absolute(*u));
    REQUIRE(!is_protocol_relative(*u));
    REQUIRE(u->scheme == u8"http");
    REQUIRE(u->authority);
    REQUIRE(u->authority->username == u8"jdoe");
    REQUIRE(u->authority->password == u8"hunter2");
    REQUIRE(!u->authority->port);
    REQUIRE(u->authority->hostname == u8"localhost");
    REQUIRE(!u->path);
}

TEST_CASE("URI 5", "[uri]")
{
    auto u = parse_uri(u8"http://jdoe:hunter2@localhost:8080");
    REQUIRE(u);
    REQUIRE(is_absolute(*u));
    REQUIRE(!is_protocol_relative(*u));
    REQUIRE(u->scheme == u8"http");
    REQUIRE(u->authority);
    REQUIRE(u->authority->username == u8"jdoe");
    REQUIRE(u->authority->password == u8"hunter2");
    REQUIRE(u->authority->port == 8080);
    REQUIRE(u->authority->hostname == u8"localhost");
    REQUIRE(!u->path);
}

TEST_CASE("URI 6", "[uri]")
{
    auto u = parse_uri(u8"http://jdoe:hunter2@127.0.0.1:8080");
    REQUIRE(u);
    REQUIRE(is_absolute(*u));
    REQUIRE(!is_protocol_relative(*u));
    REQUIRE(u->scheme == u8"http");
    REQUIRE(u->authority);
    REQUIRE(u->authority->username == u8"jdoe");
    REQUIRE(u->authority->password == u8"hunter2");
    REQUIRE(u->authority->port == 8080);
    REQUIRE(u->authority->hostname == u8"127.0.0.1");
    REQUIRE(!u->path);
}

TEST_CASE("URI 7", "[uri]")
{
    auto u = parse_uri(u8"http://jdoe:hunter2@[2001:db8::1:2]:8080");
    REQUIRE(u);
    REQUIRE(is_absolute(*u));
    REQUIRE(!is_protocol_relative(*u));
    REQUIRE(u->scheme == u8"http");
    REQUIRE(u->authority);
    REQUIRE(u->authority->username == u8"jdoe");
    REQUIRE(u->authority->password == u8"hunter2");
    REQUIRE(u->authority->port == 8080);
    REQUIRE(u->authority->hostname == u8"2001:db8::1:2");
    REQUIRE(!u->path);
}

TEST_CASE("URI 8", "[uri]")
{
    auto u = parse_uri(u8"http://localhost/");
    REQUIRE(u);
    REQUIRE(is_absolute(*u));
    REQUIRE(!is_protocol_relative(*u));
    REQUIRE(u->scheme == u8"http");
    REQUIRE(u->authority);
    REQUIRE(!u->authority->username);
    REQUIRE(!u->authority->password);
    REQUIRE(!u->authority->port);
    REQUIRE(u->authority->hostname == u8"localhost");
    REQUIRE(u->path);
    REQUIRE(u->path->path == u8"/");
    REQUIRE(!u->path->query);
    REQUIRE(!u->path->fragment);
}

TEST_CASE("URI 9", "[uri]")
{
    auto u = parse_uri(u8"http://localhost/some/path");
    REQUIRE(u);
    REQUIRE(is_absolute(*u));
    REQUIRE(!is_protocol_relative(*u));
    REQUIRE(u->scheme == u8"http");
    REQUIRE(u->authority);
    REQUIRE(!u->authority->username);
    REQUIRE(!u->authority->password);
    REQUIRE(!u->authority->port);
    REQUIRE(u->authority->hostname == u8"localhost");
    REQUIRE(u->path);
    REQUIRE(u->path->path == u8"/some/path");
    REQUIRE(!u->path->query);
    REQUIRE(!u->path->fragment);
}

TEST_CASE("URI 10", "[uri]")
{
    auto u = parse_uri(u8"http://localhost/some/path?query=value&other=value");
    REQUIRE(u);
    REQUIRE(is_absolute(*u));
    REQUIRE(!is_protocol_relative(*u));
    REQUIRE(u->scheme == u8"http");
    REQUIRE(u->authority);
    REQUIRE(!u->authority->username);
    REQUIRE(!u->authority->password);
    REQUIRE(!u->authority->port);
    REQUIRE(u->authority->hostname == u8"localhost");
    REQUIRE(u->path);
    REQUIRE(u->path->path == u8"/some/path");
    REQUIRE(u->path->query);
    REQUIRE(u->path->query == u8"query=value&other=value");
}

TEST_CASE("URI 11", "[uri]")
{
    auto u = parse_uri(u8"http://localhost/some/"
                       "path?query=value&other=value#fragment.is.here");
    REQUIRE(u);
    REQUIRE(is_absolute(*u));
    REQUIRE(!is_protocol_relative(*u));
    REQUIRE(u->scheme == u8"http");
    REQUIRE(u->authority);
    REQUIRE(!u->authority->username);
    REQUIRE(!u->authority->password);
    REQUIRE(!u->authority->port);
    REQUIRE(u->authority->hostname == u8"localhost");
    REQUIRE(u->path);
    REQUIRE(u->path->path == u8"/some/path");
    REQUIRE(u->path->query);
    REQUIRE(u->path->query == u8"query=value&other=value");
    REQUIRE(u->path->fragment);
    REQUIRE(u->path->fragment == u8"fragment.is.here");
}

TEST_CASE("URI 12", "[uri]")
{
    auto u = parse_uri(u8"http://localhost/some/"
                       "path#fragment.is.here");
    REQUIRE(u);
    REQUIRE(is_absolute(*u));
    REQUIRE(!is_protocol_relative(*u));
    REQUIRE(u->scheme == u8"http");
    REQUIRE(u->authority);
    REQUIRE(!u->authority->username);
    REQUIRE(!u->authority->password);
    REQUIRE(!u->authority->port);
    REQUIRE(u->authority->hostname == u8"localhost");
    REQUIRE(u->path);
    REQUIRE(u->path->path == u8"/some/path");
    REQUIRE(!u->path->query);
    REQUIRE(u->path->fragment);
    REQUIRE(u->path->fragment == u8"fragment.is.here");
}

TEST_CASE("URI 13", "[uri]")
{
    auto u = parse_uri(u8"http://jdoe:hunter2@[2001:db8::1:2]:8080"
                       "/some/path?query=value&other=value#fragment.is.here");
    REQUIRE(u);
    REQUIRE(is_absolute(*u));
    REQUIRE(!is_protocol_relative(*u));
    REQUIRE(u->scheme == u8"http");
    REQUIRE(u->authority);
    REQUIRE(u->authority->username == u8"jdoe");
    REQUIRE(u->authority->password == u8"hunter2");
    REQUIRE(u->authority->port == 8080);
    REQUIRE(u->authority->hostname == u8"2001:db8::1:2");
    REQUIRE(u->path);
    REQUIRE(u->path->path == u8"/some/path");
    REQUIRE(u->path->query);
    REQUIRE(u->path->query == u8"query=value&other=value");
    REQUIRE(u->path->fragment);
    REQUIRE(u->path->fragment == u8"fragment.is.here");
}

TEST_CASE("URI with invalid high-bit characters", "[uri]")
{
    auto u = parse_uri(u8"http://localhost/some/\xABpath");
    REQUIRE(!u);
    u = parse_uri(u8"http://local\xABhost/some/path");
    REQUIRE(!u);
}

TEST_CASE("URI with URL-encoded ASCII path", "[uri]")
{
    auto u = parse_uri(u8"http://localhost/%73%6F%6d%65/%70%61%74%68");
    REQUIRE(u);
    REQUIRE(u->path);
    REQUIRE(u->path->path == u8"/some/path");
}

TEST_CASE("URI with URL-encoded UTF-8 path", "[uri]")
{
    auto u = parse_uri(u8"http://localhost/s%C3%B3m%C3%A9/p%C3%A1th" //
                       "?qparam=s%C3%B3m%C3%A9d%C3%A1t%C3%A1"        //
                       "#%66%72%C3%A1%67%6D%C3%A9%6E%74");
    REQUIRE(u);
    REQUIRE(u->path);
    REQUIRE(u->path->path ==
            reinterpret_cast<char8_t const *>(
                "\x2F\x73\xC3\xB3\x6D\xC3\xA9\x2F\x70\xC3\xA1\x74\x68"));
    REQUIRE(u->path->query);
    REQUIRE(*u->path->query == u8"qparam=sómédátá");
    REQUIRE(u->path->fragment);
    REQUIRE(*u->path->fragment == u8"frágmént");
}

TEST_CASE("URI with raw UTF-8 path", "[uri]")
{
    auto u = parse_uri(
        u8"http://localhost/\x73\xC3\xB3\x6D\xC3\xA9/\x70\xC3\xA1\x74\x68",
        uri_options::skip_path_decode);
    REQUIRE(u);
    REQUIRE(u->path);
    REQUIRE(u->path->path ==
            u8"/\x73\xC3\xB3\x6D\xC3\xA9/\x70\xC3\xA1\x74\x68");
}

TEST_CASE("relative URI", "[uri]")
{
    auto u = parse_uri(u8"/some/relative/path", uri_options::allow_relative);
    REQUIRE(u);
    REQUIRE(is_relative(*u));
    REQUIRE(!is_protocol_relative(*u));
    REQUIRE(!u->scheme);
    REQUIRE(!u->authority);
    REQUIRE(u->path);
    REQUIRE(u->path->path == u8"/some/relative/path");
    REQUIRE(!u->path->query);
    REQUIRE(!u->path->fragment);
}

TEST_CASE("relative URI without flag", "[uri]")
{
    auto u = parse_uri(u8"/some/relative/path");
    REQUIRE(!u);
    REQUIRE(u.error() == ivy::net::make_uri_error(uri_errors::relative));
}

TEST_CASE("URI 15", "[uri]")
{
    auto u = parse_uri(u8"https:/authority/relative");
    REQUIRE(!u);
}

TEST_CASE("protocol-relative URI", "[uri]")
{
    auto u = parse_uri(u8"//proto.col/relative",
                       uri_options::allow_protocol_relative);
    REQUIRE(u);
    REQUIRE(!u->scheme);
    REQUIRE(u->authority);
    REQUIRE(u->authority->hostname == u8"proto.col");
    REQUIRE(u->path);
    REQUIRE(u->path->path == u8"/relative");
    REQUIRE(!u->path->query);
    REQUIRE(!u->path->fragment);
}

TEST_CASE("protocol-relative URI without flag", "[uri]")
{
    auto u = parse_uri(u8"//proto.col/relative");
    REQUIRE(!u);
    REQUIRE(u.error() ==
            ivy::net::make_uri_error(uri_errors::protocol_relative));
}

TEST_CASE("str(uri) 1", "[uri]")
{
    auto text = u8"http://jdoe:hunter2@[2001:db8::1:2]:8080"
                "/some/path?query=value&other=value#fragment.is.here";
    auto uri = parse_uri(text);
    REQUIRE(uri);
    REQUIRE(str(*uri) == text);
}

TEST_CASE("uri invalid port", "[uri]")
{
    auto text = u8"http://localhost:808080";
    auto uri = parse_uri(text);
    REQUIRE(!uri);
    REQUIRE(uri.error() == ivy::net::make_uri_error(uri_errors::invalid_port));
}

TEST_CASE("empty uri", "[uri]")
{
    auto text = u8"";
    auto uri = parse_uri(text);
    REQUIRE(!uri);
    REQUIRE(uri.error() == ivy::net::make_uri_error(uri_errors::no_data));
}

TEST_CASE("scheme-only uri", "[uri]")
{
    auto uri = parse_uri(u8"http:");
    REQUIRE(!uri);
    uri = parse_uri(u8"http://");
    REQUIRE(!uri);
}

auto make_uri_copy() -> ivy::net::uri
{
    auto uri = parse_uri(u8"http://localhost/some/path");
    ivy::net::uri uri2(*uri);
    return uri2;
}

TEST_CASE("uri is copyable", "[uri]")
{
    auto uri = make_uri_copy();
    REQUIRE(uri.path->path == u8"/some/path");
}

auto make_uri_move() -> ivy::net::uri
{
    auto uri = parse_uri(u8"http://localhost/some/path");
    return std::move(*uri);
}

TEST_CASE("uri is movable", "[uri]")
{
    auto uri = make_uri_move();
    REQUIRE(uri.path->path == u8"/some/path");
}
