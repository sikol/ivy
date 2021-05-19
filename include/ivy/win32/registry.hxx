/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_REGISTRY_HXX_INCLUDED
#define IVY_WIN32_REGISTRY_HXX_INCLUDED

#include <memory>

#include <ivy/string.hxx>
#include <ivy/win32/windows.hxx>

namespace ivy::win32 {

    namespace detail {

        struct registry;

        struct registry_deleter {
            auto operator()(registry *) const noexcept -> void;
        };

    } // namespace detail

    using registry_handle =
        std::unique_ptr<detail::registry, detail::registry_deleter>;

    auto registry_open(HKEY key, REGSAM access, wstring const &subkey)
        -> expected<registry_handle, std::error_code>;

    auto registry_create(HKEY key, REGSAM access, wstring const &subkey)
        -> expected<registry_handle, std::error_code>;

    auto registry_read_string(registry_handle &, wstring const &value_name)
        -> expected<wstring, std::error_code>;

    auto registry_write_string(registry_handle &,
                               wstring const &value_name,
                               wstring const &data)
        -> expected<void, std::error_code>;

} // namespace ivy::win32

#endif // IVY_WIN32_REGISTRY_HXX_INCLUDED
