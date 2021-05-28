/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_STRING_TRANSCODE_HXX_INCLUDED
#define IVY_STRING_TRANSCODE_HXX_INCLUDED

#include <ivy/string.hxx>

namespace ivy {

    template <typename To, typename From>
    struct string_transcoder;

    template <character_encoding target_encoding,
              character_encoding source_encoding,
              typename source_allocator,
              typename target_allocator>
    struct string_transcoder<basic_string<target_encoding, target_allocator>,
                             basic_string<source_encoding, source_allocator>> {

        static auto transcode(
            basic_string<source_encoding, source_allocator> const &s) noexcept
            -> expected<basic_string<target_encoding, target_allocator>, error>
        try {
            std::vector<typename target_encoding::char_type, target_allocator>
                chars;

            charconv<source_encoding, target_encoding> cc;

            cc.convert(s, std::back_inserter(chars));
            cc.flush(std::back_inserter(chars));
            return basic_string<target_encoding, target_allocator>(
                chars.begin(), chars.end());
        } catch (...) {
            return make_unexpected(make_error(std::current_exception()));
        }
    };

    template <character_encoding target_encoding,
              typename target_allocator,
              typename source_traits,
              typename source_allocator>
    struct string_transcoder<
        basic_string<target_encoding, target_allocator>,
        std::basic_string<char, source_traits, source_allocator>> {

        static auto transcode(
            std::basic_string<char, source_traits, source_allocator> const &s)
            -> expected<basic_string<target_encoding, target_allocator>, error>
        try {
            std::vector<typename target_encoding::char_type, target_allocator>
                chars;

            charconv<system_encoding, target_encoding> cc;

            cc.convert(s, std::back_inserter(chars));
            cc.flush(std::back_inserter(chars));
            return basic_string<target_encoding, target_allocator>(
                chars.begin(), chars.end());
        } catch (...) {
            return make_unexpected(make_error(std::current_exception()));
        }
    };

    template <character_encoding source_encoding,
              typename source_allocator,
              typename target_traits,
              typename target_allocator>
    struct string_transcoder<
        std::basic_string<char, target_traits, target_allocator>,
        basic_string<source_encoding, source_allocator>> {

        static auto
        transcode(basic_string<source_encoding, source_allocator> const &s)
            -> expected<
                std::basic_string<char, target_traits, target_allocator>,
                error>
        try {
            std::vector<system_encoding::char_type, target_allocator> chars;

            charconv<source_encoding, system_encoding> cc;

            cc.convert(s, std::back_inserter(chars));
            cc.flush(std::back_inserter(chars));
            return std::basic_string<char, target_traits, target_allocator>(
                chars.begin(), chars.end());
        } catch (...) {
            return make_unexpected(make_error(std::current_exception()));
        }
    };

    template <typename To, typename From>
    auto transcode(From const &s) noexcept -> expected<To, error>
    {
        return string_transcoder<To, From>::transcode(s);
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
