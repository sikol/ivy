/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <ivy/win32/httpsys/request_queue.hxx>

#include <ivy/http/service.hxx>
#include <ivy/win32/error.hxx>

namespace ivy::win32::httpsys {

    request_queue::request_queue(std::optional<wstring> name,
                                 SECURITY_ATTRIBUTES *security_attributes,
                                 ULONG flags)
    {
        // Create the request queue.
        auto r = ::HttpCreateRequestQueue(HTTPAPI_VERSION_2,
                                          name ? name->c_str() : nullptr,
                                          security_attributes,
                                          flags,
                                          &_handle);

        if (r != NO_ERROR) {
            auto err = make_win32_error(r);
            throw http::http_error(std::format(
                "HttpCreateRequestQueue() failed: {}", err.message()));
        }
    }

    request_queue::request_queue(request_queue &&other) noexcept = default;

    auto request_queue::operator=(request_queue &&other) noexcept
        -> request_queue & = default;

    request_queue::~request_queue() = default;

    auto request_queue::get_handle() -> HANDLE
    {
        return _handle.get();
    }

    auto request_queue::read_request() -> expected<unique_http_request, error>
    {
        // Find the size of the request.
        HTTP_REQUEST hr{};
        ULONG request_size{};

        auto r = ::HttpReceiveHttpRequest(
            _handle.get(), 0, 0, &hr, sizeof(hr), &request_size, nullptr);

        if (r != ERROR_MORE_DATA)
            return make_unexpected(http::http_error(
                std::format("HttpReceiveHttpRequest() failed: {}",
                            make_win32_error(r).message())));

        auto p = ::HeapAlloc(::GetProcessHeap(), 0, request_size);
        if (p == nullptr)
            return make_unexpected(http::http_error(
                "cannot allocate request structure: out of memory"));

        auto reqp = unique_http_request(static_cast<HTTP_REQUEST *>(p));

        r = ::HttpReceiveHttpRequest(_handle.get(),
                                     hr.RequestId,
                                     0,
                                     reqp.get(),
                                     request_size,
                                     &request_size,
                                     nullptr);
        if (r != NO_ERROR)
            return make_unexpected(http::http_error(
                std::format("HttpReceiveHttpRequest() failed: {}",
                            make_win32_error(r).message())));

        return reqp;
    }

} // namespace ivy::win32::httpsys
