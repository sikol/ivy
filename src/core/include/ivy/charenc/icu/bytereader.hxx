/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_ICU_BYTEREADER_HXX_INCLUDED
#define IVY_CHARENC_ICU_BYTEREADER_HXX_INCLUDED

#include <bit>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <ranges>

#include <ivy/charenc.hxx>
#include <ivy/charenc/error.hxx>
#include <ivy/charenc/icu/verifier.hxx>

namespace ivy::icu {

    template <typename target_type>
    class bytereader {
        std::endian _endianness;
        verifier<target_type> _verifier;
        std::byte _buffer[sizeof(target_type)];
        unsigned _bufp = 0;

    public:
        bytereader(std::endian endianness, char const *name) noexcept
            : _endianness(endianness)
            , _verifier(name)
        {
        }

        template <std::ranges::input_range input_range,
                  std::output_iterator<target_type> output_iterator>
        requires std::same_as<std::byte,
                              std::ranges::range_value_t<input_range>>
        auto convert_bytes(input_range &&r, output_iterator out) -> void
        {
            for (auto &&b : r) {
                _buffer[_bufp++] = b;

                if (_bufp < sizeof(_buffer))
                    continue;

                target_type c{};

                for (unsigned i = 0; i < sizeof(_buffer); ++i) {
                    if (_endianness == std::endian::big)
                        c |= to_integer<target_type>(_buffer[i])
                             << (8 * ((sizeof(_buffer) - 1) - i));
                    else
                        c |= to_integer<target_type>(_buffer[i]) << (8 * i);
                }

                _verifier.verify(std::span(&c, 1));
                *out++ = c;
                _bufp = 0;
            }
        }

        template <std::output_iterator<target_type> output_iterator>
        auto flush(output_iterator) -> void
        {
            if (_bufp)
                throw encoding_error("invalid encoding");

            _verifier.flush();
        }
    };

} // namespace ivy::icu

#endif // IVY_CHARENC_ICU_BYTEREADER_HXX_INCLUDED
