/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_ERROR_HXX
#define IVY_WIN32_ERROR_HXX

#include <system_error>

#include <ivy/win32/windows.hxx>

#include <ntstatus.h>
#include <bcrypt.h> // for NTSTATUS

namespace ivy::win32 {

    /*
     * win32 error category - this corresponds to FORMAT_MESSAGE_FROM_SYSTEM.
     */

    // Rather than listing every possible error here (there are thousands of
    // them), just list a few common ones that we use.
    enum struct win32_errc : DWORD {
        no_error = 0,
        io_pending = ERROR_IO_PENDING,
        more_data = ERROR_MORE_DATA,
        handle_eof = ERROR_HANDLE_EOF,
    };

    // NTSTATUS errors.  This is a separate namespace from Win32 errors.
    enum struct nt_errc : NTSTATUS {
        success = 0,
    };

} // namespace ivy::win32

namespace std {

    template <>
    struct is_error_code_enum<ivy::win32::win32_errc> : true_type {
    };

    template <>
    struct is_error_code_enum<ivy::win32::nt_errc> : true_type {
    };

}; // namespace std

namespace ivy::win32 {

    namespace detail {

        struct win32_errc_category final : std::error_category {
            auto name() const noexcept -> char const * final;
            auto message(int c) const -> std::string final;
        };

        struct nt_errc_category final : std::error_category {
            auto name() const noexcept -> char const * final;
            auto message(int c) const -> std::string final;
        };

    } // namespace detail

    auto win32_errc_category() -> detail::win32_errc_category const &;
    auto make_error_code(win32_errc e) -> std::error_code;

    auto nt_errc_category() -> detail::nt_errc_category const &;
    auto make_error_code(nt_errc e) -> std::error_code;

    // Construct a Win32 error from an error code.
    auto make_win32_error(DWORD e) -> std::error_code;
    auto make_win32_error(LSTATUS e) -> std::error_code;

    auto make_nt_error(NTSTATUS e) -> std::error_code;

    // Return a Win32 error representing GetLastError()
    auto get_last_error() -> std::error_code;

} // namespace ivy::win32

#endif // IVY_WIN32_ERROR_HXX
