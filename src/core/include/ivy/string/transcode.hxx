/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_STRING_TRANSCODE_HXX_INCLUDED
#define IVY_STRING_TRANSCODE_HXX_INCLUDED

#include <ivy/string.hxx>

namespace ivy {

    template <typename target_string,
              character_encoding source_encoding,
              typename allocator>
    auto transcode(basic_string<source_encoding, allocator> const &s)
        -> expected<target_string, error>
    {
        using target_encoding = typename target_string::encoding_type;
        using new_allocator =
            typename std::allocator_traits<allocator>::template rebind_alloc<
                typename target_encoding::char_type>;
        std::vector<typename target_encoding::char_type, new_allocator> chars;

        charconv<source_encoding, target_encoding> cc;

        try {
            cc.convert(s, std::back_inserter(chars));
            cc.flush(std::back_inserter(chars));
            return target_string(chars.begin(), chars.end());
        } catch (encoding_error const &) {
            return make_unexpected(make_error(errc::invalid_encoding));
        }
    }

    template <typename target_string, typename traits_type, typename allocator>
    auto transcode(std::basic_string<char, traits_type, allocator> const &s)
        -> expected<target_string, error>
    {
        using target_encoding = typename target_string::encoding_type;
        using new_allocator =
            typename std::allocator_traits<allocator>::template rebind_alloc<
                typename target_encoding::char_type>;
        std::vector<typename target_encoding::char_type, new_allocator> chars;

        charconv<system_encoding, target_encoding> cc;

        try {
            cc.convert(s, std::back_inserter(chars));
            cc.flush(std::back_inserter(chars));
            return target_string(chars.begin(), chars.end());
        } catch (encoding_error const &) {
            return make_unexpected(make_error(errc::invalid_encoding));
        }
    }

    template <typename target_string, std::ranges::range input_range>
    auto bytes_to_string(input_range &&r,
                         std::endian endianness = std::endian::native)
        -> expected<target_string, error>
    requires std::same_as<std::byte, std::ranges::range_value_t<input_range>>
    {
        using encoding = typename target_string::encoding_type;

        std::vector<typename encoding::char_type> chars;
        charconv<std::byte, encoding> tx({.endianness = endianness});

        try {
            tx.convert(r, std::back_inserter(chars));
            tx.flush(std::back_inserter(chars));
            return target_string(chars.begin(), chars.end());
        } catch (encoding_error const &e) {
            return make_unexpected(make_error<encoding_error>(e));
        }
    }

} // namespace ivy

#endif // IVY_STRING_TRANSCODE_HXX_INCLUDED
