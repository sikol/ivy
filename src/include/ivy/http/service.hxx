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
#include <ivy/http/response.hxx>
#include <ivy/http/status.hxx>
#include <ivy/http/version.hxx>
#include <ivy/http/request.hxx>
#include <ivy/io/pmrchannel.hxx>
#include <ivy/net/uri.hxx>
#include <ivy/noncopyable.hxx>
#include <ivy/string.hxx>

namespace ivy::http {

    class http_error : public std::runtime_error {
    public:
        http_error(std::string const &err)
            : std::runtime_error(err)
        {
        }
    };

    class request_context {
    protected:
        request_context() noexcept = default;
        ~request_context() = default;
    };

    struct http_listener {
        net::uri prefix;
        std::function<http_response (request_context &, http_request const &)> handler;
    };

    class service : public ivy::noncopyable {
    protected:
        service() noexcept;

    public:
        service(service &&) noexcept = delete;
        virtual ~service();

        auto operator=(service &&) -> service & = delete;

        virtual auto add_listener(http_listener const &)
            -> expected<void, error> = 0;

        virtual auto run() -> expected<void, error> = 0;
    };

} // namespace ivy::http

#endif // IVY_HTTP_SERVICE_HXX_INCLUDED
