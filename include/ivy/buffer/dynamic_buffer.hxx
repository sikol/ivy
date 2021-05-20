/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_BUFFER_DYNAMIC_BUFFER_HXX_INCLUDED
#define IVY_BUFFER_DYNAMIC_BUFFER_HXX_INCLUDED

#include <cassert>
#include <cstddef>
#include <span>
#include <type_traits>

#include <ivy/buffer/buffer.hxx>
#include <ivy/check.hxx>
#include <ivy/static_vector.hxx>

namespace ivy {

    /*************************************************************************
     *
     * dynamic_buffer: a buffer which grows and contracts dynamically as
     * required. Growth is managed efficiently so the data is never copied, and
     * there is no upper bound on the size of the buffer (other than available
     * memory).
     *
     * A dynamic buffer consists of a linked list of extents (which are arrays
     * of a fixed size) and a tail pointer.
     *
     * Each extent contains a read pointer and a write pointer.  The read
     * pointer is the start of the readable region, the write pointer is the
     * start of the writable region.  Invariant: readptr <= writeptr <= end.
     * If readptr == writeptr, then all data has been read from the extent.
     * If writeptr == end, then the extent cannot be written to any more.
     * If readptr == writeptr == end, the extent is "dead" and cannot be used
     * until it is reset to the empty state.  (An extent is essentially a
     * fixed_buffer, but optimised for this particular use case.)
     *
     * The head of the extent list always contains data that can be read.  As
     * soon as the head's read pointer reaches the end of the data, is it dead;
     * it is moved to the end of the list and the next extent becomes the new
     * head (or, if there is only one extent, the current head is cleared
     * and re-used).
     *
     * The tail is the extent which is currently being written to, which might
     * be the same as the head.  Once the tail's write pointer reaches the end,
     * the tail advances to the first spare extent after the tail, or a new
     * extent is allocated.
     *
     * After the tail are free extents, which will be used once the current
     * tail extent becomes full.  This is to avoid constantly freeing and
     * re-allocating extents when the buffer is busy.
     *
     * A possible configuration of the buffer with 4 extents:
     *
     *   [1]->[2]->[3]->[4]
     *    \    \    \    `- spare extent ready to be re-used once the current
     *     \    \    \      tail extent has been entirely written to.
     *      \    \    `- the list tail; new data is written here.
     *       \    `- a full extent which will be read from once the current
     *        \      head is finished with.
     *         `- the list head; this contains data which can be read out.
     */

    // Calculate how large a buffer extent should be if we want to use
    // a specific number of bytes.

    template <typename Char>
    constexpr auto extent_size_from_bytes(int nbytes)
    {
        return ((nbytes - (sizeof(char *) * 3)) / sizeof(Char)) +
               sizeof(char *) * 3;
    }

    // Return the correct extent_bytes to create a dynamic_buffer with an
    // extent capacity of 'n' bytes.  This is 'n' plus the extent's bookkeeping
    // data.
    static constexpr auto dynamic_buffer_size(std::size_t n)
    {
        return n + (sizeof(char *) * 3);
    }

    // How large the extents will be by default (in bytes).
    static constexpr std::size_t dynamic_buffer_default_extent_bytes = 4096;

    // How many ranges we can return in a single call to readable/writable_ranges.
    static constexpr std::size_t dynamic_buffer_default_max_ranges = 5;

    template <typename Char,
              std::size_t extent_bytes = dynamic_buffer_default_extent_bytes,
              std::size_t max_ranges = dynamic_buffer_default_max_ranges>
    struct dynamic_buffer {
        using size_type = std::size_t;
        using value_type = Char;
        using const_value_type = std::add_const_t<Char>;

    private:
        static constexpr std::size_t extent_size =
            extent_size_from_bytes<value_type>(extent_bytes);

        struct extent_type {
            extent_type *next = nullptr;
            value_type *read_pointer;
            value_type *write_pointer;

            static constexpr std::size_t bksize =
                // NOLINTNEXTLINE(bugprone-sizeof-expression)
                sizeof(next) + sizeof(read_pointer) + sizeof(write_pointer);

            static_assert(extent_size > bksize, "dynamic_buffer is too small");

            static constexpr std::size_t xsize = extent_size - bksize;

            Char data[xsize]; // NOLINT

            extent_type() : read_pointer(&data[0]), write_pointer(read_pointer)
            {
            }

            ~extent_type() {
                delete next;
            }

            extent_type(extent_type const &) = delete;
            extent_type(extent_type &&) = delete;
            auto operator=(extent_type const &) = delete;
            auto operator=(extent_type &&) = delete;

            constexpr auto readable() -> std::size_t
            {
                return std::distance(read_pointer, write_pointer);
            }

            constexpr auto empty()
            {
                return write_pointer == begin();
            }

            constexpr auto writable() -> std::size_t
            {
                return std::distance(write_pointer, end());
            }

            constexpr auto begin()
            {
                return &data[0];
            }

            constexpr auto end()
            {
                return &data[0] + xsize;
            }

            constexpr auto dead()
            {
                return readable() == 0 && writable() == 0;
            }

