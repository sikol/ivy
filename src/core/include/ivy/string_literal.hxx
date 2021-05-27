/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_STRING_LITERAL_HXX_INCLUDED
#define IVY_STRING_LITERAL_HXX_INCLUDED

#include <algorithm>
#include <cstddef>

namespace ivy {

    template <std::size_t N>
    struct string_literal {
        constexpr string_literal(char const (&s)[N])
        {
            std::copy_n(s, N, value);
        }

        char value[N];
    };

} // namespace ivy

#endif // IVY_STRING_LITERAL_HXX_INCLUDED
