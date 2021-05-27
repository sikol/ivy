/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_FLAGSET_HXX_INCLUDED
#define IVY_FLAGSET_HXX_INCLUDED

#include <bit>
#include <type_traits>

namespace ivy {

    /*************************************************************************
     *
     * flagset: type-safe bitset for flags.
     */

    template <typename Tag, std::unsigned_integral T>
    struct flagset;

    template <typename Tag, std::unsigned_integral T>
    struct flag {
        using flagset = ivy::flagset<Tag, T>;
        T value;
    };

    template <typename Tag, std::unsigned_integral T>
    struct flagset {
        constexpr flagset(flag<Tag, T> v) : value(v.value) {}
        explicit constexpr flagset(T v) : value(v) {}

        T value;
    };

    template <typename Tag, std::unsigned_integral T>
    constexpr auto operator|(flag<Tag, T> a, flag<Tag, T> b) noexcept
        -> flagset<Tag, T>
    {
        return flagset<Tag, T>(a.value | b.value);
    }

    template <typename Tag, std::unsigned_integral T>
    constexpr auto operator|(flagset<Tag, T> a, flag<Tag, T> b) noexcept
        -> flagset<Tag, T>
    {
        return flagset<Tag, T>(a.value | b.value);
    }

    template <typename Tag, std::unsigned_integral T>
    constexpr auto operator|=(flagset<Tag, T> &a, flag<Tag, T> b) noexcept
        -> flagset<Tag, T> &
    {
        a.value |= b.value;
        return a;
    }

    template <typename Tag, std::unsigned_integral T>
    constexpr auto operator|=(flagset<Tag, T> &a, flagset<Tag, T> b) noexcept
        -> flagset<Tag, T> &
    {
        a.value |= b.value;
        return a;
    }

    template <typename Tag, std::unsigned_integral T>
    constexpr auto operator&=(flagset<Tag, T> &a, flag<Tag, T> b) noexcept
        -> flagset<Tag, T> &
    {
        a.value &= b.value;
        return a;
    }

    template <typename Tag, std::unsigned_integral T>
    constexpr auto operator&(flagset<Tag, T> a, flag<Tag, T> b) noexcept
        -> flagset<Tag, T>
    {
        return flagset<Tag, T>(a.value & b.value);
    }

    template <typename Tag, std::unsigned_integral T>
    constexpr auto is_set(flagset<Tag, T> a, flag<Tag, T> b) noexcept -> bool
    {
        return (a.value & b.value) != 0;
    }

    template <typename Tag, std::unsigned_integral T>
    constexpr auto any_set(flagset<Tag, T> a, flagset<Tag, T> b) noexcept
        -> bool
    {
        return (a.value & b.value) != 0;
    }

    template <typename Tag, std::unsigned_integral T>
    constexpr auto all_set(flagset<Tag, T> a, flagset<Tag, T> b) noexcept
        -> bool
    {
        return (a.value & b.value) == b.value;
    }

} // namespace sk

#endif // IVY_FLAGSET_HXX_INCLUDED
