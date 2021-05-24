/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_WIN32_HEAP_HXX_INCLUDED
#define IVY_WIN32_HEAP_HXX_INCLUDED

#include <memory>

#include <ivy/win32/windows.hxx>

namespace ivy::win32 {

    template <typename T>
    struct process_heap_deleter {
        auto operator()(T *p) const noexcept -> void
        {
            ::HeapFree(::GetProcessHeap(), 0, p);
        }
    };

    template<typename T>
    using unique_heap_ptr = std::unique_ptr<T, process_heap_deleter<T>>;

    template<typename T, typename... Args>
    auto make_heap_unique(Args &&...args) -> unique_heap_ptr<T>
    {
        void *p = ::HeapAlloc(::GetProcessHeap(), 0, sizeof(T));
        if (p == nullptr)
            throw std::bad_alloc();

        T *o = new (p) T(std::forward<Args>(args)...);
        return unique_heap_ptr(o);
    }

} // namespace ivy::win32

#endif // IVY_WIN32_HEAP_HXX_INCLUDED
