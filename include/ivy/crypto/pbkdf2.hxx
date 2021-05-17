/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CRYPTO_PBKDF2_HXX_INCLUDED
#define IVY_CRYPTO_PBKDF2_HXX_INCLUDED

#include <cstddef>
#include <cstdint>
#include <span>
#include <system_error>

#include <ivy/crypto/hash.hxx>

namespace ivy {

    auto hash_pbkdf2(hash_algorithm algorithm,
                     std::span<std::uint8_t const> password,
                     std::span<std::uint8_t const> salt,
                     std::uint64_t iterations,
                     std::span<std::uint8_t> output)
        -> expected<void, std::error_code>;

} // namespace ivy

#endif // IVY_CRYPTO_PBKDF2_HXX_INCLUDED

