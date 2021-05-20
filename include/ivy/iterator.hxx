/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_ITERATOR_HXX_INCLUDED
#define IVY_ITERATOR_HXX_INCLUDED

#include <cstddef>

namespace ivy {

    class null_output_iterator final {
    public:
        using iterator_category = std::output_iterator_tag;
        using value_type = void;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = void;

        auto operator*() const noexcept -> null_output_iterator const &
        {
            return *this;
        }

        auto operator*() noexcept -> null_output_iterator &
        {
            return *this;
        }

        template <typename V>
        auto operator=(V &&) const noexcept -> void
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

    template <typename From,
              typename To,
              std::output_iterator<To> output_iterator>
    class static_cast_iterator final {
        output_iterator out;

    public:
        using iterator_category = std::output_iterator_tag;
        using value_type = void;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = void;

        static_cast_iterator(output_iterator out_) : out(out) {}

        auto operator*() const noexcept -> static_cast_iterator const &
        {
            return *this;
        }

        auto operator*() noexcept -> static_cast_iterator &
        {
            return *this;
        }

        auto operator=(From const &v) const noexcept -> void
        {
            *out++ = static_cast<To>(v);
        }

        auto operator++() noexcept -> static_cast_iterator &
        {
            return *this;
        }

        auto operator++(int) noexcept -> static_cast_iterator &
        {
            return *this;
        }
    };

} // namespace ivy

#endif // IVY_ITERATOR_HXX_INCLUDED
