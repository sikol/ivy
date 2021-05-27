/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_HTTPSYS_RESPONSE_HXX_INCLUDED
#define IVY_WIN32_HTTPSYS_RESPONSE_HXX_INCLUDED

#include <vector>

#include <ivy/win32/windows.hxx>

#include <http.h>

#include <ivy/win32/heap.hxx>
#include <ivy/string.hxx>

namespace ivy::win32::httpsys {

    /*************************************************************************
     *
     * response: represents an HTTP_RESPONSE struct.
     * 
     * This class is used to make building the HTTP_RESPONSE easier, especially
     * for header management.  After a the response is created, add_*_header()
     * can be used to add headers to the request, and then get_value() will
     * return a PHTTP_RESPONSE suitable for passing to the HTTP API.
     * 
     * response handles all details of known/unknown headers and memory
     * allocation.
     */

    class response {
        mutable HTTP_RESPONSE _response{};
        std::vector<std::unique_ptr<char[]>> _strings;
        std::vector<HTTP_UNKNOWN_HEADER> _unknown_headers;

        auto add_string(u8string const &) -> char *;
        auto add_string(astring const &) -> char *;

    public:
        response(USHORT status_code);

        // Add a header by name, which could be either a known header or
        // an unknown header.
        auto add_header(astring const &name, u8string const &value) -> void;

        // Add an unknown header by name.
        auto add_unknown_header(astring const &name, u8string const &value) -> void;

        // Add a known header by id.  The id is not validated.
        auto add_known_header(HTTP_HEADER_ID id, u8string const &value) -> void;

        auto get_value() const noexcept -> HTTP_RESPONSE const *;
    };

} // namespace ivy::win32::httpsys

#endif // IVY_WIN32_HTTPSYS_RESPONSE_HXX_INCLUDED
