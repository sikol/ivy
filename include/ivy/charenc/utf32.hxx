/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_UTF32_HXX_INCLUDED
#define IVY_CHARENC_UTF32_HXX_INCLUDED

#include <iterator>
#include <ranges>
#include <string>

#include <ivy/charenc/error.hxx>
#include <ivy/expected.hxx>

namespace ivy {

    struct utf32 {
        using char_type = char32_t;

        static auto length(char_type const *s) -> std::size_t
        {
            return std::char_traits<char32_t>::length(s);
        }

        // clang-format off
        template<std::ranges::range Range, std::output_iterator<char32_t> OutputIterator>
        static auto to_char32(Range &&r, OutputIterator out)
            -> expected<void, std::error_code>
            requires (std::same_as<char_type, std::ranges::range_value_t<Range>>)
        // clang-format on
        {
            std::ranges::copy(r, out);
            return {};
        }

        // clang-format off
        template<std::ranges::range Range, std::output_iterator<char_type> OutputIterator>
        static auto from_char32(Range &&r, OutputIterator out)
            -> expected<void, std::error_code>
            requires (std::same_as<char32_t, std::ranges::range_value_t<Range>>)
        // clang-format on
        {
            std::ranges::copy(r, out);
            return {};
        }
    };

} // namespace ivy

#endif // IVY_CHARENC_UTF32_HXX_INCLUDED
