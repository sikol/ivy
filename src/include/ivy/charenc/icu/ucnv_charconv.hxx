/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_ICU_UCNV_CHARCONV_HXX_INCLUDED
#define IVY_CHARENC_ICU_UCNV_CHARCONV_HXX_INCLUDED

#include <concepts>

#include <ivy/charenc/error.hxx>
#include <ivy/charenc/icu/ucnv.hxx>
#include <ivy/iterator/byte_decode.hxx>
#include <ivy/string_literal.hxx>

namespace ivy::icu {

    template <std::integral source_type, std::integral target_type>
    class ucnv_charconv {
        ucnv_handle _ucnv;

    public:
        ucnv_charconv(char const *source, char const *target, bool accept_invalid)
        {
            auto r = icu::make_ucnv(source, target, accept_invalid);

            if (!r)
                throw encoding_error(
                    std::format("cannot open encoder from '{}' to '{}': {}",
                                source,
                                target,
                                r.error().what()));

            _ucnv = std::move(*r);
        }

        template <std::ranges::input_range input_range,
                  std::output_iterator<target_type> output_iterator>
        requires std::same_as<source_type,
                              std::ranges::range_value_t<input_range>>
        auto convert(input_range &&r, output_iterator out) -> void
        {
            static constexpr std::size_t bufsz = 1024 / sizeof(source_type);
            source_type inbuf[bufsz];
            std::vector<std::byte> output;

            auto begin = std::ranges::begin(r);
            auto end = std::ranges::end(r);

            IVY_TRACE("convert: begin={}, end={}, len={}",
                      static_cast<void const *>(&*begin),
                      static_cast<void const *>(&*end),
                      std::ranges::size(r));

            auto bout = make_byte_decode_iterator<target_type>(
                std::endian::native, out);

            while (begin != end) {
                std::size_t i;
                for (i = 0; i < bufsz && begin != end; ++i)
                    inbuf[i] = *begin++;

                auto inbytes = as_bytes(std::span(&inbuf[0], i));
                auto ok = ucnv_convert_bytes(_ucnv, false, inbytes, output);

                if (!ok)
                    throw encoding_error(
                        std::format("conversion failed: {}", ok.error().what()));

                std::ranges::copy(output, bout);
                output.clear();
            }
        }

        template <std::output_iterator<target_type> output_iterator>
        auto flush(output_iterator out) -> void
        {
            std::vector<std::byte> output;
            std::byte dummy;
            auto r =
                ucnv_convert_bytes(_ucnv, true, std::span(&dummy, 0), output);

            if (!r)
                throw encoding_error(
                    std::format("conversion failed: {}", r.error().what()));

            std::ranges::copy(output,
                              make_byte_decode_iterator<target_type>(
                                  std::endian::native, out));
        }
    };

} // namespace ivy::icu

#endif // IVY_CHARENC_ICU_UCNV_CHARCONV_HXX_INCLUDED
