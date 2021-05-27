/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_HASH_HXX_INCLUDED
#define IVY_HASH_HXX_INCLUDED

#include <span>
#include <cstddef>

#include <ivy/siphash.hxx>
#include <ivy/static_vector.hxx>

namespace ivy {

    namespace detail {

        auto generate_sip_hash_key() -> static_vector<std::byte, 16>;

        auto get_sip_hash_key() -> static_vector<std::byte, 16> const &;

        inline auto hash_bytes(std::span<std::byte const> bytes) -> std::size_t
        {
            std::byte hash[8];
            siphash<>(bytes, get_sip_hash_key(), hash);
            return static_cast<std::size_t>(detail::sip_u8u64le(hash));
        }

        template <typename T>
        auto hash_object(T const &v) -> std::size_t
        {
            return hash_bytes(as_bytes(std::span(&v, 1)));
        }
    }

    template <typename T>
    struct hash;

    template <>
    struct hash<char> {
        auto operator()(char v) const noexcept -> std::size_t {
            return detail::hash_object(v);
        }
    };

    template <>
    struct hash<unsigned char> {
        auto operator()(unsigned char v) const noexcept -> std::size_t
        {
            return detail::hash_object(v);
        }
    };

    template <>
    struct hash<signed char> {
        auto operator()(signed char v) const noexcept -> std::size_t
        {
            return detail::hash_object(v);
        }
    };

    template <>
    struct hash<signed short> {
        auto operator()(signed short v) const noexcept -> std::size_t
        {
            return detail::hash_object(v);
        }
    };

    template <>
    struct hash<unsigned short> {
        auto operator()(unsigned short v) const noexcept -> std::size_t
        {
            return detail::hash_object(v);
        }
    };

    template <>
    struct hash<signed int> {
        auto operator()(signed int v) const noexcept -> std::size_t
        {
            return detail::hash_object(v);
        }
    };

    template <>
    struct hash<unsigned int> {
        auto operator()(unsigned int v) const noexcept -> std::size_t
        {
            return detail::hash_object(v);
        }
    };

    template <>
    struct hash<signed long> {
        auto operator()(signed long v) const noexcept -> std::size_t
        {
            return detail::hash_object(v);
        }
    };

    template <>
    struct hash<unsigned long> {
        auto operator()(unsigned long v) const noexcept -> std::size_t
        {
            return detail::hash_object(v);
        }
    };

    template <>
    struct hash<signed long long> {
        auto operator()(signed long long v) const noexcept -> std::size_t
        {
            return detail::hash_object(v);
        }
    };

    template <>
    struct hash<unsigned long long> {
        auto operator()(unsigned long long v) const noexcept -> std::size_t
        {
            return detail::hash_object(v);
        }
    };

} // namespace ivy

#endif // IVY_HASH_HXX_INCLUDED
