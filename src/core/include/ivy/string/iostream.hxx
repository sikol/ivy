/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_STRING_IOSTREAM_HXX_INCLUDED
#    define IVY_STRING_IOSTREAM_HXX_INCLUDED

#include <iostream>

#include <ivy/string/transcode.hxx>

namespace ivy {


    template <character_encoding encoding, typename allocator>
    auto operator<<(std::basic_ostream<typename encoding::char_type> &strm,
                    basic_string<encoding, allocator> const &s)
        -> std::basic_ostream<typename encoding::char_type> &
    {
        strm.write(s.data(), s.size());
        return strm;
    }

    template <character_encoding encoding, typename allocator>
    auto operator<<(std::ostream &strm,
                    basic_string<encoding, allocator> const &s)
        -> std::ostream &
    {
        using system_allocator =
            typename std::allocator_traits<allocator>::template rebind_alloc<
                typename system_encoding::char_type>;

        auto astr =
            transcode<basic_string<system_encoding, system_allocator>>(s);

        if (astr)
            strm.write(astr->data(), astr->size());

        return strm;
    }

    template <character_encoding encoding, typename allocator>
    auto operator<<(std::wostream &strm,
                    basic_string<encoding, allocator> const &s)
        -> std::wostream &
    {
        using system_allocator =
            typename std::allocator_traits<allocator>::template rebind_alloc<
                typename system_encoding::char_type>;

        auto astr =
            transcode<basic_string<system_wide_encoding, system_allocator>>(s);

        if (astr)
            strm.write(astr->data(), astr->size());

        return strm;
    }

}

#endif // IVY_STRING_IOSTREAM_HXX_INCLUDED
