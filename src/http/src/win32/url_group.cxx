/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <format>

#include <ivy/win32/windows.hxx>

#include <http.h>

#include <ivy/http/service.hxx>
#include <ivy/win32/error.hxx>
#include <ivy/win32/httpsys/request_queue.hxx>
#include <ivy/win32/httpsys/server_session.hxx>
#include <ivy/win32/httpsys/url_group.hxx>

namespace ivy::win32::httpsys {

    url_group::url_group(server_session &session)
    {
        auto r = ::HttpCreateUrlGroup(session.get_session_id(), &_handle, 0);

        if (r != NO_ERROR)
            throw http::http_error(
                std::format("HttpCreateUrlGroup() failed: {}",
                            make_win32_error(r).message()));
    }

    url_group::url_group(url_group &&) noexcept = default;

    url_group::~url_group() = default;

    auto url_group::operator=(url_group &&) noexcept -> url_group & = default;

    auto url_group::get_url_group_id() const noexcept -> HTTP_URL_GROUP_ID
    {
        return _handle.get();
    }

    auto url_group::add_url(wstring const &url, HTTP_URL_CONTEXT context)
        -> expected<void, error>
    {
        auto const ret =
            ::HttpAddUrlToUrlGroup(_handle.get(), url.c_str(), context, 0);

        if (ret != NO_ERROR)
            return make_unexpected(make_error(make_win32_error(ret)));

        return {};
    }

    auto url_group::set_request_queue(request_queue &queue)
        -> expected<void, error>
    {
        HTTP_BINDING_INFO binding{};
        binding.Flags.Present = 1;
        binding.RequestQueueHandle = queue.get_handle();

        auto r = ::HttpSetUrlGroupProperty(_handle.get(),
                                           HttpServerBindingProperty,
                                           &binding,
                                           sizeof(binding));

        if (r != NO_ERROR)
            return make_unexpected(make_error(make_win32_error(r)));

        return {};
    }

} // namespace ivy::win32::httpsys
