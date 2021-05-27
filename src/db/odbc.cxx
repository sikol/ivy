/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <format>

#include <nanodbc/nanodbc.h>

#ifdef _WIN32
#    include <ivy/win32/windows.hxx>
#endif

#include <sql.h>
#include <sqlext.h>

#include <ivy/datum/boolean.hxx>
#include <ivy/datum/integer.hxx>
#include <ivy/datum/null.hxx>
#include <ivy/datum/string.hxx>
#include <ivy/db/error.hxx>
#include <ivy/db/odbc/connect.hxx>
#include <ivy/db/odbc/connection.hxx>
#include <ivy/db/odbc/query.hxx>
#include <ivy/db/odbc/query_result.hxx>
#include <ivy/db/odbc/result_set.hxx>
#include <ivy/db/odbc/row.hxx>
#include <ivy/db/odbc/value.hxx>

namespace ivy::db::odbc {

    auto connect(u16string const &connection_string)
        -> expected<db::connection_handle, error>
    {
        auto conn = std::make_unique<connection>();
        if (auto r = conn->connect(connection_string); !r)
            return make_unexpected(r.error());

        return std::move(conn);
    }

    auto connect(u16string const &connection_string,
                 u16string const &username,
                 u16string const &password)
        -> expected<db::connection_handle, error>
    {
        auto conn = std::make_unique<connection>();
        if (auto r = conn->connect(connection_string, username, password); !r)
            return make_unexpected(r.error());

        return std::move(conn);
    }

    connection::connection() = default;
    connection::~connection() = default;

    auto connection::connect(u16string const &connection_string)
        -> expected<void, error>
    {
        auto wconnstr = transcode<wstring>(connection_string);
        if (!wconnstr)
            return make_unexpected(make_error<connection_error>(
                std::format("cannot transcode connection string: {}",
                            wconnstr.error().message())));

        try {
            std::wstring str(wconnstr->begin(), wconnstr->end());
            _connection.connect(str);
        } catch (nanodbc::database_error const &e) {
            return make_unexpected(make_error<connection_error>(e.what()));
        }

        return {};
    }

    auto connection::connect(u16string const &connection_string,
                             u16string const &username,
                             u16string const &password) -> expected<void, error>
    {
        auto wconnstr = transcode<wstring>(connection_string);
        if (!wconnstr)
            return make_unexpected(make_error<connection_error>(
                std::format("cannot transcode connection string: {}",
                            wconnstr.error().message())));

        auto wusername = transcode<wstring>(username);
        if (!wusername)
            return make_unexpected(make_error<connection_error>(
                std::format("cannot transcode username string: {}",
                            wusername.error().message())));

        auto wpassword = transcode<wstring>(password);
        if (!wpassword)
            return make_unexpected(make_error<connection_error>(
                std::format("cannot transcode password string: {}",
                            wpassword.error().message())));
        try {
            std::wstring connstr(wconnstr->begin(), wconnstr->end());
            std::wstring unstr(wusername->begin(), wusername->end());
            std::wstring pwstr(wpassword->begin(), wpassword->end());
            _connection.connect(connstr, unstr, pwstr);
        } catch (nanodbc::database_error const &e) {
            return make_unexpected(make_error<connection_error>(e.what()));
        }

        return {};
    }

    auto connection::prepare_query(u16string const &q)
        -> expected<db::query_handle, error>
    {
        auto qh = std::make_unique<query>(*this);
        if (auto r = qh->prepare(q); !r)
            return make_unexpected(r.error());

        return qh;
    }

    auto connection::disconnect() -> void
    {
        _connection.disconnect();
    }

    query::query(connection &conn)
        : _connection(&conn)
        , _statement(_connection->_connection)
    {
    }

    query::~query() = default;

    auto query::prepare(u16string const &query_string) -> expected<void, error>
    {
        auto wq = transcode<wstring>(query_string);
        if (!wq)
            return make_unexpected(make_error<query_execution_error>(std::format(
                "cannot transcode query string: {}", wq.error().message())));

        try {
            _statement.prepare(std::wstring(wq->begin(), wq->end()));
            return {};
        } catch (nanodbc::database_error const &e) {
            return make_unexpected(make_error<query_execution_error>(e.what()));
        }
    }

    auto query::execute() -> expected<db::query_result_handle, error>
    try {
        return std::make_unique<query_result>(_statement.execute(1, 0));
    } catch (nanodbc::database_error const &e) {
        return make_unexpected(make_error<query_execution_error>(e.what()));
    }

    query_result::query_result(nanodbc::result &&result) noexcept
        : _result(std::move(result))
    {
    }

    auto query_result::has_data() const -> bool
    {
        return _result.columns() != 0;
    }

    auto query_result::get_next_result_set()
        -> expected<db::result_set_handle, error>
    {
        if (first)
            first = false;
        else {
            if (!_result.next_result())
                return make_unexpected(make_error<end_of_data>());
        }

        return std::make_unique<result_set>(&_result);
    }

    result_set::result_set(nanodbc::result *result) noexcept
        : _result(result)
    {
    }

    result_set::~result_set() = default;

    auto result_set::column_count() -> std::size_t
    {
        return _result->columns();
    }

    auto result_set::get_next_row() -> expected<row_handle, error>
    try {
        if (!_result->next())
            return make_unexpected(make_error<end_of_data>());

        return std::make_unique<row>(_result);
    } catch (nanodbc::database_error const &e) {
        return make_unexpected(make_error<query_execution_error>(e.what()));
    }

    row::row(nanodbc::result *result)
        : _result(result)
    {
    }

    row::~row() = default;

    auto row::column_count() -> std::size_t
    {
        return _result->columns();
    }

    auto row::get_value(std::size_t column) -> expected<db::value_handle, error>
    {
        if (column > std::numeric_limits<short>::max())
            return make_unexpected(make_error(std::errc::value_too_large));

        return std::make_unique<value>(_result, static_cast<short>(column));
    }

    value::value(nanodbc::result *result, short column_number)
        : _result(result)
        , _column_number(column_number)
    {
    }

    auto value::as_datum() const -> expected<datum, error>
    try {
        if (_result->is_null(_column_number))
            return make_null_datum();

        auto type = _result->column_datatype(_column_number);

        switch (type) {
        case SQL_INTEGER:
        case SQL_SMALLINT:
            return make_integer_datum(
                _result->get<std::int64_t>(_column_number));

        case SQL_BIT:
            return make_boolean_datum(_result->get<int>(_column_number));

        default:
            return make_string_datum(_result->get<std::string>(_column_number));
        }
    } catch (nanodbc::database_error const &e) {
        return make_unexpected(make_error<db_error>(e.what()));
    }

} // namespace ivy::db::odbc
