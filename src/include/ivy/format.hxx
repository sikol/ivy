/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <cstdio>
#include <format>
#include <iostream>
#include <utility>

namespace ivy {

    template <typename... Args>
    auto fprint(std::ostream &strm, Args &&...args) -> bool
    {
        strm << std::format(std::forward<Args>(args)...);
        return static_cast<bool>(strm);
    }

    template <typename... Args>
    auto fprint(FILE *file, Args &&...args) -> bool
    {
        auto str = std::format(args);
        auto data = str.data();
        auto size = str.size();

        auto r = std::fwrite(data, 1, size, file);
        return (r > 0) && (r == size);
    }

    template <typename... Args>
    auto print(Args &&...args) -> bool
    {
        return fprint(std::cout, std::forward<Args>(args)...);
    }

} // namespace ivy
