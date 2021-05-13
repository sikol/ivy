/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

/*
 * Configuration file parser.
 */

#ifndef IVY_CONFIG_LEX_HXX_INCLUDED
#define IVY_CONFIG_LEX_HXX_INCLUDED

#include <any>
#include <cstdint>
#include <optional>
#include <string_view>

namespace ivy::config {

    // A token

    enum token_type {
        T_EOF = 0,
        T_INT,
        T_UINT,
        T_STRING,
        T_IDENTIFIER,
        T_LBRACE,
        T_RBRACE,
        T_LPAREN,
        T_RPAREN,
        T_EQ,
        T_SEMICOLON,
    };

    class token {
    private:
        std::any _value;
        token_type _type;

    public:
        explicit token(token_type type);
        explicit token(token_type type, std::string_view value);
        explicit token(token_type type, std::int64_t value);
        // explicit token(token_type type, std::uint64_t value);
        token(token const &other);
        token(token &&other);
        auto operator=(token const &other) -> token &;
        auto operator=(token &&other) -> token &;

        auto type() const -> token_type;
        auto value() const -> std::any const &;
    };

    auto as_string(token const &) -> std::string_view const &;
    auto as_int(token const &) -> std::int64_t;
    // auto as_uint(token const *) -> std::uint64_t;

    auto match_next(std::string_view s)
        -> std::pair<std::optional<token>, std::string_view>;

} // namespace ivy::config

#endif // IVY_CONFIG_LEX_HXX_INCLUDED
