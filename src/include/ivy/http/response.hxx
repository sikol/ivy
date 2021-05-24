/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_HTTP_RESPONSE_HXX_INCLUDED
#define IVY_HTTP_RESPONSE_HXX_INCLUDED

#include <memory>

#include <ivy/http/header.hxx>
#include <ivy/http/status.hxx>
#include <ivy/io/pmrchannel.hxx>
#include <ivy/io/transcodechannel.hxx>
#include <ivy/io/stringchannel.hxx>

namespace ivy::http {

    namespace detail {

        template <sequential_input_channel channel_type>
            requires std::same_as<std::byte, channel_value_t<channel_type>>
        auto make_response_body(channel_type &&channel)
            -> std::unique_ptr<pmrichannel<std::byte>>
        {
            return make_pmrichannel(std::move(channel));
        }

        template <sequential_input_channel channel_type>
            requires(!std::same_as<std::byte, channel_value_t<channel_type>>)
        auto make_response_body(channel_type &&channel)
            -> std::unique_ptr<pmrichannel<std::byte>>
        {
            return make_pmrichannel(
                make_itranscodechannel<typename channel_type::encoding_type,
                                       std::byte>(std::move(channel)));
        }

        template<character_encoding encoding, typename allocator>
        auto make_response_body(basic_string<encoding, allocator> const &s)
            -> std::unique_ptr<pmrichannel<std::byte>>
        {
            return make_response_body(
                basic_stringchannel<encoding, allocator>(s));
        }

        inline auto make_response_body(char8_t const *s)
            -> std::unique_ptr<pmrichannel<std::byte>>
        {
            return make_response_body(u8string(s));
        }

    } // namespace detail

    struct http_response {
        http_status status_code;
        http_header header;
        io_size_t content_length = unlimited;
        std::unique_ptr<pmrichannel<std::byte>> body;
    };

    inline auto make_http_response(int status_code)
    {
        http_response ret{.status_code = static_cast<http_status>(status_code)};
        return ret;
    }

    inline auto make_http_response(http_status status_code)
    {
        http_response ret{.status_code = status_code};
        return ret;
    }

    template <typename body_type>
    auto make_http_response(int status_code, body_type &&body)
    {
        auto ret = make_http_response(status_code);
        ret.body = detail::make_response_body(std::forward<body_type>(body));

        return ret;
    }

} // namespace ivy::http

#endif // IVY_HTTP_RESPONSE_HXX_INCLUDED
