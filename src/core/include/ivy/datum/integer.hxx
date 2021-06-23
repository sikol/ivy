/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DATUM_INTEGER_HXX_INCLUDED
#define IVY_DATUM_INTEGER_HXX_INCLUDED

#include <ivy/datum.hxx>

namespace ivy {

    class integer_type final : public datum_type {
    public:
        using native_type = std::int64_t;

        auto get() const noexcept -> datum_type const * final;
        auto name() const noexcept -> char const * final;
        auto str(std::any const &v) const -> string final;
        auto equal(std::any const &a, std::any const &b) const -> bool final;
    };

    auto get_integer_type() -> integer_type const *;

    auto make_integer_datum(std::int64_t i) -> datum;

    template<>
    auto datum_cast<std::int64_t>(datum const &) -> std::int64_t;

    template <>
    auto datum_cast<std::int64_t>(datum const *) -> std::int64_t const *;

} // namespace ivy

#endif // IVY_DATUM_INTEGER_HXX_INCLUDED
