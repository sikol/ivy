/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_STATIC_RANGE_HXX_INCLUDED
#define IVY_STATIC_RANGE_HXX_INCLUDED

#include <array>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <type_traits>

#include <ivy/check.hxx>

namespace ivy {

    namespace detail {

        template <typename T, std::size_t max_size>
        struct static_vector_storage {
        protected:
            using storage_type =
                typename std::aligned_storage<sizeof(T), alignof(T)>::type;
            static_assert(sizeof(storage_type) == sizeof(T));

            // NOLINTNEXTLINE
            storage_type _data[max_size];
            std::size_t _size = 0;

        public:
            static_vector_storage(std::initializer_list<T> items)
            {
                IVY_CHECK(items.size() <= max_size,
                          "static_range: too many items");

                for (auto &&item : items)
                    new (&_data[_size++]) T(std::move(item)); // NOLINT
            }

            static_vector_storage(static_vector_storage const &other) noexcept
                : _size(other._size)
            {
                for (std::size_t i = 0; i < _size; ++i)
                    new (&this->_data[i]) T(*(other.begin() + i)); // NOLINT
            }

            static_vector_storage(static_vector_storage &&other) noexcept
                : _size(std::exchange(other._size, 0u))
            {
                for (std::size_t i = 0; i < _size; ++i) {
                    new (&this->_data[i]) // NOLINT
                        T(std::move(*(other.begin() + i)));
                    (other.begin() + i)->~T(); // NOLINT
                }
            }

            auto operator=(static_vector_storage const &other) noexcept
                -> static_vector_storage &
            {
                if (&other == this)
                    return *this;

                for (std::size_t i = 0; i < _size; ++i)
                    (begin() + i)->~T(); // NOLINT

                for (std::size_t i = 0; i < other._size; ++i) {
                    new (&this->_data[i]) T(*(other.begin() + i)); // NOLINT
                }

                _size = other._size;
                return *this;
            }

            auto operator=(static_vector_storage &&other) noexcept
                -> static_vector_storage &
            {
                if (&other == this)
                    return *this;

                for (std::size_t i = 0; i < _size; ++i)
                    (begin() + i)->~T(); // NOLINT

                for (std::size_t i = 0; i < other._size; ++i) {
                    new (&this->_data[i]) // NOLINT
                        T(std::move(*(other.begin() + i)));
                    (other.begin() + i)->~T(); // NOLINT
                }

                _size = std::exchange(other._size, 0);
                return *this;
            }

            [[nodiscard]] auto begin() noexcept -> T *
            {
                // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
                return std::launder(reinterpret_cast<T *>(&this->_data[0]));
            }

            [[nodiscard]] auto begin() const noexcept -> T const *
            {
                return const_cast<static_vector_storage *>(this)->begin();
            }

        protected:
            static_vector_storage() noexcept = default; // NOLINT
            ~static_vector_storage()
            {
                for (std::size_t i = 0; i < _size; ++i)
                    (begin() + i)->~T(); // NOLINT
            }
        };

        // clang-format off
        template <typename T, std::size_t max_size>
            requires(std::is_trivially_copyable_v<T>)
        struct static_vector_storage<T, max_size> {
            // clang-format on
            std::array<T, max_size> _data;
            std::size_t _size = 0;

            static_vector_storage(std::initializer_list<T> items)
                : _size(items.size())
            {
                IVY_CHECK(items.size() <= max_size,
                          "static_vector: too many items");

                std::ranges::copy(items, _data.begin());
            }

            static_vector_storage(static_vector_storage const &other) noexcept
                : _size(other._size)
            {
                std::ranges::copy(other._data, _data.begin());
            }

            static_vector_storage(static_vector_storage &&other) noexcept
                : _size(std::exchange(other._size, 0))
            {
                std::copy(other._data.begin(),
                          std::next(other._data.begin(), _size),
                          _data.begin());
            }

            auto operator=(static_vector_storage const &other) noexcept
                -> static_vector_storage &
            {
                if (&other == this)
                    return *this;

                std::copy(other._data.begin(),
                          std::next(other._data.begin(), other._size),
                          _data.begin());
                _size = other._size;

                return *this;
            }

            auto operator=(static_vector_storage &&other) noexcept
                -> static_vector_storage &
            {
                if (&other == this)
                    return *this;

                std::copy(other._data.begin(),
                          std::next(other._data.begin(), other._size),
                          _data.begin());
                _size = std::exchange(other._size, 0);

                return *this;
            }

            [[nodiscard]] auto begin() noexcept -> T *
            {
                return &_data[0];
            }

            [[nodiscard]] auto begin() const noexcept -> T const *
            {
                return &_data[0];
            }

        protected:
            static_vector_storage() noexcept = default; // NOLINT
            ~static_vector_storage() = default;
        };

    } // namespace detail

    template <typename T, std::size_t max_size>
    struct static_vector : detail::static_vector_storage<T, max_size> {
        using base_type = detail::static_vector_storage<T, max_size>;

        using value_type = T;
        using const_value_type = const value_type;

        using iterator = T *;
        using const_iterator = T const *;
        using size_type = std::size_t;

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
        static_vector() noexcept = default;

        static_vector(std::initializer_list<T> items) : base_type(items) {}
        static_vector(static_vector const &other) noexcept : base_type(other) {}
        static_vector(static_vector &&other) noexcept
            : base_type(std::move(other))
        {
        }

        auto operator=(static_vector const &other) noexcept -> static_vector &
        {
            if (&other != this)
                base_type::operator=(other);

            return *this;
        }

        auto operator=(static_vector &&other) noexcept -> static_vector &
        {
            if (&other != this)
                base_type::operator=(std::move(other));

            return *this;
        }

        ~static_vector() = default;

        [[nodiscard]] auto operator[](size_type n) noexcept -> value_type &
        {
            return *(this->begin() + n);
        }

        [[nodiscard]] auto operator[](size_type n) const noexcept
            -> const_value_type &
        {
            return *(this->begin() + n);
        }

        [[nodiscard]] auto size() const noexcept -> size_type
        {
            return base_type::_size;
        }

        [[nodiscard]] auto empty() const noexcept -> bool
        {
            return size() == 0;
        }

        [[nodiscard]] auto capacity() const noexcept -> size_type
        {
            return max_size;
        }

        [[nodiscard]] auto end() noexcept -> iterator
        {
            return this->begin() + size(); // NOLINT
        }

        [[nodiscard]] auto end() const noexcept -> const_iterator
        {
            return this->begin() + size(); // NOLINT
        }

        auto push_back(T const &o) -> void
        {
            IVY_CHECK(size() < capacity(), "static_range: no capacity");

            new (&this->_data[this->_size]) T(o); // NOLINT
            ++this->_size;
        }

        auto push_back(T &&o) -> void
        {
            IVY_CHECK(size() < capacity(), "static_range: no capacity");

            new (&this->_data[this->_size]) T(std::move(o)); // NOLINT
            ++this->_size;
        }

        template <typename... Args>
        auto emplace_back(Args &&...args)
        {
            IVY_CHECK(size() < capacity(), "static_range: no capacity");

            new (&this->_data[this->_size])     // NOLINT
                T(std::forward<Args>(args)...); // NOLINT
            ++this->_size;
        }
    };

} // namespace ivy

#endif // IVY_STATIC_RANGE_HXX_INCLUDED
