/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_BYTE_HXX_INCLUDED
#define IVY_BYTE_HXX_INCLUDED

#include <bit>
#include <concepts>
#include <iterator>
#include <ranges>

namespace ivy {

    template <std::integral T, std::output_iterator<std::byte> iterator>
    auto write_bytes(std::endian, T v, iterator out) -> iterator
        requires(sizeof(T) == 1)
    {
        auto u = static_cast<std::make_unsigned_t<T>>(v);
        *out++ = static_cast<std::byte>(u & 0xFF);
        return out;
    }

    template <std::integral T, std::output_iterator<std::byte> iterator>
    auto write_bytes(std::endian endianness, T v, iterator out) -> iterator
        requires(sizeof(T) == 2)
    {
        auto u = static_cast<std::make_unsigned_t<T>>(v);

        if (endianness == std::endian::big) {
            *out++ = static_cast<std::byte>((u >> 8) & 0xFF);
            *out++ = static_cast<std::byte>(u & 0xFF);
        } else {
            *out++ = static_cast<std::byte>(u & 0xFF);
            *out++ = static_cast<std::byte>((u >> 8) & 0xFF);
        }

        return out;
    }

    template <std::integral T, std::output_iterator<std::byte> iterator>
    auto write_bytes(std::endian endianness, T v, iterator out) -> iterator
        requires(sizeof(T) == 4)
    {
        auto u = static_cast<std::make_unsigned_t<T>>(v);

        if (endianness == std::endian::big) {
            *out++ = static_cast<std::byte>((u >> 24) & 0xFF);
            *out++ = static_cast<std::byte>((u >> 16) & 0xFF);
            *out++ = static_cast<std::byte>((u >> 8) & 0xFF);
            *out++ = static_cast<std::byte>(u & 0xFF);
        } else {
            *out++ = static_cast<std::byte>(u & 0xFF);
            *out++ = static_cast<std::byte>((u >> 8) & 0xFF);
            *out++ = static_cast<std::byte>((u >> 16) & 0xFF);
            *out++ = static_cast<std::byte>((u >> 24) & 0xFF);
        }

        return out;
    }

    template <std::integral T, std::output_iterator<std::byte> iterator>
    auto write_bytes(std::endian endianness, T v, iterator out) -> iterator
        requires(sizeof(T) == 8)
    {
        auto u = static_cast<std::make_unsigned_t<T>>(v);

        if (endianness == std::endian::big) {
            *out++ = static_cast<std::byte>((u >> 56) & 0xFF);
            *out++ = static_cast<std::byte>((u >> 48) & 0xFF);
            *out++ = static_cast<std::byte>((u >> 40) & 0xFF);
            *out++ = static_cast<std::byte>((u >> 32) & 0xFF);
            *out++ = static_cast<std::byte>((u >> 24) & 0xFF);
            *out++ = static_cast<std::byte>((u >> 16) & 0xFF);
            *out++ = static_cast<std::byte>((u >> 8) & 0xFF);
            *out++ = static_cast<std::byte>(u & 0xFF);
        } else {
            *out++ = static_cast<std::byte>(u & 0xFF);
            *out++ = static_cast<std::byte>((u >> 8) & 0xFF);
            *out++ = static_cast<std::byte>((u >> 16) & 0xFF);
            *out++ = static_cast<std::byte>((u >> 24) & 0xFF);
            *out++ = static_cast<std::byte>((u >> 32) & 0xFF);
            *out++ = static_cast<std::byte>((u >> 40) & 0xFF);
            *out++ = static_cast<std::byte>((u >> 48) & 0xFF);
            *out++ = static_cast<std::byte>((u >> 56) & 0xFF);
        }

        return out;
    }

    template <std::ranges::range input_range,
              std::output_iterator<std::byte> iterator>
    auto write_bytes(std::endian endianness, input_range const &r, iterator out)
        -> iterator
    {
        for (auto v : r)
            out = write_bytes(endianness, v, out);

        return out;
    }

} // namespace ivy

#endif // IVY_BYTE_HXX_INCLUDED
