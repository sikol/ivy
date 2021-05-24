/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <ivy/win32/httpsys/header.hxx>
#include <ivy/win32/httpsys/response.hxx>

namespace ivy::win32::httpsys {

    response::response(USHORT status_code)
    {
        _response.StatusCode = status_code;
        _response.Version = HTTP_VERSION_1_1;
    }

    auto response::add_string(u8string const &s) -> char *
    {
        auto p = std::make_unique<char[]>(s.size());

        auto begin = reinterpret_cast<char const *>(s.data());
        auto end = reinterpret_cast<char const *>(s.data() + s.size());

        std::ranges::copy(begin, end, p.get());

        _strings.push_back(std::move(p));
        return _strings.back().get();
    }

    auto response::add_string(astring const &s) -> char *
    {
        auto p = std::make_unique<char[]>(s.size());

        std::ranges::copy(s, p.get());

        _strings.push_back(std::move(p));
        return _strings.back().get();
    }

    auto response::get_value() const noexcept -> HTTP_RESPONSE const *
    {
        _response.Headers.UnknownHeaderCount =
            static_cast<USHORT>(_unknown_headers.size());

        if (_unknown_headers.size())
            _response.Headers.pUnknownHeaders =
                const_cast<HTTP_UNKNOWN_HEADER *>(&_unknown_headers[0]);

        return &_response;
    }

    auto response::add_header(astring const &name, u8string const &value)
        -> void
    {
        auto id = header_name_to_id(name);
        if (id)
            add_known_header(*id, value);
        else
            add_unknown_header(name, value);
    }

    auto response::add_known_header(HTTP_HEADER_ID id, u8string const &value)
        -> void
    {
        IVY_CHECK(id > 0 && id < HttpHeaderMaximum,
                  "response: invalid header id");

        _response.Headers.KnownHeaders[id].pRawValue = add_string(value);
        _response.Headers.KnownHeaders[id].RawValueLength =
            static_cast<USHORT>(value.size());
    }

    auto response::add_unknown_header(astring const &name,
                                      u8string const &value) -> void
    {
        HTTP_UNKNOWN_HEADER hdr{.NameLength = static_cast<USHORT>(name.size()),
                                .RawValueLength =
                                    static_cast<USHORT>(value.size()),
                                .pName = add_string(name),
                                .pRawValue = add_string(value)};

        _unknown_headers.push_back(hdr);
    }

} // namespace ivy::win32::httpsys