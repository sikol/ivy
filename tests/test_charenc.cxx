/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <string_view>

#include <catch2/catch.hpp>

#include <ivy/algorithm/bintext.hxx>
#include <ivy/string.hxx>

TEST_CASE("ivy:u8string", "[ivy][string][charenc][utf8]")
{
    using namespace std::string_view_literals;

    struct test_vector {
        std::string_view utf8_bytes;
        std::u32string_view utf32_s;
    };

    std::array test_vectors{
        test_vector{"normal ASCII text"sv, U"normal ASCII text"sv},
        test_vector{"\xCE\xBA\xE1\xBD\xB9\xCF\x83\xCE\xBC\xCE\xB5"sv,
                    U"\u03BA\u1F79\u03C3\u03BC\u03B5"sv},

        // 2-byte characters
        test_vector{"\xc2\xa0"sv, U"\u00A0"sv},
        test_vector{"\xc2\xa0\xc2\xa0"sv, U"\u00A0\u00A0"sv},
        test_vector{"test \xc2\xa0\xc2\xa0test"sv, U"test \u00A0\u00A0test"sv},
        test_vector{"test\xc2\xa0test \xc2\xa0"sv, U"test\u00A0test \u00A0"sv},

        // 3-byte characters
        test_vector{"\xe0\xa0\xb0"sv, U"\u0830"sv},

        // 4-byte characters
        test_vector{"\xF0\x90\x82\x80"sv, U"\U00010080"sv},

        // Edge cases
        test_vector{"\0"sv, U"\u0000"sv},
        test_vector{"\x7f"sv, U"\u007f"sv},
        test_vector{"\xc2\x80"sv, U"\u0080"sv},
        test_vector{"\xdf\xbf"sv, U"\u07ff"sv},
        test_vector{"\xe0\xa0\x80"sv, U"\u0800"sv},
        test_vector{"\xef\xbf\xbf"sv, U"\uffff"sv},
        test_vector{"\xf0\x90\x80\x80"sv, U"\U00010000"sv},
        test_vector{"\xf4\x8f\xbf\xbf"sv, U"\U0010FFFF"sv},
        test_vector{"\xed\x9f\xbf"sv, U"\U0000D7FF"sv},
        test_vector{"\xee\x80\x80"sv, U"\U0000E000"sv},
        test_vector{"\xef\xbf\xbd"sv, U"\U0000FFFD"sv},
    };

    for (auto &&vec : test_vectors) {
        auto u8str = ivy::bytes_to_string<ivy::u8string>(
            as_bytes(std::span(vec.utf8_bytes)));
        REQUIRE(u8str);

        auto u32str = ivy::u32string(vec.utf32_s.data(), vec.utf32_s.size());

        auto hexchars = ivy::bintext_encode_to_string<ivy::hexchars_lc>(*u8str);
        INFO(hexchars);

        auto xcoded = ivy::transcode<ivy::u32string>(*u8str);
        REQUIRE(xcoded);
        REQUIRE(*xcoded == u32str);

        auto recoded = ivy::transcode<ivy::u8string>(*xcoded);
        REQUIRE(recoded);
        REQUIRE(*recoded == *u8str);
    }

    std::array invalid_utf8_sequences{
        // Truncated string
        "\xC2"sv,

        // Continuation byte with no start byte
        "\x80x"sv,
        "\x82x"sv,
        "\xbfx"sv,

        // Missing continuation byte
        "\xC2x"sv,
        "\xC0"sv,
        "\xE0\x80"sv,
        "\xF0\x80\x80"sv,

        // Invalid sequences
        "\xfe"sv,
        "\xff"sv,
        "\xfe\xfe\xff\xff"sv,

        // Invalid characters
        "\xed\xa0\x80"sv,
        "\xed\xad\xbf"sv,
        "\xed\xae\x80"sv,
        "\xed\xaf\xbf"sv,
        "\xed\xb0\x80"sv,
        "\xed\xbe\x80"sv,
        "\xed\xbf\xbf"sv,

        // UTF-16 surrogate pairs
        "\xed\xa0\x80\xed\xb0\x80"sv,
        "\xed\xa0\x80\xed\xbf\xbf"sv,
        "\xed\xad\xbf\xed\xb0\x80"sv,
        "\xed\xad\xbf\xed\xbf\xbf"sv,
        "\xed\xae\x80\xed\xb0\x80"sv,
        "\xed\xae\x80\xed\xbf\xbf"sv,
        "\xed\xaf\xbf\xed\xb0\x80"sv,
        "\xed\xaf\xbf\xed\xbf\xbf"sv,

        // Overlong encoding
        "\xC0\x80\x20"sv,
        "\xF0\x82\x82\xAC"sv,
        "\xc0\xaf"sv,
        "\xe0\x80\xaf"sv,
        "\xf0\x80\x80\xaf"sv,
        "\xf8\x80\x80\x80\xaf"sv,
        "\xfc\x80\x80\x80\x80\xaf"sv,
        "\xc0\x80"sv,
        "\xe0\x80\x80"sv,
        "\xf0\x80\x80\x80"sv,
        "\xf8\x80\x80\x80\x80"sv,
        "\xfc\x80\x80\x80\x80\x80"sv,
    };

    for (auto &&data : invalid_utf8_sequences) {
        auto hexchars = ivy::bintext_encode_to_string<ivy::hexchars_lc>(data);
        INFO(hexchars);

        std::vector<char8_t> chars(data.size());
        std::memcpy(&chars[0], data.data(), data.size());

        auto r = ivy::utf8_encoding::validate(chars);
        REQUIRE(!r);

        auto bytes = as_bytes(std::span(data));
        auto s = ivy::bytes_to_string<ivy::u8string>(bytes);
        REQUIRE(!s);
    }
}

TEST_CASE("ivy:u16string", "[ivy][string][charenc][utf16]")
{
    using namespace std::string_view_literals;

    struct test_vector {
        std::string_view bytes;
        std::u32string_view utf32_s;
    };

    std::array test_vectors_be{
        test_vector{"\x00\x24"sv, U"\u0024"sv},
        test_vector{"\x20\xac"sv, U"\u20ac"sv},
        test_vector{"\xd8\x01\xdc\x37", U"\U00010437"sv},
        test_vector{"\xd8\x52\xdf\x62", U"\U00024B62"sv},
    };

    for (auto &&vec : test_vectors_be) {
        auto bytes = as_bytes(std::span(vec.bytes));

        auto hexchars = ivy::bintext_encode_to_string<ivy::hexchars_lc>(bytes);
        INFO(hexchars);

        auto u16str =
            ivy::bytes_to_string<ivy::u16string>(bytes, std::endian::big);
        REQUIRE(u16str);

        auto u32str = ivy::u32string(vec.utf32_s.data(), vec.utf32_s.size());


        auto xcoded = ivy::transcode<ivy::u32string>(*u16str);
        REQUIRE(xcoded);
        REQUIRE(*xcoded == u32str);

        auto recoded = ivy::transcode<ivy::u16string>(*xcoded);
        REQUIRE(recoded);
        REQUIRE(*recoded == *u16str);
    }

    std::array invalid_utf16_sequences{
        "\xd8\x01"sv,
        "\xd8\x01x"sv,
    };

    for (auto &&data : invalid_utf16_sequences) {
        auto hexchars = ivy::bintext_encode_to_string<ivy::hexchars_lc>(data);
        INFO(hexchars);

        auto bytes = as_bytes(std::span(data));
        auto s = ivy::bytes_to_string<ivy::u16string>(bytes, std::endian::big);
        REQUIRE(!s);
    }
}
