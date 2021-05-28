/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <iostream>
#include <ranges>

#include <ivy/datum/null.hxx>
#include <ivy/db.hxx>
#include <ivy/db/odbc/connect.hxx>
#include <ivy/exception.hxx>
#include <ivy/format.hxx>
#include <ivy/string/transcode.hxx>

using db_connection_error =
    ivy::message<"DBCLI", 'E', "CONNECT", "Failed to connect to the database">;

using invalid_connection_string =
    ivy::message<"DBCLI", 'E', "INVCONNSTR", "Invalid connection string">;

using query_execution_error =
    ivy::message<"DBCLI", 'E', "QUERYERROR", "Failed to execute the query">;

auto format_value(ivy::db::value &v) -> std::string
{
    auto d = v.as_datum().or_throw();

    if (is<ivy::null_type>(d))
        return "NULL";

    return str(d);
}

auto print_row(ivy::db::row &row) -> void
{
    auto values = row.values();
    std::ranges::copy(values | std::views::transform(format_value),
                      std::ostream_iterator<std::string>(std::cout, "\t"));
    std::cout << "\n";
}

auto print_result_set(ivy::db::result_set &rs) -> void
{
    std::ranges::for_each(rs.rows(), print_row);
}

auto show_results(ivy::db::query_result_handle &qr) -> void
{
    if (!qr->has_data()) {
        return;
    }

    std::ranges::for_each(qr->result_sets(), print_result_set);
}

auto execute_query(ivy::db::connection_handle &conn,
                   std::string const &query_text) -> void
try {
    auto u16text = ivy::transcode<ivy::u16string>(query_text).or_throw();
    auto query = conn->prepare_query(u16text).or_throw();
    auto result = query->execute().or_throw();
    show_results(result);
} catch (...) {
    std::throw_with_nested(query_execution_error());
}

auto sqlrepl(ivy::db::connection_handle &conn) -> void
{
    std::string query;

    for (;;) {
        ivy::print("> ");
        if (!getline(std::cin, query))
            break;

        if (query.empty())
            continue;

        if (query == "\\q")
            return;

        try {
            execute_query(conn, query);
        } catch (...) {
            ivy::print_current_exception(std::cerr);
        }
    }
}

int main(int, char **argv)
try {
    if (argv[1] == nullptr) {
        ivy::fprint(std::cerr, "usage: dbcli <connection-string>\n");
        return 1;
    }

    ivy::string connstr(argv[1]);
    auto u16connstr =
        ivy::transcode<ivy::u16string>(connstr).or_throw_with_nested(
            invalid_connection_string());

    auto db = ivy::db::odbc::connect(u16connstr)
                  .or_throw_with_nested(db_connection_error());

    ivy::print("* Connected.  Type queries, or type '\\q' to exit.\n");

    sqlrepl(db);
    return 0;
} catch (...) {
    ivy::print_current_exception(std::cerr);
    return 1;
}
