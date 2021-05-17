/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <array>
#include <string_view>

#include <catch2/catch.hpp>

#include <ivy/algorithm/bintext.hxx>
#include <ivy/crypto/pbkdf2.hxx>

struct test_vector {
    std::string_view password;
    std::string_view salt;
    std::size_t iterations;
    std::string_view derived_key;
};

TEST_CASE("ivy:pbkdf2:hmac_sha1", "[ivy][crypto][pbkdf2][hmac_sha1]")
{
    using namespace std::string_view_literals;

    std::array test_vectors{
        // clang-format off
        test_vector{"70617373776f7264"sv,
                    "73616c74"sv,
                    1,
                    "0c60c80f961f0e71f3a9b524af6012062fe037a6"sv},
        test_vector{
            "70617373776f7264"sv,
            "73616c74"sv,
            2,
            "ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957"sv},
        test_vector{
            "70617373776f7264"sv,
            "73616c74"sv,
            4096,
            "4b007901b765489abead49d926f721d065a429c1"sv},
        test_vector{
            "70617373776f7264"sv,
            "73616c74"sv,
            16777216,
            "eefe3d61cd4da4e4e9945b3d6ba2158c2634e984"sv},
        test_vector{
            "70617373776f726450415353574f524470617373776f7264"sv,
            "73616c7453414c5473616c7453414c5473616c7453414c5473616c7453414c5473616c74"sv,
            4096,
            "3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038"sv},
        test_vector{
            "7061737300776f7264"sv,
            "7361006c74"sv,
            4096,
            "56fa6aa75548099dcc37d7f03425e0c3"sv},
    };

    for (auto &&vec : test_vectors) {
        std::vector<std::uint8_t> password_bytes;
        auto r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.password, std::back_inserter(password_bytes));
        REQUIRE(r);

        std::vector<std::uint8_t> salt_bytes;
        r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.salt, std::back_inserter(salt_bytes));
        REQUIRE(r);

        std::vector<std::uint8_t> expected_derived_bytes;
        r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.derived_key, std::back_inserter(expected_derived_bytes));
        REQUIRE(r);

        std::vector<std::uint8_t> derived_bytes(expected_derived_bytes.size());

        auto ok = ivy::hash_pbkdf2(ivy::hash_algorithm::sha1,
                                   password_bytes,
                                   salt_bytes,
                                   vec.iterations,
                                   derived_bytes);
        if (!ok) {
            INFO(ok.error().message());
            REQUIRE(ok);
        }

        auto derived =
            ivy::bintext_encode_to_string<ivy::hexchars_lc>(derived_bytes);
        REQUIRE(derived == vec.derived_key);
    }
}
