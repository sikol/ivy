/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_STRING_TRIM_HXX_INCLUDED
#define IVY_STRING_TRIM_HXX_INCLUDED

#include <ivy/string.hxx>

namespace ivy {

    template <typename encoding_type, typename allocator_type>
    auto triml(basic_string<encoding_type, allocator_type> s)
        -> basic_string<encoding_type, allocator_type>
    {
        auto i = std::find_if(s.begin(), s.end(), [](auto c) {
            return !ivy::isspace(c);
        });

        return basic_string<encoding_type, allocator_type>(i, s.end());
    }

} // namespace

#endif // IVY_STRING_TRIM_HXX_INCLUDED
