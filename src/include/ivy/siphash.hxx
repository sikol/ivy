/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

/*
   SipHash reference C implementation

   Copyright (c) 2012-2021 Jean-Philippe Aumasson
   <jeanphilippe.aumasson@gmail.com>
   Copyright (c) 2012-2014 Daniel J. Bernstein <djb@cr.yp.to>

   To the extent possible under law, the author(s) have dedicated all copyright
   and related and neighboring rights to this software to the public domain
   worldwide. This software is distributed without any warranty.
   You should have received a copy of the CC0 Public Domain Dedication along
   with this software. If not, see
   <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef IVY_SIPHASH_HXX_INCLUDED
#define IVY_SIPHASH_HXX_INCLUDED

#include <cstddef>
#include <span>

#include <ivy/check.hxx>

namespace ivy {

    namespace detail {

        auto sip_u32u8le(std::uint32_t v, std::byte *a) -> void {
            a[0] = static_cast<std::byte>(v);
            a[1] = static_cast<std::byte>(v >> 8);
            a[2] = static_cast<std::byte>(v >> 16);
            a[3] = static_cast<std::byte>(v >> 24);
        }

        auto sip_u64u8le(std::uint64_t v, std::byte *a) -> void {
            sip_u32u8le(static_cast<std::uint32_t>(v), a);
            sip_u32u8le(static_cast<std::uint32_t>(v >> 32), a + 4);
        }

        auto sip_u8u64le(std::byte const *a) -> std::uint64_t {
            std::uint64_t r = 0;
            r |= static_cast<std::uint64_t>(a[0]);
            r |= static_cast<std::uint64_t>(a[1]) << 8;
            r |= static_cast<std::uint64_t>(a[2]) << 16;
            r |= static_cast<std::uint64_t>(a[3]) << 24;
            r |= static_cast<std::uint64_t>(a[4]) << 32;
            r |= static_cast<std::uint64_t>(a[5]) << 40;
            r |= static_cast<std::uint64_t>(a[6]) << 48;
            r |= static_cast<std::uint64_t>(a[7]) << 56;
            return r;
        }

        auto sip_rotl(std::uint64_t x, std::uint64_t b) -> std::uint64_t
        {
            return (x << b) | (x >> (64 - b));
        }

        struct sip_state {
            std::array<std::uint64_t, 4> v = {
                0x736f6d6570736575,
                0x646f72616e646f6d,
                0x6c7967656e657261,
                0x7465646279746573,
            };
        };

        auto sip_round(sip_state &v) -> void
        {
            v.v[0] += v.v[1];
            v.v[1] = sip_rotl(v.v[1], 13);
            v.v[1] ^= v.v[0];
            v.v[0] = sip_rotl(v.v[0], 32);
            v.v[2] += v.v[3];
            v.v[3] = sip_rotl(v.v[3], 16);
            v.v[3] ^= v.v[2];
            v.v[0] += v.v[3];
            v.v[3] = sip_rotl(v.v[3], 21);
            v.v[3] ^= v.v[0];
            v.v[2] += v.v[1];
            v.v[1] = sip_rotl(v.v[1], 17);
            v.v[1] ^= v.v[2];
            v.v[2] = sip_rotl(v.v[2], 32);
        }

    } // namespace detail

    template <std::size_t crounds = 2, std::size_t drounds = 4>
    auto siphash(std::span<std::byte const> in,
                 std::span<std::byte const> k,
                 std::span<std::byte> out) -> void
    {
        IVY_CHECK(k.size() == 16, "siphash: invalid key size");
        IVY_CHECK(out.size() == 8 || out.size() == 16,
                  "siphash: invalid out size");

        auto inlen = in.size();
        auto outlen = out.size();

        std::byte const *ni = in.data();

        detail::sip_state state;

        uint64_t k0 = detail::sip_u8u64le(k.data());
        uint64_t k1 = detail::sip_u8u64le(k.data() + 8);
        uint64_t m;
        std::byte const *end = ni + inlen - (inlen % sizeof(uint64_t));
        const int left = inlen & 7;
        uint64_t b = ((uint64_t)inlen) << 56;

        state.v[3] ^= k1;
        state.v[2] ^= k0;
        state.v[1] ^= k1;
        state.v[0] ^= k0;

        if (outlen == 16)
            state.v[1] ^= 0xee;

        for (; ni != end; ni += 8) {
            m = detail::sip_u8u64le(ni);
            state.v[3] ^= m;

            for (std::size_t i = 0; i < crounds; ++i)
                sip_round(state);

            state.v[0] ^= m;
        }

        switch (left) {
        case 7:
            b |= ((uint64_t)ni[6]) << 48;
        case 6:
            b |= ((uint64_t)ni[5]) << 40;
        case 5:
            b |= ((uint64_t)ni[4]) << 32;
        case 4:
            b |= ((uint64_t)ni[3]) << 24;
        case 3:
            b |= ((uint64_t)ni[2]) << 16;
        case 2:
            b |= ((uint64_t)ni[1]) << 8;
        case 1:
            b |= ((uint64_t)ni[0]);
            break;
        case 0:
            break;
        }

        state.v[3] ^= b;

        for (std::size_t i = 0; i < crounds; ++i)
            sip_round(state);

        state.v[0] ^= b;

        if (outlen == 16)
            state.v[2] ^= 0xee;
        else
            state.v[2] ^= 0xff;

        for (std::size_t i = 0; i < drounds; ++i)
            sip_round(state);

        b = state.v[0] ^ state.v[1] ^ state.v[2] ^ state.v[3];
        detail::sip_u64u8le(b, out.data());

        if (outlen == 8)
            return;

        state.v[1] ^= 0xdd;

        for (std::size_t i = 0; i < drounds; ++i)
            sip_round(state);

        b = state.v[0] ^ state.v[1] ^ state.v[2] ^ state.v[3];
        detail::sip_u64u8le(b, out.data() + 8);
    }

} // namespace ivy

#endif // IVY_SIPHASH_HXX_INCLUDED
