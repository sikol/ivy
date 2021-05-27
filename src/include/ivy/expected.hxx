/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_EXPECTED_HXX_INCLUDED
#define IVY_EXPECTED_HXX_INCLUDED

#include <optional>
#include <type_traits>
#include <utility>

#include <ivy/check.hxx>
#include <ivy/trace.hxx>
#include <ivy/unexpected.hxx>

namespace ivy {

    class error;

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
        unexpected<Error>::unexpected(Error &&error)
            : _error(std::move(error))
        {
        }

        template <typename Error>
        unexpected<Error>::unexpected(Error const &error)
            : _error(error)
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
    auto make_unexpected(Error &&e)
        -> detail::unexpected<std::remove_cvref_t<Error>>
    {
        return detail::unexpected<std::remove_cvref_t<Error>>(
            std::forward<Error>(e));
    }

    template <typename T, typename Error>
    class expected {
        std::optional<T> _value;
        std::optional<Error> _error;

    public:
        template <typename V>
        expected(V &&value) noexcept;

        template <typename V>
        expected(V const &value);

        template <typename E>
        expected(detail::unexpected<E> &&error) noexcept;

        template <typename E>
        expected(detail::unexpected<E> const &error);

        explicit operator bool() const;

        auto operator*() const noexcept -> T const &;
        auto operator*() noexcept -> T &;
        auto operator->() const noexcept -> T const *;
        auto operator->() noexcept -> T *;

        auto error() const noexcept -> Error const &;

        auto or_throw() -> T &&requires
            std::derived_from<std::remove_cvref_t<Error>, std::exception>;
        auto or_throw() -> T &&requires
            std::same_as<std::remove_cvref_t<Error>, ivy::error>;
    };

    template <typename T, typename Error>
    template <typename V>
    expected<T, Error>::expected(V &&value) noexcept
        : _value(std::move(value))
    {
    }

    template <typename T, typename Error>
    template <typename V>
    expected<T, Error>::expected(V const &value)
        : _value(value)
    {
    }

    template <typename T, typename Error>
    template <typename E>
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

    template <typename T, typename Error>
    expected<T, Error>::operator bool() const
    {
        return _value.has_value();
    }

    template <typename T, typename Error>
    auto expected<T, Error>::operator*() const noexcept -> T const &
    {
        IVY_CHECK(_value.has_value(), "expected<>::operator*: no value");
        return *_value;
    }

    template <typename T, typename Error>
    auto expected<T, Error>::operator*() noexcept -> T &
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
    auto expected<T, Error>::operator->() noexcept -> T *
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

    template <typename T, typename Error>
    auto expected<T, Error>::or_throw() -> T &&requires
        std::derived_from<std::remove_cvref_t<Error>, std::exception>
    {
        if (*this)
            return std::move(*_value);

        throw *_error;
    }

    template <typename T, typename Error>
    auto expected<T, Error>::or_throw() -> T &&requires
        std::same_as<std::remove_cvref_t<Error>, ivy::error>
    {
        if (*this)
            return std::move(*_value);

        _error->rethrow();
    }

    template <typename Error>
    class expected<void, Error> {
        std::optional<Error> _error;

    public:
        expected() noexcept;

        template <typename E>
        expected(detail::unexpected<E> &&error) noexcept;

        template <typename E>
        expected(detail::unexpected<E> const &error);

        explicit operator bool() const;

        auto error() const noexcept -> Error const &;
    };

    template <typename Error>
    expected<void, Error>::expected() noexcept = default;

    template <typename Error>
    template <typename E>
    expected<void, Error>::expected(detail::unexpected<E> &&error) noexcept
        : _error(std::move(error.error()))
    {
    }

    template <typename Error>
    template <typename E>
    expected<void, Error>::expected(detail::unexpected<E> const &error)
        : _error(error.error())
    {
    }

    template <typename Error>
    expected<void, Error>::operator bool() const
    {
        return !_error.has_value();
    }

    template <typename Error>
    auto expected<void, Error>::error() const noexcept -> Error const &
    {
        IVY_CHECK(_error.has_value(), "expected<>::error(): no error");
        return *_error;
    }

} // namespace ivy

#endif // IVY_EXPECTED_HXX_INCLUDED
