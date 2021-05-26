/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DB_VALUE_HXX_INCLUDED
#define IVY_DB_VALUE_HXX_INCLUDED

#include <memory>

#include <ivy/noncopyable.hxx>

namespace ivy::db {

    class value : noncopyable {
    protected:
        value() = default;

    public:
        virtual ~value() = default;
    };

    using value_handle = std::unique_ptr<value>;

} // namespace ivy::db

#endif // IVY_DB_VALUE_HXX_INCLUDED
