/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_UTF16_HXX_INCLUDED
#define IVY_CHARENC_UTF16_HXX_INCLUDED

#include <concepts>
#include <cstddef>
#include <bit>
#include <iterator>
#include <ranges>
#include <string>
#include <type_traits>

#include <ivy/charenc/error.hxx>
#include <ivy/expected.hxx>
#include <ivy/iterator.hxx>
#include <ivy/trace.hxx>

namespace ivy {

    namespace detail {

        template <std::output_iterator<char16_t> OutputIterator>
        struct utf16_encoder {
            OutputIterator out;

            utf16_encoder(OutputIterator out_) : out(out_) {}

            auto put(char32_t v) -> expected<void, std::error_code>
            {
                if (v >= 0xD800u && v <= 0xDFFFu) {
                    // Cannot encode surrogates.
                    return make_unexpected(
                        make_error_code(charenc_errc::invalid_encoding));
                }

                if (v < 0x10000u) {
                    // No surrogate required, encode directly.
                    *out++ = static_cast<char16_t>(v);
                    return {};
                }

                // Need a surrogate.
                v -= 0x10000;
                static constexpr char32_t tenbits = (1u << 10) - 1;
                *out++ = static_cast<char16_t>(0xD800 + ((v >> 10) & tenbits));
                *out++ = static_cast<char16_t>(0xDC00 + (v & tenbits));
                return {};
            }
        };

        template <std::output_iterator<char32_t> OutputIterator>
        struct utf16_decoder {
            OutputIterator out;
            char32_t c32 = 0;

            utf16_decoder(OutputIterator out_) : out(out_) {}

            auto put(char16_t v) -> expected<void, std::error_code>
            {
                if (v < 0xD800u || v > 0xDFFFu) {
                    // Not a surrogate.
                    if (c32)
                        return make_unexpected(
                            make_error_code(charenc_errc::invalid_encoding));

                    *out++ = char32_t(v);
                    return {};
                }

                if (c32) {
                    if (v < 0xDC00u || v > 0xDFFFu)
                        return make_unexpected(
                            make_error_code(charenc_errc::invalid_encoding));

                    // Last character was a high surrogate, so expect the low.
                    c32 |= static_cast<char32_t>(v & ((1u << 10) - 1));

                    // Surrogate pairs must be in this range.
                    if (c32 < 0x10000u || c32 > 0x10FFFFu)
                        return make_unexpected(
                            make_error_code(charenc_errc::invalid_encoding));

                    *out++ = c32;
                    c32 = 0;
                    return {};
                }

                if (v < 0xD800u || v > 0xDBFFu)
                    // Invalid high surrogate.
                    return make_unexpected(
                        make_error_code(charenc_errc::invalid_encoding));

                // high surrogate.
                c32 = 0x10000 +
                      ((static_cast<char32_t>(v) & ((1u << 10) - 1)) << 10);
                return {};
            }

            auto ok() const -> bool
            {
                return c32 == 0;
            }
        };

    } // namespace detail

    struct utf16_encoding {
        using char_type = char16_t;

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
            detail::utf16_decoder decoder(out);
            for (char_type c : r) {
                auto ok = decoder.put(c);

                if (!ok)
                    return make_unexpected(ok.error());
            }

            if (!decoder.ok())
                return make_unexpected(
                    make_error_code(charenc_errc::invalid_encoding));

            return {};
        }

        // clang-format off
        template<std::ranges::range Range, std::output_iterator<char32_t> OutputIterator>
        static auto to_char32(Range &&r, std::endian endianness, OutputIterator out)
            -> expected<void, std::error_code>
            requires (std::same_as<std::byte, std::ranges::range_value_t<Range>>)
        // clang-format on
        {
            detail::utf16_decoder decoder(out);
            auto begin = std::ranges::begin(r);
            auto end = std::ranges::end(r);

            while (begin < end) {
                auto b0 = static_cast<char16_t>(*begin++);

                if (begin == end)
                    return make_unexpected(
                        make_error_code(charenc_errc::invalid_encoding));

                auto b1 = static_cast<char16_t>(*begin++);
                char16_t c;
                if (endianness == std::endian::big)
                    c = static_cast<char16_t>(b0 << 8) | b1;
                else
                    c = static_cast<char16_t>(b1 << 8) | b0;

                IVY_TRACE("utf16_decoder::to_char32(byte): c={:04x}", static_cast<unsigned>(c));

                auto ok = decoder.put(c);

                if (!ok)
                    return make_unexpected(ok.error());
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
            detail::utf16_encoder encoder(out);

            for (auto c : r) {
                auto ok = encoder.put(c);
                if (!ok)
                    return make_unexpected(ok.error());
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

#endif // IVY_CHARENC_UTF16_HXX_INCLUDED
