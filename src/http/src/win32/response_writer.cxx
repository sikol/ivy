/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <ivy/win32/httpsys/response_writer.hxx>
#include <ivy/win32/httpsys/request_queue.hxx>

#include <ivy/win32/error.hxx>
#include <ivy/io/channel.hxx>

namespace ivy::win32::httpsys {

    response_writer::response_writer(request_queue &queue,
                                     HTTP_REQUEST_ID request_id) noexcept
        : _request_queue(&queue)
        , _request_id(request_id)
    {
    }

    response_writer::response_writer(response_writer &&other) noexcept
        : _state(std::exchange(other._state, s_done))
        , _config(other._config)
        , _request_queue(other._request_queue)
        , _request_id(other._request_id)
    {
    }

    response_writer::~response_writer()
    {
        if (_state == s_write_body) {
            ULONG bytes_sent{};

            // Response aborted
            (void)::HttpSendResponseEntityBody(
                _request_queue->get_handle(),
                _request_id,
                HTTP_SEND_RESPONSE_FLAG_DISCONNECT,
                0,
                nullptr,
                &bytes_sent,
                nullptr,
                0,
                nullptr,
                nullptr);
        }
    }

    auto response_writer::start_response(HTTP_RESPONSE const *response,
                                         response_configuration config) noexcept
        -> expected<void, std::error_code>
    {
        _config = config;

        if (_state != s_initial)
            return make_unexpected(
                make_error_code(std::errc::invalid_argument));

        ULONG flags = 0;
        ULONG bytes_sent{};

        if (_config.has_body)
            flags |= HTTP_SEND_RESPONSE_FLAG_MORE_DATA;

        auto r =
            ::HttpSendHttpResponse(_request_queue->get_handle(),
                                   _request_id,
                                   flags,
                                   const_cast<HTTP_RESPONSE *>(response),
                                   nullptr,
                                   &bytes_sent,
                                   nullptr,
                                   0,
                                   nullptr,
                                   nullptr);

        _state = _config.has_body ? s_write_body : s_done;

        if (r != NO_ERROR) {
            _state = s_done;
            return make_unexpected(make_win32_error(r));
        }

        return {};
    }

    auto response_writer::write_data(std::span<std::byte const> data) noexcept
        -> expected<void, std::error_code>
    {
        // NB: "chunk" can refer to both HTTP transfer-encoding chunks and to
        // the HTTP_DATA_CHUNK objects, which don't necessarily involve HTTP
        // chunking.

        if (_state != s_write_body)
            return make_unexpected(
                make_error_code(std::errc::invalid_argument));

        USHORT nchunks{};
        char chunk_prefix[std::numeric_limits<io_size_t>::digits10 + 3]{};
        HTTP_DATA_CHUNK chunks[3]{};

        if (data.size() > std::numeric_limits<ULONG>::max())
            return make_unexpected(make_error_code(std::errc::value_too_large));

        if (_config.chunked) {
            nchunks = 3;

            auto tcr = std::to_chars(std::ranges::begin(chunk_prefix),
                                     std::ranges::end(chunk_prefix),
                                     data.size(),
                                     16);

            if (tcr.ec != std::errc())
                return make_unexpected(std::make_error_code(tcr.ec));

            *tcr.ptr++ = '\r';
            *tcr.ptr++ = '\n';

            chunks[0].DataChunkType = HttpDataChunkFromMemory;
            chunks[0].FromMemory.pBuffer = chunk_prefix;
            chunks[0].FromMemory.BufferLength = static_cast<ULONG>(
                std::distance(std::ranges::begin(chunk_prefix), tcr.ptr));

            chunks[1].DataChunkType = HttpDataChunkFromMemory;
            chunks[1].FromMemory.pBuffer = const_cast<std::byte *>(data.data());
            chunks[1].FromMemory.BufferLength = static_cast<ULONG>(data.size());

            chunks[2].DataChunkType = HttpDataChunkFromMemory;
            chunks[2].FromMemory.pBuffer = const_cast<char *>("\r\n");
            chunks[2].FromMemory.BufferLength = 2;
        } else {
            nchunks = 1;

            chunks[0].DataChunkType = HttpDataChunkFromMemory;
            chunks[0].FromMemory.pBuffer = const_cast<std::byte *>(data.data());
            chunks[0].FromMemory.BufferLength = static_cast<ULONG>(data.size());
        }

        ULONG bytes_sent;

        auto r = ::HttpSendResponseEntityBody(_request_queue->get_handle(),
                                              _request_id,
                                              HTTP_SEND_RESPONSE_FLAG_MORE_DATA,
                                              nchunks,
                                              chunks,
                                              &bytes_sent,
                                              nullptr,
                                              0,
                                              nullptr,
                                              nullptr);

        if (r != NO_ERROR) {
            _state = s_done;
            return make_unexpected(make_win32_error(r));
        }

        return {};
    }

    auto response_writer::finish_response() noexcept
        -> expected<void, std::error_code>
    {
        if (_state != s_write_body)
            return make_unexpected(
                make_error_code(std::errc::invalid_argument));

        ULONG r;
        ULONG bytes_sent{};
        ULONG flags = 0;

        if (_config.chunked) {
            HTTP_DATA_CHUNK chunk{};
            chunk.FromMemory.pBuffer = const_cast<char *>("0\r\n\r\n");
            chunk.FromMemory.BufferLength = 5;

            r = ::HttpSendResponseEntityBody(_request_queue->get_handle(),
                                             _request_id,
                                             flags,
                                             1,
                                             &chunk,
                                             &bytes_sent,
                                             nullptr,
                                             0,
                                             nullptr,
                                             nullptr);
        } else {
            r = ::HttpSendResponseEntityBody(_request_queue->get_handle(),
                                             _request_id,
                                             flags,
                                             0,
                                             nullptr,
                                             &bytes_sent,
                                             nullptr,
                                             0,
                                             nullptr,
                                             nullptr);
        }

        _state = s_done;

        if (r != NO_ERROR) {
            return make_unexpected(make_win32_error(r));
        }

        return {};
    }

} // namespace ivy::win32::httpsys
