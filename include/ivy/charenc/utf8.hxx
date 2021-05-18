/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_UTF8_HXX_INCLUDED
#define IVY_CHARENC_UTF8_HXX_INCLUDED

#include <concepts>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <string>
#include <type_traits>
#include <bit>

#include <ivy/charenc/error.hxx>
#include <ivy/expected.hxx>
#include <ivy/iterator.hxx>
#include <ivy/trace.hxx>

namespace ivy {

    namespace detail {

        constexpr char8_t utf8_continuation_mask = char8_t(0b10000000u);

        inline auto bit_slice(char32_t n, unsigned start, unsigned len) -> char32_t
        {
            return (n >> start) & ((1u << len) - 1);
        }

        template <std::output_iterator<char32_t> OutputIterator>
        struct utf8_decoder {
            OutputIterator out;
            char32_t c32 = 0;
            unsigned char_len = 0;
            unsigned seen = 0;

            static constexpr char8_t i2b = 0b11000000u;
            static constexpr char8_t i3b = 0b11100000u;
            static constexpr char8_t i4b = 0b11110000u;
            static constexpr char8_t icont = 0b10000000;
            static constexpr char8_t cmask = 0b00111111;

            static constexpr std::array<char32_t, 5> max_for_len = {
                0,
                0x7Fu,
                0x7FFu,
                0xFFFFu,
                0x10FFFFu,
            };

            utf8_decoder(OutputIterator out_) : out(out_) {}

