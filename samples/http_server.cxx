/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <ivy/http/service.hxx>
#include <ivy/log.hxx>
#include <ivy/log/ostream_sink.hxx>
#include <ivy/win32/httpsys.hxx>

namespace {

    auto handle_request(ivy::http::request_context &,
                        ivy::http::http_request const &req)
        -> ivy::http::http_response
    {
        auto uri = str(req.uri);
        std::string_view u(reinterpret_cast<char const *>(uri.data()),
                           uri.size());
        ivy::log_info(std::format("Received request, uri = [{}].", u));

        return ivy::http::make_http_response(200, u8"Hello, world.\r\n");
    }

} // namespace

auto main(int, char **argv) -> int
{
    auto logger = ivy::log::get_global_logger();
    logger->add_sink(ivy::log::make_ostream_sink(std::cout));

    ivy::log_notice("HTTP server starting");

    auto service = ivy::win32::make_httpsys_service();
    if (!service) {
        ivy::log_fatal("Cannot create HTTP service: {}",
                       service.error().what());
        return 1;
    }

    auto &svc = **service;

    char const *uri_string = argv[1] ? argv[1] : "http://localhost:5060/";

    auto u8uri = ivy::transcode<ivy::u8string>(ivy::astring(uri_string));
    if (!u8uri) {
        std::cerr << "invalid uri\n";
        return 1;
    }

    auto uri = ivy::net::parse_uri(*u8uri);
    if (!uri) {
        std::cerr << "invalid uri\n";
        return 1;
    }

    auto listener =
        ivy::http::http_listener{.prefix = *uri, .handler = handle_request};

    auto r = svc.add_listener(listener);
    if (!r) {
        std::cerr << "failed to add URI listener: " << r.error().what() << '\n';
        return 1;
    }

    r = svc.run();
    if (!r) {
        std::cerr << "HTTP service failure: " << r.error().what() << '\n';
        return 1;
    }

    return 0;
}