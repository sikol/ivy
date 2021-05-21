/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_HTTP_STATUS_CODE_HXX
#define IVY_HTTP_STATUS_CODE_HXX

#include <string>
#include <system_error>

namespace ivy::http {

    enum struct http_status : int {
        continue_ = 100,
        switching_protocols = 101,
        processing = 102,
        early_hints = 103,

        ok = 200,
        created = 201,
        accepted = 202,
        non_authoritative_information = 203,
        no_content = 204,
        reset_content = 205,
        partial_content = 206,
        multi_status = 207,
        already_reported = 208,
        im_used = 226,

        multiple_choices = 300,
        moved_permanently = 301,
        found = 302,
        see_other = 303,
        not_modified = 304,
        use_proxy = 305,
        switch_proxy = 306,
        temporary_redirect = 307,
        permanent_redirect = 308,

        bad_request = 400,
        unauthorized = 401,
        payment_required = 402,
        forbidden = 403,
        not_found = 404,
        method_not_allowed = 405,
        not_acceptable = 406,
        proxy_authentication_required = 407,
        request_timeout = 408,
        conflict = 409,
        gone = 410,
        length_required = 411,
        precondition_failed = 412,
        payload_too_large = 413,
        uri_too_long = 414,
        unsupported_media_type = 415,
        range_not_satisfiable = 416,
        expectation_failed = 417,
        im_a_teapot = 418,
        misdirected_request = 421,
        unprocessable_entity = 422,
        locked = 423,
        failed_dependency = 424,
        too_early = 425,
        upgrade_required = 426,
        precondition_required = 428,
        too_many_requests = 429,
        request_header_fields_too_large = 431,
        unavailable_for_legal_reasons = 451,

        internal_server_error = 500,
        not_implemented = 501,
        bad_gateway = 502,
        service_unavailable = 503,
        gateway_timeout = 504,
        http_version_not_supported = 505,
        variant_also_negotiates = 506,
        insufficient_storage = 507,
        loop_detected = 508,
        not_extended = 510,
        network_authenticated_required = 511,
    };

} // namespace yarrow::http

namespace std {
    template <>
    struct is_error_code_enum<ivy::http::http_status> : true_type {
    };
}; // namespace std

namespace ivy::http {

    namespace detail {

        struct http_errc_category : std::error_category {
            auto name() const noexcept -> char const * final;
            auto message(int c) const -> std::string final;
        };

    } // namespace detail

    auto http_errc_category() -> detail::http_errc_category const &;
    auto make_http_status(http_status) -> std::error_code;
    auto make_http_status(int) -> std::error_code;

} // namespace ivy::http

#endif // IVY_HTTP_STATUS_CODE_HXX
