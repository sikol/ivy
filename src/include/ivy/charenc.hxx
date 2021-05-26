/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_HXX_INCLUDED
#define IVY_CHARENC_HXX_INCLUDED

#include <bit>
#include <cstddef>
#include <span>
#include <stdexcept>
#include <type_traits>

#include <ivy/string_literal.hxx>

namespace ivy {

    // clang-format off

	template<typename Encoding>
	concept character_encoding = requires {
		typename Encoding::char_type;
	}
	and requires(typename Encoding::char_type const *s) {
		{ Encoding::length(s) } -> std::convertible_to<std::size_t>;
	};

    // clang-format on

    template <typename encoding>
    struct encoding_traits {
        using char_type = typename encoding::char_type;
    };

    template <>
    struct encoding_traits<std::byte> {
        using char_type = std::byte;
    };

    template <typename T>
    using encoding_char_type = typename encoding_traits<T>::char_type;

} // namespace ivy

#endif // IVY_CHARENC_HXX_INCLUDED
