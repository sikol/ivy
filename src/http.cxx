/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <ivy/http/service.hxx>
#include <ivy/http/status.hxx>

namespace ivy::http {

    namespace detail {

        auto http_errc_category::name() const noexcept -> char const *
        {
            return "http::status_code";
        }

        auto http_errc_category::message(int c) const -> std::string
        {
            switch (static_cast<http_status>(c)) {
            case http_status::continue_:
                return "Continue";

            case http_status::switching_protocols:
                return "Switching protocols";

            case http_status::processing:
                return "Processing";

            case http_status::early_hints:
                return "Early hints";

            case http_status::ok:
                return "OK";

            case http_status::created:
                return "Created";

            case http_status::accepted:
                return "Accepted";

            case http_status::non_authoritative_information:
                return "Non-authoritative information";

            case http_status::no_content:
                return "No content";

            case http_status::reset_content:
                return "Reset content";

            case http_status::partial_content:
                return "Partial content";

            case http_status::multi_status:
                return "Multi status";

            case http_status::already_reported:
                return "Already reported";

            case http_status::im_used:
                return "IM used";

            case http_status::multiple_choices:
                return "Multiple choices";

            case http_status::moved_permanently:
                return "Moved permanently";

            case http_status::found:
                return "Found";

            case http_status::see_other:
                return "See other";

            case http_status::not_modified:
                return "Not modified";

            case http_status::use_proxy:
                return "Use proxy";

            case http_status::switch_proxy:
                return "Switch proxy";

            case http_status::temporary_redirect:
                return "Temporary redirect";

            case http_status::permanent_redirect:
                return "Permanent redirect";

            case http_status::bad_request:
                return "Bad request";

            case http_status::unauthorized:
                return "Unauthorized";

            case http_status::payment_required:
                return "Payment required";

            case http_status::forbidden:
                return "Forbidden";

            case http_status::not_found:
                return "Not found";

            case http_status::method_not_allowed:
                return "Method not allowed";

            case http_status::not_acceptable:
                return "Not acceptable";

            case http_status::proxy_authentication_required:
                return "Proxy authentication required";

            case http_status::request_timeout:
                return "Request timeout";

            case http_status::conflict:
                return "Conflict";

            case http_status::gone:
                return "Gone";

            case http_status::length_required:
                return "Length required";

            case http_status::precondition_failed:
                return "Precondition failed";

            case http_status::payload_too_large:
                return "Payload too large";

            case http_status::uri_too_long:
                return "URI too long";

            case http_status::unsupported_media_type:
                return "Unsupported media type";

            case http_status::range_not_satisfiable:
                return "Range not satisfiable";

            case http_status::expectation_failed:
                return "Expectation failed";

            case http_status::im_a_teapot:
                return "I'm a teapot";

            case http_status::misdirected_request:
                return "Misdirected request";

            case http_status::unprocessable_entity:
                return "Unprocessable entity";

            case http_status::locked:
                return "Locked";

            case http_status::failed_dependency:
                return "Failed dependency";

            case http_status::too_early:
                return "Too early";

            case http_status::upgrade_required:
                return "Upgrade required";

            case http_status::precondition_required:
                return "Precondition required";

            case http_status::too_many_requests:
                return "Too many requests";

            case http_status::request_header_fields_too_large:
                return "Request header fields too large";

            case http_status::unavailable_for_legal_reasons:
                return "Unavailable for legal reasons";

            case http_status::internal_server_error:
                return "Internal server error";

            case http_status::not_implemented:
                return "Not implemented";

            case http_status::bad_gateway:
                return "Bad gateway";

            case http_status::service_unavailable:
                return "Service unavailable";

            case http_status::gateway_timeout:
                return "Gateway timeout";

            case http_status::http_version_not_supported:
                return "HTTP version not supported";

            case http_status::variant_also_negotiates:
                return "Variant also negotiates";

            case http_status::insufficient_storage:
                return "Insufficient storage";

            case http_status::loop_detected:
                return "Loop detected";

            case http_status::not_extended:
                return "Not extended";

            case http_status::network_authenticated_required:
                return "Network authenticated required";

            default:
                return "Unknown response code";
            }
        }

    } // namespace detail

    auto http_errc_category() -> detail::http_errc_category const &
    {
        static detail::http_errc_category c;
        return c;
    }

    auto make_http_status(http_status c) -> std::error_code
    {
        return {static_cast<int>(c), http_errc_category()};
    }

    auto make_http_status(int c) -> std::error_code
    {
        return {c, http_errc_category()};
    }

    service::service() noexcept = default;
    service::~service() = default;

    http_header::http_header() = default;

    http_header::http_header(http_header const &) = default;
    http_header::http_header(http_header &&) noexcept = default;

    http_header::~http_header() = default;

    auto http_header::operator=(http_header const &) -> http_header & = default;
    auto http_header::operator=(http_header &&) noexcept
        -> http_header & = default;

    auto http_header::get_field(astring const &name) -> header_field *
    {
        auto it = _fields.find(name);

        if (it == _fields.end())
            return nullptr;

        return std::addressof(it->second);
    }

    auto http_header::get_field(astring const &name) const
        -> header_field const *
    {
        return const_cast<http_header *>(this)->get_field(name);
    }

    auto http_header::add_field(header_field const &f) -> bool
    {
        auto [it, ok] = _fields.insert({f.name, f});
        return ok;
    }

    auto http_header::add_or_replace_field(header_field const &f) -> bool
    {
        auto [it, ok] = _fields.insert_or_assign(f.name, f);
        return !ok;
    }

    auto http_header::size() const noexcept -> size_type
    {
        return _fields.size();
    }

    auto http_header::begin() -> iterator
    {
        return iterator(_fields.begin());
    }

    auto http_header::begin() const -> const_iterator
    {
        return const_cast<http_header *>(this)->begin();
    }

    auto http_header::end() -> iterator
    {
        return iterator(_fields.end());
    }

    auto http_header::end() const -> const_iterator
    {
        return const_cast<http_header *>(this)->end();
    }

    auto http_header::find(key_type const &key) -> iterator {
        return iterator(_fields.find(key));
    }

    auto http_header::find(key_type const &key) const -> const_iterator
    {
        return const_cast<http_header *>(this)->find(key);
    }

    auto http_header::erase(iterator const &it) -> iterator
    {
        return iterator(_fields.erase(it._iterator));
    }

    http_header_iterator::http_header_iterator(map_type::iterator iterator)
        : _iterator(iterator)
    {
    }

    http_header_iterator::http_header_iterator(http_header_iterator const &) =
        default;
    http_header_iterator::http_header_iterator(http_header_iterator &&) noexcept =
        default;

    auto http_header_iterator::operator=(http_header_iterator const &) 
        -> http_header_iterator & = default;
    auto http_header_iterator::operator=(http_header_iterator &&) noexcept
        -> http_header_iterator & = default;

        auto http_header_iterator::operator++() -> http_header_iterator &
    {
        ++_iterator;
        return *this;
    }

    auto http_header_iterator::operator++(int) -> http_header_iterator
    {
        auto r(*this);
        ++*this;
        return r;
    }

    auto http_header_iterator::operator*() const -> header_field const &
    {
        return _iterator->second;
    }

    auto http_header_iterator::operator->() const -> header_field const *
    {
        return std::addressof(_iterator->second);
    }

    auto operator==(http_header_iterator const &a,
                    http_header_iterator const &b) -> bool
    {
        return a._iterator == b._iterator;
    }

    } // namespace ivy::http
