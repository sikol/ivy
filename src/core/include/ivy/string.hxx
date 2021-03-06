/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_STRING_HXX_INCLUDED
#define IVY_STRING_HXX_INCLUDED

#include <atomic>
#include <bit>
#include <compare>
#include <concepts>
#include <iostream>
#include <iterator>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include <ivy/charenc.hxx>
#include <ivy/charenc/ascii.hxx>
#include <ivy/charenc/system.hxx>
#include <ivy/charenc/system_wide.hxx>
#include <ivy/charenc/utf16.hxx>
#include <ivy/charenc/utf32.hxx>
#include <ivy/charenc/utf8.hxx>
#include <ivy/expected.hxx>
#include <ivy/hash.hxx>

namespace ivy {

    namespace detail {

        template <character_encoding encoding, typename allocator>
        struct string_storage {
            using char_type = typename encoding::char_type;

            std::vector<char_type, allocator> data;

            string_storage() = default;

            string_storage(std::vector<char_type, allocator> &&vec)
                : data(std::move(vec))
            {
                data.push_back(typename encoding::char_type(0));
            }

            template <std::ranges::range R>
            string_storage(R &&r)
                : data(std::ranges::begin(r), std::ranges::end(r))
            {
                data.push_back(typename encoding::char_type(0));
            }

            template <std::input_iterator iterator>
            string_storage(iterator begin, iterator end)
                : data(begin, end)
            {
                data.push_back(typename encoding::char_type(0));
            }
        };

    } // namespace detail

    template <character_encoding Encoding,
              typename allocator = std::allocator<typename Encoding::char_type>>
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

        inline static size_type npos = static_cast<size_type>(-1);

    private:
        using storage_type = detail::string_storage<Encoding, allocator>;
        mutable std::shared_ptr<storage_type> _storage;
        mutable size_type _start = 0;
        size_type _len = 0;

        auto _rematerialize() const -> void;

    public:
        basic_string();
        basic_string(basic_string const &other);
        basic_string(basic_string &&other) noexcept;
        basic_string(value_type const *);
        basic_string(value_type const *, size_type size);
        basic_string(const_iterator begin, const_iterator end);
        basic_string(std::vector<value_type, allocator> &&vec);

        template <std::input_iterator input_iterator>
        basic_string(input_iterator first, input_iterator last) requires
            std::same_as<std::iter_value_t<input_iterator>, value_type>;

        template <std::ranges::range range>
        basic_string(range &&r) requires
            std::same_as<std::ranges::range_value_t<range>, value_type>;

        auto operator=(basic_string const &other) -> basic_string &;
        auto operator=(basic_string &&other) noexcept -> basic_string &;

        auto at(size_type i) const -> value_type;
        auto operator[](size_type i) const noexcept -> value_type;

        auto empty() const noexcept -> bool;
        auto size() const noexcept -> size_type;
        auto data() const noexcept -> const_pointer;
        auto c_str() const noexcept -> const_pointer;

        auto substr(size_type begin = 0u, size_type len = npos) const noexcept
            -> basic_string;

