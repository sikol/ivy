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
#include <utility>
#include <variant>

#include <ivy/overload.hxx>

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
    struct is_error_code_enum<::ivy::errc> : true_type {
    };

} // namespace std

namespace ivy {

    namespace detail {

        struct errc_category final : std::error_category {
            [[nodiscard]] auto name() const noexcept -> char const * override;
            [[nodiscard]] auto message(int c) const -> std::string override;
        };

    } // namespace detail

    auto errc_category() -> detail::errc_category const &;
    auto make_error_code(errc e) -> std::error_code;

    template <typename T>
    concept error_type =
        std::derived_from<std::remove_cvref_t<T>, std::exception>;

    namespace detail
    {
        class error_exception_base {
        protected:
            virtual ~error_exception_base() = default;

            error_exception_base() = default;
            error_exception_base(error_exception_base &&other) noexcept =
                default;
            error_exception_base(error_exception_base const &) = default;

            auto operator=(error_exception_base &&) noexcept
                -> error_exception_base & = default;
            auto operator=(error_exception_base const &)
                -> error_exception_base & = default;

        public:
            [[noreturn]] virtual auto rethrow() const -> void = 0;
            [[nodiscard]] virtual auto ptr() -> std::exception * = 0;
        };

        template <error_type E>
        class error_exception final : public error_exception_base {
            E _exception_object;

        public:
            error_exception(error_exception &&) noexcept = default;
            error_exception(error_exception const &) = default;
            ~error_exception() override = default;

            auto operator=(error_exception &&) noexcept
                -> error_exception & = default;
            auto operator=(error_exception const &)
                -> error_exception & = default;

            template <typename... Args>
            explicit error_exception(Args &&...args)
                : _exception_object(std::forward<Args>(args)...)
            {
            }

            [[noreturn]] auto rethrow() const -> void override
            {
                throw _exception_object;
            }

            [[nodiscard]] auto ptr() -> std::exception * override
            {
                return &_exception_object;
            }
        };

        class exception_ptr_holder final : public error_exception_base {
            std::exception_ptr _exception_ptr;

        public:
            exception_ptr_holder(exception_ptr_holder &&) noexcept = default;
            exception_ptr_holder(exception_ptr_holder const &) = default;
            ~exception_ptr_holder() override = default;

            auto operator=(exception_ptr_holder &&) noexcept
                -> exception_ptr_holder & = default;
            auto operator=(exception_ptr_holder const &)
                -> exception_ptr_holder & = default;

            explicit exception_ptr_holder(std::exception_ptr const &p)
                : _exception_ptr(p)
            {
            }

            [[noreturn]] auto rethrow() const -> void override
            {
                std::rethrow_exception(_exception_ptr);
            }

            [[nodiscard]] auto ptr() -> std::exception * override
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

    class error final {
    public:
        using exception_type = std::shared_ptr<detail::error_exception_base>;
        using error_code_type = std::error_code;

        using storage_type =
            std::variant<std::monostate, exception_type, error_code_type>;

    private:
        storage_type _error_base;

    public:
        /*
         * Create an empty error that indicates success.
         */
        error() = default;
        ~error() = default;

        explicit error(std::shared_ptr<detail::error_exception_base> &&ptr)
            : _error_base(std::move(ptr))
        {
        }

        explicit error(std::error_code ec)
            : _error_base(ec)
        {
        }

        error(error const &) = default;
        error(error &&) = default;
        error &operator=(error const &) = default;
        error &operator=(error &&) = default;

        auto storage() const noexcept -> storage_type const &
        {
            return _error_base;
        }

        // Return the error message.
        [[nodiscard]] auto what() const -> std::string
        {
            return std::visit(
                overload{[](std::monostate const &) -> std::string {
                             return "success";
                         },
                         [](error_code_type const &ec) -> std::string {
                             return ec.message();
                         },
                         [](exception_type const &exc) -> std::string {
                             if (auto const *eptr = exc->ptr(); eptr)
                                 return eptr->what();

                             return "unknown exception";
                         }},
                _error_base);
        }

        // Return true if the error has a value (failure), false if okay.
        [[nodiscard]] explicit operator bool() const
        {
            return get_if<std::monostate>(&_error_base) == nullptr;
        }

