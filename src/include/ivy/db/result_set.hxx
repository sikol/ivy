/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DB_RESULT_SET_HXX_INCLUDED
#define IVY_DB_RESULT_SET_HXX_INCLUDED

#include <memory>

#include <ivy/noncopyable.hxx>

namespace ivy::db {

    class result_set : nonmovable {
    protected:
        result_set() = default;

    public:
        virtual ~result_set() = default;

        virtual auto column_count() -> std::size_t = 0;
    };

    using result_set_handle = std::unique_ptr<result_set>;

} // namespace ivy::db

#endif // IVY_DB_RESULT_SET_HXX_INCLUDED
