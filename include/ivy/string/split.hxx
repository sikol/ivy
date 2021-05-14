/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_STRING_SPLIT_HXX_INCLUDED
#define IVY_STRING_SPLIT_HXX_INCLUDED

#include <concepts>
#include <iterator>
#include <ranges>

namespace ivy {

    namespace detail {

        template <typename Char, typename Traits, typename Predicate>
        auto split_next(std::basic_string_view<Char, Traits> s, Predicate p)
            -> std::pair<std::optional<std::basic_string_view<Char, Traits>>,
                         std::basic_string_view<Char, Traits>>
        {
            auto d = std::ranges::find_if(s, p);

            if (d == s.end())
                return {{}, s};

            return {
                std::basic_string_view<Char, Traits>(s.begin(), d),
                std::basic_string_view<Char, Traits>(std::next(d), s.end())};
        }

    } // namespace detail

    template <typename Char,
              typename Traits,
              typename Predicate,
              typename Iterator>
    auto split_if(std::basic_string_view<Char, Traits> s,
                  Predicate p,
                  Iterator it) -> void
    {
        std::optional<std::basic_string_view<Char, Traits>> part;
        std::basic_string_view<Char, Traits> rest = s;

        for (;;) {
            if (std::tie(part, rest) = detail::split_next(rest, p); !part)
                break;

            *it++ = *part;
        }

        *it++ = rest;
    }

    template <typename Char,
              typename Traits,
              typename Alloc,
              typename Predicate,
              typename Iterator>
    auto split_if(std::basic_string<Char, Traits, Alloc> const &s,
                  Predicate p,
                  Iterator it) -> void
    {
        split_if(
            std::basic_string_view<Char, Traits>(s.begin(), s.end()), p, it);
    }

    template <typename Char, typename Traits, typename Iterator>
    auto split(std::basic_string_view<Char, Traits> s, Char delim, Iterator it)
        -> void
    {
        auto p = [=](Char c) { return Traits::eq(delim, c); };
        split_if(s, p, it);
    }

    template <typename Char, typename Traits, typename Alloc, typename Iterator>
    auto split(std::basic_string<Char, Traits, Alloc> const &s,
               Char delim,
               Iterator it) -> void
    {
        split(std::basic_string_view<Char, Traits>(s.begin(), s.end()),
              delim,
              it);
    }

} // namespace ivy

#endif // IVY_STRING_SPLIT_HXX_INCLUDED
