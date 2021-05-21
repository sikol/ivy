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
              character_encoding target_encoding>
    class charconv;

} // namespace ivy

#endif // IVY_CHARENC_HXX_INCLUDED