            auto put(char8_t v) -> expected<void, std::error_code>
            {
                IVY_TRACE("utf8_decoder::put: v={:08b}, c32={:032b}, "
                          "char_len={}, seen={}",
                          static_cast<unsigned>(v),
                          static_cast<unsigned>(c32),
                          char_len,
                          seen);

                if (char_len == 0) {
                    // Start of a new character.

                    if ((v & i4b) == i4b) {
                        char_len = 4;
                        seen = 1;
                        c32 |= static_cast<char32_t>(v & 0b0000'0111u) << 18u;
                        return {};
                    }

                    if ((v & i3b) == i3b) {
                        char_len = 3;
                        seen = 1;
                        c32 |= static_cast<char32_t>(v & 0b0000'1111u) << 12u;
                        return {};
                    }

                    if ((v & i2b) == i2b) {
                        char_len = 2;
                        seen = 1;
                        c32 |= static_cast<char32_t>(v & 0b0001'1111u) << 6u;
                        return {};
                    }

                    if ((v & icont) == icont)
                        return make_unexpected(
                            make_error_code(charenc_errc::invalid_encoding));

                    *out++ = char32_t(v);
                    return {};
                }

                // Inside a character, should have a continuation.
                if (((v & 0b1100'0000u) >> 6) != 0b10u) {
                    IVY_TRACE("                 : fail: expected continuation");

                    return make_unexpected(
                        make_error_code(charenc_errc::invalid_encoding));
                }

                unsigned shift = (char_len - seen - 1) * 6;
                ++seen;
                IVY_TRACE("                 : shift={}", shift);
                c32 |= static_cast<char32_t>(v & cmask) << shift;

                if (seen == char_len) {
                    IVY_TRACE("                 : check for overlong encoding: c32={:x}, max_for_len={}",
                              static_cast<unsigned>(c32),
                              static_cast<unsigned>(max_for_len[char_len - 1]));

                    // At end of the character
                    if (c32 > max_for_len[char_len] || c32 <= max_for_len[char_len - 1]) {
                        IVY_TRACE(
                            "                 : fail: c32={:x}, max_for_len={}",
                            static_cast<unsigned>(c32),
                            static_cast<unsigned>(max_for_len[char_len - 1]));

                        return make_unexpected(
                            make_error_code(charenc_errc::invalid_encoding));
                    }

                    // Invalid surrogate characters
                    if (c32 >= 0xD800u && c32 <= 0xDFFFu)
                        return make_unexpected(
                            make_error_code(charenc_errc::invalid_encoding));

                    *out++ = c32;
                    seen = 0;
                    char_len = 0;
                    c32 = 0;
                }

                return {};
            }

            auto ok() const -> bool
            {
                return seen == 0 && char_len == 0;
            }
        };

    } // namespace detail

    struct utf8 {
        using char_type = char8_t;

        static auto length(char_type const *s) -> std::size_t
        {
            return std::char_traits<char_type>::length(s);
        }

        // clang-format off
        template<std::ranges::range Range, std::output_iterator<char32_t> OutputIterator>
        static auto to_char32(Range &&r, OutputIterator out)
            -> expected<void, std::error_code>
            requires (std::same_as<char_type, std::ranges::range_value_t<Range>>)
        // clang-format on
        {
            IVY_TRACE("utf8::to_char32: start");

            detail::utf8_decoder decoder(out);
            for (char_type c : r) {
                auto r = decoder.put(c);

                if (!r)
                    return make_unexpected(r.error());
            }

            if (!decoder.ok())
                return make_unexpected(
                    make_error_code(charenc_errc::invalid_encoding));

            return {};
        }

        // clang-format off
        template<std::ranges::range Range, std::output_iterator<char32_t> OutputIterator>
        static auto to_char32(Range &&r, std::endian, OutputIterator out)
            -> expected<void, std::error_code>
            requires (std::same_as<std::byte, std::ranges::range_value_t<Range>>)
        // clang-format on
        {
            IVY_TRACE("utf8::to_char32: start");

            detail::utf8_decoder decoder(out);
            for (std::byte b : r) {
                auto r = decoder.put(std::to_integer<char8_t>(b));

                if (!r)
                    return make_unexpected(r.error());
            }

            if (!decoder.ok())
                return make_unexpected(
                    make_error_code(charenc_errc::invalid_encoding));

            return {};
        }

        // clang-format off
        template<std::ranges::range Range, std::output_iterator<char_type> OutputIterator>
        static auto from_char32(Range &&r, OutputIterator out)
            -> expected<void, std::error_code>
            requires (std::same_as<char32_t, std::ranges::range_value_t<Range>>)
        // clang-format on
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
                // 1-byte encoding
                if (c <= max1b) {
                    *out++ = static_cast<char_type>(c);
                    continue;
                }

                // 2-byte encoding
                if (c <= max2b) {
                    *out++ = static_cast<char_type>(i2b |
                                                    detail::bit_slice(c, 6, 5));
                    *out++ = static_cast<char_type>(icont |
                                                    detail::bit_slice(c, 0, 6));
                    continue;
                }

                // 3-byte encoding
                if (c <= max3b) {
                    *out++ = static_cast<char_type>(
                        i3b | detail::bit_slice(c, 12, 4));
                    *out++ = static_cast<char_type>(icont |
                                                    detail::bit_slice(c, 6, 6));
                    *out++ = static_cast<char_type>(icont |
                                                    detail::bit_slice(c, 0, 6));
                    continue;
                }

                // 4-byte encoding
                if (c <= max4b) {
                    *out++ = static_cast<char_type>(
                        i4b | detail::bit_slice(c, 18, 3));
                    *out++ = static_cast<char_type>(
                        icont | detail::bit_slice(c, 12, 6));
                    *out++ = static_cast<char_type>(icont |
                                                    detail::bit_slice(c, 6, 6));
                    *out++ = static_cast<char_type>(icont |
                                                    detail::bit_slice(c, 0, 6));
                    continue;
                }

                return make_unexpected(
                    make_error_code(charenc_errc::unrepresentable_codepoint));
            }

            return {};
        }

        // clang-format off
        template<std::ranges::range Range>
        static auto validate(Range &&r)
            -> expected<void, std::error_code>
            requires (std::same_as<char_type, std::ranges::range_value_t<Range>>)
        // clang-format on
        {
            null_output_iterator<char32_t> out;
            return to_char32(std::forward<Range>(r), out);
        }
    };

} // namespace ivy

#endif // IVY_CHARENC_UTF8_HXX_INCLUDED
