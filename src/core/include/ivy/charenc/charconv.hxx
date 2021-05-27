/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_CHARCONV_HXX_INCLUDED
#define IVY_CHARENC_CHARCONV_HXX_INCLUDED

#include <bit>
#include <cstddef>

#include <ivy/byte.hxx>
#include <ivy/charenc.hxx>
#include <ivy/charenc/icu/bytereader.hxx>
#include <ivy/charenc/icu/encoding_traits.hxx>
#include <ivy/charenc/icu/ucnv_charconv.hxx>

namespace ivy {

    struct charconv_options {
        std::endian endianness = std::endian::native;
        bool accept_invalid = false;
    };

    template <typename source_encoding, typename target_encoding>
    class charconv
        : public icu::ucnv_charconv<typename source_encoding::char_type,
                                    typename target_encoding::char_type> {
    public:
        charconv(charconv_options options = {})
            : icu::ucnv_charconv<typename source_encoding::char_type,
                                 typename target_encoding::char_type>(
                  icu::encoding_traits<source_encoding>::name(),
                  icu::encoding_traits<target_encoding>::name(),
                  options.accept_invalid)
        {
        }
    };

    template <typename target_encoding>
    class charconv<std::byte, target_encoding> {
        charconv_options _options;
        icu::bytereader<typename target_encoding::char_type> _bytereader;

    public:
        charconv(charconv_options options = {})
            : _bytereader(options.endianness,
                          icu::encoding_traits<target_encoding>::name())
        {
        }

        template <std::ranges::input_range input_range,
                  std::output_iterator<typename target_encoding::char_type>
                      output_iterator>
        requires std::same_as<std::byte,
                              std::ranges::range_value_t<input_range>>
        auto convert(input_range &&r, output_iterator out) -> void
        {
            _bytereader.convert_bytes(r, out);
        }

        template <std::output_iterator<typename target_encoding::char_type>
                      output_iterator>
        auto flush(output_iterator out) -> void
        {
            _bytereader.flush(out);
        }
    };

    template <typename source_encoding>
    class charconv<source_encoding, std::byte> {
        charconv_options _options;

    public:
        charconv(charconv_options options = {})
            : _options(options)
        {
        }

        template <std::ranges::input_range input_range,
                  std::output_iterator<std::byte> output_iterator>
        requires std::same_as<typename source_encoding::char_type,
                              std::ranges::range_value_t<input_range>>
        auto convert(input_range &&r, output_iterator out) -> void
        {
            write_bytes(_options.endianness, r, out);
        }

        template <std::output_iterator<std::byte> output_iterator>
        auto flush(output_iterator) -> void
        {
        }
    };

} // namespace ivy

#endif // IVY_CHARENC_CHARCONV_HXX_INCLUDED
