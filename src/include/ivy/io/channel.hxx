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

#include <ivy/buffer/buffer.hxx>
#include <ivy/expected.hxx>

namespace ivy {

    // Concepts of channels.

    // clang-format off

    template<typename T>
    concept channel = requires {
        typename T::value_type;
    };

    template<channel C>
    using channel_value_t = typename C::value_type;

    template<typename T>
    concept sequential_input_channel = 
        channel<T> 
        && requires(T &o, std::span<channel_value_t<T>> &b)  {
            { o.read(b) } -> std::same_as<expected<std::size_t, std::error_code>>;
        };

    template<typename T>
    concept sequential_output_channel = 
        channel<T> 
        && requires(T &o, std::span<channel_value_t<T const>> &b)  {
            { o.write(b) } -> std::same_as<expected<std::size_t, std::error_code>>;
        };

    template<typename T>
    concept sequential_channel = 
        sequential_input_channel<T> && sequential_output_channel<T>;

    // clang-format on

    enum struct layer_ownership {
        own_layer = 0,
        borrow_layer = 1,
    };

    template <typename base_layer, layer_ownership ownership>
    class layer_base;

    template <typename base_layer>
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

    template <typename base_layer>
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

    constexpr io_size_t unlimited = std::numeric_limits<io_size_t>::max();

    enum io_seek_pos { seek_begin, seek_end, seek_cur };

    /*************************************************************************
     * read_some()
     */

    // clang-format off
    template<sequential_input_channel channel_type,
             std::ranges::contiguous_range range_type>
    [[nodiscard]]
    auto read_some(channel_type &channel,
                   range_type &&range,
                   io_size_t n = unlimited)
        -> expected<io_size_t, std::error_code>
    requires std::same_as<channel_value_t<channel_type>,
                          std::ranges::range_value_t<range_type>>
    // clang-format on
    {
        std::span<channel_value_t<channel_type>> span(range);
        if (n < span.size())
            span = span.first(n);

        return channel.read(span);
    }

    // clang-format off
    template<sequential_input_channel channel_type,
             writable_buffer buffer_type>
    [[nodiscard]]
    auto read_some(channel_type &channel,
                   buffer_type &buffer,
                   io_size_t n = unlimited)
        -> expected<io_size_t, std::error_code>
    requires std::same_as<channel_value_t<channel_type>,
                          buffer_value_t<buffer_type>>
    // clang-format on
    {
        auto ranges = buffer.writable_ranges();
        if (std::ranges::size(ranges) == 0)
            return 0u;

        auto &first_range = *std::ranges::begin(ranges);
        auto nread = read_some(channel, first_range, n);

        if (nread)
            buffer.commit(*nread);

        return nread;
    }

    /*************************************************************************
     * read_all()
     */

    // clang-format off
    template <sequential_input_channel channel_type, 
              std::ranges::contiguous_range range_type>
    [[nodiscard]]
    auto read_all(channel_type &channel,
                  range_type &&range,
                  io_size_t n = unlimited)
        -> std::pair<io_size_t, std::error_code>
    requires std::same_as<channel_value_t<channel_type>,
                          std::ranges::range_value_t<range_type>>
    // clang-format on
    {
        std::span<channel_value_t<channel_type>> span(range);
        if (n < span.size())
            span = span.first(n);

        io_size_t nread = 0;

        while (!span.empty()) {
            auto ret = channel.read_some(span);

            if (!ret)
                return {nread, ret.error()};

            nread += *ret;
            span = span.subspan(*ret);
        }

        return {nread, {}};
    }

    // clang-format off
    template <sequential_input_channel channel_type, 
              writable_buffer buffer_type>
    [[nodiscard]]
    auto read_all(channel_type &channel,
                  buffer_type &buffer,
                  io_size_t n = unlimited)
        -> std::pair<io_size_t, std::error_code>
    requires std::same_as<channel_value_t<channel_type>,
                          buffer_value_t<buffer_type>>
    // clang-format on
    {
        io_size_t nread = 0;
        std::pair<io_size_t, std::error_code> ret;

        for (auto &&range : buffer.writable_ranges()) {
            ret = read_all(channel, range, n - nread);
            nread += ret.first;

            if (ret.first == 0 || ret.second || nread == n)
                break;
        }

        buffer.commit(nread);
        return {nread, ret.second};
    }

    /*************************************************************************
     * write_some()
     */

    // clang-format off
    template<sequential_output_channel channel_type, 
             std::ranges::contiguous_range range_type>
    [[nodiscard]]
    auto write_some(channel_type &channel,
                    range_type &&range,
                    io_size_t n = unlimited)
         -> expected<io_size_t, std::error_code>
    requires std::same_as<channel_value_t<channel_type>,
                          std::ranges::range_value_t<range_type>>
    // clang-format on
    {
        std::span<std::add_const_t<channel_value_t<channel_type>>> span(range);

        if (n < span.size())
            span = span.first(n);

        return channel.write_some(span);
    }

    // clang-format off
    template<sequential_output_channel channel_type,
             readable_buffer buffer_type>
    [[nodiscard]]
    auto write_some(channel_type &channel,
                    buffer_type &buffer,
                    io_size_t n = unlimited)
        -> expected<io_size_t, std::error_code>
    requires std::same_as<channel_value_t<channel_type>,
                          buffer_value_t<buffer_type>>
    // clang-format on
    {
        auto ranges = buffer.get_readable_ranges();
        if (std::ranges::size(ranges) == 0u)
            return 0u;

        auto &first_range = *std::ranges::begin(ranges);
        std::span span(first_range);
        if (n < span.size())
            span = span.first(n);

        auto nwritten = write_some(channel, span);

        if (!nwritten)
            return make_unexpected(nwritten.error());

        buffer.discard(*nwritten);
        return *nwritten;
    }

    /*************************************************************************
     * write_all()
     */

    // clang-format off
    template<sequential_output_channel channel_type,
             std::ranges::contiguous_range range_type>
    [[nodiscard]]
    auto write_all(channel_type &channel,
                   range_type &&range,
                   io_size_t n = unlimited)
         -> std::pair<io_size_t, std::error_code>
    requires std::same_as<const channel_value_t<channel_type>,
                          std::add_const_t<std::ranges::range_value_t<range_type>>>
    // clang-format on
    {
        std::span span(range);
        if (n < span.size())
            span = span.first(n);

        io_size_t nwritten = 0;

        while (!span.empty()) {
            auto ret = channel.write_some(span);

            if (!ret)
                return {nwritten, ret.error()};

            nwritten += *ret;
            span = span.subspan(*ret);
        }

        return {nwritten, {}};
    }

    // clang-format off
    template<sequential_output_channel channel_type,
             readable_buffer buffer_type>
    [[nodiscard]]
    auto write_all(channel_type &channel,
                   buffer_type &buffer,
                   io_size_t n = unlimited)
        -> std::pair<io_size_t, std::error_code>
    requires std::same_as<channel_value_t<channel_type>,
                          buffer_value_t<buffer_type>>
    // clang-format on
    {
        io_size_t nwritten = 0;

        std::pair<io_size_t, std::error_code> ret;

        for (auto &&range : buffer.readable_ranges()) {
            ret = write_all(channel, range, n - nwritten);
            nwritten += ret.first;

            if (ret.first == 0 || ret.second || nwritten == n)
                break;
        }

        buffer.discard(nwritten);
        return {nwritten, ret.second};
    }

} // namespace ivy

#endif // IVY_IO_CHANNEL_HXX_INCLUDED
