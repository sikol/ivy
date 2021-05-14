/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_LAZY_HXX_INCLUDED
#define IVY_LAZY_HXX_INCLUDED

#include <functional>
#include <optional>

namespace ivy {

    /*
     * lazy<> - store a function which returns a value and execute it when
     * get() is called.  the result is cached, so get() can be called multiple
     * times.
     */
    template <typename T>
    struct lazy {
        template <typename Callable>
        explicit lazy(Callable &&);

        T const &get();

    private:
        std::optional<T> _value;
        std::function<T()> _producer;
    };

    template <typename T>
    template <typename Callable>
    lazy<T>::lazy(Callable &&function)
    {
        _producer = function;
    }

    template <typename T>
    T const &lazy<T>::get()
    {
        if (!_value)
            _value = _producer();
        return *_value;
    }

} // namespace sk

#endif // IVY_LAZY_HXX_INCLUDED
