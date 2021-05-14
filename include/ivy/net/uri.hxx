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
            [[nodiscard]] auto name() const noexcept -> char const * final
            {
                return "sk:uri";
            }

            [[nodiscard]] auto message(int c) const -> std::string final
            {
                switch (static_cast<uri_errors>(c)) {
                case uri_errors::no_error:
                    return "no error";

                case uri_errors::parse_failed:
                    return "cannot parse URI";

                case uri_errors::invalid_port:
                    return "invalid port";

                case uri_errors::no_data:
                    return "URI is empty";

                case uri_errors::relative:
                    return "relative URI not allowed";

                case uri_errors::protocol_relative:
                    return "protocol-relative URI not allowed";

                case uri_errors::invalid_encoding:
                    return "URI encoding is invalid";

                default:
                    return "unknown URI error";
                }
            }
        };

    } // namespace detail

    inline auto uri_errc_category() -> detail::uri_errc_category const &
    {
        static detail::uri_errc_category c;
        return c;
    }

    inline auto make_uri_error(uri_errors e) -> std::error_code
    {
        return {static_cast<int>(e), uri_errc_category()};
    }

    /*
     * A URI.
     *
     * [scheme://[username[:password]@]host[:port]][/path[&query][#fragment]]
     */

    struct uri_authority {
        std::optional<std::string_view> username;
        std::optional<std::string_view> password;
        std::string_view hostname;
        std::optional<unsigned> port;
    };

    struct uri_path {
        std::string_view path;
        std::optional<std::string_view> query;
        std::optional<std::string_view> fragment;
    };

    struct uri {
        std::optional<std::string_view> scheme;
        std::optional<uri_authority> authority;
        std::optional<uri_path> path;

        // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
        std::shared_ptr<char[]> original_data;
    };

    [[nodiscard]] constexpr auto is_absolute(uri const &u) -> bool
    {
        return u.scheme.has_value() && u.authority.has_value();
    }

    [[nodiscard]] constexpr auto is_relative(uri const &u) -> bool
    {
        return !u.scheme.has_value() && !u.authority.has_value();
    }

    [[nodiscard]] constexpr auto is_protocol_relative(uri const &u) -> bool
    {
        return u.authority.has_value() && !u.scheme.has_value();
    }

    namespace detail {

        static constexpr std::uint_fast16_t hostc = 1u << 1;
        static constexpr std::uint_fast16_t userc = 1u << 2;
        static constexpr std::uint_fast16_t pathc = 1u << 3;
        static constexpr std::uint_fast16_t highc = 1u << 4;
        static constexpr std::uint_fast16_t schemec = 1u << 5;
        static constexpr std::uint_fast16_t numc = 1u << 6;
        static constexpr std::uint_fast16_t queryc = 1u << 7;
        static constexpr std::uint_fast16_t fragc = 1u << 8;
        static constexpr std::uint_fast16_t ip6c = 1u << 9;

        // A different implementation will be needed for PDP-10.
        static_assert(std::numeric_limits<unsigned char>::max() == 255);

        // clang-format off
        constexpr std::array<int, 256> chars = {
            0, /* 00, NUL */ 0, /* 01, SOH */ 0, /* 02, STX */ 0, /* 03, ETX */
            0, /* 04, EOT */ 0, /* 05, ENQ */ 0, /* 06, ACK */ 0, /* 07, BEL */
            0, /* 08, BS */  0, /* 09, HT */  0, /* 0A, LF */  0, /* 0B, VT */
            0, /* 0C, FF */  0, /* 0D, CR */  0, /* 0E, SO */  0, /* 0F, SO */
            0, /* 10, DLE */ 0, /* 11, DC1 */ 0, /* 12, DC2 */ 0, /* 13, DC3 */
            0, /* 14, DC4 */ 0, /* 15, NAK */ 0, /* 16, SYN */ 0, /* 17, ETB */
            0, /* 18, CAN */ 0, /* 19, EM */  0, /* 1A, SUB */ 0, /* 1B, ESC */
            0, /* 1C, FS */  0, /* 1D, GS */  0, /* 1E, RS */  0, /* 1F, US */
            /* 20, SPACE */ 0,
            /* 21, ! */     userc | hostc | pathc | queryc | fragc,
            /* 22, " */     0,
            /* 23, # */     0,
            /* 24, $ */     userc | hostc | pathc | queryc | fragc,
            /* 25, % */     pathc | queryc | fragc,
            /* 26, & */     userc | hostc | pathc | queryc | fragc,
            /* 27, ' */     userc | hostc | pathc | queryc | fragc,
            /* 28, ( */     userc | hostc | pathc | queryc | fragc,
            /* 29, ) */     userc | hostc | pathc | queryc | fragc,
            /* 2A, * */     userc | hostc | pathc | queryc | fragc,
            /* 2B, + */     schemec | userc | hostc | pathc | queryc | fragc,
            /* 2C, , */     userc | hostc | pathc | queryc | fragc,
            /* 2D, - */     schemec | userc | hostc | pathc | queryc | fragc,
            /* 2E, . */     schemec | userc | hostc | pathc | queryc | fragc,
            /* 2F, / */     pathc | queryc | fragc,
            /* 30, 0 */     schemec | hostc | userc | pathc | numc | queryc | fragc | ip6c,
            /* 31, 1 */     schemec | hostc | userc | pathc | numc | queryc | fragc | ip6c,
            /* 32, 2 */     schemec | hostc | userc | pathc | numc | queryc | fragc | ip6c,
            /* 33, 3 */     schemec | hostc | userc | pathc | numc | queryc | fragc | ip6c,
            /* 34, 4 */     schemec | hostc | userc | pathc | numc | queryc | fragc | ip6c,
            /* 35, 5 */     schemec | hostc | userc | pathc | numc | queryc | fragc | ip6c,
            /* 36, 6 */     schemec | hostc | userc | pathc | numc | queryc | fragc | ip6c,
            /* 37, 7 */     schemec | hostc | userc | pathc | numc | queryc | fragc | ip6c,
            /* 38, 8 */     schemec | hostc | userc | pathc | numc | queryc | fragc | ip6c,
            /* 39, 9 */     schemec | hostc | userc | pathc | numc | queryc | fragc | ip6c,
            /* 3A, : */     pathc | queryc | fragc | ip6c,
            /* 3B, ; */     userc | hostc | pathc | queryc | fragc,
            /* 3C, < */     0,
            /* 3D, = */     userc | hostc | pathc | queryc | fragc,
            /* 3E, > */     0,
            /* 3F, ? */     queryc | fragc,
            /* 40, @ */     pathc | queryc | fragc,
            /* 41, A */     schemec | hostc | userc | pathc | queryc | fragc | ip6c,
            /* 42, B */     schemec | hostc | userc | pathc | queryc | fragc | ip6c,
            /* 43, C */     schemec | hostc | userc | pathc | queryc | fragc | ip6c,
            /* 44, D */     schemec | hostc | userc | pathc | queryc | fragc | ip6c,
            /* 45, E */     schemec | hostc | userc | pathc | queryc | fragc | ip6c,
            /* 46, F */     schemec | hostc | userc | pathc | queryc | fragc | ip6c,
            /* 47, G */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 48, H */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 49, I */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 4A, J */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 4B, K */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 4C, L */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 4D, M */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 4E, N */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 4F, O */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 50, P */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 51, Q */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 52, R */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 53, S */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 54, T */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 55, U */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 56, V */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 57, W */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 58, X */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 59, Y */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 5A, Z */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 5B, [ */     0,
            /* 5C, \ */     0,
            /* 5D, ] */     0,
            /* 5E, ^ */     0,
            /* 5F, _ */     schemec | userc | hostc | pathc | queryc | fragc,
            /* 60, ` */     0,
            /* 61, a */     schemec | hostc | userc | pathc | queryc | fragc | ip6c,
            /* 62, b */     schemec | hostc | userc | pathc | queryc | fragc | ip6c,
            /* 63, c */     schemec | hostc | userc | pathc | queryc | fragc | ip6c,
            /* 64, d */     schemec | hostc | userc | pathc | queryc | fragc | ip6c,
            /* 65, e */     schemec | hostc | userc | pathc | queryc | fragc | ip6c,
            /* 66, f */     schemec | hostc | userc | pathc | queryc | fragc | ip6c,
            /* 67, g */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 68, h */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 69, i */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 6A, j */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 6B, k */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 6C, l */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 6D, m */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 6E, n */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 6F, o */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 70, p */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 71, q */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 72, r */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 73, s */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 74, t */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 75, u */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 76, v */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 77, w */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 78, x */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 79, y */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 7A, z */     schemec | hostc | userc | pathc | queryc | fragc,
            /* 7B, { */     0,
            /* 7C, | */     0,
            /* 7D, } */     0,
            /* 7E, ~ */     schemec | userc | hostc | pathc | queryc | fragc,
            /* 7F, DEL */   0,
            /* 80 - 83 */   highc, highc, highc, highc,
            /* 84 - 87 */   highc, highc, highc, highc,
            /* 88 - 8B */   highc, highc, highc, highc,
            /* 8C - 8F */   highc, highc, highc, highc,
            /* 90 - 93 */   highc, highc, highc, highc,
            /* 94 - 97 */   highc, highc, highc, highc,
            /* 98 - 9B */   highc, highc, highc, highc,
            /* 9C - 9F */   highc, highc, highc, highc,
            /* A0 - A3 */   highc, highc, highc, highc,
            /* A4 - A7 */   highc, highc, highc, highc,
            /* A8 - AB */   highc, highc, highc, highc,
            /* AC - AF */   highc, highc, highc, highc,
            /* B0 - B3 */   highc, highc, highc, highc,
            /* B4 - B7 */   highc, highc, highc, highc,
            /* B8 - BB */   highc, highc, highc, highc,
            /* BC - BF */   highc, highc, highc, highc,
            /* D0 - D3 */   highc, highc, highc, highc,
            /* D4 - D7 */   highc, highc, highc, highc,
            /* D8 - DB */   highc, highc, highc, highc,
            /* DC - DF */   highc, highc, highc, highc,
            /* E0 - E3 */   highc, highc, highc, highc,
            /* E4 - E7 */   highc, highc, highc, highc,
            /* E8 - EB */   highc, highc, highc, highc,
            /* EC - EF */   highc, highc, highc, highc,
            /* F0 - F3 */   highc, highc, highc, highc,
            /* F4 - F7 */   highc, highc, highc, highc,
            /* F8 - FB */   highc, highc, highc, highc,
            /* FC - FF */   highc, highc, highc, highc,
        };
        // clang-format on

        [[nodiscard]] inline auto isc(int what, char c) noexcept -> bool
        {
            return (chars[static_cast<unsigned char>(c)] & what) != 0;
        }

        [[nodiscard]] inline auto span(std::string_view s, int what) noexcept
            -> std::pair<std::string_view, std::string_view>
        {
            auto split = std::ranges::find_if_not(
                s, [=](auto c) { return isc(what, c); });
            return {{s.begin(), split}, {split, s.end()}};
        }

        [[nodiscard]] inline auto match_scheme(std::string_view s) noexcept
            -> std::pair<std::optional<std::string_view>, std::string_view>
        {
            auto [scheme, rest] = span(s, schemec);

            if (rest.empty() || rest[0] != ':')
                return {{}, s};

            return {scheme, rest.substr(1)};
        }

        [[nodiscard]] inline auto
        match_username_password(std::string_view s) noexcept
            -> std::tuple<std::optional<std::string_view>,
                          std::optional<std::string_view>,
                          std::string_view>
        {
            std::string_view username, password, v;
            std::tie(username, v) = span(s, userc);

            if (username.empty() || v.empty())
                return {{}, {}, s};

            if (v[0] == '@')
                return {username, {}, v.substr(1)};

            if (v[0] != ':')
                return {{}, {}, s};

            v = v.substr(1);
            std::tie(password, v) = span(v, userc);

            if (v.empty() || v[0] != '@')
                return {{}, {}, s};

            return {username, password, v.substr(1)};
        }

        [[nodiscard]] inline auto match_number(std::string_view s) noexcept
            -> std::pair<std::optional<unsigned>, std::string_view>
        {
            auto [istr, rest] = span(s, numc);

            if (istr.empty())
                return {{}, s};

            auto is = istr.data(), ilast = istr.data() + istr.size();
            unsigned i{};
            auto [p, ec] = std::from_chars(is, ilast, i);
            if (ec != std::errc() || p != ilast)
                return {{}, s};

            return {i, rest};
        }

        [[nodiscard]] inline auto match_ip6_host(std::string_view s) noexcept
            -> std::pair<std::string_view, std::string_view>
        {
            if (s.empty() || s[0] != '[')
                return {{}, s};

            auto [host, rest] = span(s.substr(1), ip6c);
            if (host.empty() || rest.empty() || rest[0] != ']')
                return {{}, s};

            return {host, rest.substr(1)};
        }

        [[nodiscard]] inline auto match_host(std::string_view s) noexcept
            -> std::tuple<std::string_view,
                          std::optional<unsigned>,
                          std::string_view>
        {
            if (s.empty())
                return {{}, {}, s};

            std::string_view host, rest;

            std::tie(host, rest) = match_ip6_host(s);
            if (host.empty())
                std::tie(host, rest) = span(s, hostc);
            if (host.empty())
                return {{}, {}, s};

            if (rest.empty() || rest[0] != ':')
                return {host, {}, rest};

            auto [port, rest_] = match_number(rest.substr(1));
            return {host, port, rest_};
        }

        [[nodiscard]] inline auto match_path_only(std::string_view s) noexcept
            -> std::pair<std::optional<std::string_view>, std::string_view>
        {
            if (s.empty() || s[0] != '/')
                return {{}, s};

            std::string_view path;
            std::tie(path, s) = span(s, pathc | highc);
            return {path, s};
        }

        [[nodiscard]] inline auto match_path_query(std::string_view s) noexcept
            -> std::pair<std::optional<std::string_view>, std::string_view>
        {
            if (s.empty() || s[0] != '?')
                return {{}, s};

            std::string_view query;
            std::tie(query, s) = span(s.substr(1), queryc | highc);
            return {query, s};
        }

        [[nodiscard]] inline auto match_path_frag(std::string_view s) noexcept
            -> std::pair<std::optional<std::string_view>, std::string_view>
        {
            if (s.empty() || s[0] != '#')
                return {{}, s};

            std::string_view frag;
            std::tie(frag, s) = span(s.substr(1), fragc | highc);
            return {frag, s};
        }

        [[nodiscard]] inline auto match_path(std::string_view s) noexcept
            -> std::pair<std::optional<uri_path>, std::string_view>
        {
            std::optional<std::string_view> path_only;
            if (std::tie(path_only, s) = match_path_only(s);
                !path_only.has_value())
                return {{}, s};

            uri_path path;
            path.path = *path_only;
            std::tie(path.query, s) = match_path_query(s);
            std::tie(path.fragment, s) = match_path_frag(s);
            return {path, s};
        }

        [[nodiscard]] inline auto match_authority(std::string_view s) noexcept
            -> std::pair<std::optional<uri_authority>, std::string_view>
        {
            if (s.size() < 2 || s.substr(0, 2) != "//")
                return {{}, s};

            auto v = s.substr(2);

            uri_authority auth;
            std::tie(auth.username, auth.password, v) =
                match_username_password(v);

            // hostname must be present
            if (std::tie(auth.hostname, auth.port, v) = match_host(v);
                auth.hostname.empty())
                return {{}, s};

            return {auth, v};
        }

        [[nodiscard]] inline auto dehex(char c) noexcept -> int
        {
            if (c >= '0' && c <= '9')
                return c - '0';
            if (c >= 'A' && c <= 'F')
                return c - 'A' + 10;
            if (c >= 'a' && c <= 'f')
                return c - 'a' + 10;
            return -1;
        }

        [[nodiscard]] inline auto dehex(char const *p) noexcept -> int
        {
            int a = dehex(*p), b = dehex(*(p + 1));

            if (a == -1 || b == -1)
                return -1;

            return (a << 4) | b;
        }

        [[nodiscard]] inline auto
        decode_path(char *original_data, std::string_view *path_view) noexcept
            -> bool
        {
            // URL-decode the path, without causing original_data to reallocate
            // since that will invalidate all the string_views.

            auto path_offset = path_view->data() - original_data;
            auto original_length = path_view->size();
            char *p = original_data + path_offset, *q = p, *start = p;
            char *end = original_data + path_offset + original_length;

            while (p < end) {
                // URL-encoded URLs should not have high-bit characters.
                if (*p < 0)
                    return false;

                if (*p != '%') {
                    *q++ = *p++;
                    continue;
                }

                p++;
                if ((end - p) < 2)
                    return false;

                int n = dehex(p);
                if (n == -1)
                    return false;

                *q++ = static_cast<char>(static_cast<unsigned char>(n));
                p += 2;
            }

            *path_view = std::string_view(start, q);
            return true;
        }

        [[nodiscard]] inline auto raw_parse_uri(std::string const &s) noexcept
            -> expected<uri, std::error_code>
        {
            if (s.empty())
                return make_unexpected(make_uri_error(uri_errors::no_data));

            uri ret;
            auto len = s.size();
            ret.original_data =
                // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
                std::shared_ptr<char[]>(new (std::nothrow) char[len]);
            if (!ret.original_data)
                return make_unexpected(
                    std::make_error_code(std::errc::not_enough_memory));

            std::ranges::copy(s, ret.original_data.get());

            std::string_view v(ret.original_data.get(), len);
            std::tie(ret.scheme, v) = detail::match_scheme(v);
            std::tie(ret.authority, v) = detail::match_authority(v);
            std::tie(ret.path, v) = detail::match_path(v);

            if (!v.empty())
                return make_unexpected(
                    make_uri_error(uri_errors::parse_failed));

            return ret;
        }

    } // namespace detail

    struct urioptions_tag {
    };
    using urioption = flag<urioptions_tag, std::uint_fast8_t>;

    namespace uri_options {
        static constexpr urioption none{0u};
        static constexpr urioption allow_relative = {1u << 0};
        static constexpr urioption allow_protocol_relative{1u << 1};
        static constexpr urioption skip_path_decode{1u << 2};
    } // namespace uri_options

    [[nodiscard]] inline auto
    parse_uri(std::string const &s,
              urioption::flagset options = uri_options::none) noexcept
        -> expected<uri, std::error_code>
    {
        auto ret = detail::raw_parse_uri(s);
        if (!ret)
            return make_unexpected(ret.error());

        if (ret->authority && ret->authority->port &&
            *ret->authority->port > 65535u)
            return make_unexpected(make_uri_error(uri_errors::invalid_port));

        if (is_protocol_relative(*ret) &&
            !is_set(options, uri_options::allow_protocol_relative))
            return make_unexpected(
                make_uri_error(uri_errors::protocol_relative));

        if (!is_absolute(*ret) &&
            !any_set(options,
                     uri_options::allow_relative |
                         uri_options::allow_protocol_relative))
            return make_unexpected(make_uri_error(uri_errors::relative));

        // "authority-relative" URLs (https:/some/path) are not allowed.
        if (ret->scheme && !ret->authority)
            return make_unexpected(make_uri_error(uri_errors::parse_failed));

        if (ret->path && !is_set(options, uri_options::skip_path_decode)) {
            if (!detail::decode_path(ret->original_data.get(),
                                     &ret->path->path))
                return make_unexpected(
                    make_uri_error(uri_errors::invalid_encoding));

            if (ret->path->query &&
                !detail::decode_path(ret->original_data.get(),
                                     std::addressof(*ret->path->query)))
                return make_unexpected(
                    make_uri_error(uri_errors::invalid_encoding));

            if (ret->path->fragment &&
                !detail::decode_path(ret->original_data.get(),
                                     std::addressof(*ret->path->fragment)))
                return make_unexpected(
                    make_uri_error(uri_errors::invalid_encoding));
        }

        return ret;
    }

    inline auto operator<<(std::ostream &strm, uri const &uri) -> std::ostream &
    {
        if (uri.scheme)
            strm << *uri.scheme << ':';

        if (uri.authority) {
            strm << "//";

            if (uri.authority->username) {
                strm << *uri.authority->username;
                if (uri.authority->password)
                    strm << ":" << *uri.authority->password;
                strm << "@";
            }

            if (uri.authority->hostname.find(':') != std::string_view::npos)
                strm << '[' << uri.authority->hostname << ']';
            else
                strm << uri.authority->hostname;

            if (uri.authority->port)
                strm << ":" << *uri.authority->port;
        }

        if (uri.path) {
            strm << uri.path->path;

            if (uri.path->query)
                strm << "?" << *uri.path->query;
            if (uri.path->fragment)
                strm << "#" << *uri.path->fragment;
        }

        return strm;
    }

    inline auto str(uri const &u) -> std::string
    {
        std::ostringstream strm;
        strm << u;
        return strm.str();
    }

} // namespace sk::net

#endif // IVY_NET_URI_HXX_INCLUDED
