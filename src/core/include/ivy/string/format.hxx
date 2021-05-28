/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_STRING_FORMAT_HXX_INCLUDED
#define IVY_STRING_FORMAT_HXX_INCLUDED

#include <format>
#include <string>

#include <ivy/string.hxx>
#include <ivy/string/transcode.hxx>

namespace std {

    template <typename encoding, typename allocator, typename char_type>
    struct formatter<ivy::basic_string<encoding, allocator>, char_type> {
        using std_string_char_type = char_type;
        using std_string_char_traits_type = char_traits<char_type>;
        using std_string_allocator_type = typename std::allocator_traits<
            allocator>::template rebind_alloc<char_type>;
        using std_string_type = basic_string<std_string_char_type,
                                             std_string_char_traits_type,
                                             std_string_allocator_type>;
        formatter<std_string_type> string_formatter;

        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx)
        {
            return string_formatter.parse(ctx);
        }

        template <typename FormatContext>
        auto format(ivy::basic_string<encoding, allocator> const &s,
                    FormatContext &ctx)
        {
            auto tx = ivy::transcode<std_string_type>(s).or_throw();
            return string_formatter.format(tx, ctx);
        }
    };
} // namespace std

#endif // IVY_STRING_FORMAT_HXX_INCLUDED
