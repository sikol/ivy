/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_STRING_U8STRING_HXX_INCLUDED
#define IVY_STRING_U8STRING_HXX_INCLUDED

#include <ivy/charenc/utf8.hxx>
#include <ivy/string.hxx>

namespace ivy {

    using u8string = basic_string<utf8>;

}

#endif // IVY_STRING_U8STRING_HXX_INCLUDED