/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <ivy/log.hxx>
#include <ivy/string/transcode.hxx>
#include <ivy/win32/httpsys.hxx>
#include <ivy/win32/httpsys/request_controller.hxx>
#include <ivy/win32/httpsys/service.hxx>

namespace ivy::win32 {

    auto make_httpsys_service()
        -> expected<std::unique_ptr<http::service>, error>
    try {
        SECURITY_ATTRIBUTES attrs{};
        attrs.nLength = sizeof(attrs);

        return std::make_unique<httpsys::service>(std::nullopt, &attrs, 0);
    } catch (...) {
        return make_unexpected(make_error(std::current_exception()));
    }

} // namespace ivy::win32

namespace ivy::win32::httpsys {

    service::service(std::optional<wstring> name,
                     SECURITY_ATTRIBUTES *security_attributes,
                     ULONG flags)
        : _http_handle(HTTP_INITIALIZE_SERVER)
        , _request_queue(name, security_attributes, flags)
        , _url_group(_server_session)
    {
    }

    auto service::get_request_queue() noexcept -> request_queue &
    {
        return _request_queue;
    }

    auto service::get_server_session() noexcept -> server_session &
    {
        return _server_session;
    }

    auto service::get_url_group() noexcept -> url_group &
    {
        return _url_group;
    }

    auto service::add_listener(http::http_listener const &lsn)
        -> expected<void, error>
    try {
        auto lsnp = std::make_unique<http::http_listener>(lsn);

        auto wstr =
            transcode<wstring>(str(lsn.prefix))
                .or_throw_with_nested(http::http_error("invalid listener URI"));

        _url_group.add_url(wstr, reinterpret_cast<HTTP_URL_CONTEXT>(lsnp.get()))
            .or_throw_with_nested(
                http::http_error("cannot add listener to URL group"));

        _listeners.push_back(std::move(lsnp));

        return {};
    } catch (...) {
        return make_unexpected(make_error(std::current_exception()));
    }

    auto service::run() -> expected<void, error>
    try {
        _url_group.set_request_queue(_request_queue)
            .or_throw_with_nested(
                http::http_error("cannot add URL group to request queue"));

        for (;;) {
            auto req = _request_queue.read_request().or_throw();

            std::thread([this, req = std::move(req)]() mutable {
                request_controller cllr(this, std::move(req));
                cllr.run();
            }).detach();
        }
    } catch (...) {
        return make_unexpected(make_error(std::current_exception()));
    }

} // namespace ivy::win32::httpsys
