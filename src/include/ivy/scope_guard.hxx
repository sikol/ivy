/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_SCOPE_GUARD_HXX_INCLUDED
#define IVY_SCOPE_GUARD_HXX_INCLUDED

namespace ivy {

    template <typename F>
    class scope_guard {
        F _func;

    public:
        scope_guard(F const &func) : _func(func) {}
        scope_guard(F &&func) : _func(std::move(func)) {}

        scope_guard(scope_guard const &) = delete;
        scope_guard(scope_guard &&) = delete;
        
        auto operator=(scope_guard const &) -> scope_guard & = delete;
        auto operator=(scope_guard &&) -> scope_guard & = delete;

        ~scope_guard()
        {
            _func();
        }
    };

} // namespace ivy

#endif // IVY_SCOPE_GUARD_HXX_INCLUDED
