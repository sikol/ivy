/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_ALGORITHM_BASE64_HXX_INCLUDED
#define IVY_ALGORITHM_BASE64_HXX_INCLUDED

#include <algorithm>
#include <iterator>
#include <ranges>
#include <system_error>

#include <ivy/expected.hxx>

namespace ivy {

    namespace detail {

        template <typename T>
        auto unb64(T c) -> int
        {
            if (c >= 'A' && c <= 'Z')
                return c - 'A';
            if (c >= 'a' && c <= 'z')
                return (c - 'a') + 26;
            if (c >= '0' && c <= '9')
                return (c - '0') + 52;
            if (c == '+')
                return 53;
            if (c == '/')
                return 54;
            return -1;
        }

        auto unb64(std::byte c) -> int
        {
            return unb64(static_cast<std::uint8_t>(c));
        }

    } // namespace detail

    template <typename InputIterator, typename OutputIterator>
    auto base64_encode(InputIterator begin,
                       InputIterator end,
                       OutputIterator out) -> void
    {
        static char b64table[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        auto left = std::distance(begin, end);

        while (left > 0) {
            unsigned char d[4] = {};
            auto todo = left > 3 ? 3 : left;

            switch (todo) {
            case 3:
                d[3] |= static_cast<std::uint8_t>(*std::next(begin, 2)) & 0x3F;
                d[2] |=
                    (static_cast<std::uint8_t>(*std::next(begin, 2)) & 0xC0) >>
                    6;
            case 2:
                d[2] |= (static_cast<std::uint8_t>(*std::next(begin)) & 0x0F)
                        << 2;
                d[1] |=
                    (static_cast<std::uint8_t>(*std::next(begin)) & 0xF0) >> 4;
            case 1:
                d[0] = (static_cast<std::uint8_t>(*begin) & 0xFC) >> 2;
                d[1] |= (static_cast<std::uint8_t>(*begin) & 0x03) << 4;
            }

            *out++ = b64table[d[0]];
            *out++ = b64table[d[1]];

            if (todo >= 3) {
                *out++ = b64table[d[2]];
                *out++ = b64table[d[3]];
            } else if (todo == 2) {
                *out++ = b64table[d[2]];
                *out++ = '=';
            } else if (todo == 1) {
                *out++ = '=';
                *out++ = '=';
            }

            left -= todo;
            std::advance(begin, todo);
        }
    }

    template <std::ranges::contiguous_range Range, typename OutputIterator>
    auto base64_encode(Range const &r, OutputIterator it) -> void
    {
        auto bytes = as_bytes(std::span(r));
        base64_encode(bytes.begin(), bytes.end(), it);
    }

    template <std::ranges::contiguous_range Range>
    auto base64_encode_to_string(Range const &r) -> std::string
    {
        std::string ret;

        auto bytes = as_bytes(std::span(r));
        base64_encode(bytes.begin(), bytes.end(), std::back_inserter(ret));
        return ret;
    }

    template <typename InputIterator, typename OutputIterator>
    auto
    base64_decode(InputIterator begin, InputIterator end, OutputIterator out)
        -> expected<std::size_t, std::error_code>
    {
        std::size_t nbytes = 0;

        while (begin != end) {
            unsigned char d[3] = {};

            auto b0 = *begin++;
            if (begin == end)
                return make_unexpected(
                    std::make_error_code(std::errc::invalid_argument));

            auto b1 = *begin++;
            if (begin == end)
                return make_unexpected(
                std::make_error_code(std::errc::invalid_argument));

            auto b2 = *begin++;
            if (begin == end)
                return make_unexpected(
                std::make_error_code(std::errc::invalid_argument));

            auto b3 = *begin++;

            int nout = 0;

            if (b3 == '=' && b2 == '=')
                nout = 1;
            else if (b3 == '=')
                nout = 2;
            else
                nout = 3;

            if (nout < 3 && begin != end)
                return make_unexpected(
                    std::make_error_code(std::errc::invalid_argument));

            switch (nout) {
            case 3: {
                auto v = detail::unb64(b3);
                if (v == -1)
                    return make_unexpected(
                        std::make_error_code(std::errc::invalid_argument));
                d[2] |= v & 0x3F;
            }

            case 2: {
                auto v = detail::unb64(b2);
                if (v == -1)
                    return make_unexpected(
                        std::make_error_code(std::errc::invalid_argument));

                d[2] |= (v & 0x03) << 6;
                d[1] |= (v & 0x3C) >> 2;
            }

            case 1: {
                auto v = detail::unb64(b1);
                if (v == -1)
                    return make_unexpected(
                        std::make_error_code(std::errc::invalid_argument));

                d[1] |= (v & 0xF) << 4;
                d[0] |= (v & 0x30) >> 4;

                v = detail::unb64(b0);
                if (v == -1)
                    return make_unexpected(
                        std::make_error_code(std::errc::invalid_argument));

                d[0] |= v << 2;
                break;
            }

            default:
                IVY_UNEXPECTED("base64_decode: invalid nout");
            }

            out = std::copy(d, d + nout, out);
            nbytes += nout;
        }

        return nbytes;
    }

    template <std::ranges::contiguous_range Range, typename OutputIterator>
    auto base64_decode(Range const &r, OutputIterator it)
        -> expected<std::size_t, std::error_code>
    {
        return base64_decode(r.begin(), r.end(), it);
    }

    template <std::ranges::contiguous_range Range>
    auto base64_decode_to_string(Range const &r)
        -> expected<std::string, std::error_code>
    {
        std::string ret;

        auto res = base64_decode(r.begin(), r.end(), std::back_inserter(ret));
        if (res)
            return ret;
        return make_unexpected(res.error());
    }

} // namespace ivy

#endif // IVY_ALGORITHM_BASE64_HXX_INCLUDED
