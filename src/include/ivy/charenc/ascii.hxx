/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_ASCII_HXX_INCLUDED
#define IVY_CHARENC_ASCII_HXX_INCLUDED

#include <bit>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <string>
#include <type_traits>

#include <ivy/error.hxx>
#include <ivy/charenc/utf32.hxx>

namespace ivy {

    struct ascii_encoding {
        using char_type = char;

        static auto length(char_type const *s) -> std::size_t
        {
            return std::char_traits<char_type>::length(s);
        }
    };

    template <>
    class charconv<ascii_encoding, utf32_encoding> {
    public:
        charconv(charconv_options = {}) noexcept {}

        template <std::ranges::input_range input_range,
                  std::output_iterator<char32_t> output_iterator>
        auto convert(input_range &&r, output_iterator out) -> void
        {
            for (auto c : r) {
                if (c < 0 || c > 0x7F)
                    throw encoding_error("invalid encoding");
                *out++ = static_cast<char32_t>(c);
            }
        }

        template <std::output_iterator<char32_t> output_iterator>
        auto flush(output_iterator) -> void
        {
        }
    };

    template <>
    class charconv<utf32_encoding, ascii_encoding> {
    public:
        charconv(charconv_options = {}) noexcept {}

        template <std::ranges::input_range input_range,
                  std::output_iterator<char> output_iterator>
        auto convert(input_range &&r, output_iterator out) -> void
        {
            for (auto c : r) {
                if (c > 0x7F)
                    throw encoding_error("invalid encoding");

                *out++ = static_cast<char>(c);
            }
        }

        template <std::output_iterator<char> output_iterator>
        auto flush(output_iterator) -> void
        {
        }
    };

} // namespace ivy

#endif // IVY_CHARENC_ASCII_HXX_INCLUDED
