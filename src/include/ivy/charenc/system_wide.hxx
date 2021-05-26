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
#include <ivy/trace.hxx>

#ifdef _WIN32
#    include <ivy/charenc/charconv.hxx>
#    include <ivy/charenc/utf16.hxx>
#    include <ivy/iterator/static_cast.hxx>
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

        static auto widen(char c) -> char_type
        {
            return static_cast<char_type>(c);
        }
    };

    namespace icu {

        template <>
        struct encoding_traits<system_wide_encoding> {
            static auto name() noexcept -> char const *
            {
                return std::endian::native == std::endian::big ? "utf-16be"
                                                               : "utf-16le";
            }
        };

    } // namespace icu

    template <character_encoding target_encoding>
    class charconv<system_wide_encoding, target_encoding> {
        charconv<utf16_encoding, target_encoding> _charconv;

    public:
        charconv(charconv_options options = {}) noexcept
            : _charconv(options)
        {
        }

        template <std::ranges::input_range input_range,
                  std::output_iterator<typename target_encoding::char_type>
                      output_iterator>
        requires std::same_as<wchar_t, std::ranges::range_value_t<input_range>>
        auto convert(input_range &&r, output_iterator out) -> void
        {
            for (auto c : r)
                _charconv.convert(std::span(&c, 1), out);
        }

        template <std::output_iterator<char16_t> output_iterator>
        auto flush(output_iterator out) -> void
        {
            _charconv.flush(out);
        }
    };

    template <character_encoding source_encoding>
    class charconv<source_encoding, system_wide_encoding> {
        charconv<source_encoding, utf16_encoding> _charconv;

    public:
        charconv(charconv_options options = {}) noexcept
            : _charconv(options)
        {
        }

        template <std::ranges::input_range input_range,
                  std::output_iterator<wchar_t> output_iterator>
        requires std::same_as<typename source_encoding::char_type,
                              std::ranges::range_value_t<input_range>>
        auto convert(input_range &&r, output_iterator out) -> void
        {
            _charconv.convert(
                r,
                make_static_cast_iterator<utf16_encoding::char_type,
                                          wchar_t>(out));
        }

        template <std::output_iterator<wchar_t> output_iterator>
        auto flush(output_iterator out) -> void
        {
            _charconv.flush(
                make_static_cast_iterator<utf16_encoding::char_type,
                                          wchar_t>(out));
        }
    };

#endif // _WIN32

} // namespace ivy

#endif // IVY_CHARENC_SYSTEM_WIDE_HXX_INCLUDED