        auto begin() const noexcept -> const_iterator;
        auto end() const noexcept -> const_iterator;
        auto cbegin() const noexcept -> const_iterator;
        auto cend() const noexcept -> const_iterator;
    };

    // System narrow (multibyte) string - may not be Unicode.
    using cstring = basic_string<system_encoding>;

    // ASCII string.
    using astring = basic_string<ascii_encoding>;

    // System wide (wchar_t) string - may not be Unicode.
    using wstring = basic_string<system_wide_encoding>;

    // UTF-8-encoded string.
    using u8string = basic_string<utf8_encoding>;

    // UTF-16-encoded string.
    using u16string = basic_string<utf16_encoding>;

    // UTF-32-encoded string.
    using u32string = basic_string<utf32_encoding>;

    // Ivy's native string type; currently this is UTF-32, because many C++ APIs
    // are designed on the assumption that one char value is one character.  For
    // example, std::basic_regex cannot be used with UTF-8 or UTF-16 strings.
    //
    // In the future we might want to consider a different approach, such as
    // using UTF-8 for storage and providing an interface to iterate over
    // characters.
    using string = u32string;

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
        : _storage(std::make_shared<storage_type>(std::span(s, s + len)))
        , _start(0)
        , _len(len)
    {
    }

    template <character_encoding Encoding, typename Alloc>
    basic_string<Encoding, Alloc>::basic_string(const_iterator begin,
                                                const_iterator end)
        : _storage(std::make_shared<storage_type>(begin, end))
        , _start(0)
        , _len(_storage->data.size() - 1)
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
        : _storage(std::make_shared<storage_type>(std::move(vec)))
        , _start(0)
        , _len(_storage->data.size() - 1)
    {
    }

    template <character_encoding Encoding, typename Alloc>
    template <std::input_iterator input_iterator>
    basic_string<Encoding, Alloc>::basic_string(input_iterator first,
                                                input_iterator last) requires
        std::same_as<std::iter_value_t<input_iterator>, value_type>
        : _storage(std::make_shared<storage_type>(std::ranges::subrange(first,
                                                                        last))),
          _start(0),
          _len(_storage->data.size() - 1)
    {
    }

    template <character_encoding Encoding, typename Alloc>
    template <std::ranges::range range>
    basic_string<Encoding, Alloc>::basic_string(range &&r) requires
        std::same_as<std::ranges::range_value_t<range>, value_type>
        : basic_string(std::ranges::begin(r), std::ranges::end(r))
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
        _start = 0;
    }

    template <character_encoding Encoding, typename Alloc>
    auto basic_string<Encoding, Alloc>::at(std::size_t i) const -> value_type
    {
        if (i >= size())
            throw std::out_of_range("string index out of range");

        return _storage->data[_start + i];
    }

    template <character_encoding Encoding, typename Alloc>
    auto basic_string<Encoding, Alloc>::empty() const noexcept -> bool
    {
        return _len == 0;
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
    auto basic_string<Encoding, Alloc>::substr(size_type begin,
                                               size_type len) const noexcept
        -> basic_string
    {
        auto ret(*this);

        IVY_CHECK(begin <= size(),
                  "ivy::basic_string::substr: start out of range");

        if (begin >= size()) {
            ret._start = 0;
            ret._len = 0;
        } else {
            if (len == npos)
                len = size() - begin;

            if (begin + len > size())
                len = size() - begin;

            ret._start += begin;
            ret._len = len;
        }

        return ret;
    }

    template <character_encoding Encoding, typename Alloc>
    auto basic_string<Encoding, Alloc>::begin() const noexcept -> const_iterator
    {
        if (_len == 0)
            return nullptr;

        return _storage->data.data() + _start;
    }

    template <character_encoding Encoding, typename Alloc>
    auto basic_string<Encoding, Alloc>::end() const noexcept -> const_iterator
    {
        if (_len == 0)
            return nullptr;

        return _storage->data.data() + _start + _len;
    }

    template <character_encoding Encoding, typename Alloc>
    auto basic_string<Encoding, Alloc>::cbegin() const noexcept
        -> const_iterator
    {
        return begin();
    }

    template <character_encoding Encoding, typename Alloc>
    auto basic_string<Encoding, Alloc>::cend() const noexcept -> const_iterator
    {
        return end();
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

        return std::lexicographical_compare_three_way(
            a_span.begin(), a_span.end(), b_span.begin(), b_span.end());
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

    template <typename encoding, typename alloc>
    struct hash<basic_string<encoding, alloc>> {
        auto operator()(basic_string<encoding, alloc> const &v) const noexcept
            -> std::size_t
        {
            auto bytes = as_bytes(std::span(v.data(), v.size()));
            return detail::hash_bytes(bytes);
        }
    };

    extern template class basic_string<utf8_encoding>;
    extern template class basic_string<utf16_encoding>;
    extern template class basic_string<utf32_encoding>;
    extern template class basic_string<ascii_encoding>;
    extern template class basic_string<system_encoding>;
    extern template class basic_string<system_wide_encoding>;

} // namespace ivy

namespace std {

    template <typename encoding, typename alloc>
    struct hash<ivy::basic_string<encoding, alloc>> {
        auto
        operator()(ivy::basic_string<encoding, alloc> const &v) const noexcept
            -> size_t
        {
            return ivy::hash<ivy::basic_string<encoding, alloc>>{}(v);
        }
    };

} // namespace std

#endif // IVY_STRING_HXX_INCLUDED
