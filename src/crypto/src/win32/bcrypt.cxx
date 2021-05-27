/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <ivy/win32/bcrypt.hxx>
#include <ivy/win32/error.hxx>

#include <ntstatus.h>

namespace ivy {

    auto bcrypt_hash::supported(LPCWSTR algorithm_id) -> bool
    {
        BCRYPT_ALG_HANDLE algorithm;
        auto status =
            ::BCryptOpenAlgorithmProvider(&algorithm, algorithm_id, nullptr, 0);

        if (status != STATUS_SUCCESS)
            return false;

        ::BCryptCloseAlgorithmProvider(algorithm, 0);
        return true;
    }

    auto
    bcrypt_hash::make(LPCWSTR hash_name,
                      std::optional<std::span<std::uint8_t const>> hmac_secret)
        -> expected<bcrypt_hash, std::error_code>
    {
        bcrypt_hash hash;

        // Open the algorithm.

        DWORD algorithm_flags = 0;
        if (hmac_secret)
            algorithm_flags |= BCRYPT_ALG_HANDLE_HMAC_FLAG;

        auto status = ::BCryptOpenAlgorithmProvider(
            &hash.algorithm, hash_name, nullptr, algorithm_flags);

        if (status != STATUS_SUCCESS)
            return make_unexpected(win32::make_nt_error(status));

        // Fetch the hash object size.

        DWORD hash_object_length{};
        ULONG n{};

        status = ::BCryptGetProperty(hash.algorithm,
                                     BCRYPT_OBJECT_LENGTH,
                                     (PUCHAR)&hash_object_length,
                                     sizeof(hash_object_length),
                                     &n,
                                     0);

        if (status != STATUS_SUCCESS)
            return make_unexpected(win32::make_nt_error(status));

        // Allocate the hash object.

        hash.hash_object.reset(new (std::nothrow) UCHAR[hash_object_length]);
        if (!hash.hash_object)
            return make_unexpected(
                std::make_error_code(std::errc::not_enough_memory));

        // Create the hash.

        PUCHAR hmac_key{};
        ULONG hmac_key_size{};
        if (hmac_secret) {
            hmac_key = const_cast<UCHAR *>(reinterpret_cast<UCHAR const *>(hmac_secret->data()));

            if (hmac_secret->size() > std::numeric_limits<ULONG>::max())
                return make_unexpected(
                    std::make_error_code(std::errc::value_too_large));

            hmac_key_size = static_cast<ULONG>(hmac_secret->size());
        }

        status = ::BCryptCreateHash(hash.algorithm,
                                    &hash.hash,
                                    hash.hash_object.get(),
                                    hash_object_length,
                                    hmac_key,
                                    hmac_key_size,
                                    0);

        if (status != STATUS_SUCCESS)
            return make_unexpected(win32::make_nt_error(status));

        return hash;
    }

    bcrypt_hash::bcrypt_hash() = default;

    bcrypt_hash::~bcrypt_hash()
    {
        if (algorithm)
            ::BCryptCloseAlgorithmProvider(algorithm, 0);

        if (hash)
            ::BCryptDestroyHash(hash);
    }

    bcrypt_hash::bcrypt_hash(bcrypt_hash &&other) noexcept
        : algorithm(std::exchange(other.algorithm, nullptr)),
          hash(std::exchange(other.hash, nullptr)),
          hash_object(std::move(other.hash_object))
    {
    }

    auto bcrypt_hash::operator=(bcrypt_hash &&other) noexcept -> bcrypt_hash &
    {
        if (&other != this) {
            algorithm = std::exchange(other.algorithm, nullptr);
            hash = std::exchange(other.hash, nullptr);
            hash_object = std::move(other.hash_object);
        }

        return *this;
    }

    auto bcrypt_hash::add(std::span<std::byte const> data) noexcept
        -> expected<void, std::error_code>
    {
        auto bytes =
            const_cast<UCHAR *>(reinterpret_cast<UCHAR const *>(data.data()));

        if (data.size() > std::numeric_limits<ULONG>::max())
            return make_unexpected(
                std::make_error_code(std::errc::value_too_large));

        ULONG size = static_cast<ULONG>(data.size());

        auto status = ::BCryptHashData(hash, bytes, size, 0);
        if (status != STATUS_SUCCESS)
            return make_unexpected(win32::make_nt_error(status));

        return {};
    }

    auto bcrypt_hash::finish(std::span<std::byte> result) noexcept
        -> expected<std::size_t, std::error_code>
    {
        DWORD hash_length{};
        ULONG n{};

        auto status = ::BCryptGetProperty(algorithm,
                                          BCRYPT_HASH_LENGTH,
                                          (PUCHAR)&hash_length,
                                          sizeof(hash_length),
                                          &n,
                                          0);

        if (status != STATUS_SUCCESS)
            return make_unexpected(win32::make_nt_error(status));

        if (result.size() < hash_length)
            return make_unexpected(
                std::make_error_code(std::errc::value_too_large));

        auto bytes = reinterpret_cast<UCHAR *>(result.data());

        status = ::BCryptFinishHash(hash, bytes, hash_length, 0);
        if (status != STATUS_SUCCESS)
            return make_unexpected(win32::make_nt_error(status));

        return hash_length;
    }

} // namespace ivy
