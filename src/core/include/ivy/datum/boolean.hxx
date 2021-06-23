/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DATUM_BOOLEAN_HXX_INCLUDED
#define IVY_DATUM_BOOLEAN_HXX_INCLUDED

#include <ivy/datum.hxx>

namespace ivy {

    class boolean_type final : public datum_type {
    public:
        using native_type = bool;

        auto get() const noexcept -> datum_type const * final;
        auto name() const noexcept -> char const * final;
        auto str(std::any const &v) const -> string final;
        auto equal(std::any const &a, std::any const &b) const -> bool final;
    };

    auto get_boolean_type() -> boolean_type const *;

    auto make_boolean_datum(bool b) -> datum;

    template <>
    auto datum_cast<bool>(datum const &) -> bool;

    template <>
    auto datum_cast<bool>(datum const *) -> bool const *;

} // namespace ivy

#endif // IVY_DATUM_BOOLEAN_HXX_INCLUDED
