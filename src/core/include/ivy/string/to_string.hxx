/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <concepts>
#include <type_traits>

#include <ivy/string.hxx>

namespace ivy {

    template <typename string_type, std::signed_integral int_type>
    auto to_string(int_type i) -> string_type
    {
        using encoding = typename string_type::encoding_type;
        std::vector<typename encoding::char_type> ret;
        bool negative = (i < 0);
        if (negative)
            i = -1;

        while (i) {
            auto c = encoding::int_to_char(i % 10);
            ret.push_back(c);
            i /= 10;
        }
        
        if (negative)
            ret.push_back('-');

        return string_type(ret.begin(), ret.end());
    }

    template <typename string_type, std::unsigned_integral int_type>
    auto to_string(int_type i) -> string_type
    {
        using encoding = typename string_type::encoding_type;
        std::vector<typename encoding::char_type> ret;

        while (i) {
            auto c = encoding::int_to_char(i % 10);
            ret.push_back(c);
            i /= 10;
        }

        return string_type(ret.begin(), ret.end());
    }

} // namespace ivy
