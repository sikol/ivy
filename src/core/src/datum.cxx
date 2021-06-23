/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <ivy/check.hxx>
#include <ivy/datum.hxx>
#include <ivy/datum/integer.hxx>
#include <ivy/datum/string.hxx>
#include <ivy/datum/null.hxx>
#include <ivy/datum/boolean.hxx>
#include <ivy/string/to_string.hxx>

namespace ivy {

    /*************************************************************************
     *
     * datum
     */

    datum::datum(datum_type const *type, std::any const &value)
        : _type(type), _value(value)
    {
    }

    datum::datum(datum_type const *type, std::any &&value)
        : _type(type), _value(std::move(value))
    {
    }

    datum::datum(datum const &) = default;
    datum::datum(datum &&) noexcept = default;

    auto datum::operator=(datum const &) -> datum & = default;
    auto datum::operator=(datum &&) noexcept -> datum & = default;

    auto datum::type() const noexcept -> datum_type const *
    {
        return _type;
    }

    auto datum::str() const -> string
    {
        return _type->str(_value);
    }

    auto datum::equal_to(datum const &other) const -> bool
    {
        if (this == &other)
            return true;

        if (_type != other._type)
            return false;

        return _type->equal(_value, other._value);
    }

    auto datum::storage() const noexcept -> std::any const &
    {
        return _value;
    }

    auto str(datum const &d) -> string
    {
        return d.str();
    }

    auto operator==(datum const &a, datum const &b) -> bool
    {
        return a.equal_to(b);
    }

    /*************************************************************************
     *
     * bad_datum_cast
     */

    bad_datum_cast::bad_datum_cast() : std::runtime_error("bad datum cast") {}

    /*************************************************************************
     *
     * integer_type
     */

    auto integer_type::name() const noexcept -> char const *
    {
        return "integer";
    }

    auto integer_type::str(std::any const &v) const -> string
    {
        auto *i = std::any_cast<std::int64_t>(&v);
        IVY_CHECK(i != nullptr, "integer_type: bad value");

        return to_string<string>(*i);
    }

    auto integer_type::equal(std::any const &a, std::any const &b) const -> bool
    {
        auto *aptr = std::any_cast<std::int64_t>(&a);
        IVY_CHECK(aptr != nullptr, "integer_type: bad value");

        auto *bptr = std::any_cast<std::int64_t>(&b);
        IVY_CHECK(bptr != nullptr, "integer_type: bad value");

        return *aptr == *bptr;
    }

    auto integer_type::get() const noexcept -> datum_type const *
    {
        return get_integer_type();
    }

    auto get_integer_type() -> integer_type const *
    {
        static integer_type type;
        return &type;
    }

    auto make_integer_datum(std::int64_t i) -> datum
    {
        return datum(get_integer_type(), i);
    }

    template <>
    auto datum_cast<std::int64_t>(datum const &d) -> std::int64_t
    {
        auto const *i = std::any_cast<std::int64_t>(&d.storage());
        if (!i)
            throw bad_datum_cast();
        return *i;
    }

    template <>
    auto datum_cast<std::int64_t>(datum const *d) -> std::int64_t const *
    {
        auto const *i = std::any_cast<std::int64_t>(&d->storage());
        return i;
    }

    /*************************************************************************
     *
     * string_type
     */

    auto string_type::name() const noexcept -> char const *
    {
        return "string";
    }

    auto string_type::str(std::any const &v) const -> string
    {
        auto *s = std::any_cast<string>(&v);
        IVY_CHECK(s != nullptr, "string_type: bad value");

        return *s;
    }

    auto string_type::equal(std::any const &a, std::any const &b) const -> bool
    {
        auto *aptr = std::any_cast<string>(&a);
        IVY_CHECK(aptr != nullptr, "string_type: bad value");

        auto *bptr = std::any_cast<string>(&b);
        IVY_CHECK(bptr != nullptr, "string_type: bad value");

        return *aptr == *bptr;
    }

    auto string_type::get() const noexcept -> datum_type const *
    {
        return get_string_type();
    }

    auto get_string_type() -> string_type const *
    {
        static string_type type;
        return &type;
    }

    auto make_string_datum(string const &s) -> datum
    {
        return datum(get_string_type(), s);
    }

    auto make_string_datum(string::value_type const *s) -> datum
    {
        return datum(get_string_type(), string(s));
    }

    template <>
    auto datum_cast<string>(datum const &d) -> string
    {
        auto const *s = std::any_cast<string>(&d.storage());
        if (!s)
            throw bad_datum_cast();
        return *s;
    }

    template <>
    auto datum_cast<string>(datum const *d) -> string const *
    {
        auto const *s = std::any_cast<string>(&d->storage());
        return s;
    }

    /*************************************************************************
     *
     * null_type
     */

    auto null_type::name() const noexcept -> char const *
    {
        return "null";
    }

    auto null_type::str(std::any const &v) const -> string
    {
        auto *s = std::any_cast<nullptr_t>(&v);
        IVY_CHECK(s != nullptr, "null_type: bad value");

        return U"null";
    }

    auto null_type::equal(std::any const &a, std::any const &b) const -> bool
    {
        auto *aptr = std::any_cast<nullptr_t>(&a);
        IVY_CHECK(aptr != nullptr, "null_type: bad value");

        auto *bptr = std::any_cast<nullptr_t>(&b);
        IVY_CHECK(bptr != nullptr, "null_type: bad value");

        return true;
    }

    auto null_type::get() const noexcept -> datum_type const *
    {
        return get_null_type();
    }

    auto get_null_type() -> null_type const *
    {
        static null_type type;
        return &type;
    }

    auto make_null_datum() -> datum
    {
        return datum(get_null_type(), nullptr);
    }

    /*************************************************************************
     *
     * boolean_type
     */

    auto boolean_type::name() const noexcept -> char const *
    {
        return "boolean";
    }

    auto boolean_type::str(std::any const &v) const -> string
    {
        auto *b = std::any_cast<bool>(&v);
        IVY_CHECK(b != nullptr, "boolean_type: bad value");

        return *b ? U"true" : U"false";
    }

    auto boolean_type::equal(std::any const &a, std::any const &b) const -> bool
    {
        auto *aptr = std::any_cast<bool>(&a);
        IVY_CHECK(aptr != nullptr, "boolean_type: bad value");

        auto *bptr = std::any_cast<bool>(&b);
        IVY_CHECK(bptr != nullptr, "boolean_type: bad value");

        return *aptr == *bptr;
    }

    auto boolean_type::get() const noexcept -> datum_type const *
    {
        return get_boolean_type();
    }

    auto get_boolean_type() -> boolean_type const *
    {
        static boolean_type type;
        return &type;
    }

    auto make_boolean_datum(bool b) -> datum
    {
        return datum(get_boolean_type(), b);
    }

    template <>
    auto datum_cast<bool>(datum const &d) -> bool
    {
        auto const *i = std::any_cast<bool>(&d.storage());
        if (!i)
            throw bad_datum_cast();
        return *i;
    }

    template <>
    auto datum_cast<bool>(datum const *d) -> bool const *
    {
        auto const *i = std::any_cast<bool>(&d->storage());
        return i;
    }

} // namespace ivy
