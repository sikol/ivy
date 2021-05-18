/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_STRING_HXX_INCLUDED
#define IVY_STRING_HXX_INCLUDED

#include <atomic>
#include <bit>
#include <compare>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <ivy/charenc.hxx>
#include <ivy/expected.hxx>

namespace ivy {

    namespace detail {

        template <character_encoding Encoding, typename Alloc>
        struct string_storage {
            using char_type = typename Encoding::char_type;

            std::vector<char_type, Alloc> data;

            string_storage() = default;

            string_storage(std::vector<char_type, Alloc> &&vec)
                : data(std::move(vec))
            {
                data.push_back(typename Encoding::char_type(0));
            }

            template <std::ranges::range R>
            string_storage(R &&r)
                : data(std::ranges::begin(r), std::ranges::end(r))
            {
                data.push_back(typename Encoding::char_type(0));
            }
        };

    } // namespace detail

    template <character_encoding Encoding,
              typename Alloc = std::allocator<typename Encoding::char_type>>
    class basic_string {
    public:
        using encoding_type = Encoding;
        using value_type = typename Encoding::char_type;
        using pointer = value_type *;
        using const_pointer = value_type const *;
        using reference = value_type &;
        using const_reference = value_type const &;
        using difference_type = std::make_unsigned_t<std::size_t>;
        using size_type = std::size_t;
        using iterator = const_pointer;
        using const_iterator = const_pointer;

    private:
        using storage_type = detail::string_storage<Encoding, Alloc>;
        mutable std::shared_ptr<storage_type> _storage;
        size_type _start = 0;
        size_type _len = 0;

        auto _rematerialize() const -> void;

    public:
        basic_string();
        basic_string(basic_string const &other);
        basic_string(basic_string &&other) noexcept;
        basic_string(value_type const *);
        basic_string(value_type const *, size_type size);
        basic_string(std::vector<value_type, Alloc> &&vec);

        auto operator=(basic_string const &other) -> basic_string &;
        auto operator=(basic_string &&other) noexcept -> basic_string &;

        auto at(size_type i) const -> value_type;
        auto operator[](size_type i) const noexcept -> value_type;

        auto size() const noexcept -> size_type;
        auto data() const noexcept -> const_pointer;
        auto c_str() const noexcept -> const_pointer;

        auto begin() const noexcept -> const_iterator;
        auto end() const noexcept -> const_iterator;
    };

    template <character_encoding Encoding, typename Alloc>
    basic_string<Encoding, Alloc>::basic_string() = default;

    template <character_encoding Encoding, typename Alloc>
    basic_string<Encoding, Alloc>::basic_string(basic_string const &other) =
        default;

    template <character_encoding Encoding, typename Alloc>
    basic_string<Encoding, Alloc>::basic_string(basic_string &&other) noexcept =
        default;

    template <character_encoding Encoding, typename Alloc>
    basic_string<Encoding, Alloc>::basic_string(value_type const *s,
                                                size_type len)
        : _storage(std::make_shared<storage_type>(std::span(s, s + len))),
          _start(0), _len(len)
    {
    }

    template <character_encoding Encoding, typename Alloc>
    basic_string<Encoding, Alloc>::basic_string(value_type const *s)
        : basic_string(s, Encoding::length(s))
    {
    }

    template <character_encoding Encoding, typename Alloc>
    basic_string<Encoding, Alloc>::basic_string(
        std::vector<value_type, Alloc> &&vec)
        : _storage(std::make_shared<storage_type>(std::move(vec))), _start(0),
          _len(_storage->data.size() - 1)
    {
    }

    template <character_encoding Encoding, typename Alloc>
    auto basic_string<Encoding, Alloc>::operator=(basic_string const &other)
        -> basic_string &
    {
        if (&other != this) {
            _start = other._start;
            _len = other._len;
            _storage = other._storage;
        }

        return *this;
    }

    template <character_encoding Encoding, typename Alloc>
    auto basic_string<Encoding, Alloc>::operator=(basic_string &&other) noexcept
        -> basic_string &
    {
        if (&other != this) {
            _start = std::exchange(other._start, 0);
            _len = std::exchange(other._len, 0);
            _storage = std::move(other._storage);
        }

        return *this;
    }

    template <character_encoding Encoding, typename Alloc>
    auto basic_string<Encoding, Alloc>::operator[](std::size_t i) const noexcept
        -> value_type
    {
        IVY_CHECK(i < size(), "ivy::basic_string::op[]: out of range");
        return _storage->data[_start + i];
    }

    template <character_encoding Encoding, typename Alloc>
    auto basic_string<Encoding, Alloc>::_rematerialize() const -> void
    {
        _storage =
            std::make_shared<storage_type>(std::span(data(), data() + size()));
    }

