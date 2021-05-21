/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_TRACE_HXX_INCLUDED
#define IVY_TRACE_HXX_INCLUDED

#ifdef IVY_ENABLE_TRACING
#    include <array>
#    include <atomic>
#    include <cstdio>
#    include <cstdlib>
#    include <format>
#    include <iostream>
#    include <mutex>

namespace ivy::detail {

    [[nodiscard]] inline auto tracing_enabled() -> bool
    {
        static std::atomic<int> enabled;

        switch (enabled) {
        case 1:
            return true;

        case -1:
            return false;

        default:
#    ifdef _WIN32
            std::array<char, 16> buf{};
            const char *s = buf.data();
            size_t size{};
            auto err = getenv_s(&size, buf.data(), buf.size(), "IVY_TRACE");
            if (err != 0 || size == 0)
                s = nullptr;
#    else
            char const *s = std::getenv("IVY_TRACE");
#    endif
            if (s != nullptr && (*s != 0)) {
                enabled = 1;
                return true;
            }

            enabled = -1;
            return false;
        }
    }

    template <typename... Args>
    inline auto trace_print(Args &&...args) -> void
    {
        static std::mutex print_guard;

        std::string s(std::format(std::forward<Args>(args)...));

        std::lock_guard lock(print_guard);
        std::cerr.write(s.data(), static_cast<std::streamsize>(s.size()));
        std::cerr.write("\n", 1);
    }

#    define IVY_TRACE(...)                                                     \
        do {                                                                   \
            if (::ivy::detail::tracing_enabled()) {                            \
                ::ivy::detail::trace_print(__VA_ARGS__);                       \
            }                                                                  \
        } while (0)

} // namespace ivy::detail

#else

#    define IVY_TRACE(...) ((void)0)

#endif

#endif // IVY_TRACE_HXX_INCLUDED
