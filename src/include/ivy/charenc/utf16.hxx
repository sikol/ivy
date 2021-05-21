/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_UTF16_HXX_INCLUDED
#define IVY_CHARENC_UTF16_HXX_INCLUDED

#include <bit>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <string>
#include <type_traits>

#include <ivy/charenc/error.hxx>
#include <ivy/charenc/utf32.hxx>
#include <ivy/expected.hxx>
#include <ivy/iterator.hxx>
#include <ivy/trace.hxx>

namespace ivy {

    struct utf16_encoding {
        using char_type = char16_t;

        static auto length(char_type const *s) noexcept -> std::size_t
        {
            return std::char_traits<char_type>::length(s);
        }
    };

    template <>
    class charconv<utf16_encoding, utf32_encoding> {
        char32_t c32 = 0;

    public:
        charconv(charconv_options = {}) noexcept {}

        template <std::ranges::input_range input_range,
                  std::output_iterator<char32_t> output_iterator>
        auto convert(input_range &&r, output_iterator out) -> void
        {
            for (auto c : r) {
                if (c32) {
                    // Last character was a high surrogate, so expect the low.

                    if (c < 0xDC00u || c > 0xDFFFu)
                        throw encoding_error("invalid encoding");

                    c32 |= static_cast<char32_t>(c & ((1u << 10) - 1));

                    // Surrogate pairs must be in this range.
                    if (c32 < 0x10000u || c32 > 0x10FFFFu)
                        throw encoding_error("invalid encoding");

                    *out++ = c32;
                    c32 = 0;
                    continue;
                }

                if (c < 0xD800u || c > 0xDFFFu) {
                    *out++ = static_cast<char32_t>(c);
                    continue;
                }

                if (c < 0xD800u || c > 0xDBFFu)
                    // Invalid high surrogate.
                    throw encoding_error("invalid encoding");

                // high surrogate.
                c32 = 0x10000 +
                      ((static_cast<char32_t>(c) & ((1u << 10) - 1)) << 10);
            }
        }

        template <std::output_iterator<char32_t> output_iterator>
        auto flush(output_iterator) -> void
        {
            if (c32)
                throw encoding_error("invalid encoding");
        }
    };

    template <>
    class charconv<utf32_encoding, utf16_encoding> {
    public:
        charconv(charconv_options = {}) noexcept {}

        template <std::ranges::input_range input_range,
                  std::output_iterator<char8_t> output_iterator>
        auto convert(input_range &&r, output_iterator out) -> void
        {
            for (auto c : r) {
                if (c >= 0xD800u && c <= 0xDFFFu) {
                    // Cannot encode surrogates.
                    throw encoding_error("invalid encoding");
                }

                if (c < 0x10000u) {
                    // No surrogate required, encode directly.
                    *out++ = static_cast<char16_t>(c);
                    continue;
                }

                // Need a surrogate.
                c -= 0x10000;
                static constexpr char32_t tenbits = (1u << 10) - 1;
                *out++ = static_cast<char16_t>(0xD800 + ((c >> 10) & tenbits));
                *out++ = static_cast<char16_t>(0xDC00 + (c & tenbits));
            }
        }

        template <std::output_iterator<char32_t> output_iterator>
        auto flush(output_iterator) -> void
        {
        }
    };

} // namespace ivy

#endif // IVY_CHARENC_UTF16_HXX_INCLUDED
