/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_HTTP_REQUEST_HXX_INCLUDED
#define IVY_HTTP_REQUEST_HXX_INCLUDED

#include <ivy/http/header.hxx>
#include <ivy/http/method.hxx>
#include <ivy/http/version.hxx>
#include <ivy/net/uri.hxx>

namespace ivy::http {

    struct http_request {
        net::uri uri;
        net::uri listener_address;
        net::uri request_path;

        http_version version;
        http_method method;
        http_header header;
    };

    // True if this request will accept a chunked response.
    auto can_chunk_response(http_request const &) noexcept -> bool;

} // namespace ivy::http

#endif // IVY_HTTP_REQUEST_HXX_INCLUDED
