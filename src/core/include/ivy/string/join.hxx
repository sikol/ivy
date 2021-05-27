/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_STRING_JOIN_HXX
#define IVY_STRING_JOIN_HXX

#include <concepts>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>

namespace ivy {

    template <std::input_iterator Iterator, typename Separator>
    auto join(Iterator begin, Iterator end, Separator &&sep) -> std::string
    {
        bool first = true;
        std::ostringstream strm;

        for (; begin != end; ++begin) {
            if (!first)
                strm << sep;
            first = false;

            strm << *begin;
        }

        return strm.str();
    }

    template <std::ranges::range Range, typename Separator>
    auto join(Range &&range, Separator &&sep) -> std::string
    {
        return join(std::ranges::begin(range), std::ranges::end(range), sep);
    }

} // namespace ivy

#endif // IVY_STRING_JOIN_HXX
