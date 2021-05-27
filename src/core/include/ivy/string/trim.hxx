/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_STRING_TRIM_HXX_INCLUDED
#define IVY_STRING_TRIM_HXX_INCLUDED

#include <string_view>

namespace ivy {

    template <typename Char, typename Traits>
    auto triml(std::basic_string_view<Char, Traits> s)
        -> std::basic_string_view<Char, Traits>
    {
        auto i = std::find_if(s.begin(), s.end(), [](Char c) {
            return !std::isspace(static_cast<char32_t>(
                static_cast<std::make_unsigned_t<Char>>(c)));
        });

        return std::basic_string_view<Char, Traits>(i, s.end());
    }

} // namespace

#endif // IVY_STRING_TRIM_HXX_INCLUDED
