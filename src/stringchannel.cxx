/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <ivy/io/stringchannel.hxx>

namespace ivy {

    template class basic_stringchannel<utf8_encoding>;
    template class basic_stringchannel<utf16_encoding>;
    template class basic_stringchannel<utf32_encoding>;
    template class basic_stringchannel<ascii_encoding>;
    template class basic_stringchannel<system_wide_encoding>;

} // namespace ivy
