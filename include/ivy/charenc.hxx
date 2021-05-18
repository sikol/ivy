/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_HXX_INCLUDED
#define IVY_CHARENC_HXX_INCLUDED

#include <cstddef>
#include <type_traits>

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

}

#endif // IVY_CHARENC_HXX_INCLUDED
