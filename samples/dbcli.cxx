/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <iostream>

#include <ivy/db.hxx>
#include <ivy/db/odbc/connect.hxx>

int main(int, char **argv)
{
    if (!argv[1]) {
        std::cerr << "usage: dbcli <connection-string>\n";
        return 1;
    }


}
