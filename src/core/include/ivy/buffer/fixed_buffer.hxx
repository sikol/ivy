/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_BUFFER_FIXED_BUFFER_HXX_INCLUDED
#define IVY_BUFFER_FIXED_BUFFER_HXX_INCLUDED

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <ranges>
#include <span>
#include <type_traits>

#include <ivy/buffer/buffer.hxx>
#include <ivy/static_vector.hxx>

namespace ivy {

    /*************************************************************************
     *
     * A fixed_buffer contains a fixed-size memory region.  Reads fill up the
     * buffer, and writes drain it, while moving the read and write windows
     * from the start of the buffer to the end.  Once the entire buffer has
     * been filled, it cannot be used until reset() is called to return it to
     * the empty state.
     *
     */

    template <typename Char, std::size_t buffer_size>
    struct fixed_buffer {
        using array_type = std::array<Char, buffer_size>;
        using value_type = typename array_type::value_type;
        using const_value_type = std::add_const_t<value_type>;
        using iterator = typename array_type::iterator;
        using size_type = typename array_type::size_type;

        // The data stored in this buffer.
        array_type data;
        typename array_type::iterator read_pointer = data.begin();
        typename array_type::iterator write_pointer = data.begin();

        // Create a new, empty buffer.
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
        fixed_buffer() = default;
        ~fixed_buffer() = default;

        // fixed_buffer cannot be copied or moved, because the buffer contains
        // the entire std::array<>.  In principle we could allow copying, but
        // copying a buffer is almost certainly a programming error.

        fixed_buffer(fixed_buffer const &) = delete;
        auto operator=(fixed_buffer const &) -> fixed_buffer & = delete;
        fixed_buffer(fixed_buffer &&) noexcept = delete;
        auto operator=(fixed_buffer &&) noexcept -> fixed_buffer & = delete;

        // reset(): reset this extent to empty, discarding any data it contains.
        auto reset() -> void;

        // write(): write some data into this buffer and adjust the write
        // window.  Returns the number of objects written.  If the buffer is too
        // small to contain all the data, the return value will be less than the
        // range size.
        auto write(const_value_type *dptr, size_type dsize) -> size_type;

        // commit(n): mark n objects at the start of the buffer as being
        // readable data.
        auto commit(size_type n) -> size_type;

        // read(): read some data from this buffer and adjust the read window.
        // Returns the number of bytes copied.  If the buffer doesn't have
        // enough data to satisfy the request, the return value will be less
        // than the range size.
        auto read(value_type *dptr, size_type dsize) -> size_type;

        // discard(n): remove up to n characters from the read window.  Returns
        // the number of characters removed.
        auto discard(size_type n) -> size_type;

        // Return our read window.
        auto readable_ranges() -> static_vector<std::span<const_value_type>, 1>;

        // Return our write window.
        auto writable_ranges() -> static_vector<std::span<value_type>, 1>;

    private:
        [[nodiscard]] auto _end() -> typename array_type::iterator
        {
            return data.begin() + buffer_size;
        }

        [[nodiscard]] auto _writable() -> size_type
        {
            return std::distance(write_pointer, _end());
        }

        [[nodiscard]] auto _readable() -> size_type
        {
            return std::distance(read_pointer, write_pointer);
        }
    };

    template <typename Char, std::size_t buffer_size>
    auto fixed_buffer<Char, buffer_size>::reset() -> void
    {
        read_pointer = data.begin();
        write_pointer = data.begin();
    }

    template <typename Char, std::size_t buffer_size>
    auto fixed_buffer<Char, buffer_size>::write(const_value_type *dptr,
                                                size_type dsize) -> size_type
    {
        dsize = std::min(dsize, _writable());
        write_pointer = std::copy(dptr, dptr + dsize, write_pointer);
        return dsize;
    }

    /*
     * fixed_buffer::commit()
     */
    template <typename Char, std::size_t buffer_size>
    auto fixed_buffer<Char, buffer_size>::commit(size_type n) -> size_type
    {
        n = std::min(n, _writable());
        write_pointer += n;
        return n;
    }

    /*
     * fixed_buffer::read()
     */
    template <typename Char, std::size_t buffer_size>
    auto fixed_buffer<Char, buffer_size>::read(value_type *dptr,
                                               size_type dsize) -> size_type
    {
        dsize = std::min(dsize, _readable());
        std::copy(read_pointer, read_pointer + dsize, dptr);
        read_pointer += dsize;
        return dsize;
    }

    /*
     * fixed_buffer::discard()
     */
    template <typename Char, std::size_t buffer_size>
    auto fixed_buffer<Char, buffer_size>::discard(size_type n) -> size_type
    {
        n = std::min(n, _readable());
        read_pointer += n;
        return n;
    }

    /*
     * fixed_buffer::readable_ranges()
     */
    template <typename Char, std::size_t buffer_size>
    auto fixed_buffer<Char, buffer_size>::readable_ranges()
        -> static_vector<std::span<const_value_type>, 1>
    {
        return {std::span(read_pointer, write_pointer)};
    }

    /*
     * fixed_buffer::writable_ranges()
     */
    template <typename Char, std::size_t buffer_size>
    auto fixed_buffer<Char, buffer_size>::writable_ranges()
        -> static_vector<std::span<value_type>, 1>
    {
        return {std::span(write_pointer, data.end())};
    }

    // NOLINT(cppcoreguidelines-avoid-magic-numbers)
    static_assert(buffer<fixed_buffer<char, 4096>>);

} // namespace ivy

#endif // IVY_BUFFER_FIXED_BUFFER_HXX_INCLUDED
