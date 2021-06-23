/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_IO_TRANSCODECHANNEL_HXX_INCLUDED
#define IVY_IO_TRANSCODECHANNEL_HXX_INCLUDED

#include <deque>
#include <span>
#include <system_error>

#include <ivy/charenc.hxx>
#include <ivy/expected.hxx>
#include <ivy/io/channel.hxx>
#include <ivy/string.hxx>

namespace ivy {

    template <typename source_encoding,
              typename target_encoding,
              sequential_channel base_channel,
              layer_ownership ownership>
    class itranscodechannel final : layer_base<base_channel, ownership> {
        std::deque<encoding_char_type<target_encoding>> _buffer;
        charconv<source_encoding, target_encoding> _charconv;

    public:
        using value_type = encoding_char_type<target_encoding>;
        using encoding_type = target_encoding;

        itranscodechannel(itranscodechannel &&) noexcept = default;
        auto operator=(itranscodechannel &&) noexcept
            -> itranscodechannel & = default;

        itranscodechannel(itranscodechannel const &) = delete;
        auto operator=(itranscodechannel const &)
            -> itranscodechannel & = delete;

        itranscodechannel(base_channel &c) requires(
            ownership == layer_ownership::borrow_layer)
            : layer_base<base_channel, ownership>(c)
        {
        }

        itranscodechannel(base_channel &&c) requires(ownership ==
                                                     layer_ownership::own_layer)
            : layer_base<base_channel, ownership>(std::move(c))
        {
        }

        auto read(std::span<value_type>) noexcept -> expected<io_size_t, error>;
    };

    template <typename source_encoding,
              typename target_encoding,
              sequential_input_channel base_channel>
    auto make_itranscodechannel(base_channel &b)
    {
        return itranscodechannel<source_encoding,
                                 target_encoding,
                                 base_channel,
                                 layer_ownership::borrow_layer>(b);
    }

    template <typename source_encoding,
              typename target_encoding,
              sequential_input_channel base_channel>
    auto make_itranscodechannel(base_channel &&b)
    {
        return itranscodechannel<source_encoding,
                                 target_encoding,
                                 base_channel,
                                 layer_ownership::own_layer>(std::move(b));
    }

    template <typename source_encoding,
              typename target_encoding,
              sequential_channel base_channel,
              layer_ownership ownership>
    auto itranscodechannel<source_encoding,
                           target_encoding,
                           base_channel,
                           ownership>::read(std::span<value_type> buf) noexcept
        -> expected<io_size_t, error>
    {
        if (_buffer.empty()) {
            typename encoding_traits<source_encoding>::char_type
                cbuf[1024 /
                     sizeof(
                         typename encoding_traits<source_encoding>::char_type)];

            auto r = this->get_base_layer().read(cbuf);
            if (!r)
                return make_unexpected(r.error());

            auto cdata = std::span(cbuf);
            _charconv.convert(cdata.subspan(0, *r),
                              std::back_inserter(_buffer));
        }

        auto can_read = std::min(_buffer.size(), buf.size());
        std::ranges::copy(_buffer | std::views::take(can_read),
                          std::ranges::begin(buf));
        _buffer.erase(_buffer.begin(), std::next(_buffer.begin(), can_read));
        return can_read;
    }

} // namespace ivy

#endif // IVY_IO_TRANSCODECHANNEL_HXX_INCLUDED
