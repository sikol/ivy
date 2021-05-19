/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_SYSTEM_WIDE_HXX_INCLUDED
#define IVY_CHARENC_SYSTEM_WIDE_HXX_INCLUDED

#include <bit>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <string>
#include <type_traits>

#include <ivy/charenc/error.hxx>
#include <ivy/expected.hxx>
#include <ivy/iterator.hxx>
#include <ivy/trace.hxx>

#ifdef _WIN32
#    include <ivy/charenc/utf16.hxx>
#else
#    error system_wide_encoding is only supported on Windows
#endif

namespace ivy {

#ifdef _WIN32
    // The system wide encoding is always UTF-16 on Windows.

    struct system_wide_encoding {
        static_assert(sizeof(wchar_t) == sizeof(char16_t));

        using char_type = wchar_t;

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
                auto ok = decoder.put(static_cast<char16_t>(c));

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
                auto b0 = static_cast<char_type>(*begin++);

                if (begin == end)
                    return make_unexpected(
                        make_error_code(charenc_errc::invalid_encoding));

                auto b1 = static_cast<char_type>(*begin++);
                char16_t c;
                if (endianness == std::endian::big)
                    c = static_cast<char_type>(b0 << 8) | b1;
                else
                    c = static_cast<char_type>(b1 << 8) | b0;

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
#endif // _WIN32

} // namespace ivy

#endif // IVY_CHARENC_SYSTEM_WIDE_HXX_INCLUDED
