/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_IO_CHANNEL_HXX_INCLUDED
#define IVY_IO_CHANNEL_HXX_INCLUDED

#include <cstddef>
#include <span>
#include <system_error>
#include <type_traits>

#include <ivy/expected.hxx>

namespace ivy {

    enum struct layer_ownership {
        own_layer = 0,
        borrow_layer = 1,
    };

    template<typename base_layer, layer_ownership ownership>
    class layer_base;

    template<typename base_layer>
    class layer_base<base_layer, layer_ownership::own_layer> {
        base_layer _layer_storage;

    protected:
        layer_base() = default;

        layer_base(layer_base &&) noexcept = default;
        layer_base(layer_base const &) = delete;

        auto operator=(layer_base &&) noexcept -> layer_base & = default;
        auto operator=(layer_base const &) -> layer_base & = delete;

        ~layer_base() = default;

        layer_base(base_layer &&base)
            : _layer_storage(std::move(base))
        {
        }

    public:
        auto get_base_layer() -> base_layer &
        {
            return _layer_storage;
        }

        auto get_base_layer() const -> base_layer const &
        {
            return _layer_storage;
        }
    };

    template<typename base_layer>
    class layer_base<base_layer, layer_ownership::borrow_layer> {
        base_layer *_layer_storage;

    protected:
        layer_base(layer_base &&) noexcept = default;
        layer_base(layer_base const &) noexcept = default;

        auto operator=(layer_base &&) noexcept -> layer_base & = default;
        auto operator=(layer_base const &) noexcept -> layer_base & = default;

        ~layer_base() = default;

        layer_base(base_layer &base)
            : _layer_storage(&base)
        {
        }

    public:
        auto get_base_layer() -> base_layer &
        {
            return *_layer_storage;
        }

        auto get_base_layer() const -> base_layer const &
        {
            return *_layer_storage;
        }
    };

    using io_size_t = std::size_t;
    using io_offset_t = std::make_signed_t<std::size_t>;

    enum io_seek_pos { seek_begin, seek_end, seek_cur };

} // namespace ivy

#endif // IVY_IO_CHANNEL_HXX_INCLUDED
