/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_BUFFER_RANGE_BUFFER_HXX_INCLUDED
#define IVY_BUFFER_RANGE_BUFFER_HXX_INCLUDED

#include <cstddef>
#include <cstring>
#include <ranges>
#include <span>

#include <ivy/buffer.hxx>

namespace ivy {

    /*************************************************************************
     *
     * range buffer: a buffer that exposes a range of `T` as a writable buffer,
     * or a range of `const T` as a readable buffer.  This can be used to wrap
     * existing ranges, e.g. to write directly into the destination.
     *
     */

    /*************************************************************************
     *
     * readable_range_buffer: a readable_buffer that exposes data from a
     * contiguous range.
     */
    template <std::ranges::contiguous_range Range>
    struct readable_range_buffer {
        using value_type =
            std::remove_const_t<std::ranges::range_value_t<Range>>;
        using const_value_type = std::add_const_t<value_type>;
        using size_type = std::ranges::range_size_t<Range>;

        std::span<const_value_type> read_window;

        explicit readable_range_buffer(Range const &range) : read_window(range)
        {
        }

        auto read(value_type *dptr, size_type dsize) -> size_type
        {
            auto can_read = std::min(dsize, read_window.size());

            auto will_read = read_window.subspan(0, can_read);
            auto const *read_data = std::ranges::data(will_read);
            auto read_size = std::ranges::size(will_read);
            std::memcpy(dptr, read_data, read_size);

            discard(can_read);
            return can_read;
        }

        auto readable_ranges() -> std::vector<std::span<const_value_type>>
        {
            return {read_window};
        }

        auto discard(size_type n) -> size_type
        {
            auto will_discard = std::min(n, read_window.size());
            read_window = read_window.subspan(will_discard);
            return will_discard;
        }
    };

    static_assert(readable_buffer<readable_range_buffer<std::span<char>>>);

    // Create a readable_range_buffer from a range.
    template <std::ranges::contiguous_range Range>
    auto make_readable_range_buffer(Range &&range)
    {
        using range_type = std::remove_reference_t<decltype(range)>;
        return readable_range_buffer<range_type>(range);
    }

    /*************************************************************************
     *
     * writable_range_buffer: a writable_buffer that exposes data from a
     * contiguous range.
     */
    template <std::ranges::contiguous_range Range>
    struct writable_range_buffer {
        using value_type = std::ranges::range_value_t<Range>;
        using const_value_type = std::add_const_t<value_type>;
        using size_type = typename std::span<value_type>::size_type;

        std::span<value_type> write_window;

        explicit writable_range_buffer(Range &range) : write_window(range) {}

        auto write(const_value_type *dptr, size_type dsize) -> size_type
        {
            auto can_write = std::min(write_window.size(), dsize);
            std::span will_write(dptr, can_write);
            std::ranges::copy(will_write, write_window.begin());
            commit(can_write);
            return can_write;
        }

        auto writable_ranges() -> std::vector<std::span<value_type>>
        {
            return {write_window};
        }

        auto commit(size_type n) -> size_type
        {
            auto will_commit = std::min(write_window.size(), n);
            write_window = write_window.subspan(will_commit);
            return will_commit;
        }
    };

    static_assert(writable_buffer<writable_range_buffer<std::span<char>>>);

    // Create a writable_range_buffer from a range.
    template <std::ranges::contiguous_range Range>
    auto make_writable_range_buffer(Range &&range)
    {
        using range_type = std::remove_reference_t<decltype(range)>;
        return writable_range_buffer<range_type>(range);
    }

} // namespace ivy

#endif // IVY_BUFFER_RANGE_BUFFER_HXX_INCLUDED
