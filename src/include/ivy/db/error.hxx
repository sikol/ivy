/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DB_ERROR_HXX_INCLUDED
#define IVY_DB_ERROR_HXX_INCLUDED

#include <stdexcept>

namespace ivy::db {

    class db_error : public std::runtime_error {
    public:
        db_error(std::string const &msg)
            : std::runtime_error(msg)
        {
        }
    };

    class connection_error : public db_error {
    public:
        connection_error(std::string const &msg)
            : db_error(msg)
        {
        }
    };

    class query_execution_error : public db_error {
    public:
        query_execution_error(std::string const &msg)
            : db_error(msg)
        {
        }
    };

    class end_of_data : public db_error {
    public:
        end_of_data()
            : db_error("end of data")
        {
        }
    };

} // namespace ivy::db

#endif // IVY_DB_ERROR_HXX_INCLUDED
