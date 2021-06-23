/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <iostream>
#include <ranges>

#include <ivy/cmdline.hxx>
#include <ivy/datum/null.hxx>
#include <ivy/db.hxx>
#include <ivy/db/odbc/connect.hxx>
#include <ivy/exception.hxx>
#include <ivy/format.hxx>
#include <ivy/string/transcode.hxx>

using dbcli_facility = ivy::facility<"DBCLI">;

using db_connection_error = ivy::message<dbcli_facility,
                                         'E',
                                         "CONNECT",
                                         "Failed to connect to the database">;

using invalid_connection_string = ivy::
    message<dbcli_facility, 'E', "INVCONNSTR", "Invalid connection string">;

using query_execution_error = ivy::
    message<dbcli_facility, 'E', "QUERYERROR", "Failed to execute the query">;

using db_connected = ivy::message<dbcli_facility,
                                  'I',
                                  "CONNECTED",
                                  "Connected to database; type \\q to quit">;

auto format_value(ivy::db::value &v) -> std::string
{
    auto d = v.as_datum().or_throw();

    if (is<ivy::null_type>(d))
        return "NULL";

    return ivy::transcode<std::string>(str(d)).or_throw();
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

struct dbcli_options {
    ivy::u8string driver;
    ivy::u8string connection_string;
};

int main(int argc, char **argv)
try {
    ivy::cmdline<dbcli_options> cmdline;
    cmdline.add_option(u8"driver", &dbcli_options::driver);
    cmdline.add_argument(u8"connection-string",
                         &dbcli_options::connection_string);

    auto options = cmdline.parse(argc, argv).or_throw();

    auto u16connstr = ivy::transcode<ivy::u16string>(options.connection_string)
                          .or_throw_with_nested(invalid_connection_string());

    auto db = ivy::db::odbc::connect(u16connstr)
                  .or_throw_with_nested(db_connection_error());

    std::cout << db_connected();

    sqlrepl(db);
    return 0;
} catch (...) {
    ivy::print_current_exception(std::cerr);
    return 1;
}
