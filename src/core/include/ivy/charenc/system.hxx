/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_SYSTEM_HXX_INCLUDED
#define IVY_CHARENC_SYSTEM_HXX_INCLUDED

#include <vector>
#include <span>
#include <string>

#include <ivy/charenc/utf32.hxx>

namespace ivy {

    struct system_encoding {
        using char_type = char;

        static auto length(char_type const *s) -> std::size_t
        {
            return std::char_traits<char_type>::length(s);
        }

        static auto widen(char c) -> char_type
        {
            return c;
        }
    };

    namespace icu {

        template <>
        struct encoding_traits<system_encoding> {
            static auto name() noexcept -> char const *
            {
                return nullptr;
            }
        };

    } // namespace icu

} // namespace ivy

#endif // IVY_CHARENC_SYSTEM_HXX_INCLUDED
