/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DB_ODBC_ROW_HXX_INCLUDED
#define IVY_DB_ODBC_ROW_HXX_INCLUDED

#include <cstddef>

#include <nanodbc/nanodbc.h>

#include <ivy/db/row.hxx>
#include <ivy/db/value.hxx>

namespace ivy::db::odbc {

    class row final : public db::row {
        nanodbc::result *_result;

    public:
        row(nanodbc::result *);
        ~row();

        auto column_count() -> std::size_t override;
        auto get_value(std::size_t column) noexcept
            -> expected<db::value_handle, error> override;
    };

} // namespace ivy::db::odbc

#endif // IVY_DB_ODBC_ROW_HXX_INCLUDED
