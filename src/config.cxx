/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <iostream>
#include <regex>

#include <ivy/config/lex.hxx>
#include <ivy/config/parse.hxx>
#include <ivy/overload.hxx>
#include <ivy/string/match.hxx>
#include <ivy/string/strconv.hxx>
#include <ivy/string/trim.hxx>
#include <ivy/trace.hxx>

namespace ivy::config {

    /*************************************************************************
     * token
     */

    token::token(token_type type) : _type(type) {}

    token::token(token_type type, std::string_view value)
        : _type(type), _value(value)
    {
    }

    token::token(token_type type, std::int64_t value)
        : _type(type), _value(value)
    {
    }

#if 0
    token::token(token_type type, std::uint64_t value)
        : _type(type), _value(value)
    {
    }
#endif

    token::token(token const &other) = default;
    token::token(token &&other) = default;
    auto token::operator=(token const &other) -> token & = default;
    auto token::operator=(token &&other) -> token & = default;

    auto token::type() const -> token_type
    {
        return _type;
    }

    auto token::value() const -> std::any const &
    {
        return _value;
    }

    auto as_string(token const &t) -> std::string_view const &
    {
        return *any_cast<std::string_view>(&t.value());
    }

    auto as_int(token const &t) -> std::int64_t
    {
        return any_cast<std::int64_t>(t.value());
    }

    // auto as_uint(token const *t) -> std::uint64_t
    //{
    //    return any_cast<std::uint64_t>(t->value());
    //}

    /*************************************************************************
     * match_next
     */

    auto match_next(std::string_view s)
        -> std::pair<std::optional<token>, std::string_view>
    {
        s = triml(s);

        if (s.empty())
            return {token(T_EOF), s};

        if (auto [i, rest] = match_int<std::int64_t>(s); i) {
            return {token(T_INT, *i), rest};
        }

        static std::regex identifier_regex("^[a-zA-Z][a-zA-Z0-9_-]*");
        if (auto [m, rest] = match_regex(s, identifier_regex); m) {
            return {token(T_IDENTIFIER,
                          std::string_view((*m)[0].first, (*m)[0].second)),
                    rest};
        }

        switch (s[0]) {
        case '{':
            return {token(T_LBRACE), s.substr(1)};
        case '}':
            return {token(T_RBRACE), s.substr(1)};
        case '(':
            return {token(T_LPAREN), s.substr(1)};
        case ')':
            return {token(T_RPAREN), s.substr(1)};
        case '=':
            return {token(T_EQ), s.substr(1)};
        case ';':
            return {token(T_SEMICOLON), s.substr(1)};
        }

        return {{}, s};
    }

    /*************************************************************************
     * datum
     */

    datum::datum() = default;

    datum::datum(bool b) : _value(b) {}

    datum::datum(std::string_view v) : _value(v) {}

    datum::datum(std::int64_t v) : _value(v) {}

    auto datum::storage() const noexcept -> storage_type const &
    {
        return _value;
    }

    datum::operator bool() const noexcept
    {
        return _value.index() > 0;
    }

    auto as_string(datum const &value) -> std::optional<datum::string_type>
    {
        if (auto const *s = get_if<std::string_view>(&value.storage()); s)
            return *s;
        return std::nullopt;
    }

    auto as_int(datum const &value) -> std::optional<datum::int_type>
    {
        if (auto const *i = get_if<datum::int_type>(&value.storage()); i)
            return *i;
        return std::nullopt;
    }

    auto as_bool(datum const &value) -> std::optional<bool>
    {
        if (auto const *b = get_if<bool>(&value.storage()); b)
            return *b;
        return std::nullopt;
    }

    auto str(datum const &value) -> std::string
    {
        auto const &ss = value.storage();

        return std::visit(
            overload([](std::monostate) -> std::string { return "<empty>"; },
                     [](bool b) -> std::string { return b ? "true" : "false"; },
                     [](std::string_view sv) -> std::string {
                         return std::string(sv.begin(), sv.end());
                     },
                     [](std::int64_t i) -> std::string {
                         return std::to_string(i);
                     }),
            ss);
    }

    auto operator==(datum const &d, std::string const &s) -> bool
    {
        auto v = as_string(d);
        return v && (*v == s);
    }

    auto operator==(datum const &d, std::string_view sv) -> bool
    {
        auto v = as_string(d);
        return v && (*v == sv);
    }

    auto operator==(datum const &d, char const *s) -> bool
    {
        auto v = as_string(d);
        return v && (*v == s);
    }

    auto operator==(datum const &d, std::int64_t i) -> bool
    {
        auto v = as_int(d);
        return v && (*v == i);
    }

    auto operator==(datum const &d, bool b) -> bool
    {
        auto v = as_bool(d);
        return v && (*v == b);
    }

    auto operator<<(std::ostream &strm, datum const &v) -> std::ostream &
    {
        strm << str(v);
        return strm;
    }

    /*************************************************************************
     * item
     */

    item::item(datum name) : _name(name) {}
    item::item(datum name, datum value) : _name(name), _value(value) {}

    item::item(item const &) = default;
    item::item(item &&) noexcept = default;

    auto item::operator=(item const &) -> item & = default;
    auto item::operator=(item &&) noexcept -> item & = default;

    auto item::name() const noexcept -> datum const &
    {
        return _name;
    }

    auto item::value() const noexcept -> datum const &
    {
        return _value;
    }

    auto item::add_subitem(item const &si) -> void
    {
        _subitems.push_back(si);
    }

    auto item::size() const noexcept -> size_type
    {
        return _subitems.size();
    }

