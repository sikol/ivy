/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_HTTP_SERVICE_HXX_INCLUDED
#define IVY_HTTP_SERVICE_HXX_INCLUDED

#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <ivy/error.hxx>
#include <ivy/http/header.hxx>
#include <ivy/http/method.hxx>
#include <ivy/http/version.hxx>
#include <ivy/net/uri.hxx>
#include <ivy/noncopyable.hxx>
#include <ivy/string.hxx>

namespace ivy::http {

    class http_error : public std::runtime_error {
    public:
        http_error(std::string const &err) : std::runtime_error(err) {}
    };

    struct http_request {
        net::uri uri;
        net::uri listener_address;
        net::uri request_path;

        http_version version;
        http_method method;
        http_header header;
    };

    struct http_listener {
        net::uri prefix;
        std::function<void(http_request const &)> handler;
    };

    class service : public ivy::noncopyable {
    protected:
        service() noexcept;

    public:
        service(service &&) noexcept = delete;
        auto operator=(service &&) -> service & = delete;

        virtual ~service();

        virtual auto add_listener(http_listener const &)
            -> expected<void, error> = 0;
    };

} // namespace ivy::http

#endif // IVY_HTTP_SERVICE_HXX_INCLUDED
