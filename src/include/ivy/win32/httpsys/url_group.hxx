/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_HTTPSYS_URL_GROUP_HXX_INCLUDED
#define IVY_WIN32_HTTPSYS_URL_GROUP_HXX_INCLUDED

#include <system_error>

#include <ivy/win32/windows.hxx>

#include <http.h>

#include <wil/resource.h>

#include <ivy/expected.hxx>
#include <ivy/string.hxx>
#include <ivy/noncopyable.hxx>

namespace ivy::win32::httpsys {

    /*************************************************************************
     *
     * url_group
     *
     * Creating a url_group calls HttpCreateUrlGroup().  Destructing it calls
     * HttpCloseUrlGroup().
     *
     * url_group::url_group() may throw http_error if HttpCreateUrlGroup()
     * fails.
     */

    // RAII handle to the url group.

    class request_queue;
    class server_session;

    class url_group : public ivy::noncopyable {
        using handle_type = wil::unique_any<HTTP_URL_GROUP_ID,
                                            decltype(::HttpCloseUrlGroup),
                                            ::HttpCloseUrlGroup>;

        handle_type _handle{};

    public:
        url_group(server_session &session);
        url_group(url_group &&) noexcept;
        ~url_group();

        auto operator=(url_group &&) noexcept -> url_group &;

        [[nodiscard]] auto get_url_group_id() const noexcept
            -> HTTP_URL_GROUP_ID;

        [[nodiscard]] auto set_request_queue(request_queue &)
            -> expected<void, std::error_code>;

        [[nodiscard]] auto add_url(wstring const &url, HTTP_URL_CONTEXT context)
            -> expected<void, std::error_code>;
    };

} // namespace ivy::win32::httpsys

#endif // IVY_WIN32_HTTPSYS_URL_GROUP_HXX_INCLUDED