        // Rethrow the stored error object.
        [[noreturn]] auto rethrow() const -> void
        {
            std::visit(overload{[](std::monostate const &) -> void {
                                    throw std::runtime_error("success");
                                },
                                [](error_code_type const &ec) -> void {
                                    throw std::system_error(ec);
                                },
                                [](exception_type const &exc) -> void {
                                    exc->rethrow();
                                }},
                       _error_base);

            throw std::runtime_error("rethrow of empty error type");
        }

        // is<T>(): test if the stored error is or derives from T.
        template <error_type T>
        [[nodiscard]] auto is() const -> bool
        {
            return get<T>() != nullptr;
        }

        // get<T>(): if is<T>(), return a pointer to the error,
        // otherwise return nullptr.
        template <error_type T>
        [[nodiscard]] auto get() const -> T const *
        {
            return std::visit(
                overload{
                    [](std::monostate const &) -> T const * { return nullptr; },
                    [](error_code_type const &) -> T const * {
                        return nullptr;
                    },
                    [](exception_type const &exc) -> T const * {
                        return dynamic_cast<T const *>(exc->ptr());
                    }},
                _error_base);
        }
    };

    template <typename T>
    auto get_if(error const &) -> T const * = delete;

    template <error_type T>
    auto get_if(error const &e) -> T const *
    {
        auto const &storage = e.storage();
        return std::visit(
            overload{
                [](std::monostate const &) -> T const * { return nullptr; },
                [](std::error_code const &) -> T const * { return nullptr; },
                [](error::exception_type const &exc) -> T const * {
                    return dynamic_cast<T const *>(exc->ptr());
                }},
            storage);
    }

    template <>
    inline auto get_if<std::error_code>(error const &e)
        -> std::error_code const *
    {
        auto const &storage = e.storage();
        return std::visit(
            overload{[](std::monostate const &) -> std::error_code const * {
                         return nullptr;
                     },
                     [](std::error_code const &ec) -> std::error_code const * {
                         return &ec;
                     },
                     [](error::exception_type const &)
                         -> std::error_code const * { return nullptr; }},
            storage);
    }

    template <typename T>
    auto is(error const &e) -> bool
    {
        return get_if<T>(e) != nullptr;
    }

    template <typename T>
    auto operator==(error const &e, T errc)
        -> bool requires std::is_error_code_enum_v<T>
    {
        using std::make_error_code;
        auto *ec = get_if<std::error_code>(e);

        if (!ec)
            return false;

        return *ec == make_error_code(errc);
    }

    template <typename T>
    auto operator==(error const &e, T errc)
        -> bool requires std::is_error_condition_enum_v<T>
    {
        using std::make_error_code;
        auto *ec = get_if<std::error_code>(e);

        if (!ec)
            return false;

        return *ec == make_error_code(errc);
    }

    inline auto operator==(error const &e, std::error_code ec) -> bool
    {
        auto *ec_ = get_if<std::error_code>(e);

        if (!ec_)
            return false;

        return *ec_ == ec;
    }

    template <error_type T, typename... Args>
    auto make_error(Args &&...args) -> error
    {
        using errtype = std::remove_cvref_t<T>;
        auto base = std::make_shared<detail::error_exception<errtype>>(
            std::forward<Args>(args)...);
        return error(std::move(base));
    }

    inline auto make_error(std::exception_ptr const &ep) -> error
    {
        auto base = std::make_shared<detail::exception_ptr_holder>(ep);
        return error(std::move(base));
    }

    inline auto make_error(std::error_code ec) -> error
    {
        return error(ec);
    }

    template <typename Err>
    auto make_error(Err e)
        -> error requires std::is_error_code_enum_v<std::remove_cvref_t<Err>>
    {
        return error(make_error_code(e));
    }

    template <typename Err>
    auto make_error(Err e) -> error requires
        std::is_error_condition_enum_v<std::remove_cvref_t<Err>>
    {
        using std::make_error_code;
        return error(make_error_code(e));
    }

} // namespace ivy

#endif // IVY_ERROR_HXX_INCLUDED
