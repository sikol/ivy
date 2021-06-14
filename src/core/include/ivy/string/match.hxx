/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_STRING_MATCH_HXX
#define IVY_STRING_MATCH_HXX

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <regex>
#include <span>
#include <utility>

#include <srell.hpp>

#include <ivy/regex.hxx>
#include <ivy/string.hxx>

namespace ivy {

    namespace detail {

        template <typename Char>
        auto char_to_int(Char c) -> int
        {
            static_assert(('Z' - 'A') == 25,
                          "non-ASCII character sets are not supported");

            if (c < 0)
                return -1;

            auto ic = char32_t(c);
            if (ic >= char32_t('0') && ic <= char32_t('9'))
                return ic - char32_t('0');

            if (ic >= char32_t('A') && ic <= char32_t('Z'))
                return 10 + (ic - char32_t('A'));

            if (ic >= char32_t('a') && ic <= char32_t('z'))
                return 10 + (ic - char32_t('a'));

            return -1;
        }

    } // namespace detail

    template <typename encoding, typename allocator>
    auto match_string(basic_string<encoding, allocator> const &s,
                      basic_string<encoding, allocator> const &prefix)
        -> std::pair<std::optional<basic_string<encoding, allocator>>,
                     basic_string<encoding, allocator>>
    {
        if (prefix.size() > s.size())
            return {{}, s};

        auto s_prefix = s.substr(0, prefix.size());
        if (!std::ranges::equal(s_prefix, prefix))
            return {{}, s};

        return {prefix, s.substr(prefix.size())};
    }

    template <typename encoding, typename allocator>
    auto match_string(basic_string<encoding, allocator> const &s,
                      typename encoding::char_type const *prefix)
        -> std::pair<std::optional<basic_string<encoding, allocator>>,
                     basic_string<encoding, allocator>>
    {
        std::span<typename encoding::char_type const> pspan(
            prefix, encoding::length(prefix));

        if (pspan.size() > s.size())
            return {{}, s};

        auto s_prefix = s.substr(0, pspan.size());
        if (!std::ranges::equal(s_prefix, pspan))
            return {{}, s};

        return {s_prefix, s.substr(s_prefix.size())};
    }

    template <typename Int, typename encoding, typename allocator>
    auto match_int(basic_string<encoding, allocator> s, unsigned base = 10)
        -> std::pair<std::optional<Int>, basic_string<encoding, allocator>>
    {
        auto begin = s.begin();
        auto end = s.end();

        if (begin == end)
            return {{}, s};

        bool negative = false;
        Int value = 0;

        if (*begin == '-') {
            if (!std::is_signed_v<Int>)
                return {{}, s};

            negative = true;
            ++begin;
        }

        for (; begin != end; ++begin) {
            auto intv = detail::char_to_int(*begin);
            if (intv < 0 || static_cast<unsigned>(intv) >= base)
                break;

            value *= base;
            value += intv;
        }

        if (begin == s.begin())
            return {{}, s};

        if (negative)
            value = -value;

        return {value, basic_string<encoding, allocator>(begin, end)};
    }

    template <typename encoding, typename allocator>
    auto match_regex(basic_string<encoding, allocator> const &s,
                     u32regex const &regex)
        -> std::pair<std::optional<u32smatch>,
                     basic_string<encoding, allocator>>
    {
        using string_type = basic_string<encoding, allocator>;

        srell::match_results<typename string_type::iterator> matches;
        auto r = regex_search(s.begin(),
                              s.end(),
                              matches,
                              regex,
                              srell::regex_constants::match_continuous);

        if (r)
            return {std::move(matches),
                    string_type(matches[0].second, s.cend())};

        return {{}, s};
    }

    template <typename encoding, typename allocator>
    auto match_whitespace(basic_string<encoding, allocator> const &s)
        -> std::pair<std::optional<basic_string<encoding, allocator>>,
                     basic_string<encoding, allocator>>
    {
        auto begin = s.begin();
        auto end = s.end();

        auto i =
            std::find_if(begin, end, [](auto c) { return !ivy::isspace(c); });

        if (i == begin)
            return {{}, s};

        return {basic_string<encoding, allocator>(begin, i),
                basic_string<encoding, allocator>(i, end)};
    }

#if 0
    template<typename Char, typename Traits>
    auto match_regex(std::basic_string_view<Char, Traits> const &s,
                     std::basic_string_view<Char, Traits> const &regstr)
    {
        std::basic_regex<Char, Traits> regex(regstr);
        return match_regex(s, regex);
    }

    template <typename Char, typename Traits, typename Alloc>
    auto match_regex(std::basic_string_view<Char, Traits> const &s,
                     std::basic_string<Char, Traits, Alloc> const &regstr)
    {
        auto sv =
            std::basic_string_view<Char, Traits>(s.data(), s.data() + s.size());
        return match_regex(s, sv);
    }

    template <typename Char, typename Traits, typename Alloc>
    auto match_regex(std::basic_string_view<Char, Traits> const &s,
                     char const *regstr)
    {
        auto sv =
            std::basic_string_view<Char, Traits>(regstr);
        return match_regex(s, sv);
    }
#endif

} // namespace ivy

#endif // IVY_STRING_MATCH_HXX
