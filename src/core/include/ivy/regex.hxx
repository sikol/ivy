/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_REGEX_HXX_INCLUDED
#define IVY_REGEX_HXX_INCLUDED

#include <locale>
#include <regex>
#include <string>

#include <srell.hpp>

#include <ivy/charenc/utf32.hxx>
#include <ivy/string.hxx>

namespace ivy {

    // Disabled because MSVC's std::basic_regex is deficient and doesn't support
    // custom traits.
#if 0

    // Minimal implementation of std::regex_traits for UTF-32.
    // This implementation ignores the imbued locale.
    class utf32_regex_traits {
    public:
        using char_type = utf32_encoding::char_type;
        using string_type = basic_string<utf32_encoding>;
        using locale_type = std::locale;
        using char_class_type = std::uint_least16_t;

        static auto length(char_type const *p) -> std::size_t
        {
            return std::char_traits<char32_t>::length(p);
        }

        auto translate_nocase(char_type c) const -> char_type
        {
            return tolower(c);
        }

        auto translate(char_type c) const -> char_type
        {
            return c;
        }

        template <typename forward_iterator>
        auto transform(forward_iterator first, forward_iterator last) const
            -> string_type
        {
            return string_type(first, last);
        }

        template <typename forward_iterator>
        auto transform_primary(forward_iterator first, forward_iterator last) const
            -> string_type
        {
            return string_type(first, last);
        }

        template <typename forward_iterator>
        auto lookup_collatename(forward_iterator first, forward_iterator last) const
            -> string_type
        {
            return string_type();
        }

        template <typename forward_iterator>
        auto lookup_classname(forward_iterator first,
                              forward_iterator last,
                              bool icase = false) const -> char_class_type
        {
            char_class_type ret = 0;

            if (icase)
                ret |= cc_icase;

            string name(first, last);
            if (name == U"alnum")
                return ret | cc_alnum;
            if (name == U"alpha")
                return ret | cc_alpha;
            if (name == U"blank")
                return ret | cc_blank;
            if (name == U"cntrl")
                return ret | cc_cntrl;
            if (name == U"digit")
                return ret | cc_digit;
            if (name == U"graph")
                return ret | cc_graph;
            if (name == U"lower")
                return ret | cc_lower;
            if (name == U"print")
                return ret | cc_print;
            if (name == U"punct")
                return ret | cc_punct;
            if (name == U"space")
                return ret | cc_space;
            if (name == U"upper")
                return ret | cc_upper;
            if (name == U"xdigit")
                return ret | cc_xdigit;
            if (name == U"d")
                return ret | cc_digit;
            if (name == U"s")
                return ret | cc_space;
            if (name == U"w")
                return ret | cc_word;
            return 0;
        }

        auto isctype(char_type c, char_class_type cc) const -> bool
        {
            if (cc & cc_icase) {
                if ((cc & (cc_upper | cc_lower)) && (isupper(c) || islower(c)))
                    return true;
            }

            if ((cc & cc_upper) && isupper(c))
                return true;
            if ((cc & cc_lower) && islower(c))
                return true;
            if ((cc & cc_alnum) && isalnum(c))
                return true;
            if ((cc & cc_alpha) && isalpha(c))
                return true;
            if ((cc & cc_blank) && isblank(c))
                return true;
            if ((cc & cc_cntrl) && iscntrl(c))
                return true;
            if ((cc & cc_digit) && isdigit(c))
                return true;
            if ((cc & cc_graph) && isgraph(c))
                return true;
            if ((cc & cc_print) && isprint(c))
                return true;
            if ((cc & cc_punct) && ispunct(c))
                return true;
            if ((cc & cc_space) && isspace(c))
                return true;
            if ((cc & cc_xdigit) && isxdigit(c))
                return true;
            if ((cc & cc_word) && (isalnum(c) || c == '_'))
                return true;

            return false;
        }

        auto value(char_type c, int radix) const -> int
        {
            switch (radix) {
            case 8:
                if (c >= '0' && c <= '7')
                    return c - '0';
                return -1;

            case 10:
                if (c >= '0' && c <= '9')
                    return c - '0';
                return -1;

            case 16:
                if (c >= '0' && c <= '9')
                    return c - '0';

                if (c >= 'a' && c <= 'f')
                    return (c - 'a') + 10;

                if (c >= 'A' && c <= 'F')
                    return (c - 'F') + 10;

                return -1;

            default:
                return -1;
            }
        }

        auto imbue(locale_type loc) -> locale_type
        {
            return std::exchange(_locale, loc);
        }

        auto getloc() const -> locale_type
        {
            return _locale;
        }

    private:
        static constexpr char_class_type cc_icase = 1 << 0;
        static constexpr char_class_type cc_alnum = 1 << 1;
        static constexpr char_class_type cc_alpha = 1 << 2;
        static constexpr char_class_type cc_blank = 1 << 3;
        static constexpr char_class_type cc_cntrl = 1 << 4;
        static constexpr char_class_type cc_digit = 1 << 5;
        static constexpr char_class_type cc_graph = 1 << 6;
        static constexpr char_class_type cc_lower = 1 << 7;
        static constexpr char_class_type cc_print = 1 << 8;
        static constexpr char_class_type cc_punct = 1 << 9;
        static constexpr char_class_type cc_space = 1 << 10;
        static constexpr char_class_type cc_upper = 1 << 11;
        static constexpr char_class_type cc_xdigit = 1 << 12;
        static constexpr char_class_type cc_word = 1 << 13;

        std::locale _locale;
    };

    using u32regex = std::basic_regex<char32_t, utf32_regex_traits>;

    using regex = u32regex;
#endif

    // Use SRELL for a Unicode-aware regex for now.
    using u32regex = srell::u32regex;
    using u32smatch = srell::match_results<u32string::const_iterator>;
    using regex = u32regex;
    using srell::regex_search;

} // namespace ivy

#endif // IVY_REGEX_HXX_INCLUDED
