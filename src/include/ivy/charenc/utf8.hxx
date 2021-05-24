/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_UTF8_HXX_INCLUDED
#define IVY_CHARENC_UTF8_HXX_INCLUDED

#include <bit>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <string>
#include <type_traits>

#include <ivy/error.hxx>
#include <ivy/charenc/utf32.hxx>

#include <ivy/buffer/fixed_buffer.hxx>
#include <ivy/expected.hxx>
#include <ivy/iterator.hxx>
#include <ivy/trace.hxx>

namespace ivy {

    namespace detail {

        constexpr char8_t utf8_continuation_mask = char8_t(0b10000000u);

        inline auto bit_slice(char32_t n, unsigned start, unsigned len)
            -> char32_t
        {
            return (n >> start) & ((1u << len) - 1);
        }

    } // namespace detail

    struct utf8_encoding {
        using char_type = char8_t;

        static auto length(char_type const *s) -> std::size_t
        {
            return std::char_traits<char_type>::length(s);
        }
    };

    template <>
    class charconv<utf8_encoding, utf32_encoding> {
        char32_t c32 = 0;
        int char_len = 0;
        int seen = 0;

    public:
        charconv(charconv_options = {}) noexcept {}

        template <std::ranges::input_range input_range,
                  std::output_iterator<char32_t> output_iterator>
        auto convert(input_range &&r, output_iterator out) -> void
        {
            static constexpr char8_t i2b = 0b11000000u;
            static constexpr char8_t i3b = 0b11100000u;
            static constexpr char8_t i4b = 0b11110000u;
            static constexpr char8_t icont = 0b10000000;
            static constexpr char8_t cmask = 0b00111111;

            static constexpr char32_t max_for_len[] = {
                0,
                0x7Fu,
                0x7FFu,
                0xFFFFu,
                0x10FFFFu,
            };

            for (auto c : r) {
                if (char_len == 0) {
                    // Start of a new character.

                    if ((c & i4b) == i4b) {
                        char_len = 4;
                        seen = 1;
                        c32 = static_cast<char32_t>(c & 0b0000'0111u) << 18u;
                    } else if ((c & i3b) == i3b) {
                        char_len = 3;
                        seen = 1;
                        c32 = static_cast<char32_t>(c & 0b0000'1111u) << 12u;
                    } else if ((c & i2b) == i2b) {
                        char_len = 2;
                        seen = 1;
                        c32 = static_cast<char32_t>(c & 0b0001'1111u) << 6u;
                    } else if ((c & icont) == icont) {
                        throw encoding_error("invalid encoding");
                    } else {
                        *out++ = static_cast<char32_t>(c);
                    }

                    continue;
                }

                // Inside a character, should have a continuation.
                if (((c & 0b1100'0000u) >> 6) != 0b10u)
                    throw encoding_error("invalid encoding");

                unsigned shift = (char_len - seen - 1) * 6;
                ++seen;
                auto cval = static_cast<char32_t>(c & cmask) << shift;
                c32 |= cval;

                if (seen == char_len) {
                    // At end of the character
                    if (c32 > max_for_len[char_len] ||
                        c32 <= max_for_len[char_len - 1]) {
                        throw encoding_error("invalid encoding");
                    }

                    // Invalid surrogate characters
                    if (c32 >= 0xD800u && c32 <= 0xDFFFu)
                        throw encoding_error("invalid encoding");

                    seen = 0;
                    char_len = 0;
                    *out++ = c32;
                }
            }
        }

        template <std::output_iterator<char32_t> output_iterator>
        auto flush(output_iterator) -> void
        {
            if (seen)
                throw encoding_error("invalid encoding");
        }
    };

    template <>
    class charconv<utf32_encoding, utf8_encoding> {
    public:
        charconv(charconv_options = {}) noexcept {}

        template <std::ranges::input_range input_range,
                  std::output_iterator<char8_t> output_iterator>
        auto convert(input_range &&r, output_iterator out) -> void
        {
            static constexpr char32_t max1b = 0x7Fu;
            static constexpr char32_t max2b = 0x7FFu;
            static constexpr char32_t max3b = 0xFFFFu;
            static constexpr char32_t max4b = 0x10FFFFu;

            static constexpr char8_t i2b = 0b11000000u;
            static constexpr char8_t i3b = 0b11100000u;
            static constexpr char8_t i4b = 0b11110000u;
            static constexpr char8_t icont = 0b10000000;

            for (auto c : r) {
                if (c <= max1b) {
                    *out++ = static_cast<utf8_encoding::char_type>(c);

                } else if (c <= max2b) {
                    *out++ = static_cast<utf8_encoding::char_type>(
                        i2b | detail::bit_slice(c, 6, 5));
                    *out++ = static_cast<utf8_encoding::char_type>(
                        icont | detail::bit_slice(c, 0, 6));

                } else if (c <= max3b) {
                    *out++ = static_cast<utf8_encoding::char_type>(
                        i3b | detail::bit_slice(c, 12, 4));
                    *out++ = static_cast<utf8_encoding::char_type>(
                        icont | detail::bit_slice(c, 6, 6));
                    *out++ = static_cast<utf8_encoding::char_type>(
                        icont | detail::bit_slice(c, 0, 6));

                } else if (c <= max4b) {
                    *out++ = static_cast<utf8_encoding::char_type>(
                        i4b | detail::bit_slice(c, 18, 3));
                    *out++ = static_cast<utf8_encoding::char_type>(
                        icont | detail::bit_slice(c, 12, 6));
                    *out++ = static_cast<utf8_encoding::char_type>(
                        icont | detail::bit_slice(c, 6, 6));
                    *out++ = static_cast<utf8_encoding::char_type>(
                        icont | detail::bit_slice(c, 0, 6));

                } else
                    throw encoding_error("invalid encoding");
            }
        }

        template <std::output_iterator<char32_t> output_iterator>
        auto flush(output_iterator) -> void
        {
        }
    };

} // namespace ivy

#endif // IVY_CHARENC_UTF8_HXX_INCLUDED