            void clear()
            {
                read_pointer = begin();
                write_pointer = begin();
            }
        };

        // static_assert(sizeof(extent_type) <= extent_bytes);

        // Head and tail of the extent list.  The tail is not the actual tail
        // of the list, but rather the extent we're currently writing to.
        extent_type *_head = nullptr;
        extent_type *_tail = nullptr;

        // Remove the current head (read) extent.
        auto _kill_head() noexcept -> void;

        // Add a new write extent at the end of the buffer.
        auto _make_tail() -> void;

        // Free all extents.
        auto _xfree() noexcept -> void;

    public:
        // Create a new, empty buffer with a single extent.
        dynamic_buffer();
        ~dynamic_buffer();

        // dynamic_buffer is not copyable, but can be moved.
        dynamic_buffer(dynamic_buffer const &other) = delete;
        auto operator=(dynamic_buffer const &other) -> dynamic_buffer & = delete;
        dynamic_buffer(dynamic_buffer &&other) noexcept;
        auto operator=(dynamic_buffer &&other) noexcept -> dynamic_buffer &;

        auto write(const_value_type *dptr, size_type dsize) -> size_type;
        auto read(value_type *dptr, size_type dsize) -> size_type;

        auto readable_ranges()
            -> static_vector<std::span<const_value_type>, max_ranges>;
        auto discard(size_type n) -> size_type;

        auto writable_ranges()
            -> static_vector<std::span<value_type>, max_ranges>;
        auto commit(size_type n) -> size_type;

    private:
        // Ensure that at least one entire writable extent is available.
        auto _ensure_minfree() -> void;
    };

    static_assert(buffer<dynamic_buffer<char>>);

    template <typename Char, std::size_t extent_size, std::size_t max_ranges>
    dynamic_buffer<Char, extent_size, max_ranges>::dynamic_buffer()
    : _head(new extent_type), _tail(_head)
    {
    }

    template <typename Char, std::size_t extent_size, std::size_t max_ranges>
    dynamic_buffer<Char, extent_size, max_ranges>::dynamic_buffer(
        dynamic_buffer &&other) noexcept
        : _head(std::exchange(other._head, nullptr)),
          _tail(std::exchange(other._tail, nullptr))
    {
    }

    template <typename Char, std::size_t extent_size, std::size_t max_ranges>
    auto dynamic_buffer<Char, extent_size, max_ranges>::operator=(
        dynamic_buffer &&other) noexcept -> dynamic_buffer &
    {
        _xfree();
        _head = std::exchange(other._head, nullptr);
        _tail = std::exchange(other._tail, nullptr);
    }

    template <typename Char, std::size_t extent_size, std::size_t max_ranges>
    dynamic_buffer<Char, extent_size, max_ranges>::~dynamic_buffer()
    {
        _xfree();
    }

    template <typename Char, std::size_t extent_size, std::size_t max_ranges>
    auto dynamic_buffer<Char, extent_size, max_ranges>::_kill_head() noexcept
        -> void
    {
        IVY_CHECK(_head->dead(),
                 "INTERNAL ERROR: dynamic_buffer::_kill_head() trying "
                 "to remove active head");

        IVY_CHECK(_head != _tail,
                 "INTERNAL ERROR: dynamic_buffer::_kill_head() trying "
                 "to remove our only head");

        auto old_head = std::exchange(_head, _head->next);
        old_head->next = std::exchange(_tail->next, old_head);

        old_head->clear();
    }

    template <typename Char, std::size_t extent_size, std::size_t max_ranges>
    auto dynamic_buffer<Char, extent_size, max_ranges>::_make_tail() -> void
    {
        if (_tail->next) {
            IVY_CHECK(_tail->next->empty(),
                     "INTERNAL ERROR: dynamic_buffer::_make_tail: "
                     "next tail not empty");
            _tail = _tail->next;
            return;
        }

        auto *new_tail = new extent_type;
        new_tail->next = _tail->next;
        _tail->next = new_tail;
        _tail = _tail->next;
    }

    template <typename Char, std::size_t extent_size, std::size_t max_ranges>
    auto dynamic_buffer<Char, extent_size, max_ranges>::_xfree() noexcept
        -> void
    {
        delete _head;
        _head = nullptr;
        _tail = nullptr;
    }

    template <typename Char, std::size_t extent_size, std::size_t max_ranges>
    auto dynamic_buffer<Char, extent_size, max_ranges>::_ensure_minfree()
        -> void
    {
        if (_tail->empty())
            return;

        if (_tail->next) {
            IVY_CHECK(
                _tail->next->empty(),
                "INTERNAL ERROR: dynamic_buffer::_ensure_minfree: next tail "
                "not empty");
            return;
        }

        _make_tail();
    }

