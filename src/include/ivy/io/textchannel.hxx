/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_IO_TEXTCHANNEL_HXX_INCLUDED
#define IVY_IO_TEXTCHANNEL_HXX_INCLUDED

#include <system_error>
#include <span>

#include <ivy/charenc.hxx>
#include <ivy/expected.hxx>
#include <ivy/io/channel.hxx>
#include <ivy/string.hxx>

namespace ivy {

    template <typename base_channel, layer_ownership ownership>
    class textchannel final : layer_base<base_channel, ownership> {
    private:
        std::error_code _error;

    protected:
        friend auto make_textchannel(base_channel &)
            -> textchannel<base_channel, layer_ownership::borrow_layer>;

        friend auto make_textchannel(base_channel &&)
            -> textchannel<base_channel, layer_ownership::own_layer>;

    public:
        using value_type = typename base_channel::value_type;
        using encoding_type = typename base_channel::encoding_type;

        textchannel(textchannel &&) noexcept = default;
        auto operator=(textchannel &&) noexcept -> textchannel & = default;

        textchannel(textchannel const &) = delete;
        auto operator=(textchannel const &) -> textchannel & = delete;

        textchannel(base_channel &c) requires(ownership ==
                                              layer_ownership::borrow_layer)
            : layer_base<base_channel, ownership>(c)
        {
        }

        textchannel(base_channel &&c) requires(ownership ==
                                               layer_ownership::own_layer)
            : layer_base(std::move(c))
        {
        }

        auto read(std::span<value_type>) noexcept
            -> expected<io_size_t, std::error_code>;

        auto write(std::span<value_type const>) noexcept
            -> expected<io_size_t, std::error_code>;

        auto get_error() const noexcept -> std::error_code;
        auto set_error(std::error_code) noexcept -> std::error_code;
        auto clear_error() noexcept -> std::error_code;

        explicit operator bool() const noexcept;
    };

    template <typename base_channel>
    auto make_textchannel(base_channel &b)
    {
        return textchannel<base_channel, layer_ownership::borrow_layer>(b);
    }

    template <typename base_channel>
    auto make_textchannel(base_channel &&b)
    {
        return textchannel<base_channel, layer_ownership::own_layer>(
            std::move(b));
    }

    template <typename base_channel, layer_ownership ownership>
    auto textchannel<base_channel, ownership>::get_error() const noexcept
        -> std::error_code
    {
        return _error;
    }

    template <typename base_channel, layer_ownership ownership>
    auto textchannel<base_channel, ownership>::set_error(
        std::error_code error) noexcept -> std::error_code
    {
        return std::exchange(_error, error);
    }

    template <typename base_channel, layer_ownership ownership>
    auto textchannel<base_channel, ownership>::clear_error() noexcept
        -> std::error_code
    {
        return std::exchange(_error, std::error_code());
    }

    template <typename base_channel, layer_ownership ownership>
    auto textchannel<base_channel, ownership>::read(
        std::span<value_type> buf) noexcept
        -> expected<io_size_t, std::error_code>
    {
        if (_error)
            return make_unexpected(_error);

        return this->get_base_layer().read(buf);
    }

    template <typename base_channel, layer_ownership ownership>
    auto textchannel<base_channel, ownership>::write(
        std::span<value_type const> buf) noexcept
        -> expected<io_size_t, std::error_code>
    {
        if (_error)
            return make_unexpected(_error);

        return this->get_base_layer().write(buf);
    }

    template <typename base_channel, layer_ownership ownership>
    textchannel<base_channel, ownership>::operator bool() const noexcept
    {
        return !_error;
    }

    /*************************************************************************
     *
     * textchannel formatted I/O.
     *
     */

    template <typename base_channel, layer_ownership ownership>
    auto operator<<(textchannel<base_channel, ownership> &chan,
                    typename textchannel<base_channel, ownership>::value_type c)
        -> textchannel<base_channel, ownership> &
    {
        chan.write(std::span(&c, 1));
        return chan;
    }

