/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DB_ROW_HXX_INCLUDED
#define IVY_DB_ROW_HXX_INCLUDED

#include <cstddef>
#include <memory>

#include <ivy/db/value.hxx>
#include <ivy/expected.hxx>
#include <ivy/noncopyable.hxx>

namespace ivy::db {

    class value_range;
    class row;

    class value_iterator {
        value_range *_value_range{};
        std::size_t _cur{};

        friend class value_range;
        friend auto operator==(value_iterator const &,
                               value_iterator const &) noexcept -> bool;

        value_iterator(value_range *);

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = value;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;

        value_iterator();
        value_iterator(value_iterator const &);
        value_iterator(value_iterator &&) noexcept;

        auto operator=(value_iterator const &) -> value_iterator &;
        auto operator=(value_iterator &&) noexcept -> value_iterator &;

        auto operator*() const noexcept -> reference;
        auto operator->() const noexcept -> pointer;
        auto operator++() -> value_iterator &;
        auto operator++(int) -> value_iterator &;
    };

    auto operator==(value_iterator const &, value_iterator const &) noexcept
        -> bool;

    static_assert(std::input_iterator<value_iterator>);

    class value_range {
        row *_row;
        value_handle _cur;
        std::size_t _ncols;
        friend class row;
        friend class value_iterator;

        value_range(row *);

    public:
        using value_type = value;
        using iterator = value_iterator;
        using const_iterator = value_iterator;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        [[nodiscard]] auto begin() const -> iterator;
        [[nodiscard]] auto end() const -> iterator;
    };

    static_assert(std::ranges::input_range<value_range>);

    class row : nonmovable {
    protected:
        row() = default;

    public:
        virtual ~row() = default;

        [[nodiscard]] virtual auto column_count() -> std::size_t = 0;
        [[nodiscard]] virtual auto get_value(std::size_t column)
            -> expected<value_handle, error> = 0;

        [[nodiscard]] auto values() -> value_range;
    };

    using row_handle = std::unique_ptr<row>;

} // namespace ivy::db

#endif // IVY_DB_ROW_HXX_INCLUDED
