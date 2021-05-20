/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

/*
 * Concepts of buffers.
 */

#ifndef IVY_BUFFER_BUFFER_HXX_INCLUDED
#define IVY_BUFFER_BUFFER_HXX_INCLUDED

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <ranges>
#include <span>
#include <type_traits>

namespace ivy {

    /*************************************************************************
     *
     * Concept of a buffer that holds objects of type Char.
     */
    template<typename Buffer>
    concept basic_buffer =
        requires() {
            typename Buffer::value_type;
            typename Buffer::const_value_type;
            typename Buffer::size_type;
        };


    /*************************************************************************
     *
     * Concept of a buffer that can be read from.
     */
    template<typename Buffer>
    concept readable_buffer =
        basic_buffer<Buffer>
        and requires(Buffer &b,
                     typename Buffer::value_type *data,
                     typename Buffer::size_type n) {

            // Copy data out of the buffer.
            { b.read(data, n) } -> std::same_as<typename Buffer::size_type>;

            // Get the buffer's readable extents.
            { b.readable_ranges() } -> std::ranges::range<>;

            // Remove data from the start of the buffer.
            { b.discard(n) } -> std::same_as<typename Buffer::size_type>;
        };

    /*************************************************************************
     *
     * Concept of a buffer that can be written to.
     */
    template<typename Buffer>
    concept writable_buffer =
        basic_buffer<Buffer>
        and requires(Buffer &b,
                     typename Buffer::value_type const *data,
                     typename Buffer::size_type n) {

            // Copy data into the buffer.
            { b.write(data, n) } -> std::same_as<typename Buffer::size_type>;

            // Get the buffer's writable extents.
            { b.writable_ranges() } -> std::ranges::range<>;

            // Mark empty space as readable.
            { b.commit(n) } -> std::same_as<typename Buffer::size_type>;

        };

    /*************************************************************************
     *
     * buffer: concept of a buffer that can be both read from and written to.
     */

    template<typename Buffer>
    concept buffer =
        readable_buffer<Buffer>
        and writable_buffer<Buffer>;

    // clang-format on

    /*************************************************************************
     *
     * Buffer utilities.
     */

    // Get the size type of a buffer.
    template <basic_buffer Buffer>
    using buffer_size_t = typename std::remove_reference_t<Buffer>::size_type;

    // Get the value type of a buffer.
    template <basic_buffer Buffer>
    using buffer_value_t = typename std::remove_reference_t<Buffer>::value_type;

    // Get the const value type of a butter.
    template <basic_buffer Buffer>
    using buffer_const_value_t =
        typename std::remove_reference_t<Buffer>::const_value_type;

    /*************************************************************************
     *
     * buffer_write(): append data to a buffer.
     *
     */

    // clang-format off
    template<writable_buffer Buffer, std::ranges::contiguous_range Range>
    auto buffer_write(Buffer &buf, Range const &range)
    requires std::same_as<std::remove_cvref_t<buffer_value_t<Buffer>>,
                          std::remove_cvref_t<std::ranges::range_value_t<Range>>>
    // clang-format on
    {
        auto const *data = std::ranges::data(range);
        auto size = std::ranges::size(range);

        return buf.write(data, size);
    }

    /*************************************************************************
     *
     * buffer_read(): read data from a buffer.
     *
     */

    template<readable_buffer Buffer, std::ranges::contiguous_range Range>
    auto buffer_read(Buffer &buf, Range &&range) {
        auto size = std::ranges::size(range);
        auto *data = std::ranges::data(range);

        return buf.read(data, size);
    }

    /*************************************************************************
     * buffer_copy(from, to): append all of the data in `from` to `to`, as if
     * calling from.read(buf) then to.write(buf).
     */
    // clang-format off
    template <readable_buffer From, readable_buffer To>
    auto buffer_copy(From const &from, To &to) -> void
        requires std::same_as<buffer_value_t<From>, buffer_value_t<To>>
    // clang-format on
    {
        for (auto &&range : from.readable_ranges()) {
            to.write(range);
        }
    }

    /*************************************************************************
     * buffer_move(from, to): append all of the data in `from` to `to`, then
     * remove all the data in `from`, as if calling buffer_copy(from, to)
     * followed by from.clear();
     */
    // clang-format off
    template <readable_buffer From, writable_buffer To>
    auto buffer_move(From &from, To &to) -> void
        requires std::same_as<buffer_value_t<From>, buffer_value_t<To>>
    // clang-format on
    {
        for (auto &&range : from.readable_ranges()) {
            to.write(range);
        }

        from.clear();
    }

} // namespace sk

#endif // IVY_BUFFER_BUFFER_HXX_INCLUDED
