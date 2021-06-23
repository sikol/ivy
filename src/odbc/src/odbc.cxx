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
#include <ivy/string/transcode.hxx>

namespace ivy::db::odbc {

    auto connect(u16string const &connection_string) noexcept
        -> expected<db::connection_handle, error>
    try {
        auto conn = std::make_unique<connection>();
        if (auto r = conn->connect(connection_string); !r)
            return make_unexpected(r.error());

        return std::move(conn);
    } catch (...) {
        return make_unexpected(make_error(std::current_exception()));
    }

    auto connect(u16string const &connection_string,
                 u16string const &username,
                 u16string const &password) noexcept
        -> expected<db::connection_handle, error>
    try {
        auto conn = std::make_unique<connection>();
        if (auto r = conn->connect(connection_string, username, password); !r)
            return make_unexpected(r.error());

        return std::move(conn);
    } catch (...) {
        return make_unexpected(make_error(std::current_exception()));
    }

    connection::connection() = default;
    connection::~connection() = default;

    auto connection::connect(u16string const &connection_string) noexcept
        -> expected<void, error>
    try {
        auto wconnstr = transcode<wstring>(connection_string)
                            .or_throw_with_nested(connection_error(
                                "cannot transcode connection string"));

        std::wstring str(wconnstr.begin(), wconnstr.end());
        _connection.connect(str);
        return {};
    } catch (nanodbc::database_error const &e) {
        return make_unexpected(make_error<connection_error>(e.what()));
    } catch (...) {
        return make_unexpected(make_error(std::current_exception()));
    }

    auto connection::connect(u16string const &connection_string,
                             u16string const &username,
                             u16string const &password) noexcept
        -> expected<void, error>
    try {
        auto wconnstr = transcode<wstring>(connection_string)
                            .or_throw_with_nested(connection_error(
                                "cannot transcode connection string"));

        auto wusername = transcode<wstring>(username).or_throw_with_nested(
            connection_error("cannot transcode username string"));

        auto wpassword = transcode<wstring>(password).or_throw_with_nested(
            connection_error("cannot transcode password string"));

        std::wstring connstr(wconnstr.begin(), wconnstr.end());
        std::wstring unstr(wusername.begin(), wusername.end());
        std::wstring pwstr(wpassword.begin(), wpassword.end());
        _connection.connect(connstr, unstr, pwstr);
        return {};
    } catch (nanodbc::database_error const &e) {
        return make_unexpected(make_error<connection_error>(e.what()));
    } catch (...) {
        return make_unexpected(make_error(std::current_exception()));
    }

    auto connection::prepare_query(u16string const &q) noexcept
        -> expected<db::query_handle, error>
    try {
        auto ret = std::make_unique<query>(*this);
        ret->prepare(q).or_throw();
        return std::move(ret);
    } catch (...) {
        return make_unexpected(make_error(std::current_exception()));
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

    auto query::prepare(u16string const &query_string) noexcept
        -> expected<void, error>
    try {
        auto wq = transcode<wstring>(query_string)
                      .or_throw_with_nested("cannot transcode query string");
        _statement.prepare(std::wstring(wq.begin(), wq.end()));
        return {};
    } catch (nanodbc::database_error const &e) {
        return make_unexpected(make_error<query_execution_error>(e.what()));
    } catch (...) {
        return make_unexpected(make_error(std::current_exception()));
    }

    auto query::execute() noexcept -> expected<db::query_result_handle, error>
    try {
        return std::make_unique<query_result>(_statement.execute(1, 0));
    } catch (nanodbc::database_error const &e) {
        return make_unexpected(make_error<query_execution_error>(e.what()));
    } catch (...) {
        return make_unexpected(make_error(std::current_exception()));
    }

    query_result::query_result(nanodbc::result &&result) noexcept
        : _result(std::move(result))
    {
    }

    auto query_result::has_data() const noexcept -> bool
    {
        return _result.columns() != 0;
    }

    auto query_result::get_next_result_set() noexcept
        -> expected<db::result_set_handle, error>
    try {
        if (first)
            first = false;
        else {
            if (!_result.next_result())
                return make_unexpected(make_error<end_of_data>());
        }

        return std::make_unique<result_set>(&_result);
    } catch (...) {
        return make_unexpected(make_error(std::current_exception()));
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

    auto result_set::get_next_row() noexcept -> expected<row_handle, error>
    try {
        if (!_result->next())
            return make_unexpected(make_error<end_of_data>());

        return std::make_unique<row>(_result);
    } catch (nanodbc::database_error const &e) {
        return make_unexpected(make_error<query_execution_error>(e.what()));
    } catch (...) {
        return make_unexpected(make_error(std::current_exception()));
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

    auto row::get_value(std::size_t column) noexcept
        -> expected<db::value_handle, error>
    try {
        if (column > std::numeric_limits<short>::max())
            return make_unexpected(make_error(std::errc::value_too_large));

        return std::make_unique<value>(_result, static_cast<short>(column));
    } catch (...) {
        return make_unexpected(make_error(std::current_exception()));
    }

    value::value(nanodbc::result *result, short column_number)
        : _result(result)
        , _column_number(column_number)
    {
    }

    auto value::as_datum() const noexcept -> expected<datum, error>
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
            auto str = _result->get<std::string>(_column_number);
            return make_string_datum(transcode<string>(str).or_throw());
        }
    } catch (nanodbc::database_error const &e) {
        return make_unexpected(make_error<db_error>(e.what()));
    } catch (...) {
        return make_unexpected(make_error(std::current_exception()));
    }

} // namespace ivy::db::odbc
