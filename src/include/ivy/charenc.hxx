/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_HXX_INCLUDED
#define IVY_CHARENC_HXX_INCLUDED

#include <bit>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <span>

namespace ivy {

    class encoding_error : public std::runtime_error {
    public:
        encoding_error(std::string const &err) : std::runtime_error(err) {}
    };

    // clang-format off

	template<typename Encoding>
	concept character_encoding = requires {
		typename Encoding::char_type;
	}
	and requires(typename Encoding::char_type const *s) {
		{ Encoding::length(s) } -> std::convertible_to<std::size_t>;
	};

    // clang-format on

    struct charconv_options {
        std::endian endianness = std::endian::native;
    };

    template <typename source_encoding,
              typename target_encoding>
    class charconv;

    template<typename encoding>
    struct encoding_traits {
        using char_type = typename encoding::char_type;
    };

    template<>
    struct encoding_traits<std::byte> {
        using char_type = std::byte;
    };

    template<typename T>
    using encoding_char_type = typename encoding_traits<T>::char_type;

    template <character_encoding source_encoding>
    class charconv<source_encoding, std::byte> {
        charconv_options _options;

    public:
        charconv(charconv_options options = {}) noexcept
            : _options(options)
        {
        }

        template <std::ranges::input_range input_range,
                  std::output_iterator<std::byte> output_iterator>
        auto convert(input_range &&r, output_iterator &&out) -> void
        {
            for (auto &&b : r) {
                std::ranges::copy(as_bytes(std::span(&b, 1)), out);
            }
        }

        template <std::output_iterator<char32_t> output_iterator>
        auto flush(output_iterator &&) -> void
        {
        }
    };

} // namespace ivy

#endif // IVY_CHARENC_HXX_INCLUDED
