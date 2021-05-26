/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <format>

#include <nanodbc/nanodbc.h>

#include <ivy/db/error.hxx>
#include <ivy/db/odbc/connect.hxx>
#include <ivy/db/odbc/connection.hxx>
#include <ivy/db/odbc/query.hxx>
#include <ivy/db/odbc/result_set.hxx>
#include <ivy/db/odbc/row.hxx>
#include <ivy/db/odbc/value.hxx>

namespace ivy::db::odbc {

    auto connect(u8string const &connection_string)
        -> expected<db::connection_handle, error>
    {
        auto wconnstr = transcode<wstring>(connection_string);
        if (!wconnstr)
            return make_unexpected(connection_error(
                std::format("cannot transcode connection string: {}",
                            wconnstr.error().message())));

        auto conn = std::make_unique<connection>();
        if (auto r = conn->connect(*wconnstr); !r)
            return make_unexpected(r.error());

        return std::move(conn);
    }

    auto connect(u8string const &connection_string,
                 u8string const &username,
                 u8string const &password)
        -> expected<db::connection_handle, error>
    {
        auto wconnstr = transcode<wstring>(connection_string);
        if (!wconnstr)
            return make_unexpected(connection_error(
                std::format("cannot transcode connection string: {}",
                            wconnstr.error().message())));

        auto wusername = transcode<wstring>(username);
        if (!wusername)
            return make_unexpected(connection_error(
                std::format("cannot transcode username string: {}",
                            wusername.error().message())));

        auto wpassword = transcode<wstring>(password);
        if (!wpassword)
            return make_unexpected(connection_error(
                std::format("cannot transcode password string: {}",
                            wpassword.error().message())));

        auto conn = std::make_unique<connection>();
        if (auto r = conn->connect(*wconnstr, *wusername, *wpassword); !r)
            return make_unexpected(r.error());

        return std::move(conn);
    }

    connection::connection() = default;
    connection::~connection() = default;

    auto connection::connect(wstring const &connection_string)
        -> expected<void, error>
    {
        try {
            std::wstring str(connection_string.begin(),
                             connection_string.end());
            _connection.connect(str);
        } catch (nanodbc::database_error const &e) {
            return make_unexpected(connection_error(e.what()));
        }

        return {};
    }

    auto connection::connect(wstring const &connection_string,
                             wstring const &username,
                             wstring const &password) -> expected<void, error>
    {
        try {
            std::wstring connstr(connection_string.begin(),
                                 connection_string.end());
            std::wstring unstr(username.begin(), username.end());
            std::wstring pwstr(password.begin(), password.end());
            _connection.connect(connstr, unstr, pwstr);
        } catch (nanodbc::database_error const &e) {
            return make_unexpected(connection_error(e.what()));
        }

        return {};
    }

    auto connection::prepare_query(u8string const &q)
        -> expected<db::query_handle, error>
    {
        auto wq = transcode<wstring>(q);
        if (!wq)
            return make_unexpected(query_execution_error(std::format(
                "cannot transcode query string: {}", wq.error().message())));

        auto qh = std::make_unique<query>(*this);
        if (auto r = qh->prepare(*wq); !r)
            return make_unexpected(r.error());

        return qh;
    }

    query::query(connection &conn)
        : _connection(&conn)
        , _statement(_connection->_connection)
    {
    }

    query::~query() = default;

    auto query::prepare(wstring const &query_string) -> expected<void, error>
    {
        try {
            _statement.prepare(
                std::wstring(query_string.begin(), query_string.end()));
            return {};
        } catch (nanodbc::database_error const &e) {
            return make_unexpected(query_execution_error(e.what()));
        }
    }

    auto query::execute() -> expected<db::result_set_handle, error>
    {
        try {
            return std::make_unique<result_set>(_statement.execute(1, 0));
        } catch (nanodbc::database_error const &e) {
            return make_unexpected(query_execution_error(e.what()));
        }
    }

    result_set::result_set(nanodbc::result &&result) noexcept
        : _result(std::move(result))
    {
    }

    result_set::~result_set() = default;

    auto result_set::column_count() -> std::size_t
    {
        return _result.columns();
    }

    row::row(nanodbc::result &result)
        : _result(&result)
    {
    }

    row::~row() = default;

    auto row::column_count() -> std::size_t
    {
        return _result->columns();
    }

    value::value(nanodbc::result *result, std::size_t column_number)
        : _result(result)
        , _column_number(column_number)
    {
    }

} // namespace ivy::db::odbc
