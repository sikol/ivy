/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_UTF32_HXX_INCLUDED
#define IVY_CHARENC_UTF32_HXX_INCLUDED

#include <deque>
#include <iterator>
#include <ranges>
#include <string>

#include <ivy/charenc/error.hxx>
#include <ivy/charenc.hxx>
#include <ivy/expected.hxx>
#include <ivy/iterator.hxx>

namespace ivy {

    struct utf32_encoding {
        using char_type = char32_t;

        static auto length(char_type const *s) -> std::size_t
        {
            return std::char_traits<char32_t>::length(s);
        }
    };

    template <character_encoding target_encoding>
    class charconv<std::byte, target_encoding> {
        charconv<target_encoding, utf32_encoding> _charconv;
        charconv_options _options;
        std::uint8_t _buffer[sizeof(typename target_encoding::char_type)];
        unsigned _bufp = 0;

    public:
        charconv(charconv_options options) noexcept : _options(options) {}

        template <std::ranges::input_range input_range,
                  std::output_iterator<char32_t> output_iterator>
        auto convert(input_range &&r, output_iterator &&out) -> void
        {
            null_output_iterator nout;

            for (auto &&b : r) {
                _buffer[_bufp++] = std::to_integer<std::uint8_t>(b);

                if (_bufp < sizeof(_buffer))
                    continue;

                typename target_encoding::char_type c{};
                for (unsigned i = 0; i < sizeof(_buffer); ++i) {
                    if (_options.endianness == std::endian::big)
                        c |= static_cast<typename target_encoding::char_type>(
                                 _buffer[i])
                             << (8 * ((sizeof(_buffer) - 1) - i));
                    else
                        c |= static_cast<typename target_encoding::char_type>(
                                 _buffer[i])
                             << (8 * i);
                }

                _charconv.convert(std::span(&c, 1), nout);
                *out++ = c;
                _bufp = 0;
            }
        }

        template <std::output_iterator<char32_t> output_iterator>
        auto flush(output_iterator &&) -> void
        {
            if (_bufp)
                throw encoding_error("invalid encoding");

            null_output_iterator nout;
            _charconv.flush(nout);
        }
    };

} // namespace ivy

#endif // IVY_CHARENC_UTF32_HXX_INCLUDED
