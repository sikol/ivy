/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_UTF16_HXX_INCLUDED
#define IVY_CHARENC_UTF16_HXX_INCLUDED

#include <string>
#include <cstddef>
#include <bit>

#include <ivy/charenc/icu/encoding_traits.hxx>

namespace ivy {

    struct utf16_encoding {
        using char_type = char16_t;

        static auto length(char_type const *s) noexcept -> std::size_t
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
        struct encoding_traits<utf16_encoding> {
            static auto name() noexcept -> char const *
            {
                return std::endian::native == std::endian::big ? "utf-16be"
                                                               : "utf-16le";
            }
        };

    } // namespace icu

} // namespace ivy

#endif // IVY_CHARENC_UTF16_HXX_INCLUDED
