/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DATUM_NULL_HXX_INCLUDED
#define IVY_DATUM_NULL_HXX_INCLUDED

#include <string>

#include <ivy/datum.hxx>

namespace ivy {

    class null_type final : public datum_type {
    public:
        using native_type = nullptr_t;

        auto get() const noexcept -> datum_type const * final;
        auto name() const noexcept -> char const * final;
        auto str(std::any const &v) const -> std::string final;
        auto equal(std::any const &a, std::any const &b) const -> bool final;
    };

    auto get_null_type() -> null_type const *;

    auto make_null_datum() -> datum;

} // namespace ivy

#endif // IVY_DATUM_NULL_HXX_INCLUDED
