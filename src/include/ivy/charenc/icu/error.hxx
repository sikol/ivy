/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_ICU_ERROR_HXX_INCLUDED
#define IVY_CHARENC_ICU_ERROR_HXX_INCLUDED

#include <system_error>
#include <string>

namespace ivy::icu {

    enum struct errc : unsigned {
        no_error = 0,
    };

}

namespace std {

    template <>
    struct is_error_code_enum<ivy::icu::errc> : true_type {
    };

} // namespace std

namespace ivy::icu {

    namespace detail {

        struct errc_category : std::error_category {
            auto name() const noexcept -> char const * final;
            auto message(int c) const -> std::string final;
        };

    } // namespace detail

    auto errc_category() -> detail::errc_category const &;
    auto make_error_code(errc e) -> std::error_code;

} // namespace ivy::icu

#endif // IVY_CHARENC_ICU_ERROR_HXX_INCLUDED
