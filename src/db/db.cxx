/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <ivy/db.hxx>

namespace ivy::db {

    auto result_set::rows() -> row_range
    {
        return row_range(this);
    }

    row_range::row_range(result_set *rs)
        : _rs(rs)
    {
    }

    auto row_range::begin() const -> iterator
    {
        return iterator(const_cast<row_range *>(this));
    }

    auto row_range::end() const -> iterator
    {
        return iterator();
    }

    row_iterator::row_iterator(row_range *rr)
        : _row_range(rr)
    {
        ++*this;
    }

    row_iterator::row_iterator() = default;

    row_iterator::row_iterator(row_iterator const &) = default;
    row_iterator::row_iterator(row_iterator &&) noexcept = default;

    auto row_iterator::operator=(row_iterator const &)
        -> row_iterator & = default;
    auto row_iterator::operator=(row_iterator &&) noexcept
        -> row_iterator & = default;

    auto row_iterator::operator*() const noexcept -> reference
    {
        return *_row_range->_cur;
    }

    auto row_iterator::operator->() const noexcept -> pointer
    {
        return _row_range->_cur.get();
    }

    auto row_iterator::operator++() -> row_iterator &
    {
        auto next = _row_range->_rs->get_next_row();
        if (next) {
            _row_range->_cur = std::move(*next);
            return *this;
        }

        if (next.error().is<ivy::db::end_of_data>()) {
            _row_range->_cur.reset();
            _row_range = nullptr;
            return *this;
        }

        next.error().rethrow();
    }

    auto operator==(row_iterator const &a, row_iterator const &b) noexcept
        -> bool
    {
        return !!a._row_range == !!b._row_range;
    }

    auto query_result::result_sets() -> result_set_range
    {
        return result_set_range(this);
    }

    result_set_range::result_set_range(query_result *qs)
        : _query_result(qs)
    {
    }

    auto result_set_range::begin() const -> iterator
    {
        return iterator(const_cast<result_set_range *>(this));
    }

    auto result_set_range::end() const -> iterator
    {
        return iterator();
    }

    result_set_iterator::result_set_iterator(result_set_range *rr)
        : _result_set_range(rr)
    {
        ++*this;
    }

    result_set_iterator::result_set_iterator() = default;

    result_set_iterator::result_set_iterator(result_set_iterator const &) =
        default;
    result_set_iterator::result_set_iterator(result_set_iterator &&) noexcept =
        default;

    auto result_set_iterator::operator=(result_set_iterator const &)
        -> result_set_iterator & = default;
    auto result_set_iterator::operator=(result_set_iterator &&) noexcept
        -> result_set_iterator & = default;

    auto result_set_iterator::operator*() const noexcept -> reference
    {
        return *_result_set_range->_cur;
    }

    auto result_set_iterator::operator->() const noexcept -> pointer
    {
        return _result_set_range->_cur.get();
    }

    auto result_set_iterator::operator++() -> result_set_iterator &
    {
        auto next = _result_set_range->_query_result->get_next_result_set();
        if (next) {
            _result_set_range->_cur = std::move(*next);
            return *this;
        }

        if (next.error().is<ivy::db::end_of_data>()) {
            _result_set_range->_cur.reset();
            _result_set_range = nullptr;
            return *this;
        }

        next.error().rethrow();
    }

    auto operator==(result_set_iterator const &a,
                    result_set_iterator const &b) noexcept -> bool
    {
        return !!a._result_set_range == !!b._result_set_range;
    }

    auto row::values() -> value_range
    {
        return value_range(this);
    }

    value_range::value_range(row *r)
        : _row(r)
        , _ncols(_row->column_count())
    {
    }

    auto value_range::begin() const -> iterator
    {
        return iterator(const_cast<value_range *>(this));
    }

    auto value_range::end() const -> iterator
    {
        return iterator();
    }

    value_iterator::value_iterator(value_range *vr)
        : _value_range(vr)
    {
        ++*this;
    }

    value_iterator::value_iterator() = default;

    value_iterator::value_iterator(value_iterator const &) = default;
    value_iterator::value_iterator(value_iterator &&) noexcept = default;

    auto value_iterator::operator=(value_iterator const &)
        -> value_iterator & = default;
    auto value_iterator::operator=(value_iterator &&) noexcept
        -> value_iterator & = default;

    auto value_iterator::operator*() const noexcept -> reference
    {
        return *_value_range->_cur.get();
    }

    auto value_iterator::operator->() const noexcept -> pointer
    {
        return _value_range->_cur.get();
    }

    auto value_iterator::operator++() -> value_iterator &
    {
        if (_cur == _value_range->_ncols) {
            _value_range->_cur.reset();
            _value_range = nullptr;
            return *this;
        }

        _value_range->_cur = _value_range->_row->get_value(_cur).or_throw();
        ++_cur;
        return *this;
    }

    auto operator==(value_iterator const &a, value_iterator const &b) noexcept
        -> bool
    {
        return !a._value_range == !b._value_range;
    }

} // namespace ivy::db
