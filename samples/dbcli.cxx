/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <iostream>
#include <ranges>

#include <ivy/datum/null.hxx>
#include <ivy/db.hxx>
#include <ivy/db/odbc/connect.hxx>
#include <ivy/format.hxx>

auto print_value(ivy::db::value &v) -> void {
    auto d = v.as_datum().or_throw();

    if (is<ivy::null_type>(d))
        ivy::print("NULL\t");
    else
        ivy::print("{}\t", str(d));
}

auto print_row(ivy::db::row &row) -> void
{
    std::ranges::for_each(row.values(), print_value);
    ivy::print("\n");
}

auto print_result_set(ivy::db::result_set &rs) -> void {
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
                   ivy::u16string const &query_text) -> void
try {
    auto query = conn->prepare_query(query_text).or_throw();
    auto result = query->execute().or_throw();
    show_results(result);
} catch (ivy::db::db_error const &e) {
    ivy::fprint(std::cerr, "{}\n", e.what());
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

        auto u16query = ivy::transcode<ivy::u16string>(query);
        if (!u16query) {
            ivy::fprint(std::cerr,
                        "cannot transcode query: {}\n",
                        u16query.error().message());
            continue;
        }

        execute_query(conn, *u16query);
    }
}

int main(int, char **argv)
{
    if (argv[1] == nullptr) {
        ivy::fprint(std::cerr, "usage: dbcli <connection-string>\n");
        return 1;
    }

    ivy::string connstr(argv[1]);
    auto u16connstr = ivy::transcode<ivy::u16string>(connstr);
    if (!u16connstr) {
        ivy::fprint(std::cerr,
                    "invalid connection string: {}\n",
                    u16connstr.error().message());
        return 1;
    }

    auto db = ivy::db::odbc::connect(*u16connstr);
    if (!db) {
        ivy::fprint(std::cerr, "connection failed: {}\n", db.error().what());
        return 1;
    }

    ivy::print("* Connected.  Type queries, or type '\\q' to exit.\n");

    sqlrepl(*db);
}
