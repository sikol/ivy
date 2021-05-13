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
#include <string_view>
#include <utility>

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

    template <typename Char, typename Traits>
    auto match_string(std::basic_string_view<Char, Traits> s,
                      std::basic_string_view<Char, Traits> prefix)
        -> std::pair<std::optional<std::basic_string_view<Char, Traits>>,
                     std::basic_string_view<Char, Traits>>
    {
        if (prefix.size() > s.size())
            return {{}, s};

        auto s_prefix = s.substr(0, prefix.size());
        if (!std::ranges::equal(s_prefix, prefix))
            return {{}, s};

        return {prefix, s.substr(prefix.size())};
    }

    template <typename Int, typename Char, typename Traits>
    auto match_int(std::basic_string_view<Char, Traits> s, unsigned base = 10)
        -> std::pair<std::optional<Int>, std::basic_string_view<Char, Traits>>
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

        return {value, std::basic_string_view<Char, Traits>(begin, end)};
    }

    template <typename Char, typename Traits, typename RTraits>
    auto match_regex(std::basic_string_view<Char, Traits> s,
                     std::basic_regex<Char, RTraits> const &regex)
        -> std::pair<
            std::optional<std::match_results<
                typename std::basic_string_view<Char, Traits>::const_iterator>>,
            std::string_view>
    {
        using string_view_type = std::basic_string_view<Char, Traits>;
        using iterator_type = typename string_view_type::const_iterator;
        using match_results_type = std::match_results<iterator_type>;

        match_results_type matches;
        auto r = std::regex_search(s.begin(), s.end(), matches, regex);

        if (!r || (matches[0].first != s.begin()))
            return {{}, s};

        return {std::move(matches),
                string_view_type(matches[0].second, s.end())};
    }

    template <typename Char, typename Traits>
    auto match_whitespace(std::basic_string_view<Char, Traits> s)
        -> std::pair<std::optional<std::basic_string_view<Char, Traits>>,
                     std::basic_string_view<Char, Traits>>
    {
        auto begin = s.begin();
        auto end = s.end();

        auto i = std::find_if(begin, end, [](Char c) {
            return !std::isspace(static_cast<char32_t>(
                static_cast<std::make_unsigned_t<Char>>(c)));
        });

        if (i == begin)
            return {{}, s};

        return {std::basic_string_view<Char, Traits>(begin, i),
                std::basic_string_view<Char, Traits>(i, end)};
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