    auto item::operator[](size_type n) const noexcept -> const_reference
    {
        IVY_CHECK(n < _subitems.size(),
                  "config::item::operator[]: out of range");
        return _subitems[n];
    }

    auto item::begin() const noexcept -> const_iterator
    {
        return &_subitems[0];
    }

    auto item::end() const noexcept -> const_iterator
    {
        return &_subitems[0] + _subitems.size();
    }

    /*************************************************************************
     * config_error
     */

    config_error::config_error(std::string const &error_text)
        : std::runtime_error(error_text), _error_text(error_text)
    {
    }

    auto config_error::error() const noexcept -> std::string const &
    {
        return _error_text;
    }

    /*************************************************************************
     * parser
     */

    namespace {

        auto parse_qstring(std::string_view s)
            -> std::pair<std::optional<std::string_view>, std::string_view>
        {
            static constexpr char quote = '"';

            s = triml(s);

            auto begin = s.begin();
            auto end = s.end();

            if (begin == end || *begin != quote)
                return {{}, s};

            ++begin;
            auto string_start = begin;

            while (begin < end) {
                switch (*begin) {
                case quote:
                    return {std::string_view(string_start, begin),
                            std::string_view(std::next(begin), end)};

                default:
                    ++begin;
                    break;
                }
            }

            return {{}, s};
        }

        auto parse_value(std::string_view s)
            -> std::pair<std::optional<datum>, std::string_view>
        {
            if (auto [qstring, rest] = parse_qstring(s); qstring)
                return {datum(*qstring), rest};

            auto [tok, rest] = match_next(s);

            if (!tok)
                return {{}, s};

            switch (tok->type()) {
            case T_IDENTIFIER:
                return {datum(as_string(*tok)), rest};

            case T_INT:
                return {datum(as_int(*tok)), rest};

            default:
                return {{}, s};
            }
        }

        auto expect(std::string_view s, token_type ttype)
            -> std::pair<std::optional<token>, std::string_view>
        {
            auto [tok, rest] = match_next(s);

            if (tok && (tok->type() == ttype))
                return {tok, rest};

            return {{}, s};
        }

        auto parse_item(std::string_view s)
            -> std::pair<std::optional<item>, std::string_view>
        {
            std::string_view rest = s;
            std::optional<datum> vname;

            // Match the item name.

            IVY_TRACE("config: parse_item: looking for name, rest=[{}...]",
                      rest.substr(0, 10));

            if (std::tie(vname, rest) = parse_value(rest); !vname) {
                IVY_TRACE("config: parse_item: no name");
                return {{}, s};
            }

            // Match the value; might be empty for a bool.

            IVY_TRACE("config: parse_item: looking for value, rest=[{}...]",
                      rest.substr(0, 10));

            std::optional<datum> vval;
            if (std::tie(vval, rest) = parse_value(rest); !vval) {
                IVY_TRACE("config: no value");
                vval = datum(true);
            }

            auto itm = item(*vname, *vval);

            // Match optional { ... }

            IVY_TRACE(
                "config: parse_item: looking for ';' or '{{', rest=[{}...]",
                rest.substr(0, 10));

            std::optional<token> tok;
            if (std::tie(tok, rest) = match_next(rest); !tok) {
                IVY_TRACE("config: parse_item: no ';' or '{{'");
                return {{}, s};
            }

            if (tok->type() == T_LBRACE) {
                IVY_TRACE("config: parse_item: found '{{'");

                std::optional<item> sitm;

                for (;;) {
                    IVY_TRACE("config: parse_item: looking for subitem");

                    if (std::tie(sitm, rest) = parse_item(rest); !sitm) {
                        IVY_TRACE("config: parse_item: no subitem, break");
                        break;
                    }

                    IVY_TRACE("config: parse_item: found subitem, name=[{}]",
                              str(sitm->name()));

                    itm.add_subitem(std::move(*sitm));
                }

                IVY_TRACE("config: parse_item: finished looking for subitems");

                // Match closing '}'
                if (std::tie(tok, rest) = expect(rest, T_RBRACE); !tok) {
                    IVY_TRACE(
                        "config: parse_item: did not find '}' after subitems");
                    return {{}, s};
                }

                IVY_TRACE("config: parse_item: subitems done");
                if (std::tie(tok, rest) = match_next(rest); !tok) {
                    IVY_TRACE("config: parse_item: no token after subitems");
                    return {{}, s};
                }
            }

            // Match ';'

            IVY_TRACE("config: parse_item: looking for ';', rest=[{}...]",
                      rest.substr(0, 10));

            if (tok->type() != T_SEMICOLON) {
                IVY_TRACE("config: parse_item: did not find ';'");
                return {{}, s};
            }

            IVY_TRACE("config: parse_item: done, return item");

            return {itm, rest};
        }

    } // namespace

    auto parse(std::string const &text) -> expected<config, config_error>
    {
        config ret;
        ret.text = std::make_shared<std::string>(text);

        std::optional<item> itm;
        std::string_view rest = *ret.text;

        try {
            for (;;) {
                IVY_TRACE("config: looking for an item");

                if (std::tie(itm, rest) = parse_item(rest); !itm) {
                    IVY_TRACE("config: no item, break");
                    break;
                }

                IVY_TRACE("config: got item, name=[{}]", str(itm->name()));

                ret.items.push_back(std::move(*itm));
            }
        } catch (config_error const &e) {
            return make_unexpected(e);
        }

        rest = triml(rest);

        if (!rest.empty())
            return make_unexpected(
                config_error("unexpected data at end of input"));

        return ret;
    }

} // namespace ivy::config
