/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DB_ODBC_QUERY_HXX_INCLUDED
#define IVY_DB_ODBC_QUERY_HXX_INCLUDED

#include <memory>

#include <ivy/error.hxx>
#include <ivy/expected.hxx>
#include <ivy/noncopyable.hxx>

namespace ivy::db::odbc {

    class query final : public db::query {
    protected:
        connection *_connection;
        nanodbc::statement _statement;

    public:
        query(connection &);
        virtual ~query();

        auto prepare(wstring const &query_string) -> expected<void, error>;

        auto execute() -> expected<db::result_set_handle, error> override;
    };

    using query_handle = std::unique_ptr<query>;

} // namespace ivy::db

#endif // IVY_DB_ODBC_QUERY_HXX_INCLUDED
