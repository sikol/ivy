/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <format>

#include <ivy/charenc.hxx>
#include <ivy/charenc/error.hxx>
#include <ivy/charenc/utf16.hxx>
#include <ivy/charenc/utf8.hxx>

namespace ivy {

    namespace detail {

        auto charenc_errc_category::name() const noexcept -> char const *
        {
            return "ivy:charenc";
        }

        auto charenc_errc_category::message(int c) const -> std::string
        {
            switch (static_cast<charenc_errc>(c)) {
            case charenc_errc::no_error:
                return "no error";

            case charenc_errc::unrepresentable_character:
                return "source character cannot be represented in the target "
                       "character set";

            case charenc_errc::unrepresentable_codepoint:
                return "source codepoint cannot be represented in the target "
                       "encoding";

            default:
                return "unknown error " + std::to_string(c);
            }
        }

    } // namespace detail

    auto charenc_errc_category() -> detail::charenc_errc_category const &
    {
        static detail::charenc_errc_category c;
        return c;
    }

    auto make_error_code(charenc_errc e) -> std::error_code
    {
        return {static_cast<int>(e), charenc_errc_category()};
    }

} // namespace ivy
