/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_HTTP_VERSION_HXX_INCLUDED
#define IVY_HTTP_VERSION_HXX_INCLUDED

#include <utility>
#include <compare>
#include <cstdint>

namespace ivy::http {

    class http_version {
        std::uint8_t _version;

        friend auto operator==(http_version, http_version) noexcept -> bool;

    public:
        http_version(unsigned major, unsigned minor)
            : _version(static_cast<std::uint8_t>(major << 4) | static_cast<std::uint8_t>(minor))
        {
            IVY_CHECK(major <= 0xF, "http_version: major version too large");
            IVY_CHECK(minor <= 0xF, "http_version: minor version too large");
        }

        auto major() const noexcept -> unsigned
        {
            return (_version >> 4) & 0xF;
        }

        auto minor() const noexcept -> unsigned
        {
            return _version & 0xF;
        }
    };

    inline auto operator==(http_version a, http_version b) noexcept -> bool
    {
        return a._version == b._version;
    }

    inline auto operator<=>(http_version a, http_version b)
        -> std::strong_ordering
    {
        return std::pair(a.major(), a.minor()) <=> std::pair(b.major(), b.minor());
    }

} // namespace ivy::http

#endif // IVY_HTTP_VERSION_HXX_INCLUDED
