/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DB_QUERY_RESULT_HXX_INCLUDED
#define IVY_DB_QUERY_RESULT_HXX_INCLUDED

#include <memory>

#include <ivy/db/result_set.hxx>
#include <ivy/noncopyable.hxx>

namespace ivy::db {

    class result_set_range;
    class query_result;

    class result_set_iterator {
        result_set_range *_result_set_range{};
        friend class result_set_range;
        friend auto operator==(result_set_iterator const &,
                               result_set_iterator const &) noexcept -> bool;

        result_set_iterator(result_set_range *);

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = result_set;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;

        result_set_iterator();
        result_set_iterator(result_set_iterator const &);
        result_set_iterator(result_set_iterator &&) noexcept;

        auto operator=(result_set_iterator const &) -> result_set_iterator &;
        auto operator=(result_set_iterator &&) noexcept
            -> result_set_iterator &;

        auto operator*() const noexcept -> reference;
        auto operator->() const noexcept -> pointer;
        auto operator++() -> result_set_iterator &;
        auto operator++(int) -> result_set_iterator &;
    };

    auto operator==(result_set_iterator const &,
                    result_set_iterator const &) noexcept -> bool;

    static_assert(std::input_iterator<result_set_iterator>);

    class result_set_range {
        query_result *_query_result;
        result_set_handle _cur;
        friend class query_result;
        friend class result_set_iterator;

        result_set_range(query_result *);

    public:
        using value_type = result_set;
        using iterator = result_set_iterator;
        using const_iterator = result_set_iterator;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        [[nodiscard]] auto begin() const -> iterator;
        [[nodiscard]] auto end() const -> iterator;
    };

    static_assert(std::ranges::input_range<result_set_range>);

    class query_result : nonmovable {
    public:
        query_result() = default;
        virtual ~query_result() = default;

        [[nodiscard]] virtual auto has_data() const -> bool = 0;
        [[nodiscard]] virtual auto get_next_result_set()
            -> expected<result_set_handle, error> = 0;

        [[nodiscard]] auto result_sets() -> result_set_range;
    };

    using query_result_handle = std::unique_ptr<query_result>;

} // namespace ivy::db

#endif // IVY_DB_QUERY_RESULT_HXX_INCLUDED
