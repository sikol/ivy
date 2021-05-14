/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_ERROR_HXX_INCLUDED
#define IVY_ERROR_HXX_INCLUDED

#include <concepts>
#include <exception>
#include <memory>
#include <type_traits>

namespace ivy {

    // clang-format off
	template<typename T>
	concept error_type = std::derived_from<T, std::exception>;
    // clang-format on

    /*
     * 'error' stores an std::exception as an error value which can be
     * returned and queried.
     */
    struct error {
        /*
         * Create an empty error that indicates success.
         */
        error() = default;

        /*
         * Create an error from an exception type.  The exception is copied
         * and stored inside the error object.
         */
        template <error_type Error>
        error(Error &&e)
            : exception_ptr(std::make_shared<std::remove_cv<Error>::type>(
                  std::forward<Error>(e)))
        {
        }

        error(error const &) = default;
        error(error &&) = default;
        error &operator=(error const &) = default;
        error &operator=(error &&) = default;

        // Return the error message.
        auto what() const -> char const *
        {
            if (!exception_ptr)
                return "success";
            return exception_ptr->what();
        }

        // Return true if the error is empty (success), false if an error
        // condition is present.
        operator bool() const
        {
            return !exception_ptr.operator bool();
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
            if (!exception_ptr)
                return nullptr;
            return dynamic_cast<T const *>(exception_ptr.get());
        }

        // The exception object that we're holding.
        std::shared_ptr<std::exception> exception_ptr;
    };

} // namespace sk

#endif // IVY_ERROR_HXX_INCLUDED
