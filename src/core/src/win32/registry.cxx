/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <new>

#include <ivy/noncopyable.hxx>
#include <ivy/win32/error.hxx>
#include <ivy/win32/registry.hxx>
#include <ivy/string/transcode.hxx>

namespace ivy::win32 {

    namespace detail {

        struct registry : ivy::nonmovable {
            HKEY _handle;

            registry(HKEY handle) : _handle(handle) {}

            ~registry()
            {
                ::RegCloseKey(_handle);
            }
        };

    } // namespace detail

    auto registry_open(HKEY key, REGSAM access, wstring const &subkey)
        -> expected<registry_handle, std::error_code>
    {
        HKEY handle{};

        auto status = ::RegOpenKeyExW(key, subkey.c_str(), 0, access, &handle);
        if (status != ERROR_SUCCESS)
            return make_unexpected(make_win32_error(status));

        auto *handlep = new (std::nothrow) detail::registry(handle);
        if (!handlep)
            return make_unexpected(
                std::make_error_code(std::errc::not_enough_memory));

        return registry_handle(handlep);
    }

    auto registry_create(HKEY key, REGSAM access, wstring const &subkey)
        -> expected<registry_handle, std::error_code>
    {
        HKEY handle{};

        DWORD disposition = 0;
        auto status = ::RegCreateKeyExW(key,
                                      subkey.c_str(),
                                      0,
                                      nullptr,
                                      0,
                                      access,
                                      nullptr,
                                      &handle,
                                      &disposition);
        if (status != ERROR_SUCCESS)
            return make_unexpected(make_win32_error(status));

        auto *handlep = new (std::nothrow) detail::registry(handle);
        if (!handlep)
            return make_unexpected(
                std::make_error_code(std::errc::not_enough_memory));

        return registry_handle(handlep);
    }

    auto registry_read_string(registry_handle &handle, wstring const &value_name)
        -> expected<wstring, std::error_code>
    {
        std::vector<std::byte> bytes;
        DWORD nbytes{};

        // Call once to get the size.
        LSTATUS status{};

        do {
            status = ::RegGetValueW(handle->_handle,
                                  nullptr,
                                  value_name.c_str(),
                                  RRF_RT_REG_SZ,
                                  nullptr,
                                  &bytes[0],
                                  &nbytes);
        } while (status == ERROR_MORE_DATA);

        if (status != ERROR_SUCCESS)
            return make_unexpected(make_win32_error(status));

        auto str = bytes_to_string<wstring>(bytes);
        if (!str)
            return make_unexpected(make_error_code(errc::invalid_encoding));

        // Registry strings end with a nul byte
        wstring ret(*str);

        if (ret[ret.size() - 1] == u'\0')
            ret = str->substr(0, str->size() - 1);

        return ret;
    }

    auto registry_write_string(registry_handle &handle,
                               wstring const &value_name,
                               wstring const &data)
        -> expected<void, std::error_code>
    {
        auto bytes = as_bytes(std::span(data));
        if (bytes.size() > std::numeric_limits<DWORD>::max())
            return make_unexpected(
                std::make_error_code(std::errc::value_too_large));
        DWORD nbytes = static_cast<DWORD>(bytes.size());

        auto status =
            RegSetValueExW(handle->_handle,
                           value_name.c_str(),
                           0,
                           REG_SZ,
                           reinterpret_cast<BYTE const *>(bytes.data()),
                           nbytes);

        if (status != NO_ERROR)
            return make_unexpected(make_win32_error(status));

        return {};
    }

} // namespace ivy::win32
