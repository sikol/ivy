/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CRYPTO_SHA256_HXX_INCLUDED
#define IVY_CRYPTO_SHA256_HXX_INCLUDED

#include <concepts>
#include <cstddef>
#include <memory>
#include <ranges>
#include <span>

#include <ivy/expected.hxx>

namespace ivy {

    enum struct hash_algorithm : std::uint8_t {
        sha1,
        sha224,
        sha256,
        sha384,
        sha512
    };

    template <hash_algorithm algorithm>
    struct digest_length;

    template <>
    struct digest_length<hash_algorithm::sha1> {
        static constexpr std::size_t value = 20;
    };

    template <>
    struct digest_length<hash_algorithm::sha224> {
        static constexpr std::size_t value = 28;
    };

    template <>
    struct digest_length<hash_algorithm::sha256> {
        static constexpr std::size_t value = 32;
    };

    template <>
    struct digest_length<hash_algorithm::sha384> {
        static constexpr std::size_t value = 48;
    };

    template <>
    struct digest_length<hash_algorithm::sha512> {
        static constexpr std::size_t value = 64;
    };

    template <hash_algorithm algorithm>
    inline constexpr std::size_t digest_length_v =
        digest_length<algorithm>::value;

    inline constexpr std::size_t max_digest_length = 64;

    namespace detail {

        struct hash_engine;

        struct hash_engine_deleter {
            auto operator()(hash_engine *) noexcept -> void;
        };

    } // namespace detail

    using hash_handle =
        std::unique_ptr<detail::hash_engine, detail::hash_engine_deleter>;

    auto hash_available(hash_algorithm) -> bool;
    auto hash_create(hash_algorithm) -> expected<hash_handle, std::error_code>;
    auto hash_update(hash_handle &, std::span<std::byte const> data)
        -> expected<void, std::error_code>;
    auto hash_finish(hash_handle &, std::span<std::byte> output)
        -> expected<std::size_t, std::error_code>;

    class hash {
    private:
        hash_handle engine;

    public:
        hash();
        hash(hash_handle &&engine) noexcept;
        hash(hash &&other) noexcept;
        hash(hash const &) = delete;
        ~hash();

        auto operator=(hash &&) noexcept -> hash &;
        auto operator=(hash const &) -> hash & = delete;

        auto update(std::span<std::byte const> data) noexcept
            -> expected<void, std::error_code>;

        auto finish(std::span<std::byte> output) noexcept
            -> expected<std::size_t, std::error_code>;
    };

    template <typename T>
    class hash_iterator {
        hash *_hash;

    public:
        hash_iterator(hash &) noexcept;

        auto operator=(T const &) -> hash_iterator &;
        auto operator++() noexcept -> hash_iterator &;
        auto operator++(int) noexcept -> hash_iterator &;
    };

    template <typename T>
    hash_iterator<T>::hash_iterator(hash &h) noexcept : _hash(&h)
    {
    }

    template <typename T>
    auto hash_iterator<T>::operator=(T const &o) -> hash_iterator &
    {
        auto bytes = as_bytes(std::span(&o, 1));
        auto r = _hash->update(o);

        if (!r)
            throw std::system_error(r.error());

        return *this;
    }

    template <typename T>
    auto hash_iterator<T>::operator++() noexcept -> hash_iterator &
    {
        return *this;
    }

    template <typename T>
    auto hash_iterator<T>::operator++(int) noexcept -> hash_iterator &
    {
        return *this;
    }

    template <std::ranges::contiguous_range Range>
    auto hash_data(hash_algorithm algorithm,
                   Range &&data,
                   std::span<std::byte> output) noexcept
        -> expected<std::size_t, std::error_code>
    {
        auto hash = hash_create(algorithm);
        if (!hash)
            return make_unexpected(hash.error());

        auto bytes = as_bytes(std::span(data));
        if (auto ok = hash_update(*hash, bytes); !ok)
            return make_unexpected(ok.error());

        if (auto nbytes = hash_finish(*hash, output); !nbytes)
            return make_unexpected(nbytes.error());
        else
            return *nbytes;
    }

    template <std::ranges::contiguous_range Range,
              std::output_iterator<std::byte> OutputIterator>
    auto hash_data(hash_algorithm algorithm,
                   Range &&data,
                   OutputIterator out) noexcept
        -> expected<std::size_t, std::error_code>
    {
        std::array<std::byte, max_digest_length> digest_buffer;

        auto nbytes =
            hash_data(algorithm, std::forward<Range>(data), digest_buffer);
        if (!nbytes)
            return make_unexpected(nbytes.error());

        auto digest = std::span(digest_buffer).subspan(0, *nbytes);

        std::ranges::copy(digest, out);
        return *nbytes;
    }

} // namespace ivy

#endif // IVY_CRYPTO_SHA256_HXX_INCLUDED
