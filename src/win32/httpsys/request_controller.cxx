/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <ivy/http/service.hxx>
#include <ivy/log.hxx>
#include <ivy/win32/error.hxx>
#include <ivy/win32/httpsys/request_context.hxx>
#include <ivy/win32/httpsys/request_controller.hxx>
#include <ivy/win32/httpsys/service.hxx>

namespace ivy::win32::httpsys {

    auto make_http_request(HTTP_REQUEST const *req)
        -> expected<http::http_request, error>
    {
        auto raw_url_bytes = std::span<std::byte const>(
            reinterpret_cast<std::byte const *>(req->CookedUrl.pFullUrl),
            req->CookedUrl.FullUrlLength);

        auto uri16str = bytes_to_string<u16string>(raw_url_bytes);
        if (!uri16str)
            return make_unexpected(
                http::http_error("Invalid request URI: cannot decode bytes"));

        auto uristr = transcode<u8string>(*uri16str);
        if (!uristr)
            return make_unexpected(
                http::http_error("Invalid request URI: cannot transcode"));

        log_trace(
            "HTTP: Parse request URI [{}]",
            std::string_view(reinterpret_cast<char const *>(uristr->data()),
                             uristr->size()));

        auto uri = net::parse_uri(*uristr);
        if (!uri)
            return make_unexpected(
                http::http_error("Invalid request URI: cannot parse"));

        auto version = http::http_version(req->Version.MajorVersion,
                                          req->Version.MinorVersion);

        return http::http_request{std::move(*uri), {}, {}, version, {}, {}};
    }

    request_controller::request_controller(
        service *service, unique_http_request &&httpsys_request)
        : _service(service)
        , _httpsys_request(std::move(httpsys_request))
        , _writer(_service->get_request_queue(), _httpsys_request->RequestId)
    {
    }

    auto request_controller::run() noexcept -> void
    {
        http::http_listener *lsnr = reinterpret_cast<http::http_listener *>(
            _httpsys_request->UrlContext);

        auto reqs = make_http_request(_httpsys_request.get());
        if (!reqs) {
            log_warning("HTTP: Failed to receive request: {}",
                        reqs.error().what());
            (void)send_internal_error();
            return;
        }

        reqs->listener_address = lsnr->prefix;

        request_context ctx;
        http::http_response response;

        try {
            response = lsnr->handler(ctx, *reqs);
        } catch (...) {
            log_warning("HTTP: Failed to handle request: unexpected exception "
                        "from handler");
            (void)send_internal_error();
            return;
        }

        if (auto r = send_response(*reqs, response); !r) {
            log_warning(std::format("HTTP: Failed to send response: {}",
                                    r.error().message()));
        }
    }

    auto request_controller::send_internal_error() noexcept
        -> expected<void, std::error_code>
    {
        HTTP_RESPONSE response{};
        response.Version = HTTP_VERSION_1_1;
        response.StatusCode =
            static_cast<short>(http::http_status::internal_server_error);

        if (auto r = _writer.start_response(&response, {}); !r)
            return make_unexpected(r.error());

        if (auto r = _writer.finish_response(); !r)
            return make_unexpected(r.error());

        return {};
    }

    auto request_controller::send_response(http::http_request &req,
                                        http::http_response &resp) noexcept
        -> expected<void, std::error_code>
    {
        bool has_body = !!resp.body;
        bool chunked_response = has_body && can_chunk_response(req);

        if (chunked_response)
            resp.header.add_or_replace_field(
                {"transfer-encoding", u8"chunked"});

        HTTP_RESPONSE response{};

        response.Version = HTTP_VERSION_1_1;
        response.StatusCode = static_cast<short>(resp.status_code);

        if (auto r = _writer.start_response(
                &response, {.has_body = has_body, .chunked = chunked_response});
            !r)
            return make_unexpected(r.error());

        if (has_body) {
            std::byte buf[1024]{};

            for (;;) {
                auto nread = resp.body->read(buf);

                if (!nread) {
                    if (nread.error() != errc::end_of_file) {
                        // Do not call finish_response here() so we abort.
                        return make_unexpected(nread.error());
                    }

                    break;
                }

                if (auto r =
                        _writer.write_data(std::span(buf).subspan(0, *nread));
                    !r)
                    return make_unexpected(r.error());
            }
        }

        if (auto r = _writer.finish_response(); !r)
            return make_unexpected(r.error());

        return {};
    }

} // namespace ivy::win32::httpsys
