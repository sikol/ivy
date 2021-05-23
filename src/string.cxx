/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <ivy/string.hxx>

namespace ivy {

    template class basic_string<utf8_encoding>;
    template class basic_string<utf16_encoding>;
    template class basic_string<utf32_encoding>;
    template class basic_string<ascii_encoding>;
    template class basic_string<system_wide_encoding>;

} // namespace ivy
