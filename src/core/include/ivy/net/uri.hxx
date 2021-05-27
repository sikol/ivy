/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_NET_URI_HXX_INCLUDED
#define IVY_NET_URI_HXX_INCLUDED

#include <algorithm>
#include <array>
#include <charconv>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>

#include <ivy/expected.hxx>
#include <ivy/flagset.hxx>
#include <ivy/string.hxx>

namespace ivy::net {

    enum struct uri_errors : int {
        no_error = 0,
        parse_failed = 1,
        invalid_port = 2,
        no_data = 3,
        relative = 4,
        protocol_relative = 5,
        invalid_encoding = 6,
    };

}

namespace std {

    template <>
    struct is_error_code_enum<ivy::net::uri_errors> : true_type {
    };

} // namespace std

namespace ivy::net {

    namespace detail {

        struct uri_errc_category : std::error_category {
            [[nodiscard]] auto name() const noexcept -> char const * final;
            [[nodiscard]] auto message(int c) const -> std::string final;
        };

    } // namespace detail

    auto uri_errc_category() -> detail::uri_errc_category const &;
    auto make_uri_error(uri_errors e) -> std::error_code;

    /*
     * A URI.
     *
     * [scheme://[username[:password]@]host[:port]][/path[&query][#fragment]]
     */

    struct uri_authority {
        std::optional<ivy::u8string> username;
        std::optional<ivy::u8string> password;
        ivy::u8string hostname;
        std::optional<unsigned> port;
    };

    struct uri_path {
        ivy::u8string path;
        std::optional<ivy::u8string> query;
        std::optional<ivy::u8string> fragment;
    };

    struct uri {
        std::optional<ivy::u8string> scheme;
        std::optional<uri_authority> authority;
        std::optional<uri_path> path;
    };

    [[nodiscard]] auto is_absolute(uri const &u) -> bool;
    [[nodiscard]] auto is_relative(uri const &u) -> bool;
    [[nodiscard]] auto is_protocol_relative(uri const &u) -> bool;

    struct urioptions_tag {
    };
    using urioption = flag<urioptions_tag, std::uint_fast8_t>;

    namespace uri_options {
        static constexpr urioption none{0u};
        static constexpr urioption allow_relative = {1u << 0};
        static constexpr urioption allow_protocol_relative{1u << 1};
        static constexpr urioption skip_path_decode{1u << 2};
    } // namespace uri_options

    [[nodiscard]] auto
    parse_uri(ivy::u8string const &s,
              urioption::flagset options = uri_options::none) noexcept
        -> expected<uri, std::error_code>;

    auto operator<<(std::ostream &strm, uri const &uri) -> std::ostream &;
    auto str(uri const &u) -> ivy::u8string;

} // namespace ivy::net

#endif // IVY_NET_URI_HXX_INCLUDED
