/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DB_RESULT_SET_HXX_INCLUDED
#define IVY_DB_RESULT_SET_HXX_INCLUDED

#include <memory>

#include <ivy/noncopyable.hxx>
#include <ivy/db/row.hxx>

namespace ivy::db {

    class result_set;
    class row_range;
    using result_set_handle = std::unique_ptr<result_set>;

    class row_iterator {
        row_range *_row_range{};
        friend class row_range;
        friend auto operator==(row_iterator const &,
                               row_iterator const &) noexcept -> bool;

        row_iterator(row_range *);

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = row;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;

        row_iterator();
        row_iterator(row_iterator const &);
        row_iterator(row_iterator &&) noexcept;

        auto operator=(row_iterator const &) -> row_iterator &;
        auto operator=(row_iterator &&) noexcept -> row_iterator &;

        auto operator*() const noexcept -> reference;
        auto operator->() const noexcept -> pointer;
        auto operator++() -> row_iterator &;
        auto operator++(int) -> row_iterator &;
    };

    auto operator==(row_iterator const &, row_iterator const &) noexcept -> bool;

    static_assert(std::input_iterator<row_iterator>);

    class row_range {
        result_set *_rs;
        row_handle _cur;
        friend class result_set;
        friend class row_iterator;

        row_range(result_set *);

    public:
        using value_type = row;
        using iterator = row_iterator;
        using const_iterator = row_iterator;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        [[nodiscard]] auto begin() const -> iterator;
        [[nodiscard]] auto end() const -> iterator;
    };

    static_assert(std::ranges::input_range<row_range>);

    class result_set : nonmovable {
    protected:
        result_set() = default;

    public:
        virtual ~result_set() = default;

        [[nodiscard]] auto has_data() -> bool;
        [[nodiscard]] virtual auto column_count() -> std::size_t = 0;
        [[nodiscard]] virtual auto get_next_row() noexcept
            -> expected<row_handle, error> = 0;

        [[nodiscard]] auto rows() -> row_range;
    };

} // namespace ivy::db

#endif // IVY_DB_RESULT_SET_HXX_INCLUDED
