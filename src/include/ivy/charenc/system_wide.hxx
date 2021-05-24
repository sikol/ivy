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

#include <ivy/error.hxx>
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
        using char_type = wchar_t;

        static auto length(char_type const *s) -> std::size_t
        {
            return std::char_traits<char_type>::length(s);
        }
    };

    template <>
    class charconv<system_wide_encoding, utf32_encoding> {
        charconv<utf16_encoding, utf32_encoding> _charconv;

    public:
        charconv(charconv_options options = {}) noexcept : _charconv(options) {}

        template <std::ranges::input_range input_range,
                  std::output_iterator<char32_t> output_iterator>
        auto convert(input_range &&r, output_iterator out) -> void
        {
            for (auto &&c : r) {
                auto c16 = static_cast<char16_t>(c);
                _charconv.convert(std::span(&c16, 1), out);
            }
        }

        template <std::output_iterator<char32_t> output_iterator>
        auto flush(output_iterator out) -> void
        {
            _charconv.flush(out);
        }
    };

    template <>
    class charconv<utf32_encoding, system_wide_encoding> {
        charconv<utf32_encoding, utf16_encoding> _charconv;

    public:
        charconv(charconv_options options = {}) noexcept : _charconv(options) {}

        template <std::ranges::input_range input_range,
                  std::output_iterator<wchar_t> output_iterator>
        auto convert(input_range &&r, output_iterator out) -> void
        {
            auto cit = make_static_cast_iterator<char16_t, wchar_t>(out);
            _charconv.convert(r, cit);
        }

        template <std::output_iterator<char32_t> output_iterator>
        auto flush(output_iterator out) -> void
        {
            static_cast_iterator<char16_t, wchar_t> cit(out);
            _charconv.flush(cit);
        }
    };

#endif // _WIN32

} // namespace ivy

#endif // IVY_CHARENC_SYSTEM_WIDE_HXX_INCLUDED
