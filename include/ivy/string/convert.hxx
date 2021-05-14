/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_STRING_STRCONV_HXX
#define IVY_STRING_STRCONV_HXX

#include <sstream>
#include <string>
#include <optional>
#include <cstdlib>
#include <charconv>

#include <ivy/expected.hxx>

namespace ivy {

    template <typename T>
    auto strtoi(std::string_view v, int base = 10) noexcept
        -> expected<T, std::error_code>
    {
        T i;

        auto [p, ec] = std::from_chars(v.data(), v.data() + v.size(), i, base);
        if (ec != std::errc())
            return make_unexpected(std::make_error_code(ec));
        if (p != (v.data() + v.size()))
            return make_unexpected(
                std::make_error_code(std::errc::invalid_argument));

        return i;
    }

} // namespace ivy

#endif // IVY_STRING_STRCONV_HXX
