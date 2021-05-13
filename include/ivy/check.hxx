/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHECK_HXX_INCLUDED
#define IVY_CHECK_HXX_INCLUDED

#include <ivy/unexpected.hxx>

#ifndef NDEBUG

#    define IVY_CHECKED

#    define IVY_CHECK(cond, msg)                                               \
        do {                                                                   \
            if (!(cond)) {                                                     \
                IVY_UNEXPECTED(                                                \
                    (std::string("invariant failure: ") + (msg)).c_str());     \
            }                                                                  \
        } while (0)

#else

#    define IVY_CHECK(cond, msg) ((void)0)

#endif

#endif // IVY_CHECK_HXX_INCLUDED
