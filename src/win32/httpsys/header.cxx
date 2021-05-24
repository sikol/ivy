/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <ivy/win32/httpsys/header.hxx>

#include <span>
#include <unordered_map>
#include <utility>

#include <ivy/string.hxx>

namespace ivy::win32::httpsys {

    namespace {

        using known_header_t = std::pair<astring, HTTP_HEADER_ID>;

        auto get_known_headers() -> std::span<known_header_t const>
        {
            static std::pair<astring, HTTP_HEADER_ID> known_headers[] = {
                {"cache-control", HttpHeaderCacheControl},
                {"connection", HttpHeaderConnection},
                {"date", HttpHeaderDate},
                {"keep-alive", HttpHeaderKeepAlive},
                {"pragma", HttpHeaderPragma},
                {"trailer", HttpHeaderTrailer},
                {"transfer-encoding", HttpHeaderTransferEncoding},
                {"upgrade", HttpHeaderUpgrade},
                {"via", HttpHeaderVia},
                {"warning", HttpHeaderWarning},
                {"allow", HttpHeaderAllow},
                {"content-length", HttpHeaderContentLength},
                {"content-type", HttpHeaderContentType},
                {"content-encoding", HttpHeaderContentEncoding},
                {"content-language", HttpHeaderContentLanguage},
                {"content-location", HttpHeaderContentLocation},
                {"content-md5", HttpHeaderContentMd5},
                {"content-range", HttpHeaderContentRange},
                {"expires", HttpHeaderExpires},
                {"last-modified", HttpHeaderLastModified},
                {"accept", HttpHeaderAccept},
                {"accept-charset", HttpHeaderAcceptCharset},
                {"accept-encoding", HttpHeaderAcceptEncoding},
                {"accept-language", HttpHeaderAcceptLanguage},
                {"authorization", HttpHeaderAuthorization},
                {"cookie", HttpHeaderCookie},
                {"expect", HttpHeaderExpect},
                {"from", HttpHeaderFrom},
                {"host", HttpHeaderHost},
                {"if-match", HttpHeaderIfMatch},
                {"if-modified-since", HttpHeaderIfModifiedSince},
                {"if-none-match", HttpHeaderIfNoneMatch},
                {"if-range", HttpHeaderIfRange},
                {"if-unmodified-since", HttpHeaderIfUnmodifiedSince},
                {"max-fowards", HttpHeaderMaxForwards},
                {"proxy-authorization", HttpHeaderProxyAuthorization},
                {"referer", HttpHeaderReferer},
                {"range", HttpHeaderRange},
                {"te", HttpHeaderTe},
                {"translate", HttpHeaderTranslate},
                {"user-agent", HttpHeaderUserAgent},
                {"request-maximum", HttpHeaderRequestMaximum},
                {"accept-ranges", HttpHeaderAcceptRanges},
                {"age", HttpHeaderAge},
                {"etag", HttpHeaderEtag},
                {"location", HttpHeaderLocation},
                {"proxy-authenticate", HttpHeaderProxyAuthenticate},
                {"retry-after", HttpHeaderRetryAfter},
                {"server", HttpHeaderServer},
                {"set-cookie", HttpHeaderSetCookie},
                {"vary", HttpHeaderVary},
                {"www-authenticate", HttpHeaderWwwAuthenticate},
                {"response-maximum", HttpHeaderResponseMaximum},
            };

            return known_headers;
        }

        auto generate_header_name_to_id_map()
            -> std::unordered_map<astring, HTTP_HEADER_ID>
        {
            std::unordered_map<astring, HTTP_HEADER_ID> map;

            for (auto &&[name, id] : get_known_headers()) {
                map[name] = id;
            }

            return map;
        }

        auto get_header_name_to_id_map()
            -> std::unordered_map<astring, HTTP_HEADER_ID> const &
        {
            static auto map = generate_header_name_to_id_map();
            return map;
        }

        auto generate_header_id_to_name_map()
            -> std::unordered_map<HTTP_HEADER_ID, astring>
        {
            std::unordered_map<HTTP_HEADER_ID, astring> map;

            for (auto &&[name, id] : get_known_headers()) {
                map[id] = name;
            }

            return map;
        }

        auto get_header_id_to_name_map()
            -> std::unordered_map<HTTP_HEADER_ID, astring> const &
        {
            static auto map = generate_header_id_to_name_map();
            return map;
        }

    } // namespace

    auto header_id_to_name(HTTP_HEADER_ID id) -> astring {
        auto const &map = get_header_id_to_name_map();
        auto it = map.find(id);
        IVY_CHECK(it != map.end(), "header_id_to_name: invalid id");
        return it->second;
    }

    auto header_name_to_id(astring name) -> std::optional<HTTP_HEADER_ID> {
        auto const &map = get_header_name_to_id_map();
        auto it = map.find(name);

        if (it == map.end())
            return {};

        return it->second;
    }

} // namespace ivy::win32::httpsys
