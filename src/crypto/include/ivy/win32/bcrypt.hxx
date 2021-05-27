/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_BCRYPT_HXX_INCLUDED
#define IVY_WIN32_BCRYPT_HXX_INCLUDED

#include <cstddef>
#include <memory>
#include <span>
#include <system_error>

#include <ivy/expected.hxx>
#include <ivy/win32/windows.hxx>

#include <bcrypt.h>

namespace ivy {

    class bcrypt_hash {
        BCRYPT_ALG_HANDLE algorithm{};
        BCRYPT_HASH_HANDLE hash{};
        std::unique_ptr<UCHAR[]> hash_object;

    public:
        bcrypt_hash();
        ~bcrypt_hash();

        bcrypt_hash(bcrypt_hash const &) = delete;
        auto operator=(bcrypt_hash const &) -> bcrypt_hash & = delete;

        bcrypt_hash(bcrypt_hash &&) noexcept;
        auto operator=(bcrypt_hash &&) noexcept -> bcrypt_hash &;

        auto add(std::span<std::byte const> data) noexcept
            -> expected<void, std::error_code>;

        auto finish(std::span<std::byte> result) noexcept
            -> expected<std::size_t, std::error_code>;

        static auto make(LPCWSTR algorithm_id, 
            std::optional<std::span<std::uint8_t const>> hmac_secret = {})
            -> expected<bcrypt_hash, std::error_code>;

        static auto supported(LPCWSTR algorithm_id) -> bool;
    };

} // namespace ivy

#endif // IVY_WIN32_BCRYPT_HXX_INCLUDED
