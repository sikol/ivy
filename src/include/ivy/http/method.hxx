/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_HTTP_METHOD_HXX_INCLUDED
#define IVY_HTTP_METHOD_HXX_INCLUDED

namespace ivy::http {

    enum struct http_method : unsigned {
        unknown,
        options,
        get,
        head,
        post,
        put,
        delete_,
        trace,
        connect,
        track,
        move,
        copy,
        propfind,
        proppatch,
        mkcol,
        lock,
        unlock,
        search,
    };

}

#endif // IVY_HTTP_METHOD_HXX_INCLUDED
