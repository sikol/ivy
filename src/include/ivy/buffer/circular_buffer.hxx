/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_BUFFER_CIRCULAR_BUFFER_HXX_INCLUDED
#define IVY_BUFFER_CIRCULAR_BUFFER_HXX_INCLUDED

#include <algorithm>
#include <array>
#include <cassert>
#include <ranges>
#include <span>

#include <ivy/buffer/buffer.hxx>
#include <ivy/check.hxx>
#include <ivy/static_vector.hxx>

namespace ivy {

    /*************************************************************************
     *
     * circular_buffer: a fixed-size buffer which can wrap around when it
     * reaches the end of the buffer.  Unlike fixed_buffer, which becomes
     * useless and has to be reset once the entire buffer has been written and
     * read once, circular_buffer can be continually read and written forever.
     * However, it can never contain more data at once than its fixed size.
     *
     */

    template <typename Char, std::size_t buffer_size>
    struct circular_buffer {
        using array_type = std::array<Char, buffer_size + 1>;
        using size_type = std::size_t;
        using value_type = Char;
        using const_value_type = std::add_const_t<Char>;

        // Create a new, empty buffer.
        // We don't initialise this->data because we are tracking the valid
        // region.
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
        circular_buffer() = default;
        ~circular_buffer() = default;

        // circular_buffer cannot be copied or moved, because the buffer
        // contains the entire std::array<>.  In principle we could allow
        // copying, but copying a buffer is almost certainly a programming
        // error.

        circular_buffer(circular_buffer const &) = delete;
        auto operator=(circular_buffer const &) -> circular_buffer & = delete;
        circular_buffer(circular_buffer &&) = delete;
        auto operator=(circular_buffer &&) -> circular_buffer & = delete;

        // The data stored in this buffer.
        array_type data;

        // Reset the buffer.
        auto clear() -> void
        {
            read_pointer = data.begin();
            write_pointer = data.begin();
        }

        /*
         * read_pointer and write_pointer store the current locations where
         * we can read data from resp. write data to.  The readable range
         * is from the read pointer to one before write pointer, and the
         * writable range is from the write pointer to one before the read
         * pointer.
         *
         * The two pointers can be in one of the following states:
         *
         *  v begin                    end v
         * [...............................]
         *             ^--write ptr
         *             ^--read ptr
         *
         *      The read and write pointers point to the same location.
         *      This means we can write to the entire buffer, and no data
         *      can be read.  This is how the buffer starts out.
         *
         *  v begin                    end v
         * [...............................]
         *   read ptr -^  ^- write ptr
         *
         *      3 bytes of data have been written to the buffer, and now the
         *      write pointer is ahead of the read pointer.
         *
         *  v begin                    end v
         * [...............................]
         *       ^     ^- read ptr
         *        \ write ptr
         *
         *      Enough data has been written to the buffer that the write
         *      pointer has wrapped around.
         *
         *  v begin                    end v
         * [...............................]
         *            ^^- read ptr
         *             \ write ptr
         *
         *      The buffer is full; all data can be read and no data can be
         *      written.
         *
         */

        // The current read pointer.  Data from the read pointer until one
        // object prior to the write pointer can be read.  If
        // read_pointer == write_pointer, this means no data can be read.
        typename array_type::iterator read_pointer = data.begin();

        // The current write pointer.  Space from the write pointer to
        // the read pointer can be written to.  If read_pointer ==
        // write_pointer, this means the entire buffer can be written to.
        typename array_type::iterator write_pointer = data.begin();

        // Write data to the buffer.  Returns the number of objects written,
        // which might less than the size of the range if the buffer is full.
        auto write(const_value_type *dptr, size_type dsize) -> size_type;

        // Read data from the buffer.  As much data will be read as possible,
        // and the number of objects read will be returned.  If the return value
        // is less than the requested number of objects, the buffer is now
        // empty.
        auto read(value_type *dptr, size_type dsize) -> size_type;

        // Return a list of ranges which represent data in the buffer
        // which can be read.  Writing data to the buffer will not invalidate
        // the ranges.
        //
        // After reading the data, discard() should be called to remove the
        // data from the buffer.
        auto readable_ranges() -> static_vector<std::span<const_value_type>, 2>;

        // Discard up to n bytes of readable data from the start of the buffer.
        // Returns the number of bytes discarded.
        auto discard(size_type n) -> size_type;

        // Return a list of ranges representing space in the buffer
        // which can be written to.  After writing the data, commit() should be
        // called to mark the space as used.
        auto writable_ranges() -> static_vector<std::span<value_type>, 2>;

