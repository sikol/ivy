/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_NONCOPYABLE_HXX_INCLUDED
#define IVY_NONCOPYABLE_HXX_INCLUDED

namespace ivy {

    struct noncopyable {
        noncopyable() = default;
        noncopyable(noncopyable &&) noexcept = default;
        auto operator=(noncopyable &&) noexcept -> noncopyable & = default;

        noncopyable(noncopyable const &) = delete;
        auto operator=(noncopyable const &) -> noncopyable & = delete;
    };

    struct nonmovable : noncopyable {
        nonmovable() = default;
        nonmovable(nonmovable &&) = delete;
        auto operator=(nonmovable &&) noexcept -> nonmovable & = delete;
    };

} // namespace ivy

#endif // IVY_NONCOPYABLE_HXX_INCLUDED
