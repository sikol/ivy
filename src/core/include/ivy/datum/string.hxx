/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DATUM_STRING_HXX_INCLUDED
#define IVY_DATUM_STRING_HXX_INCLUDED

#include <string>

#include <ivy/datum.hxx>

namespace ivy {

    class string_type final : public datum_type {
    public:
        using native_type = string;

        auto get() const noexcept -> datum_type const * final;
        auto name() const noexcept -> char const * final;
        auto str(std::any const &v) const -> string final;
        auto equal(std::any const &a, std::any const &b) const -> bool final;
    };

    auto get_string_type() -> string_type const *;

    auto make_string_datum(string const &s) -> datum;
    auto make_string_datum(string::value_type const *s) -> datum;

    template <>
    auto datum_cast<string>(datum const &) -> string;

    template <>
    auto datum_cast<string>(datum const *) -> string const *;

} // namespace ivy

#endif // IVY_DATUM_STRING_HXX_INCLUDED
