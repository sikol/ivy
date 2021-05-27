/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <format>

#include <ivy/error.hxx>

namespace ivy {

    namespace detail {

        auto errc_category::name() const noexcept -> char const *
        {
            return "ivy";
        }

        auto errc_category::message(int c) const -> std::string
        {
            switch (static_cast<errc>(c)) {
            case errc::no_error:
                return "no error";

            case errc::unrepresentable_character:
                return "source character cannot be represented in the target "
                       "character set";

            case errc::unrepresentable_codepoint:
                return "source codepoint cannot be represented in the target "
                       "encoding";

            case errc::invalid_encoding:
                return "invalid encoding";

            case errc::end_of_file:
                return "end of file";

            default:
                return "unknown error " + std::to_string(c);
            }
        }

    } // namespace detail

    auto errc_category() -> detail::errc_category const &
    {
        static detail::errc_category c;
        return c;
    }

    auto make_error_code(errc e) -> std::error_code
    {
        return {static_cast<int>(e), errc_category()};
    }

} // namespace ivy