        // Mark n bytes of previously empty space as containing data.
        auto commit(size_type n) -> size_type;
    };

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    static_assert(buffer<circular_buffer<char, 4096>>);

    /*
     * circular_buffer::write()
     */
    template <typename Char, std::size_t buffer_size>
    auto circular_buffer<Char, buffer_size>::write(const_value_type *dptr,
                                                   size_type dsize) -> size_type
    {
        auto const *cptr = dptr;
        auto const *cend = dptr + dsize;

        if (read_pointer <= write_pointer) {
            auto wend = data.end();

            if (read_pointer == data.begin())
                wend--;

            auto wr = std::min(std::distance(write_pointer, wend),
                               std::distance(cptr, cend));
            write_pointer = std::copy(cptr, cptr + wr, write_pointer);
            cptr += wr;

            if (write_pointer == data.end())
                write_pointer = data.begin();
        }

        if (read_pointer > write_pointer) {
            while ((cptr < cend) && write_pointer < (read_pointer - 1))
                *write_pointer++ = *cptr++;
        }

        return cptr - dptr;
    }

    /*
     * circular_buffer::writable_ranges()
     */
    template <typename Char, std::size_t buffer_size>
    auto circular_buffer<Char, buffer_size>::writable_ranges()
        -> static_vector<std::span<value_type>, 2>
    {
        static_vector<std::span<value_type>, 2> ret;
        auto theoretical_write_pointer = write_pointer;

        // If read ptr == write ptr, we can write to the entire buffer.
        if (read_pointer <= theoretical_write_pointer) {
            // Determine the last buffer position we can write to.
            auto wend = data.end();

            // If the read_pointer is at the start of the buffer, we have
            // to leave one empty byte at the end to stop the write pointer
            // running into the read pointer.
            if (read_pointer == data.begin())
                wend--;

            // Write between write_pointer and the end of the buffer.
            auto span = std::span<value_type>(
                theoretical_write_pointer,
                theoretical_write_pointer + (wend - theoretical_write_pointer));

            if (!span.empty())
                ret.push_back(span);

            // Set write_pointer past the data we write.
            theoretical_write_pointer +=
                static_cast<typename array_type::difference_type>(span.size());

            // write_pointer may be left pointing at end(); if there's
            // space left at the start of the buffer, wrap it now.
            if (theoretical_write_pointer == data.end() &&
                read_pointer != data.begin())
                theoretical_write_pointer = data.begin();
        }

        if (read_pointer > theoretical_write_pointer) {
            auto span = std::span<value_type>(theoretical_write_pointer,
                                              read_pointer - 1);
            if (!span.empty())
                ret.push_back(span);
        }

        return ret;
    }

    /*
     * circular_buffer::commit()
     */
    template <typename Char, std::size_t buffer_size>
    auto circular_buffer<Char, buffer_size>::commit(size_type n) -> size_type
    {
        auto bytes_left = n;
        size_type bytes_written = 0;

        // If read ptr == write ptr, we can write to the entire buffer.
        if (read_pointer <= write_pointer) {
            // Determine the last buffer position we can write to.
            auto wend = data.end();

            // If the read_pointer is at the start of the buffer, we have
            // to leave one empty byte at the end to stop the write pointer
            // running into the read pointer.
            if (read_pointer == data.begin())
                wend--;

            // Write between write_pointer and the end of the buffer.
            auto can_write =
                std::min(static_cast<typename array_type::size_type>(
                             wend - write_pointer),
                         bytes_left);

            bytes_left -= can_write;
            bytes_written += can_write;

            // Set write_pointer past the data we write.
            write_pointer +=
                static_cast<typename array_type::difference_type>(can_write);

            // write_pointer may be left pointing at end(); if there's
            // space left at the start of the buffer, wrap it now.
            if (write_pointer == data.end() && read_pointer != data.begin())
                write_pointer = data.begin();

            // Fall through to check read_pointer > write_pointer, in case
            // we wrapped.
        }

        if (read_pointer > write_pointer) {
            // Write between write_pointer and read_pointer - 1.
            auto can_write =
                std::min(static_cast<typename array_type::size_type>(
                             read_pointer - data.begin() - 1),
                         bytes_left);

            bytes_written += can_write;
            write_pointer +=
                static_cast<typename array_type::difference_type>(can_write);
            IVY_CHECK(write_pointer < read_pointer,
                     "INTERNAL ERROR: sk::circular_buffer::commit() "
                     "pointer confusion");
        }

        return bytes_written;
    }

