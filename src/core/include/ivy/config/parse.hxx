/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CONFIG_PARSE_HXX_INCLUDED
#define IVY_CONFIG_PARSE_HXX_INCLUDED

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <optional>
#include <stdexcept>
#include <variant>

#include <ivy/expected.hxx>
#include <ivy/string.hxx>
#include <ivy/string/format.hxx>

namespace ivy::config {

    /*
     * A datum represents a configuration value of some type.
     */

    class datum {
    public:
        using storage_type =
            std::variant<std::monostate, bool, string, std::int64_t>;

    private:
        storage_type _value;

    public:
        using string_type = string;
        using int_type = std::int64_t;

        datum();
        explicit datum(bool);
        explicit datum(string);
        explicit datum(std::int64_t);

        auto storage() const noexcept -> storage_type const &;

        explicit operator bool() const noexcept;
    };

    auto as_bool(datum const &value) -> std::optional<bool>;
    auto as_string(datum const &value) -> std::optional<datum::string_type>;
    auto as_int(datum const &value) -> std::optional<datum::int_type>;
    auto str(datum const &value) -> string;

    auto operator==(datum const &d, string const &s) -> bool;
    auto operator==(datum const &d, string::value_type const *s) -> bool;
    auto operator==(datum const &d, bool b) -> bool;
    auto operator==(datum const &d, std::int64_t i) -> bool;

    auto operator<<(std::ostream &strm, datum const &v) -> std::ostream &;

    class item {
        datum _name;
        datum _value;
        std::vector<item> _subitems;

    public:
        using iterator = item const *;
        using const_iterator = iterator;
        using value_type = item;
        using size_type = std::size_t;
        using reference = item const &;
        using const_reference = reference;

        item(datum name);
        item(datum name, datum v);
        item(item const &);
        item(item &&) noexcept;

        auto operator=(item const &) -> item &;
        auto operator=(item &&) noexcept -> item &;

        auto add_subitem(item const &) -> void;

        auto name() const noexcept -> datum const &;
        auto value() const noexcept -> datum const &;

        auto size() const noexcept -> size_type;
        auto operator[](size_type n) const noexcept -> const_reference;
        auto begin() const noexcept -> const_iterator;
        auto end() const noexcept -> const_iterator;
    };

    class config_error : public std::runtime_error {
        std::string _error_text;

    public:
        config_error(std::string const &error_text);

        auto error() const noexcept -> std::string const &;
    };

    auto parse(string const &text) -> expected<item, config_error>;

} // namespace ivy::config

namespace std {

    template <typename char_type>
    struct formatter<ivy::config::datum, char_type> {
        formatter<ivy::string> string_formatter;

        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx)
        {
            return string_formatter.parse(ctx);
        }

        template <typename FormatContext>
        auto format(ivy::config::datum const &d, FormatContext &ctx)
        {
            auto tx = ivy::transcode<ivy::string>(str(d)).or_throw();
            return string_formatter.format(tx, ctx);
        }
    };

} // namespace std

#endif // IVY_CONFIG_PARSE_HXX_INCLUDED
