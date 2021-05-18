/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <string_view>

#include <catch2/catch.hpp>

#include <ivy/algorithm/bintext.hxx>
#include <ivy/string/u32string.hxx>
#include <ivy/string/u16string.hxx>
#include <ivy/string/u8string.hxx>

TEST_CASE("ivy:u8string", "[ivy][string][charenc][utf8]")
{
    using namespace std::string_view_literals;

    struct test_vector {
        std::u8string_view utf8_s;
        std::u32string_view utf32_s;
    };

    std::array test_vectors{
        test_vector{u8"normal ASCII text"sv, U"normal ASCII text"sv},
        test_vector{u8"\xCE\xBA\xE1\xBD\xB9\xCF\x83\xCE\xBC\xCE\xB5"sv,
                    U"\u03BA\u1F79\u03C3\u03BC\u03B5"sv},

        // 2-byte characters
        test_vector{u8"\u00A0"sv, U"\u00A0"sv},
        test_vector{u8"\u00A0\u00A0"sv, U"\u00A0\u00A0"sv},
        test_vector{u8"test \u00A0\u00A0test"sv, U"test \u00A0\u00A0test"sv},
        test_vector{u8"test\u00A0test \u00A0"sv, U"test\u00A0test \u00A0"sv},

        // 3-byte characters
        test_vector{u8"\u0830"sv, U"\u0830"sv},

        // 4-byte characters
        test_vector{u8"\U00010080"sv, U"\U00010080"sv},

        // Edge cases
        test_vector{u8"\0"sv, U"\u0000"sv},
        test_vector{u8"\x7f"sv, U"\u007f"sv},
        test_vector{u8"\xc2\x80"sv, U"\u0080"sv},
        test_vector{u8"\xdf\xbf"sv, U"\u07ff"sv},
        test_vector{u8"\xe0\xa0\x80"sv, U"\u0800"sv},
        test_vector{u8"\xef\xbf\xbf"sv, U"\uffff"sv},
        test_vector{u8"\xf0\x90\x80\x80"sv, U"\U00010000"sv},
        test_vector{u8"\xf4\x8f\xbf\xbf"sv, U"\U0010FFFF"sv},
        test_vector{u8"\xed\x9f\xbf"sv, U"\U0000D7FF"sv},
        test_vector{u8"\xee\x80\x80"sv, U"\U0000E000"sv},
        test_vector{u8"\xef\xbf\xbd"sv, U"\U0000FFFD"sv},
    };

    for (auto &&vec : test_vectors) {
        auto u8str = ivy::u8string(vec.utf8_s.data(), vec.utf8_s.size());
        auto u32str = ivy::u32string(vec.utf32_s.data(), vec.utf32_s.size());

        auto hexchars = ivy::bintext_encode_to_string<ivy::hexchars_lc>(u8str);
        INFO(hexchars);

        auto xcoded = ivy::transcode<ivy::u32string>(u8str);
        REQUIRE(xcoded);
        REQUIRE(*xcoded == u32str);

        auto recoded = ivy::transcode<ivy::u8string>(*xcoded);
        REQUIRE(recoded);
        REQUIRE(*recoded == u8str);
    }

    std::array invalid_utf8_sequences{
        // Truncated string
        u8"\xC2"sv,

        // Continuation byte with no start byte
        u8"\x80x"sv,
        u8"\x82x"sv,
        u8"\xbfx"sv,

        // Missing continuation byte
        u8"\xC2x"sv,
        u8"\xC0"sv,
        u8"\xE0\x80"sv,
        u8"\xF0\x80\x80"sv,

        // Invalid sequences
        u8"\xfe"sv,
        u8"\xff"sv,
        u8"\xfe\xfe\xff\xff"sv,

        // Invalid characters
        u8"\xed\xa0\x80"sv,
        u8"\xed\xad\xbf"sv,
        u8"\xed\xae\x80"sv,
        u8"\xed\xaf\xbf"sv,
        u8"\xed\xb0\x80"sv,
        u8"\xed\xbe\x80"sv,
        u8"\xed\xbf\xbf"sv,

        // UTF-16 surrogate pairs
        u8"\xed\xa0\x80\xed\xb0\x80"sv,
        u8"\xed\xa0\x80\xed\xbf\xbf"sv,
        u8"\xed\xad\xbf\xed\xb0\x80"sv,
        u8"\xed\xad\xbf\xed\xbf\xbf"sv,
        u8"\xed\xae\x80\xed\xb0\x80"sv,
        u8"\xed\xae\x80\xed\xbf\xbf"sv,
        u8"\xed\xaf\xbf\xed\xb0\x80"sv,
        u8"\xed\xaf\xbf\xed\xbf\xbf"sv,

        // Overlong encoding
        u8"\xC0\x80\x20"sv,
        u8"\xF0\x82\x82\xAC"sv,
        u8"\xc0\xaf"sv,
        u8"\xe0\x80\xaf"sv,
        u8"\xf0\x80\x80\xaf"sv,
        u8"\xf8\x80\x80\x80\xaf"sv,
        u8"\xfc\x80\x80\x80\x80\xaf"sv,
        u8"\xc0\x80"sv,
        u8"\xe0\x80\x80"sv,
        u8"\xf0\x80\x80\x80"sv,
        u8"\xf8\x80\x80\x80\x80"sv,
        u8"\xfc\x80\x80\x80\x80\x80"sv,
    };

    for (auto &&data : invalid_utf8_sequences) {
        auto hexchars = ivy::bintext_encode_to_string<ivy::hexchars_lc>(data);
        INFO(hexchars);

        auto r = ivy::utf8::validate(data);
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
