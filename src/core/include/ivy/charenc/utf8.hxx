/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_UTF8_HXX_INCLUDED
#define IVY_CHARENC_UTF8_HXX_INCLUDED

#include <cstddef>
#include <string>

#include <ivy/charenc/icu/encoding_traits.hxx>

namespace ivy {

    struct utf8_encoding {
        using char_type = char8_t;

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
        struct encoding_traits<utf8_encoding> {
            static auto name() noexcept -> char const *
            {
                return "utf-8";
            }
        };

    } // namespace icu

} // namespace ivy

#endif // IVY_CHARENC_UTF8_HXX_INCLUDED
