/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_ITERATOR_BYTE_DECODE_HXX_INCLUDED
#define IVY_ITERATOR_BYTE_DECODE_HXX_INCLUDED

#include <bit>
#include <concepts>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace ivy {

    class short_byte_error : public std::runtime_error {
    public:
        short_byte_error()
            : std::runtime_error("short byte")
        {
        }
    };

    template <std::integral T, std::output_iterator<T> iterator_type>
    class byte_decode_iterator {
        using unsigned_T = std::make_unsigned_t<T>;
        iterator_type _out{};
        unsigned_T _cur{};
        unsigned _nread{};
        std::endian _endianness = std::endian::native;

    public:
        using iterator_category = std::output_iterator_tag;
        using value_type = void;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = void;

        byte_decode_iterator() = default;

        byte_decode_iterator(std::endian endianness, iterator_type out)
            : _out(out)
            , _endianness(endianness)
        {
        }

        auto operator*() const noexcept -> byte_decode_iterator const &
        {
            return *this;
        }

        auto operator*() noexcept -> byte_decode_iterator &
        {
            return *this;
        }

        auto operator=(std::byte const &v) noexcept -> void
        {
            _cur |= to_integer<unsigned_T>(v) << (8 * _nread);
            ++_nread;

            if (_nread == sizeof(T)) {
                *_out++ = static_cast<T>(_cur);
                _cur = 0;
                _nread = 0;
            }
        }

        auto operator++() noexcept -> byte_decode_iterator &
        {
            return *this;
        }

        auto operator++(int) noexcept -> byte_decode_iterator &
        {
            return *this;
        }

        auto pending() const noexcept -> bool
        {
            if (_nread)
                return sizeof(T) - _nread;

            return 0;
        }
    };

    template <typename T, std::output_iterator<T> iterator>
    auto make_byte_decode_iterator(std::endian endianness, iterator it)
    {
        return byte_decode_iterator<T, iterator>(endianness, it);
    }

#if 0
    template <std::integral T, std::input_iterator iterator_type>
    class byte_decode_iterator {
        using unsigned_T = std::make_unsigned_t<T>;
        iterator_type _begin{};
        iterator_type _end{};
        std::endian _endianness = std::endian::native;

    public:
        using iterator_category = std::output_iterator_tag;
        using value_type = void;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = void;

        byte_decode_iterator() = default;

        byte_decode_iterator(std::endian endianness,
                             iterator_type first,
                             iterator_type last)
            : _begin(first)
            , _end(last)
            , _endianness(endianness)
        {
        }

        auto operator*() const noexcept -> byte_decode_iterator const &
        {
            return *this;
        }

        auto operator*() noexcept -> byte_decode_iterator &
        {
            return *this;
        }

        auto operator=(std::byte const &v) noexcept -> void
        {
            _cur |= to_integer<unsigned_T>(v) << (8 * _nread);
            ++_nread;

            if (_nread == sizeof(T)) {
                *_out++ = _cur;
                _cur = 0;
                _nread = 0;
            }
        }

        auto operator++() noexcept -> byte_decode_iterator &
        {
            return *this;
        }

        auto operator++(int) noexcept -> byte_decode_iterator &
        {
            return *this;
        }
    };

    template <typename T, std::input_iterator iterator>
    auto make_byte_decode_iterator(std::endian endianness, iterator it)
    {
        return byte_decode_iterator<T, iterator>(endianness, it);
    }
#endif

} // namespace ivy

#endif // IVY_ITERATOR_BYTE_DECODE_HXX_INCLUDED