    template <character_encoding Encoding, typename Alloc>
    auto basic_string<Encoding, Alloc>::at(std::size_t i) const -> value_type
    {
        if (i >= size())
            throw std::out_of_range("string index out of range");

        return _storage->data[_start + i];
    }

    template <character_encoding Encoding, typename Alloc>
    auto basic_string<Encoding, Alloc>::size() const noexcept -> size_type
    {
        return _len;
    }

    template <character_encoding Encoding, typename Alloc>
    auto basic_string<Encoding, Alloc>::data() const noexcept -> const_pointer
    {
        return &_storage->data[_start];
    }

    template <character_encoding Encoding, typename Alloc>
    auto basic_string<Encoding, Alloc>::c_str() const noexcept -> const_pointer
    {
        if (_storage->data[_start + _len] != typename Encoding::char_type(0))
            _rematerialize();

        return data();
    }

    template <character_encoding Encoding, typename Alloc>
    auto basic_string<Encoding, Alloc>::begin() const noexcept -> const_iterator
    {
        return &_storage->data[0] + _start;
    }

    template <character_encoding Encoding, typename Alloc>
    auto basic_string<Encoding, Alloc>::end() const noexcept -> const_iterator
    {
        return &_storage->data[0] + _start + _len;
    }

    template <character_encoding Encoding, typename Alloc>
    auto operator==(basic_string<Encoding, Alloc> const &a,
                    basic_string<Encoding, Alloc> const &b) noexcept -> bool
    {
        auto a_span = std::span<typename Encoding::char_type const>(
            a.data(), a.data() + a.size());
        auto b_span = std::span<typename Encoding::char_type const>(
            b.data(), b.data() + b.size());

        return (a_span.size() == b_span.size()) &&
               std::ranges::equal(a_span, b_span);
    }

    template <character_encoding Encoding, typename Alloc>
    auto operator==(basic_string<Encoding, Alloc> const &a,
                    typename Encoding::char_type const *s) noexcept -> bool
    {
        auto a_span = std::span(a.data(), a.data() + a.size());
        auto b_span = std::span(s, s + Encoding::length(s));

        return (a_span.size() == b_span.size()) &&
               std::ranges::equal(a_span, b_span);
    }

    template <character_encoding Encoding, typename Alloc>
    auto operator<=>(basic_string<Encoding, Alloc> const &a,
                     basic_string<Encoding, Alloc> const &b) noexcept
        -> std::strong_ordering
    {
        auto a_span = std::span<typename Encoding::char_type const>(
            a.data(), a.data() + a.size());
        auto b_span = std::span<typename Encoding::char_type const>(
            b.data(), b.data() + b.size());

        return a_span <=> b_span;
    }

    template <character_encoding Encoding, typename Alloc>
    auto operator+(basic_string<Encoding, Alloc> const &a,
                   basic_string<Encoding, Alloc> const &b)
        -> basic_string<Encoding, Alloc>
    {
        std::vector<typename Encoding::char_type, Alloc> chars(a.size() +
                                                               b.size());
        std::ranges::copy(a, &chars[0]);
        std::ranges::copy(b, &chars[a.size()]);
        return basic_string<Encoding, Alloc>(std::move(chars));
    }

    template <typename Target,
              character_encoding SourceEncoding,
              typename Alloc>
    auto transcode(basic_string<SourceEncoding, Alloc> const &s)
        -> expected<Target, std::error_code>
    {
        using TargetEncoding = typename Target::encoding_type;

        std::vector<char32_t> utf32;

        auto r = SourceEncoding::to_char32(s, std::back_inserter(utf32));
        if (!r)
            return make_unexpected(r.error());

        std::vector<typename Target::value_type> tchars;
        r = TargetEncoding::from_char32(utf32, std::back_inserter(tchars));
        if (!r)
            return make_unexpected(r.error());

        return Target(&tchars[0], tchars.size());
    }

    template <typename Target, std::ranges::range Range>
    auto bytes_to_string(Range &&r,
                         std::endian endianness = std::endian::native)
        -> expected<Target, std::error_code>
    {
        using encoding = typename Target::encoding_type;

        auto bytes = as_bytes(std::span(r));

        std::vector<char32_t> utf32;
        auto ok =
            encoding::to_char32(bytes, endianness, std::back_inserter(utf32));
        if (!ok)
            return make_unexpected(ok.error());

        std::vector<typename Target::value_type> tchars;
        ok = encoding::from_char32(utf32, std::back_inserter(tchars));
        if (!ok)
            return make_unexpected(ok.error());

        return Target(&tchars[0], tchars.size());
    }

} // namespace ivy

#endif // IVY_STRING_HXX_INCLUDED
