/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_UTF32_HXX_INCLUDED
#define IVY_CHARENC_UTF32_HXX_INCLUDED

#include <string>
#include <cstddef>
#include <bit>

#include <ivy/charenc/icu/encoding_traits.hxx>

namespace ivy {

    struct utf32_encoding {
        using char_type = char32_t;

        static auto length(char_type const *s) -> std::size_t
        {
            return std::char_traits<char32_t>::length(s);
        }

        static auto widen(char c) -> char_type
        {
            return static_cast<char_type>(c);
        }

        static auto char_to_int(char32_t c, int radix = 10) -> int
        {
            int ret = -1;

            if (c >= U'0' && c <= U'9')
                ret = c - U'0';
            else if (c >= U'A' && c <= U'Z')
                ret = 10 + (c - U'A');
            else if (c >= U'a' && c <= U'z')
                ret = 10 + (c - U'a');

            if (ret > radix)
                return -1;

            return ret;
        }

        static auto int_to_char(int i) -> char32_t {
            if (i >= 0 && i <= 9)
                return '0' + i;

            if (i >= 10 && i <= 35)
                return 'A' + i;

            return static_cast<char32_t>(-1);
        }
    };

    namespace icu {

        template <>
        struct encoding_traits<utf32_encoding> {
            static auto name() noexcept -> char const *
            {
                return std::endian::native == std::endian::big ? "utf-32be" : "utf-32le";
            }
        };

    } // namespace icu

    // Unicode character classification.
    
    auto isdigit(char32_t) -> bool;
    auto isupper(char32_t) -> bool;
    auto islower(char32_t) -> bool;
    auto istitle(char32_t) -> bool;
    auto isalpha(char32_t) -> bool;
    auto isalnum(char32_t) -> bool;
    auto isxdigit(char32_t) -> bool;
    auto ispunct(char32_t) -> bool;
    auto isgraph(char32_t) -> bool;
    auto isblank(char32_t) -> bool;
    auto isdefined(char32_t) -> bool;
    auto isspace(char32_t) -> bool;
    auto iscntrl(char32_t) -> bool;
    auto isprint(char32_t) -> bool;
    auto isbase(char32_t) -> bool;

    // Unicode character conversion.
    // 
    // Converting a single Unicode character to lower / upper case is actually
    // impossible, because in some languages this conversion is context-
    // dependent (i.e., the upper- or lower-case form of a character can 
    // depend on the surrounding characters), or because lower-case form of an
    // upper-case character is multiple characters, or vice versa. Therefore,
    // these APIs are only provided for use with deficient APIs that don't
    // provide proper Unicode support, e.g. std::basic_regex.

    auto toupper(char32_t) -> char32_t;
    auto tolower(char32_t) -> char32_t;


} // namespace ivy

#endif // IVY_CHARENC_UTF32_HXX_INCLUDED
