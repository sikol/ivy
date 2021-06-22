/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CONFIG_BIND_HXX_INCLUDED
#define IVY_CONFIG_BIND_HXX_INCLUDED

#include <map>
#include <vector>

#include <ivy/config/parse.hxx>
#include <ivy/error.hxx>
#include <ivy/exception.hxx>
#include <ivy/expected.hxx>
#include <ivy/string.hxx>
#include <ivy/string/format.hxx>

namespace ivy::config {

    using config_facility = facility<"CONFIG">;

    using config_wrong_type =
        ivy::message<config_facility,
                     'E',
                     "BADTYPE",
                     "Option was expected to have type '{}'">;

    using config_duplicate_option =
        ivy::message<config_facility,
                     'E',
                     "DUPOPT",
                     "Attempt to configure option '{}' that already exists">;

    using config_unknown_option =
        ivy::message<config_facility, 'E', "UNKOPT", "Unknown option '{}'">;

    template <typename T>
    class block;

    /*
     * Bind parsers
     */

    template <typename T>
    class option_parser;

    template <std::integral T>
    class option_parser<T> final {
    public:
        auto parse(item const &itm) const noexcept -> expected<T, error>
        try {
            auto i = as_int(itm.value());
            if (!i)
                return make_unexpected(
                    make_error<config_wrong_type>("integer"));

            return static_cast<T>(*i);
        } catch (...) {
            return make_unexpected(make_error(std::current_exception()));
        }
    };

    template <>
    class option_parser<string> final {
    public:
        auto parse(item const &itm) const noexcept -> expected<string, error>
        try {
            auto str = as_string(itm.value());
            if (!str)
                return make_unexpected(make_error<config_wrong_type>("string"));

            return *str;
        } catch (...) {
            return make_unexpected(make_error(std::current_exception()));
        }
    };

    template <typename child_block>
    class block_parser final {
        block<child_block> _child_schema;

    public:
        block_parser(block<child_block> child_schema)
            : _child_schema(child_schema)
        {
        }

        auto parse(item const &itm) const noexcept -> expected<child_block, error>
        try {
            child_block child{};
            _child_schema.load(child, itm).or_throw();
            return child;
        } catch (...) {
            return make_unexpected(make_error(std::current_exception()));
        }
    };

    /*
     * Setters
     */

    template <typename block_type>
    class item_setter {
    public:
        virtual ~item_setter() = default;
        virtual auto set(block_type &, item const &) const noexcept
            -> expected<void, error> = 0;
    };

    template <typename block_type,
              typename value_type,
              typename parser_type = option_parser<value_type>>
    class single_item_setter final : public item_setter<block_type> {
    private:
        value_type block_type::*_value_ptr;
        parser_type _parser;

    public:
        single_item_setter(value_type block_type::*value_ptr,
                           parser_type const &parser = {})
            : _value_ptr(value_ptr)
            , _parser(parser)
        {
        }

        auto set(block_type &block, item const &itm) const noexcept
            -> expected<void, error> override
        try {
            auto v = _parser.parse(itm).or_throw();
            block.*_value_ptr = v;
            return {};
        } catch (...) {
            return make_unexpected(make_error(std::current_exception()));
        }
    };

    template <typename block_type,
              typename value_type,
              typename parser_type = option_parser<value_type>>
    class vector_item_setter final : public item_setter<block_type> {
    private:
        std::vector<value_type> block_type::*_value_ptr;
        parser_type _parser;

    public:
        vector_item_setter(std::vector<value_type> block_type::*value_ptr,
                           parser_type const &parser = {})
            : _value_ptr(value_ptr)
            , _parser(parser)
        {
        }

        auto set(block_type &block, item const &itm) const noexcept
            -> expected<void, error> override
        try {
            auto v = _parser.parse(itm).or_throw();
            (block.*_value_ptr).push_back(v);
            return {};
        } catch (...) {
            return make_unexpected(make_error(std::current_exception()));
        }
    };

    template <typename C>
    class block final {
    public:
        using option_ptr = std::shared_ptr<item_setter<C>>;

    private:
        std::map<string, option_ptr> _options;

    public:
        static auto create() -> block
        {
            return block();
        }

        block() = default;
        block(block const &) = default;
        block(block &&) = default;

        template <typename option_type>
        requires std::derived_from<option_type, item_setter<C>>
        auto add_option(string const &name, option_type const &option)
            -> expected<void, error>
        try {
            auto optr = std::make_shared<option_type>(option);

            auto [it, ok] = _options.insert(std::pair(name, std::move(optr)));
            if (!ok)
                return make_unexpected(
                    make_error<config_duplicate_option>(name));

            return {};
        } catch (...) {
            return make_unexpected(make_error(std::current_exception()));
        }

        template <typename value_type>
        auto add_option(string const &name, value_type C::*value_ptr) -> block &
        {
            auto option = single_item_setter<C, value_type>(value_ptr);
            add_option(name, option);
            return *this;
        }

        template <typename value_type>
        auto add_option(string const &name,
                        std::vector<value_type> C::*value_ptr) -> block &
        {
            auto option = vector_item_setter<C, value_type>(value_ptr);
            add_option(name, option);
            return *this;
        }

        template <typename child_type>
        auto add_block(string const &name,
                       child_type C::*value_ptr,
                       block<child_type> const &child_schema) -> block &
        {
            auto parser = block_parser<child_type>(child_schema);
            auto option =
                single_item_setter<C, child_type, block_parser<child_type>>(
                    value_ptr, parser);
            add_option(name, option);
            return *this;
        }

        template <typename child_type>
        auto add_block(string const &name,
                       std::vector<child_type> C::*value_ptr,
                       block<child_type> const &child_schema) -> block &
        {
            auto parser = block_parser<child_type>(child_schema);
            auto option =
                vector_item_setter<C, child_type, block_parser<child_type>>(
                    value_ptr, parser);
            add_option(name, option);
            return *this;
        }

        auto load(C &obj, item const &item) const noexcept
            -> expected<void, error>
        try {
            for (auto &&subitem : item) {
                auto subitem_name = as_string(subitem.name());
                if (!subitem_name)
                    return make_unexpected(
                        make_error<config_unknown_option>(subitem.name()));

                auto it = _options.find(*subitem_name);
                if (it == _options.end())
                    return make_unexpected(
                        make_error<config_unknown_option>(subitem.name()));

                it->second->set(obj, subitem);
            }

            return {};
        } catch (...) {
            return make_unexpected(make_error(std::current_exception()));
        }
    };

} // namespace ivy::config

#endif // IVY_CONFIG_BIND_HXX_INCLUDED
