/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_HTTPSYS_SERVICE_HXX_INCLUDED
#define IVY_WIN32_HTTPSYS_SERVICE_HXX_INCLUDED

#include <memory>
#include <optional>
#include <vector>

#include <ivy/http/service.hxx>
#include <ivy/string.hxx>
#include <ivy/win32/httpsys/initialise_handle.hxx>
#include <ivy/win32/httpsys/request_queue.hxx>
#include <ivy/win32/httpsys/server_session.hxx>
#include <ivy/win32/httpsys/url_group.hxx>

namespace ivy::win32::httpsys {

    class service final : public http::service {
        initialise_handle _http_handle;
        request_queue _request_queue;
        server_session _server_session;
        url_group _url_group;
        std::vector<std::unique_ptr<http::http_listener>> _listeners;

    public:
        service(std::optional<wstring> name,
                SECURITY_ATTRIBUTES *security_attributes,
                ULONG flags);

        service(service &&) noexcept = delete;
        auto operator=(service &&) -> service & = delete;

        service(service const &) = delete;
        auto operator=(service const &) -> service & = delete;

        [[nodiscard]] auto get_request_queue() noexcept -> request_queue &;

        [[nodiscard]] auto get_server_session() noexcept -> server_session &;

        [[nodiscard]] auto get_url_group() noexcept -> url_group &;

        [[nodiscard]] auto add_listener(http::http_listener const &)
            -> expected<void, error> override;

        [[nodiscard]] auto run() -> expected<void, error> override;
    };

} // namespace ivy::win32::httpsys

#endif // IVY_WIN32_HTTPSYS_SERVICE_HXX_INCLUDED
