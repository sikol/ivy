/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DB_ROW_HXX_INCLUDED
#define IVY_DB_ROW_HXX_INCLUDED

#include <memory>
#include <cstddef>

#include <ivy/db/value.hxx>
#include <ivy/noncopyable.hxx>
#include <ivy/expected.hxx>

namespace ivy::db {

    class row : nonmovable {
    protected:
        row() = default;

    public:
        virtual ~row() = default;

        virtual auto column_count() -> std::size_t = 0;
        virtual auto get_value(std::size_t column)
            -> expected<value_handle, error> = 0;
    };

} // namespace ivy::db

#endif // IVY_DB_ROW_HXX_INCLUDED
