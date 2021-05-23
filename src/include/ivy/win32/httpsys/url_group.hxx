/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_HTTPSYS_URL_GROUP_HXX_INCLUDED
#define IVY_WIN32_HTTPSYS_URL_GROUP_HXX_INCLUDED

#include <http.h>
#include <ivy/win32/httpsys/server_session.hxx>
#include <ivy/win32/windows.hxx>

#include <wil/resource.h>

namespace ivy::win32 {

    /*************************************************************************
     *
     * http_url_group
     *
     * Creating an http_server_session calls HttpCreateUrlGroup().
     * Destructing it calls HttpCloseUrlGroup().
     * Dereferencing returns the handle to the URL group.
     *
     * http_url_group::http_url_group() may throw http_error if
     * HttpCreateUrlGroup() fails.
     */

    // RAII handle to the url group.
    using unique_http_url_group = wil::unique_any<HTTP_URL_GROUP_ID,
                                                  decltype(::HttpCloseUrlGroup),
                                                  ::HttpCloseUrlGroup>;

    class http_url_group : public ivy::noncopyable {
        unique_http_url_group _handle{};

    public:
        http_url_group(http_server_session &session);
        http_url_group(http_url_group &&) noexcept;
        ~http_url_group();

        auto operator=(http_url_group &&) noexcept -> http_url_group &;

        auto operator*() noexcept -> unique_http_url_group &;
        auto operator->() noexcept -> unique_http_url_group *;

        auto add_url(wstring const &url, HTTP_URL_CONTEXT context)
            -> expected<void, std::error_code>;
    };

} // namespace ivy::win32

#endif // IVY_WIN32_HTTPSYS_URL_GROUP_HXX_INCLUDED
