/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_ITERATOR_NULL_HXX_INCLUDED
#define IVY_ITERATOR_NULL_HXX_INCLUDED

#include <iterator>

namespace ivy {

    class null_iterator final {
    public:
        using iterator_category = std::output_iterator_tag;
        using value_type = void;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = void;

        auto operator*() const noexcept -> null_iterator const &
        {
            return *this;
        }

        auto operator*() noexcept -> null_iterator &
        {
            return *this;
        }

        template <typename V>
        auto operator=(V &&) const noexcept -> void
        {
        }

        auto operator++() noexcept -> null_iterator &
        {
            return *this;
        }

        auto operator++(int) noexcept -> null_iterator &
        {
            return *this;
        }
    };

} // namespace ivy

#endif // IVY_ITERATOR_NULL_HXX_INCLUDED
