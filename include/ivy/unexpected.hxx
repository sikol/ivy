/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_UNEXPECTED_HXX_INCLUDED
#define IVY_UNEXPECTED_HXX_INCLUDED

#include <iostream>
#include <sstream>

namespace ivy {

    [[noreturn]] inline auto
    unexpected(char const *file, int line, char const *what) noexcept -> void
    {
        std::ostringstream strm;

        strm << "*** FATAL INTERNAL INCONSISTENCY DETECTED ***\n";
        strm << "    in file: " << file << '\n';
        strm << "       line: " << line << '\n';
        strm << "      error: " << what << '\n';
        strm << "*** The program will now terminate. ***\n";

        auto s = strm.str();
        std::cerr.write(s.data(), s.size());
        std::cerr.flush();

        std::abort();
    }

#define IVY_UNEXPECTED(what)                                                   \
    do {                                                                       \
        ::ivy::unexpected(__FILE__, __LINE__, what);                           \
    } while (0)

} // namespace ivy

#endif // IVY_UNEXPECTED_HXX_INCLUDED
