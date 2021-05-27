/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_ERROR_HXX_INCLUDED
#define IVY_ERROR_HXX_INCLUDED

#include <concepts>
#include <exception>
#include <memory>
#include <system_error>
#include <type_traits>

namespace ivy {

    enum struct errc : unsigned {
        no_error = 0,

        // Source character cannot be represented in target character set
        unrepresentable_character = 1,

        // Source codepoint cannot be represented in target encoding
        unrepresentable_codepoint = 2,

        // Source encoding is invalid
        invalid_encoding = 3,

        end_of_file = 4,
    };

}

namespace std {

    template <>
    struct is_error_code_enum<ivy::errc> : true_type {
    };

} // namespace std

namespace ivy {

    namespace detail {

        struct errc_category : std::error_category {
            auto name() const noexcept -> char const * final;
            auto message(int c) const -> std::string final;
        };

    } // namespace detail

    auto errc_category() -> detail::errc_category const &;
    auto make_error_code(errc e) -> std::error_code;

    template <typename T>
    concept error_type = std::derived_from<std::remove_cvref_t<T>, std::exception>;

    namespace detail
    {

        class error_exception_base {
        public:
            [[noreturn]] virtual auto rethrow() const -> void = 0;
            virtual auto ptr() -> std::exception * = 0;
        };

        template <error_type E>
        class error_exception final : public error_exception_base {
            E exception_object;

        public:
            template <typename... Args>
            error_exception(Args &&...args)
                : exception_object(std::forward<Args>(args)...)
            {
            }

            [[noreturn]] auto rethrow() const -> void override
            {
                throw exception_object;
            }

            auto ptr() -> std::exception * override
            {
                return &exception_object;
            }
        };

        class exception_ptr_holder final
            : public error_exception_base {
            std::exception_ptr exception_ptr;

        public:
            exception_ptr_holder(std::exception_ptr const &p)
                : exception_ptr(p)
            {
            }

            [[noreturn]] auto rethrow() const -> void override
            {
                std::rethrow_exception(exception_ptr);
            }

            auto ptr() -> std::exception * override
            {
                try {
                    rethrow();
                } catch (std::exception &e) {
                    return &e;
                } catch (...) {
                    return nullptr;
                }
            }
        };

    } // namespace detail

    /*
     * 'error' stores an std::exception as an error value which can be
     * returned and queried.
     */
    class error {
        // The exception object that we're holding.
        std::shared_ptr<detail::error_exception_base> error_base;

    public:
        /*
         * Create an empty error that indicates success.
         */
        error() = default;

        error(std::shared_ptr<detail::error_exception_base> &&ptr)
            : error_base(std::move(ptr))
        {
        }

        error(error const &) = default;
        error(error &&) = default;
        error &operator=(error const &) = default;
        error &operator=(error &&) = default;

        // Return the error message.
        auto what() const -> char const *
        {
            if (!error_base)
                return "success";
            auto eptr = error_base->ptr();
            if (eptr)
                return eptr->what();
            return "unknown exception";
        }

        // Return true if the error is empty (success), false if an error
        // condition is present.
        operator bool() const
        {
            return !error_base.operator bool();
        }

        // Rethrow the stored error object.
        [[noreturn]] auto rethrow() const -> void
        {
            error_base->rethrow();
        }

        // is<T>(): test if the stored error is or derives from T.
        template <error_type T>
        auto is() const -> bool
        {
            return get<T>() != nullptr;
        }

        // get<T>(): if is<T>(), return a pointer to the error,
        // otherwise return nullptr.
        template <error_type T>
        auto get() const -> T const *
        {
            if (!error_base)
                return nullptr;
            return dynamic_cast<T const *>(error_base->ptr());
        }
    };

    template <error_type T, typename... Args>
    auto make_error(Args &&... args) -> error
    {
        using errtype = std::remove_cvref_t<T>;
        auto base = std::make_shared<detail::error_exception<errtype>>(
            std::forward<Args>(args)...);
        return error(std::move(base));
    }

    inline auto make_error(std::exception_ptr const &ep) -> error {
        auto base = std::make_shared<detail::exception_ptr_holder>(ep);
        return error(std::move(base));
    }

    inline auto make_error(std::error_code e) -> error
    {
        return make_error<std::system_error>(e);
    }

    template<typename Err>
    auto make_error(Err e) -> error requires std::is_error_code_enum_v<std::remove_cvref_t<Err>>
    {
        return make_error<std::system_error>(make_error_code(e));
    }

    template <typename Err>
    auto make_error(Err e)
        -> error requires std::is_error_condition_enum_v<std::remove_cvref_t<Err>>
    {
        using std::make_error_code;
        return make_error<std::system_error>(make_error_code(e));
    }

} // namespace ivy

#endif // IVY_ERROR_HXX_INCLUDED
