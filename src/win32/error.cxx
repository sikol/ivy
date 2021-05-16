/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <format>

#include <ivy/win32/error.hxx>

namespace ivy::win32 {

    namespace {

        auto format_error(DWORD errcode) -> std::string
        {
            LPSTR msgbuf{};

            auto len = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                          FORMAT_MESSAGE_FROM_SYSTEM |
                                          FORMAT_MESSAGE_IGNORE_INSERTS,
                                      nullptr,
                                      errcode,
                                      0,
                                      reinterpret_cast<LPSTR>(&msgbuf),
                                      0,
                                      nullptr);

            if (len == 0)
                return "[FormatMessageA() failed]";

            std::string ret(msgbuf, len);
            LocalFree(msgbuf);

            return ret;
        }

    } // namespace

    namespace detail {

        auto win32_errc_category::name() const noexcept -> char const *
        {
            return "win32";
        }

        auto win32_errc_category::message(int c) const -> std::string
        {
            return format_error(static_cast<DWORD>(c));
        }

        auto nt_errc_category::name() const noexcept -> char const *
        {
            return "ntstatus";
        }

        auto nt_errc_category::message(int c) const -> std::string
        {
            return std::format("{:08x}", static_cast<unsigned long>(c));
        }

    } // namespace detail

    auto win32_errc_category() -> detail::win32_errc_category const &
    {
        static detail::win32_errc_category c;
        return c;
    }

    auto nt_errc_category() -> detail::nt_errc_category const &
    {
        static detail::nt_errc_category c;
        return c;
    }

    auto make_error_code(win32_errc e) -> std::error_code
    {
        return {static_cast<int>(e), win32_errc_category()};
    }

    auto make_win32_error(DWORD e) -> std::error_code
    {
        return {static_cast<int>(e), win32_errc_category()};
    }

    auto make_win32_error(LSTATUS e) -> std::error_code
    {
        return {static_cast<int>(e), win32_errc_category()};
    }

    auto get_last_error() -> std::error_code
    {
        return make_win32_error(GetLastError());
    }

    auto make_error_code(nt_errc e) -> std::error_code
    {
        return {static_cast<int>(e), win32_errc_category()};
    }

    auto make_nt_error(NTSTATUS e) -> std::error_code
    {
        return {static_cast<int>(e), nt_errc_category()};
    }

} // namespace ivy::win32
