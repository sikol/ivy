/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_ICU_VERIFIER_HXX
#define IVY_CHARENC_ICU_VERIFIER_HXX

#include <cstddef>
#include <format>
#include <ranges>
#include <utility>
#include <vector>

#include <ivy/charenc.hxx>
#include <ivy/charenc/charconv.hxx>
#include <ivy/charenc/icu/ucnv.hxx>
#include <ivy/string_literal.hxx>

namespace ivy::icu {

    template <typename source_type>
    class verifier {
        ucnv_handle _ucnv;

    public:
        verifier(char const *name)
        {
            auto r = icu::make_ucnv(name, "utf-16", false);

            if (!r)
                throw encoding_error(
                    std::format("cannot open encoder for '{}': {}",
                                name,
                                r.error().what()));

            _ucnv = std::move(*r);
        }

        template <std::ranges::input_range input_range>
        requires std::same_as<source_type,
                              std::ranges::range_value_t<input_range>>
        auto verify(input_range &&r) -> void
        {
            static constexpr std::size_t bufsz = 1024 / sizeof(source_type);
            source_type inbuf[bufsz];
            std::vector<std::byte> output;

            auto begin = std::ranges::begin(r);
            auto end = std::ranges::end(r);

            while (begin < end) {
                std::size_t i;
                for (i = 0; i < bufsz && begin < end; ++i)
                    inbuf[i] = *begin++;

                auto inbytes = as_bytes(std::span(inbuf).subspan(0, i));
                auto ok = ucnv_convert_bytes(_ucnv, false, inbytes, output);

                if (!ok)
                    throw encoding_error(std::format(
                        "encoding verify failed: {}", ok.error().what()));
                output.clear();
            }
        }

        auto flush() -> void
        {
            std::vector<std::byte> output;
            std::byte dummy;
            auto r = ucnv_convert_bytes(_ucnv, true, std::span(&dummy, 0), output);

            if (!r)
                throw encoding_error(std::format("encoding verify failed: {}",
                                                 r.error().what()));
        }
    };

} // namespace ivy::icu

#endif // IVY_CHARENC_ICU_VERIFIER_HXX
