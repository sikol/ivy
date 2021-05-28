/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_EXPECTED_HXX_INCLUDED
#define IVY_EXPECTED_HXX_INCLUDED

#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

#include <ivy/check.hxx>
#include <ivy/unexpected.hxx>

namespace ivy {

    class error;

    namespace detail {

        template <typename Error>
        class unexpected {
            Error _error;

        public:
            explicit unexpected(Error const &error);
            explicit unexpected(Error &&error);

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
        std::variant<T, Error> _state;

    public:
        template <typename V>
        requires(!std::same_as<std::remove_cvref_t<V>, expected<T, Error>>)
            expected(V &&value)
        noexcept
            : _state(std::forward<V>(value))
        {
        }

        template <typename E>
        expected(detail::unexpected<E> &&error) noexcept;

        template <typename E>
        expected(detail::unexpected<E> const &error);

        [[nodiscard]] explicit operator bool() const;

        [[nodiscard]] auto operator*() const noexcept -> T const &;
        [[nodiscard]] auto operator*() noexcept -> T &;
        [[nodiscard]] auto operator->() const noexcept -> T const *;
        [[nodiscard]] auto operator->() noexcept -> T *;

        auto error() const noexcept -> Error const &;

        auto or_throw() && -> T requires
            std::derived_from<std::remove_cvref_t<Error>, std::exception>;

        auto or_throw() && -> T requires
            std::same_as<std::remove_cvref_t<Error>, ivy::error>;

        template <typename Nested>
        auto or_throw_with_nested(Nested &&n) && -> T requires
            std::derived_from<std::remove_cvref_t<Error>, std::exception>;

        template <typename Nested>
        auto or_throw_with_nested(Nested &&n) && -> T requires
            std::same_as<std::remove_cvref_t<Error>, ivy::error>;
    };

    template <typename T, typename Error>
    template <typename E>
    expected<T, Error>::expected(detail::unexpected<E> &&error) noexcept
        : _state(std::move(error.error()))
    {
    }

    template <typename T, typename Error>
    template <typename E>
    expected<T, Error>::expected(detail::unexpected<E> const &error)
        : _state(error.error())
    {
    }

    template <typename T, typename Error>
    expected<T, Error>::operator bool() const
    {
        return get_if<T>(&_state) != nullptr;
    }

    template <typename T, typename Error>
    auto expected<T, Error>::operator*() const noexcept -> T const &
    {
        auto *v = get_if<T>(&_state);
        IVY_CHECK(v != nullptr, "expected<>: no value");
        return *v;
    }

    template <typename T, typename Error>
    auto expected<T, Error>::operator*() noexcept -> T &
    {
        auto *v = get_if<T>(&_state);
        IVY_CHECK(v != nullptr, "expected<>: no value");
        return *v;
    }

    template <typename T, typename Error>
    auto expected<T, Error>::operator->() const noexcept -> T const *
    {
        auto *v = get_if<T>(&_state);
        IVY_CHECK(v != nullptr, "expected<>: no value");
        return *v;
    }

    template <typename T, typename Error>
    auto expected<T, Error>::operator->() noexcept -> T *
    {
        auto *v = get_if<T>(&_state);
        IVY_CHECK(v != nullptr, "expected<>: no value");
        return v;
    }

    template <typename T, typename Error>
    auto expected<T, Error>::error() const noexcept -> Error const &
    {
        auto *e = get_if<Error>(&_state);
        IVY_CHECK(e != nullptr, "expected<>: no error");
        return *e;
    }

    template <typename T, typename Error>
    auto expected<T, Error>::or_throw() && -> T requires
        std::derived_from<std::remove_cvref_t<Error>, std::exception>
    {
        auto *v = get_if<T>(&_state);
        if (v)
            return std::move(*v);

        throw error();
    }

    template <typename T, typename Error>
    auto expected<T, Error>::or_throw() &&
        -> T requires std::same_as<std::remove_cvref_t<Error>, ivy::error>
    {
        auto *v = get_if<T>(&_state);
        if (v)
            return std::move(*v);

        error().rethrow();
    }

    template <typename T, typename Error>
    template <typename Nested>
    auto expected<T, Error>::or_throw_with_nested(Nested &&n) && -> T requires
        std::derived_from<std::remove_cvref_t<Error>, std::exception>
    {
        auto *v = get_if<T>(&_state);
        if (v)
            return std::move(*v);

        try {
            throw error();
        } catch (...) {
            std::throw_with_nested(std::forward<Nested>(n));
        }
    }

    template <typename T, typename Error>
    template <typename Nested>
    auto expected<T, Error>::or_throw_with_nested(Nested &&n) &&
        -> T requires std::same_as<std::remove_cvref_t<Error>, ivy::error>
    {
        auto *v = get_if<T>(&_state);
        if (v)
            return std::move(*v);

        try {
            error().rethrow();
        } catch (...) {
            std::throw_with_nested(std::forward<Nested>(n));
        }
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

        auto or_throw() -> void requires
            std::derived_from<std::remove_cvref_t<Error>, std::exception>;

        auto or_throw() -> void requires
            std::same_as<std::remove_cvref_t<Error>, ivy::error>;

        template <typename Nested>
        auto or_throw_with_nested(Nested &&e) -> void requires
            std::derived_from<std::remove_cvref_t<Error>, std::exception>;

        template <typename Nested>
        auto or_throw_with_nested(Nested &&e) -> void requires
            std::same_as<std::remove_cvref_t<Error>, ivy::error>;
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

    template <typename Error>
    auto expected<void, Error>::or_throw() -> void requires
        std::derived_from<std::remove_cvref_t<Error>, std::exception>
    {
        if (!*this)
            throw *_error;
    }

    template <typename Error>
    auto expected<void, Error>::or_throw()
        -> void requires std::same_as<std::remove_cvref_t<Error>, ivy::error>
    {
        if (!*this)
            _error->rethrow();
    }

    template <typename Error>
    template <typename Nested>
    auto expected<void, Error>::or_throw_with_nested(Nested &&n)
        -> void requires
        std::derived_from<std::remove_cvref_t<Error>, std::exception>
    {
        if (!*this)
            try {
                throw *_error;
            } catch (...) {
                std::throw_with_nested(std::forward<Nested>(n));
            }
    }

    template <typename Error>
    template <typename Nested>
    auto expected<void, Error>::or_throw_with_nested(Nested &&n)
        -> void requires std::same_as<std::remove_cvref_t<Error>, ivy::error>
    {
        if (!*this)
            try {
                _error->rethrow();
            } catch (...) {
                std::throw_with_nested(std::forward<Nested>(n));
            }
    }

} // namespace ivy

#endif // IVY_EXPECTED_HXX_INCLUDED
