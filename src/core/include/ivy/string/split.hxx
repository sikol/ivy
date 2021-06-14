/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_STRING_SPLIT_HXX_INCLUDED
#define IVY_STRING_SPLIT_HXX_INCLUDED

#include <concepts>
#include <iterator>
#include <ranges>

#include <ivy/string.hxx>

namespace ivy {

    namespace detail {

        template <typename encoding, typename allocator, typename predicate>
        auto split_next(basic_string<encoding, allocator> const &s, predicate p)
            -> std::pair<std::optional<basic_string<encoding, allocator>>,
                         basic_string<encoding, allocator>>
        {
            auto d = std::ranges::find_if(s, p);

            if (d == s.end())
                return {{}, s};

            return {basic_string<encoding, allocator>(s.begin(), d),
                    basic_string<encoding, allocator>(std::next(d), s.end())};
        }

    } // namespace detail

    template <typename encoding,
              typename allocator,
              typename predicate,
              typename iterator>
    auto split_if(basic_string<encoding, allocator> const &s,
                  predicate p,
                  iterator it) -> void
    {
        std::optional<basic_string<encoding, allocator>> part;
        basic_string<encoding, allocator> rest = s;

        for (;;) {
            if (std::tie(part, rest) = detail::split_next(rest, p); !part)
                break;

            *it++ = *part;
        }

        *it++ = rest;
    }

    template <typename encoding, typename allocator, typename iterator>
    auto split(basic_string<encoding, allocator> const &s, typename encoding::char_type delim, iterator it)
        -> void
    {
        auto p = [=](typename encoding::char_type c) { return delim == c; };
        split_if(s, p, it);
    }

} // namespace ivy

#endif // IVY_STRING_SPLIT_HXX_INCLUDED
