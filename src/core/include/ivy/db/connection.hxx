/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_DB_CONNECTION_HXX_INCLUDED
#define IVY_DB_CONNECTION_HXX_INCLUDED

#include <memory>

#include <ivy/db/query.hxx>
#include <ivy/noncopyable.hxx>
#include <ivy/string.hxx>

namespace ivy::db {

    class connection : nonmovable {
    protected:
        connection() = default;

    public:
        virtual ~connection() = default;

        [[nodiscard]] virtual auto prepare_query(u16string const &) noexcept
            -> expected<query_handle, error> = 0;

        virtual auto disconnect() -> void = 0;
    };

    using connection_handle = std::unique_ptr<connection>;

} // namespace ivy::db

#endif // IVY_DB_CONNECTION_HXX_INCLUDED
