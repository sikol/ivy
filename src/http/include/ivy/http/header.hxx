/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_HTTP_HEADER_HXX_INCLUDED
#define IVY_HTTP_HEADER_HXX_INCLUDED

#include <unordered_map>

#include <ivy/string.hxx>

namespace ivy::http {

    struct header_field final {
        astring name;
        u8string value;
    };

    class http_header_iterator final {
        using map_type = std::unordered_map<astring, header_field>;
        map_type::iterator _iterator;

        friend auto operator==(http_header_iterator const &a,
                               http_header_iterator const &b) -> bool;
        friend class http_header;

        explicit http_header_iterator(map_type::iterator iterator);

    public:
        http_header_iterator(http_header_iterator const &);
        http_header_iterator(http_header_iterator &&) noexcept;

        auto operator=(http_header_iterator const &) -> http_header_iterator &;
        auto operator=(http_header_iterator &&) noexcept
            -> http_header_iterator &;

        auto operator++() -> http_header_iterator &;
        auto operator++(int) -> http_header_iterator;

        auto operator*() const -> header_field const &;
        auto operator->() const -> header_field const *;
    };

    class http_header final {
        std::unordered_map<astring, header_field> _fields;

    public:
        using key_type = astring;
        using value_type = header_field;
        using pointer = value_type *;
        using reference = value_type &;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using iterator = http_header_iterator;
        using const_iterator = http_header_iterator;

        http_header();
        http_header(http_header const &);
        http_header(http_header &&) noexcept;
        ~http_header();

        auto operator=(http_header const &) -> http_header &;
        auto operator=(http_header &&) noexcept -> http_header &;

        // Retrieve the given field, or nullptr if not found.
        auto get_field(astring const &name) const -> header_field const *;
        auto get_field(astring const &name) -> header_field *;

        // Add a new field if not present; returns true if field was added,
        // false if it already existed.
        auto add_field(header_field const &) -> bool;

        // Add a new field or replace an existing field; returns true if the
        // field already existed, false if not.
        auto add_or_replace_field(header_field const &) -> bool;

        // Container interface.
        auto size() const noexcept -> size_type;

        auto begin() -> iterator;
        auto begin() const -> const_iterator;

        auto end() -> iterator;
        auto end() const -> const_iterator;

        auto find(key_type const &) -> iterator;
        auto find(key_type const &) const -> const_iterator;

        auto erase(iterator const &it) -> iterator;
    };

} // namespace ivy::http

#endif // IVY_HTTP_HEADER_HXX_INCLUDED
