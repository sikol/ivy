/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <ivy/crypto/hash.hxx>
#include <ivy/scope_guard.hxx>
#include <ivy/win32/bcrypt.hxx>
#include <ivy/win32/error.hxx>
#include <ivy/win32/windows.hxx>

#include <bcrypt.h>
#include <ntstatus.h>

namespace ivy {

    namespace detail {

        /*
         * TODO: This could be improved by caching the algorithm handle or,
         * on Windows 10, using the predefined pseudohandle.
         */

        struct hash_engine {
            bcrypt_hash hash;
        };

        auto hash_engine_deleter::operator()(hash_engine *e) noexcept -> void
        {
            delete e;
        }

        auto hash_algorithm_to_bcrypt_id(hash_algorithm algorithm)
            -> expected<LPCWSTR, std::error_code>
        {
            switch (algorithm) {
            case hash_algorithm::md5:
                return BCRYPT_MD5_ALGORITHM;

            case hash_algorithm::sha1:
                return BCRYPT_SHA1_ALGORITHM;

            case hash_algorithm::sha256:
                return BCRYPT_SHA256_ALGORITHM;

            case hash_algorithm::sha384:
                return BCRYPT_SHA384_ALGORITHM;

            case hash_algorithm::sha512:
                return BCRYPT_SHA512_ALGORITHM;

            default:
                return make_unexpected(
                    std::make_error_code(std::errc::invalid_argument));
            }
        }

    } // namespace detail

    auto hash_pbkdf2(hash_algorithm algorithm,
                     std::span<std::uint8_t const> password,
                     std::span<std::uint8_t const> salt,
                     std::uint64_t iterations,
                     std::span<std::uint8_t> output)
        -> expected<void, std::error_code>
    {
        auto bcrypt_id = detail::hash_algorithm_to_bcrypt_id(algorithm);
        if (!bcrypt_id)
            return make_unexpected(bcrypt_id.error());

        BCRYPT_ALG_HANDLE algo_handle;

        auto status = ::BCryptOpenAlgorithmProvider(
            &algo_handle, *bcrypt_id, nullptr, BCRYPT_ALG_HANDLE_HMAC_FLAG);

        if (status != STATUS_SUCCESS)
            return make_unexpected(win32::make_nt_error(status));

        scope_guard algo_guard(
            [&]() { ::BCryptCloseAlgorithmProvider(algo_handle, 0); });

        auto *pw_bytes = const_cast<UCHAR *>(password.data());
        if (password.size() > std::numeric_limits<ULONG>::max())
            return make_unexpected(
                std::make_error_code(std::errc::value_too_large));
        auto pw_size = static_cast<ULONG>(password.size());

        auto *salt_bytes = const_cast<UCHAR *>(salt.data());
        if (salt.size() > std::numeric_limits<ULONG>::max())
            return make_unexpected(
                std::make_error_code(std::errc::value_too_large));
        auto salt_size = static_cast<ULONG>(salt.size());

        auto *key_bytes = output.data();
        if (output.size() > std::numeric_limits<ULONG>::max())
            return make_unexpected(
                std::make_error_code(std::errc::value_too_large));
        auto key_size = static_cast<ULONG>(output.size());

        status = ::BCryptDeriveKeyPBKDF2(algo_handle,
                                         pw_bytes,
                                         pw_size,
                                         salt_bytes,
                                         salt_size,
                                         iterations,
                                         key_bytes,
                                         key_size,
                                         0);
        if (status != STATUS_SUCCESS)
            return make_unexpected(win32::make_nt_error(status));

        return {};
    }

    auto hash_available(hash_algorithm algorithm) -> bool
    {
        auto bcrypt_algorithm = detail::hash_algorithm_to_bcrypt_id(algorithm);
        if (!bcrypt_algorithm)
            return false;

        return bcrypt_hash::supported(*bcrypt_algorithm);
    }

    auto hash_create(hash_algorithm algorithm)
        -> expected<hash_handle, std::error_code>
    {
        auto bcrypt_algo = detail::hash_algorithm_to_bcrypt_id(algorithm);

        if (!bcrypt_algo)
            return make_unexpected(bcrypt_algo.error());

        // Allocate the engine and handle.
        auto *engine_ = new (std::nothrow) detail::hash_engine;
        if (!engine_)
            return make_unexpected(
                std::make_error_code(std::errc::not_enough_memory));

        auto engine = hash_handle(engine_);

        // Create the hash.
        auto hash = bcrypt_hash::make(*bcrypt_algo);
        if (!hash)
            return make_unexpected(hash.error());

        engine->hash = std::move(*hash);

        return std::move(engine);
    }

    auto hash_create_hmac(hash_algorithm algorithm,
                          std::span<std::uint8_t const> secret)
        -> expected<hash_handle, std::error_code>
    {
        auto bcrypt_algo = detail::hash_algorithm_to_bcrypt_id(algorithm);

        if (!bcrypt_algo)
            return make_unexpected(bcrypt_algo.error());

        // Allocate the engine and handle.
        auto *engine_ = new (std::nothrow) detail::hash_engine;
        if (!engine_)
            return make_unexpected(
                std::make_error_code(std::errc::not_enough_memory));

        auto engine = hash_handle(engine_);

        // Create the hash.
        auto hash = bcrypt_hash::make(*bcrypt_algo, secret);
        if (!hash)
            return make_unexpected(hash.error());

        engine->hash = std::move(*hash);

        return std::move(engine);
    }

    auto hash_update(hash_handle &h, std::span<std::byte const> data)
        -> expected<void, std::error_code>
    {
        return h->hash.add(data);
    }

    auto hash_finish(hash_handle &h, std::span<std::byte> output)
        -> expected<std::size_t, std::error_code>
    {
        return h->hash.finish(output);
    }

    hash::hash() = default;
    hash::hash(hash_handle &&engine_) noexcept : engine(std::move(engine_)) {}
    hash::hash(hash &&other) noexcept : engine(std::move(other.engine)) {}
    hash::~hash() = default;

    auto hash::operator=(hash &&other) noexcept -> hash &
    {
        if (&other != this) {
            engine = std::move(other.engine);
        }

        return *this;
    }

    auto hash::update(std::span<std::byte const> data) noexcept
        -> expected<void, std::error_code>
    {
        return engine->hash.add(data);
    }

    auto hash::finish(std::span<std::byte> output) noexcept
        -> expected<std::size_t, std::error_code>
    {
        return engine->hash.finish(output);
    }

} // namespace ivy
