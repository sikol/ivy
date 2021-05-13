/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_EXPECTED_HXX_INCLUDED
#define IVY_EXPECTED_HXX_INCLUDED

#include <optional>
#include <utility>
#include <type_traits>

#include <ivy/trace.hxx>
#include <ivy/check.hxx>
#include <ivy/unexpected.hxx>

namespace ivy {

    namespace detail {

        template <typename Error>
        class unexpected {
        private:
            Error _error;

        public:
            unexpected(Error const &error);
            unexpected(Error &&error);

            auto error() const noexcept -> Error const &;
            auto error() noexcept -> Error &;
        };

        template <typename Error>
        unexpected<Error>::unexpected(Error &&error) : _error(std::move(error))
        {
        }

        template <typename Error>
        unexpected<Error>::unexpected(Error const &error) : _error(error)
        {
        }

        template <typename Error>
        auto unexpected<Error>::error() const noexcept -> Error const &
        {
            return _error;
        }

        template <typename Error>
        auto unexpected<Error>::error() noexcept -> Error &
        {
            return _error;
        }

    } // namespace detail

    template <typename Error>
    auto make_unexpected(Error &&e) -> detail::unexpected<std::remove_cvref_t<Error>>
    {
        return detail::unexpected<std::remove_cvref_t<Error>>(std::forward<Error>(e));
    }

    template <typename T, typename Error>
    class expected {
        std::optional<T> _value;
        std::optional<Error> _error;

    public:
        template<typename V>
        expected(V &&value) noexcept;

        template<typename V>
        expected(V const &value);

        template<typename E>
        expected(detail::unexpected<E> &&error) noexcept;

        template<typename E>
        expected(detail::unexpected<E> const &error);

        explicit operator bool() const;

        auto operator*() const noexcept -> T const &;
        auto operator->() const noexcept -> T const *;

        auto error() const noexcept -> Error const &;
    };

    template<typename T, typename Error>
    template<typename V>
    expected<T, Error>::expected(V &&value) noexcept : _value(std::move(value))
    {
    }

    template<typename T, typename Error>
    template<typename V>
    expected<T, Error>::expected(V const &value) : _value(value)
    {
    }

    template<typename T, typename Error>
    template<typename E>
    expected<T, Error>::expected(detail::unexpected<E> &&error) noexcept
        : _error(std::move(error.error()))
    {
    }

    template <typename T, typename Error>
    template <typename E>
    expected<T, Error>::expected(detail::unexpected<E> const &error)
        : _error(error.error())
    {
    }

    template<typename T, typename Error>
    expected<T, Error>::operator bool() const
    {
        return _value.has_value();
    }

    template<typename T, typename Error>
    auto expected<T, Error>::operator*() const noexcept -> T const &
    {
        IVY_CHECK(_value.has_value(), "expected<>::operator*: no value");
        return *_value;
    }

    template <typename T, typename Error>
    auto expected<T, Error>::operator->() const noexcept -> T const *
    {
        IVY_CHECK(_value.has_value(), "expected<>::operator->: no value");
        return std::addressof(*_value);
    }

    template <typename T, typename Error>
    auto expected<T, Error>::error() const noexcept -> Error const &
    {
        IVY_CHECK(_error.has_value(), "expected<>::error(): no error");
        return *_error;
    }

} // namespace ivy

#endif // IVY_EXPECTED_HXX_INCLUDED
