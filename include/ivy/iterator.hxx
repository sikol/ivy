/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_ITERATOR_HXX_INCLUDED
#define IVY_ITERATOR_HXX_INCLUDED

#include <cstddef>

namespace ivy {

    template<typename T>
    class null_output_iterator final {
    public:
        using iterator_category = std::output_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;

        using value_type = T;
        auto operator*() const noexcept -> null_output_iterator const &
        {
            return *this;
        }

        auto operator*() noexcept -> null_output_iterator &
        {
            return *this;
        }

        template <typename V>
        auto operator=(V &&o) const noexcept -> void
        {
        }

        auto operator++() noexcept -> null_output_iterator &
        {
            return *this;
        }

        auto operator++(int) noexcept -> null_output_iterator &
        {
            return *this;
        }
    };

} // namespace ivy

#endif // IVY_ITERATOR_HXX_INCLUDED
