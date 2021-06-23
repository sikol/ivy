/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DATUM_HXX_INCLUDED
#define IVY_DATUM_HXX_INCLUDED

#include <any>
#include <stdexcept>

#include <ivy/string.hxx>

namespace ivy {

    class datum_type {
    public:
        virtual auto get() const noexcept -> datum_type const * = 0;
        virtual auto name() const noexcept -> char const * = 0;
        virtual auto str(std::any const &v) const -> string = 0;
        virtual auto equal(std::any const &a, std::any const &b) const
            -> bool = 0;
    };

    class datum final {
        datum_type const *_type;
        std::any _value;

    public:
        datum(datum_type const *type, std::any const &value);
        datum(datum_type const *type, std::any &&value);

        datum(datum const &other);
        datum(datum &&other) noexcept;

        auto operator=(datum const &other) -> datum &;
        auto operator=(datum &&other) noexcept -> datum &;

        auto type() const noexcept -> datum_type const *;
        auto str() const -> string;
        auto equal_to(datum const &other) const -> bool;
        auto storage() const noexcept -> std::any const &;
    };

    auto str(datum const &) -> string;
    auto operator==(datum const &a, datum const &b) -> bool;

    template<typename Type>
    auto is(datum const &d) -> bool
    {
        return d.type() == Type().get();
    }

    class bad_datum_cast : public std::runtime_error {
    public:
        bad_datum_cast();
    };

    template<typename To>
    auto datum_cast(datum const &) -> To = delete;

    template<typename To>
    auto datum_cast(datum const *) -> To const * = delete;

} // namespace ivy

#endif // IVY_DATUM_HXX_INCLUDED
