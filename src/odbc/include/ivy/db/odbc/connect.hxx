/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DB_ODBC_HXX_INCLUDED
#define IVY_DB_ODBC_HXX_INCLUDED

#include <ivy/db/connection.hxx>
#include <ivy/error.hxx>
#include <ivy/string.hxx>

namespace ivy::db::odbc {

    auto connect(u16string const &connection_string) noexcept
        -> expected<db::connection_handle, error>;

    auto connect(u16string const &connection_string,
                 u16string const &username,
                 u16string const &password) noexcept
        -> expected<db::connection_handle, error>;

} // namespace ivy::db::odbc

#endif // IVY_DB_ODBC_HXX_INCLUDED