    template <typename base_channel, layer_ownership ownership>
    auto operator<<(
        textchannel<base_channel, ownership> &chan,
        typename textchannel<base_channel, ownership>::value_type const *c)
        -> textchannel<base_channel, ownership> &
    {
        auto str = std::span(
            c, textchannel<base_channel, ownership>::encoding_type::length(c));
        chan.write(str);
        return chan;
    }

    template <typename base_channel,
              layer_ownership ownership,
              typename allocator>
    auto
    operator<<(textchannel<base_channel, ownership> &chan,
               basic_string<
                   typename textchannel<base_channel, ownership>::encoding_type,
                   allocator> const &s)
        -> textchannel<base_channel, ownership> &
    {
        chan.write(s);
        return chan;
    }

    namespace detail {

        template <typename base_channel,
                  layer_ownership ownership,
                  std::signed_integral integer>
        auto write_number(textchannel<base_channel, ownership> &chan, integer v)
        {
            using char_type =
                typename textchannel<base_channel, ownership>::value_type;

            char_type b[std::numeric_limits<integer>::digits10 + 2];
            char_type *p = std::ranges::end(b);
            bool negative = false;
            if (v < 0) {
                negative = true;
                v = -v;
            }

            while (v) {
                *--p = char_type('0' + (v % 10));
                v /= 10;
            }

            if (negative)
                *--p = '-';

            chan.write(std::span(p, std::ranges::end(b)));
        }

        template <typename base_channel,
                  layer_ownership ownership,
                  std::unsigned_integral integer>
        auto write_number(textchannel<base_channel, ownership> &chan, integer v)
        {
            using char_type =
                typename textchannel<base_channel, ownership>::value_type;

            char_type b[std::numeric_limits<integer>::digits10 + 2];
            char_type *p = std::ranges::end(b);

            while (v) {
                *--p = char_type('0' + (v % 10));
                v /= 10;
            }

            chan.write(std::span(p, std::ranges::end(b)));
        }

    } // namespace detail

    template <typename base_channel, layer_ownership ownership>
    auto operator<<(textchannel<base_channel, ownership> &chan, short i)
        -> textchannel<base_channel, ownership> &
    {
        detail::write_number(chan, i);
        return chan;
    }

    template <typename base_channel, layer_ownership ownership>
    auto operator<<(textchannel<base_channel, ownership> &chan,
                    unsigned short i) -> textchannel<base_channel, ownership> &
    {
        detail::write_number(chan, i);
        return chan;
    }

    template <typename base_channel, layer_ownership ownership>
    auto operator<<(textchannel<base_channel, ownership> &chan, int i)
        -> textchannel<base_channel, ownership> &
    {
        detail::write_number(chan, i);
        return chan;
    }

    template <typename base_channel, layer_ownership ownership>
    auto operator<<(textchannel<base_channel, ownership> &chan, unsigned int i)
        -> textchannel<base_channel, ownership> &
    {
        detail::write_number(chan, i);
        return chan;
    }

    template <typename base_channel, layer_ownership ownership>
    auto operator<<(textchannel<base_channel, ownership> &chan, long i)
        -> textchannel<base_channel, ownership> &
    {
        detail::write_number(chan, i);
        return chan;
    }

    template <typename base_channel, layer_ownership ownership>
    auto operator<<(textchannel<base_channel, ownership> &chan, unsigned long i)
        -> textchannel<base_channel, ownership> &
    {
        detail::write_number(chan, i);
        return chan;
    }

    template <typename base_channel, layer_ownership ownership>
    auto operator<<(textchannel<base_channel, ownership> &chan, long long i)
        -> textchannel<base_channel, ownership> &
    {
        detail::write_number(chan, i);
        return chan;
    }

    template <typename base_channel, layer_ownership ownership>
    auto operator<<(textchannel<base_channel, ownership> &chan,
                    unsigned long long i)
        -> textchannel<base_channel, ownership> &
    {
        detail::write_number(chan, i);
        return chan;
    }

} // namespace ivy

#endif // IVY_IO_TEXTCHANNEL_HXX_INCLUDED
