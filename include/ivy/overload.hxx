/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_OVERLOAD_HXX_INCLUDED
#define IVY_OVERLOAD_HXX_INCLUDED

namespace ivy {

    template <typename... Ts>
    struct overload : Ts... {
        using Ts::operator()...;
    };

    template <typename... Ts>
    overload(Ts...) -> overload<Ts...>;

} // namespace ivy

#endif // IVY_OVERLOAD_HXX_INCLUDED