    template <typename Char, std::size_t extent_size, std::size_t max_ranges>
    auto dynamic_buffer<Char, extent_size, max_ranges>::write(
        const_value_type *dptr, size_type dsize) -> size_type
    {
        auto *cptr = dptr;
        auto *cend = cptr + dsize;

        IVY_CHECK(cptr <= cend, "dynamic_buffer::write: dsize overflow");

        while (cptr < cend) {
            // Is the tail full?
            if (_tail->writable() == 0)
                _make_tail();

            // Copy as much as we can into the current tail.
            auto wr = std::min(static_cast<size_type>(cend - cptr),
                               _tail->writable());

            IVY_CHECK((_tail->write_pointer + wr) <= _tail->end() &&
                         (cptr + wr) <= cend,
                     "INTERNAL ERROR: dynamic_buffer: write: overflow");

            _tail->write_pointer =
                std::copy(cptr, cptr + wr, _tail->write_pointer);
            cptr += wr;
        }

        IVY_CHECK(static_cast<std::size_t>(cptr - dptr) == dsize,
                 "dynamic_buffer::write(): short write");
        return dsize;
    }

    template <typename Char, std::size_t extent_size, std::size_t max_ranges>
    auto dynamic_buffer<Char, extent_size, max_ranges>::read(value_type *dptr,
                                                             size_type dsize)
        -> size_type
    {
        auto *cptr = dptr;
        auto *cend = cptr + dsize;

        IVY_CHECK(cptr <= cend, "dynamic_buffer::read: dsize overflow");

        while (cptr < cend) {
            IVY_CHECK(
                !_head->dead(),
                "INTERNAL ERROR: dynamic_buffer::read: unexpected dead head");

            if (_head->readable() == 0) {
                break;
            }

            auto rd = std::min(static_cast<size_type>(cend - cptr),
                               _head->readable());

            IVY_CHECK((_head->read_pointer + rd) <= _head->write_pointer &&
                         (cptr + rd) <= cend,
                     "INTERNAL ERROR: dynamic_buffer: read: overflow");

            std::copy(_head->read_pointer, _head->read_pointer + rd, cptr);
            cptr += rd;
            _head->read_pointer += rd;

            if (_head->dead()) {
                if (_head != _tail)
                    _kill_head();
                else
                    _head->clear();
            }
        }

        return cptr - dptr;
    }

    template <typename Char, std::size_t extent_size, std::size_t max_ranges>
    auto dynamic_buffer<Char, extent_size, max_ranges>::writable_ranges()
        -> static_vector<std::span<Char>, max_ranges>
    {
        static_vector<std::span<Char>, max_ranges> ret;

        // Make sure we always return a reasonable amount of writable space.
        _ensure_minfree();

        auto *tptr = _tail;

        IVY_CHECK(_tail != nullptr,
                 "INTERNAL ERROR: dynamic_buffer::writable_ranges: no tail?");

        for (;;) {
            if (ret.size() == ret.capacity())
                break;

            if (!tptr)
                break;

            IVY_CHECK(
                tptr->writable() > 0,
                "INTERNAL ERROR: dynamic_buffer::writable_ranges: unexpected "
                "full tail");

            ret.push_back(std::span(tptr->write_pointer, tptr->end()));

            tptr = tptr->next;
        }

        return ret;
    }

    template <typename Char, std::size_t extent_size, std::size_t max_ranges>
    auto dynamic_buffer<Char, extent_size, max_ranges>::commit(std::size_t n)
        -> size_type
    {
        auto cn = n;

        while (cn > 0) {
            if (_tail->writable() == 0) {
                IVY_CHECK(_tail->next != nullptr,
                         "dynamic_buffer::commit(): no tail");
                _tail = _tail->next;
            }

            auto cancommit = std::min(cn, _tail->writable());
            _tail->write_pointer += cancommit;
            cn -= cancommit;
        }

        IVY_CHECK(cn == 0, "dynamic_buffer::commit(): short commit");
        return n - cn;
    }

    template <typename Char, std::size_t extent_size, std::size_t max_ranges>
    auto dynamic_buffer<Char, extent_size, max_ranges>::readable_ranges()
        -> static_vector<std::span<const_value_type>, max_ranges>
    {
        static_vector<std::span<const_value_type>, max_ranges> ret;

        auto *hptr = _head;
        while (ret.size() < ret.capacity()) {
            IVY_CHECK(
                _head->readable() > 0,
                "INTERNAL ERROR: dynamic_buffer::readable_ranges: unreadable "
                "head");

            ret.push_back(std::span(hptr->read_pointer, hptr->write_pointer));
            hptr = hptr->next;
            if (!hptr)
                break;
        }

        return ret;
    }

    template <typename Char, std::size_t extent_size, std::size_t max_ranges>
    auto dynamic_buffer<Char, extent_size, max_ranges>::discard(size_type n)
        -> size_type
    {
        auto left = n;

        while (left > 0) {
            IVY_CHECK(
                _head->readable() > 0,
                "INTERNAL ERROR: dynamic_buffer::discard: unreadable head");

            auto candisc = std::min(left, _head->readable());
            _head->read_pointer += candisc;

            if (_head->dead()) {
                if (_head != _tail)
                    _kill_head();
                else
                    _head->clear();
            }

            left -= candisc;
        }

        return n - left;
    }

} // namespace sk

#endif // IVY_BUFFER_DYNAMIC_BUFFER_HXX_INCLUDED
