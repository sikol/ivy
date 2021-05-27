/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_IO_PMRCHANNEL_HXX_INCLUDED
#define IVY_IO_PMRCHANNEL_HXX_INCLUDED

#include <memory>
#include <span>
#include <system_error>

#include <ivy/expected.hxx>
#include <ivy/io/channel.hxx>

namespace ivy {

    template <typename T>
    class pmrchannel_base {
    protected:
        pmrchannel_base() = default;
        virtual ~pmrchannel_base() = default;

    public:
        using value_type = T;

        pmrchannel_base(pmrchannel_base const &) = delete;
        pmrchannel_base(pmrchannel_base &&) = delete;

        auto operator=(pmrchannel_base const &) -> pmrchannel_base & = delete;
        auto operator=(pmrchannel_base &&) -> pmrchannel_base & = delete;
    };

    template <typename T>
    class pmrichannel : public pmrchannel_base<T> {
    public:
        using typename pmrchannel_base<T>::value_type;

        virtual auto read(std::span<value_type>) noexcept
            -> expected<io_size_t, std::error_code> = 0;
    };

    template <sequential_input_channel base_channel, layer_ownership ownership>
    class pmrichannel_wrapper
        : public pmrichannel<typename base_channel::value_type>,
          public layer_base<base_channel, ownership> {
    public:
        using value_type = typename base_channel::value_type;

        pmrichannel_wrapper(base_channel &c) requires(
            ownership == layer_ownership::borrow_layer)
            : layer_base<base_channel, ownership>(c)
        {
        }

        pmrichannel_wrapper(base_channel &&c) requires(
            ownership == layer_ownership::own_layer)
            : layer_base<base_channel, ownership>(std::move(c))
        {
        }

        auto read(std::span<value_type> buf) noexcept
            -> expected<io_size_t, std::error_code> override
        {
            return this->get_base_layer().read(buf);
        }
    };

    template <sequential_input_channel base_channel>
    auto make_pmrichannel(base_channel &b)
    {
        return std::make_unique<
            pmrichannel_wrapper<base_channel, layer_ownership::borrow_layer>>(
            b);
    }

    template <sequential_input_channel base_channel>
    auto make_pmrichannel(base_channel &&b)
    {
        return std::make_unique<
            pmrichannel_wrapper<base_channel, layer_ownership::own_layer>>(
            std::move(b));
    }

    template <typename T>
    class pmrochannel : public pmrchannel_base<T> {
    public:
        using typename pmrchannel_base<T>::value_type;

        virtual auto write(std::span<value_type const>) noexcept
            -> expected<io_size_t, std::error_code> = 0;
    };

    template <sequential_output_channel base_channel, layer_ownership ownership>
    class pmrochannel_wrapper
        : public pmrochannel<typename base_channel::value_type>,
          public layer_base<base_channel, ownership> {
    public:
        using value_type = typename base_channel::value_type;

        pmrochannel_wrapper(base_channel &c) requires(
            ownership == layer_ownership::borrow_layer)
            : layer_base<base_channel, ownership>(c)
        {
        }

        pmrochannel_wrapper(base_channel &&c) requires(
            ownership == layer_ownership::own_layer)
            : layer_base<base_channel, ownership>(std::move(c))
        {
        }

        auto write(std::span<value_type const> buf) noexcept
            -> expected<io_size_t, std::error_code> override
        {
            return this->get_base_layer().write(buf);
        }
    };

    template <sequential_output_channel base_channel>
    auto make_pmrochannel(base_channel &b)
    {
        return std::make_unique<
            pmrochannel_wrapper<base_channel, layer_ownership::borrow_layer>>(
            b);
    }

    template <sequential_output_channel base_channel>
    auto make_pmrochannel(base_channel &&b)
    {
        return std::make_unique<
            pmrochannel_wrapper<base_channel, layer_ownership::own_layer>>(
            std::move(b));
    }

    template <typename T>
    class pmrchannel : public pmrichannel<T>, public pmrochannel<T> {
    public:
        using typename pmrchannel_base<T>::value_type;
    };

    template <sequential_channel base_channel, layer_ownership ownership>
    class pmrchannel_wrapper
        : public pmrchannel<typename base_channel::value_type>,
          public layer_base<base_channel, ownership> {
    public:
        using value_type = typename base_channel::value_type;

        pmrchannel_wrapper(base_channel &c) requires(
            ownership == layer_ownership::borrow_layer)
            : layer_base<base_channel, ownership>(c)
        {
        }

        pmrchannel_wrapper(base_channel &&c) requires(
            ownership == layer_ownership::own_layer)
            : layer_base<base_channel, ownership>(std::move(c))
        {
        }

        auto write(std::span<value_type const> buf) noexcept
            -> expected<io_size_t, std::error_code> override
        {
            return this->get_base_layer().write(buf);
        }

        auto read(std::span<value_type> buf) noexcept
            -> expected<io_size_t, std::error_code> override
        {
            return this->get_base_layer().read(buf);
        }
    };

    template <sequential_channel base_channel>
    auto make_pmrchannel(base_channel &b)
    {
        return std::make_unique<
            pmrchannel_wrapper<base_channel, layer_ownership::borrow_layer>>(
            b);
    }

    template <sequential_channel base_channel>
    auto make_pmrchannel(base_channel &&b)
    {
        return std::make_unique<
            pmrchannel_wrapper<base_channel, layer_ownership::own_layer>>(
            std::move(b));
    }

} // namespace ivy

#endif // IVY_IO_PMRCHANNEL_HXX_INCLUDED
