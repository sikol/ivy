/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DB_QUERY_HXX_INCLUDED
#define IVY_DB_QUERY_HXX_INCLUDED

#include <memory>

#include <ivy/db/result_set.hxx>
#include <ivy/error.hxx>
#include <ivy/expected.hxx>
#include <ivy/noncopyable.hxx>

namespace ivy::db {

    class query : nonmovable {
    protected:
        query() = default;

    public:
        virtual ~query() = default;
        virtual auto execute() -> expected<result_set_handle, error> = 0;
    };

    using query_handle = std::unique_ptr<query>;

} // namespace ivy::db

#endif // IVY_DB_QUERY_HXX_INCLUDED
