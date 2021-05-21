/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_ERROR_HXX_INCLUDED
#define IVY_CHARENC_ERROR_HXX_INCLUDED

#include <system_error>

namespace ivy {

    enum struct charenc_errc : unsigned {
        no_error = 0,

        // Source character cannot be represented in target character set
        unrepresentable_character = 1,

        // Source codepoint cannot be represented in target encoding
        unrepresentable_codepoint = 2,

        // Source encoding is invalid
        invalid_encoding = 3,
    };

}

namespace std {

    template <>
    struct is_error_code_enum<ivy::charenc_errc> : true_type {
    };

} // namespace std

namespace ivy {

    namespace detail {

        struct charenc_errc_category : std::error_category {
            auto name() const noexcept -> char const * final;
            auto message(int c) const -> std::string final;
        };

    } // namespace detail

    auto charenc_errc_category() -> detail::charenc_errc_category const &;
    auto make_error_code(charenc_errc e) -> std::error_code;

} // namespace ivy

#endif // IVY_CHARENC_ERROR_HXX_INCLUDED
