/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DB_ODBC_RESULT_SET_HXX_INCLUDED
#define IVY_DB_ODBC_RESULT_SET_HXX_INCLUDED

#include <memory>

#include <nanodbc/nanodbc.h>

#include <ivy/db/result_set.hxx>
#include <ivy/noncopyable.hxx>

namespace ivy::db::odbc {

    class result_set : public db::result_set {
        nanodbc::result *_result;

    public:
        result_set(nanodbc::result *) noexcept;

        virtual ~result_set();

        [[nodiscard]] auto column_count() -> std::size_t override;
        [[nodiscard]] auto get_next_row()
            -> expected<row_handle, error> override;
    };

    using result_set_handle = std::unique_ptr<result_set>;

} // namespace ivy::db::odbc

#endif // IVY_DB_ODBC_RESULT_SET_HXX_INCLUDED
