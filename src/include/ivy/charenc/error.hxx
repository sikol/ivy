/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_ERROR_HXX_INCLUDED
#define IVY_CHARENC_ERROR_HXX_INCLUDED

#include <stdexcept>
#include <string>

namespace ivy {

    class encoding_error : public std::runtime_error {
    public:
        encoding_error(std::string const &err)
            : std::runtime_error(err)
        {
        }
    };

} // namespace ivy

#endif // IVY_CHARENC_ERROR_HXX_INCLUDED
