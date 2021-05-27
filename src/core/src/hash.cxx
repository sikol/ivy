/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <random>

#include <ivy/hash.hxx>
#include <ivy/static_vector.hxx>

namespace ivy {

    namespace detail {

        auto generate_sip_hash_key() -> static_vector<std::byte, 16>
        {
            static_vector<std::byte, 16> bytes;

            std::random_device rd;
            std::uniform_int_distribution<unsigned> dist(0, 255);
            for (unsigned i = 0; i < bytes.capacity(); ++i)
                bytes.push_back(static_cast<std::byte>(dist(rd)));

            return bytes;
        }

        auto get_sip_hash_key() -> static_vector<std::byte, 16> const &
        {
            static auto key = generate_sip_hash_key();
            return key;
        }

    } // namespace detail

} // namespace ivy