    /*
     * circular_buffer::read()
     */
    template <typename Char, std::size_t buffer_size>
    auto circular_buffer<Char, buffer_size>::read(value_type *dptr,
                                                  size_type dsize) -> size_type
    {
        auto *cptr = dptr;
        auto *cend = dptr + dsize;

        if (read_pointer > write_pointer) {
            auto rd = std::min(std::distance(cptr, cend),
                               std::distance(read_pointer, data.end()));
            cptr = std::copy(read_pointer, read_pointer + rd, cptr);
            read_pointer += rd;

            if (read_pointer == data.end())
                read_pointer = data.begin();
        }

        if (read_pointer < write_pointer) {
            auto rd = std::min(std::distance(cptr, cend),
                               std::distance(read_pointer, write_pointer));
            cptr = std::copy(read_pointer, read_pointer + rd, cptr);
            read_pointer += rd;
        }

        IVY_CHECK(read_pointer < data.end(),
                 "INTERNAL ERROR: sk::circular_buffer::read() "
                 "pointer confusion");

        return (cptr - dptr);
    }

    /*
     * circular_buffer::readable_ranges()
     */
    template <typename Char, std::size_t buffer_size>
    auto circular_buffer<Char, buffer_size>::readable_ranges()
        -> static_vector<std::span<const_value_type>, 2>
    {
        static_vector<std::span<const_value_type>, 2> ret;

        // If read_pointer == write_pointer, the buffer is empty.
        if (read_pointer == write_pointer)
            return ret;

        auto theoretical_read_pointer = read_pointer;

        // If read_pointer > write_pointer, we can read from read_pointer
        // until the end of the buffer.
        if (theoretical_read_pointer > write_pointer) {
            auto can_read = static_cast<typename array_type::size_type>(
                data.end() - theoretical_read_pointer);
            auto span = std::span<const_value_type>(
                theoretical_read_pointer,
                theoretical_read_pointer +
                    static_cast<std::ptrdiff_t>(can_read));

            if (!span.empty())
                ret.push_back(span);

            theoretical_read_pointer += static_cast<ptrdiff_t>(can_read);

            // If read_pointer reached the end of the buffer, move it back to
            // the start.
            if (theoretical_read_pointer == data.end())
                theoretical_read_pointer = data.begin();

            // Fall through to check for read_pointer < write_pointer in case
            // we wrapped.
        }

        // If read_pointer < write_pointer, we can read from read_pointer
        // up to write_pointer.
        if (theoretical_read_pointer < write_pointer) {
            auto can_read = static_cast<typename array_type::size_type>(
                write_pointer - theoretical_read_pointer);

            auto span = std::span<const_value_type>(
                theoretical_read_pointer,
                theoretical_read_pointer +
                    static_cast<std::ptrdiff_t>(can_read));

            if (!span.empty())
                ret.push_back(span);
            theoretical_read_pointer += static_cast<ptrdiff_t>(can_read);
        }

        IVY_CHECK(theoretical_read_pointer < data.end(),
                 "INTERNAL ERROR: sk::circular_buffer::readable_ranges() "
                 "pointer confusion");

        return ret;
    }

    /*
     * circular_buffer::discard()
     */
    template <typename Char, std::size_t buffer_size>
    auto circular_buffer<Char, buffer_size>::discard(size_type n) -> size_type
    {
        // If read_pointer == write_pointer, the buffer is empty.
        if (read_pointer == write_pointer)
            return 0;

        auto bytes_left = n;
        size_type bytes_read = 0;

        // If read_pointer > write_pointer, we can read from read_pointer
        // until the end of the buffer.
        if (read_pointer > write_pointer) {
            auto can_read =
                std::min(bytes_left,
                         static_cast<typename array_type::size_type>(
                             data.end() - read_pointer));

            bytes_left -= can_read;
            bytes_read += can_read;
            read_pointer += static_cast<ptrdiff_t>(can_read);

            // If read_pointer reached the end of the buffer, move it back to
            // the start.
            if (read_pointer == data.end())
                read_pointer = data.begin();

            // Fall through to check for read_pointer < write_pointer in case
            // we wrapped.
        }

        // If read_pointer < write_pointer, we can read from read_pointer
        // up to write_pointer.
        if (read_pointer < write_pointer) {
            auto can_read =
                std::min(bytes_left,
                         static_cast<typename array_type::size_type>(
                             write_pointer - read_pointer));

            read_pointer += static_cast<ptrdiff_t>(can_read);
            bytes_read += can_read;
        }

        IVY_CHECK(read_pointer < data.end(),
                 "INTERNAL ERROR: sk::circular_buffer::readable_ranges() "
                 "pointer confusion");
        return bytes_read;
    }

} // namespace sk

#endif // IVY_BUFFER_CIRCULAR_BUFFER_HXX_INCLUDED
