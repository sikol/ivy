/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_HTTP_SERVICE_HXX_INCLUDED
#define IVY_HTTP_SERVICE_HXX_INCLUDED

#include <stdexcept>
#include <string>
#include <unordered_map>

#include <ivy/noncopyable.hxx>
#include <ivy/string.hxx>

namespace ivy::http {

    class http_error : public std::runtime_error {
    public:
        http_error(std::string const &err) : std::runtime_error(err) {}
    };

    struct header_field {
        astring name;
        u8string value;
    };

    //class http_header {
    //    std::unordered_map<astring, header_field> _fields;

    //public:
    //    http_header();
    //};

    class service : public ivy::noncopyable {
    protected:
        service() = default;

    public:
        service(service &&) noexcept = delete;
        auto operator=(service &&) -> service & = delete;

        virtual ~service() = default;
    };

} // namespace ivy::http

#endif // IVY_HTTP_SERVICE_HXX_INCLUDED
