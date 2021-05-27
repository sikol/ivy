/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DB_ODBC_QUERY_RESULT_HXX_INCLUDED
#define IVY_DB_ODBC_QUERY_RESULT_HXX_INCLUDED

#include <ivy/db/query_result.hxx>
#include <ivy/db/result_set.hxx>
#include <ivy/noncopyable.hxx>

namespace ivy::db::odbc {

    class query_result : public db::query_result {
        nanodbc::result _result;
        bool first = true;

    public:
        query_result(nanodbc::result &&) noexcept;

        [[nodiscard]] auto has_data() const noexcept -> bool override;
        [[nodiscard]] auto get_next_result_set() noexcept
            -> expected<db::result_set_handle, error> override;
    };

} // namespace ivy::db::odbc

#endif // IVY_DB_ODBC_QUERY_RESULT_HXX_INCLUDED
