/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DB_ODBC_VALUE_HXX_INCLUDED
#define IVY_DB_ODBC_VALUE_HXX_INCLUDED

#include <memory>

#include <nanodbc/nanodbc.h>

#include <ivy/db/value.hxx>

namespace ivy::db::odbc {

    class row;

    class value : public db::value {
    protected:
        nanodbc::result *_result;
        std::size_t _column_number;

        friend class row;

        value(nanodbc::result *, std::size_t);
    };

    using value_handle = std::unique_ptr<value>;

} // namespace ivy::db::odbc

#endif // IVY_DB_ODBC_VALUE_HXX_INCLUDED
