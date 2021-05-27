/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DB_ODBC_CONNECTION_HXX_INCLUDED
#define IVY_DB_ODBC_CONNECTION_HXX_INCLUDED

#include <memory>

#include <nanodbc/nanodbc.h>

#include <ivy/db/connection.hxx>
#include <ivy/noncopyable.hxx>
#include <ivy/string.hxx>

namespace ivy::db::odbc {

    class query;

    class connection final : public db::connection {
    protected:
        friend class query;

        nanodbc::connection _connection;

    public:
        connection();
        ~connection();

        [[nodiscard]] auto connect(u16string const &connection_string) noexcept
            -> expected<void, error>;

        [[nodiscard]] auto connect(u16string const &connection_string,
                                   u16string const &username,
                                   u16string const &password) noexcept
            -> expected<void, error>;

        [[nodiscard]] auto prepare_query(u16string const &) noexcept
            -> expected<db::query_handle, error> override;

        auto disconnect() -> void override;
    };

    using connection_handle = std::unique_ptr<connection>;

} // namespace ivy::db::odbc

#endif // IVY_DB_CONNECTION_HXX_INCLUDED
