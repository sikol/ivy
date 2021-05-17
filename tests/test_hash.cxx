/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <string_view>
#include <vector>

#include <catch2/catch.hpp>

#include <ivy/algorithm/bintext.hxx>
#include <ivy/crypto/hash.hxx>

auto hash_string(ivy::hash_algorithm algorithm, std::string_view data)
    -> ivy::expected<std::string, std::error_code>
{
    std::vector<std::byte> digest;
    auto r = ivy::hash_data(algorithm, data, std::back_inserter(digest));
    if (!r)
        return ivy::make_unexpected(r.error());

    return ivy::bintext_encode_to_string<ivy::hexchars_lc>(digest);
}

struct test_vector {
    std::string_view message;
    std::string_view digest;
};

struct hmac_test_vector {
    std::string_view message;
    std::string_view key;
    std::string_view digest;
};

/*
 * Test vectors from
 * https://csrc.nist.gov/projects/cryptographic-algorithm-validation-program/secure-hashing.
 */

TEST_CASE("ivy:hash:sha1", "[ivy][hash][sha1]")
{
    using namespace std::string_view_literals;

    REQUIRE(ivy::hash_available(ivy::hash_algorithm::sha1));

    std::array test_vectors{
        test_vector{""sv, "da39a3ee5e6b4b0d3255bfef95601890afd80709"sv},
        test_vector{"36"sv, "c1dfd96eea8cc2b62785275bca38ac261256e278"sv},
        test_vector{"195a"sv, "0a1c2d555bbe431ad6288af5a54f93e0449c9232"sv},
        test_vector{"df4bd2"sv, "bf36ed5d74727dfd5d7854ec6b1d49468d8ee8aa"sv},
        test_vector{"549e959e"sv, "b78bae6d14338ffccfd5d5b5674a275f6ef9c717"sv},
        test_vector{"f7fb1be205"sv,
                    "60b7d5bb560a1acf6fa45721bd0abb419a841a89"sv},
        test_vector{"c0e5abeaea63"sv,
                    "a6d338459780c08363090fd8fc7d28dc80e8e01f"sv},
        test_vector{"63bfc1ed7f78ab"sv,
                    "860328d80509500c1783169ebf0ba0c4b94da5e5"sv},
        test_vector{"7e3d7b3eada98866"sv,
                    "24a2c34b976305277ce58c2f42d5092031572520"sv},
        test_vector{"9e61e55d9ed37b1c20"sv,
                    "411ccee1f6e3677df12698411eb09d3ff580af97"sv},
        test_vector{"9777cf90dd7c7e863506"sv,
                    "05c915b5ed4e4c4afffc202961f3174371e90b5c"sv},
        test_vector{"4eb08c9e683c94bea00dfa"sv,
                    "af320b42d7785ca6c8dd220463be23a2d2cb5afc"sv},
        test_vector{"0938f2e2ebb64f8af8bbfc91"sv,
                    "9f4e66b6ceea40dcf4b9166c28f1c88474141da9"sv},
        test_vector{"74c9996d14e87d3e6cbea7029d"sv,
                    "e6c4363c0852951991057f40de27ec0890466f01"sv},
        test_vector{"51dca5c0f8e5d49596f32d3eb874"sv,
                    "046a7b396c01379a684a894558779b07d8c7da20"sv},
        test_vector{"3a36ea49684820a2adc7fc4175ba78"sv,
                    "d58a262ee7b6577c07228e71ae9b3e04c8abcda9"sv},
        test_vector{"3552694cdf663fd94b224747ac406aaf"sv,
                    "a150de927454202d94e656de4c7c0ca691de955d"sv},
        test_vector{"f216a1cbde2446b1edf41e93481d33e2ed"sv,
                    "35a4b39fef560e7ea61246676e1b7e13d587be30"sv},
        test_vector{"a3cf714bf112647e727e8cfd46499acd35a6"sv,
                    "7ce69b1acdce52ea7dbd382531fa1a83df13cae7"sv},
        test_vector{"148de640f3c11591a6f8c5c48632c5fb79d3b7"sv,
                    "b47be2c64124fa9a124a887af9551a74354ca411"sv},
        test_vector{"63a3cc83fd1ec1b6680e9974a0514e1a9ecebb6a"sv,
                    "8bb8c0d815a9c68a1d2910f39d942603d807fbcc"sv},
        test_vector{"875a90909a8afc92fb7070047e9d081ec92f3d08b8"sv,
                    "b486f87fb833ebf0328393128646a6f6e660fcb1"sv},
        test_vector{"444b25f9c9259dc217772cc4478c44b6feff62353673"sv,
                    "76159368f99dece30aadcfb9b7b41dab33688858"sv},
        test_vector{"487351c8a5f440e4d03386483d5fe7bb669d41adcbfdb7"sv,
                    "dbc1cb575ce6aeb9dc4ebf0f843ba8aeb1451e89"sv},
        test_vector{"46b061ef132b87f6d3b0ee2462f67d910977da20aed13705"sv,
                    "d7a98289679005eb930ab75efd8f650f991ee952"sv},
        test_vector{"3842b6137bb9d27f3ca5bafe5bbb62858344fe4ba5c41589a5"sv,
                    "fda26fa9b4874ab701ed0bb64d134f89b9c4cc50"sv},
        test_vector{"44d91d3d465a4111462ba0c7ec223da6735f4f5200453cf132c3"sv,
                    "c2ff7ccde143c8f0601f6974b1903eb8d5741b6e"sv},
        test_vector{"cce73f2eabcb52f785d5a6df63c0a105f34a91ca237fe534ee399d"sv,
                    "643c9dc20a929608f6caa9709d843ca6fa7a76f4"sv},
        test_vector{
            "664e6e7946839203037a65a12174b244de8cbc6ec3f578967a84f9ce"sv,
            "509ef787343d5b5a269229b961b96241864a3d74"sv},
        test_vector{
            "9597f714b2e45e3399a7f02aec44921bd78be0fefee0c5e9b499488f6e"sv,
            "b61ce538f1a1e6c90432b233d7af5b6524ebfbe3"sv},
        test_vector{
            "75c5ad1f3cbd22e8a95fc3b089526788fb4ebceed3e7d4443da6e081a35e"sv,
            "5b7b94076b2fc20d6adb82479e6b28d07c902b75"sv},
        test_vector{
            "dd245bffe6a638806667768360a95d0574e1a0bd0d18329fdb915ca484ac0d"sv,
            "6066db99fc358952cf7fb0ec4d89cb0158ed91d7"sv},
        test_vector{
            "0321794b739418c24e7c2e565274791c4be749752ad234ed56cb0a6347430c6b"sv,
            "b89962c94d60f6a332fd60f6f07d4f032a586b76"sv},
        test_vector{
            "4c3dcf95c2f0b5258c651fcd1d51bd10425d6203067d0748d37d1340d9ddda7db3"sv,
            "17bda899c13d35413d2546212bcd8a93ceb0657b"sv},
        test_vector{
            "b8d12582d25b45290a6e1bb95da429befcfdbf5b4dd41cdf3311d6988fa17cec0723"sv,
            "badcdd53fdc144b8bf2cc1e64d10f676eebe66ed"sv},
        test_vector{
            "6fda97527a662552be15efaeba32a3aea4ed449abb5c1ed8d9bfff544708a425d69b72"sv,
            "01b4646180f1f6d2e06bbe22c20e50030322673a"sv},
        test_vector{
            "09fa2792acbb2417e8ed269041cc03c77006466e6e7ae002cf3f1af551e8ce0bb506d705"sv,
            "10016dc3a2719f9034ffcc689426d28292c42fc9"sv},
        test_vector{
            "5efa2987da0baf0a54d8d728792bcfa707a15798dc66743754406914d1cfe3709b1374eaeb"sv,
            "9f42fa2bce6ef021d93c6b2d902273797e426535"sv},
        test_vector{
            "2836de99c0f641cd55e89f5af76638947b8227377ef88bfba662e5682babc1ec96c6992bc9a0"sv,
            "cdf48bacbff6f6152515323f9b43a286e0cb8113"sv},
        test_vector{
            "42143a2b9e1d0b354df3264d08f7b602f54aad922a3d63006d097f683dc11b90178423bff2f7fe"sv,
            "b88fb75274b9b0fd57c0045988cfcef6c3ce6554"sv},
        test_vector{
            "eb60c28ad8aeda807d69ebc87552024ad8aca68204f1bcd29dc5a81dd228b591e2efb7c4df75ef03"sv,
            "c06d3a6a12d9e8db62e8cff40ca23820d61d8aa7"sv},
        test_vector{
            "7de4ba85ec54747cdc42b1f23546b7e490e31280f066e52fac117fd3b0792e4de62d5843ee98c72015"sv,
            "6e40f9e83a4be93874bc97cdebb8da6889ae2c7a"sv},
        test_vector{
            "e70653637bc5e388ccd8dc44e5eace36f7398f2bac993042b9bc2f4fb3b0ee7e23a96439dc01134b8c7d"sv,
            "3efc940c312ef0dfd4e1143812248db89542f6a5"sv},
        test_vector{
            "dd37bc9f0b3a4788f9b54966f252174c8ce487cbe59c53c22b81bf77621a7ce7616dcb5b1e2ee63c2c309b"sv,
            "a0cf03f7badd0c3c3c4ea3717f5a4fb7e67b2e56"sv},
        test_vector{
            "5f485c637ae30b1e30497f0fb7ec364e13c906e2813daa34161b7ac4a4fd7a1bddd79601bbd22cef1f57cbc7"sv,
            "a544e06f1a07ceb175a51d6d9c0111b3e15e9859"sv},
        test_vector{
            "f6c237fb3cfe95ec8414cc16d203b4874e644cc9a543465cad2dc563488a659e8a2e7c981e2a9f22e5e868ffe1"sv,
            "199d986ed991b99a071f450c6b1121a727e8c735"sv},
        test_vector{
            "da7ab3291553c659873c95913768953c6e526d3a26590898c0ade89ff56fbd110f1436af590b17fed49f8c4b2b1e"sv,
            "33bac6104b0ad6128d091b5d5e2999099c9f05de"sv},
        test_vector{
            "8cfa5fd56ee239ca47737591cba103e41a18acf8e8d257b0dbe8851134a81ff6b2e97104b39b76e19da256a17ce52d"sv,
            "76d7db6e18c1f4ae225ce8ccc93c8f9a0dfeb969"sv},
        test_vector{
            "57e89659d878f360af6de45a9a5e372ef40c384988e82640a3d5e4b76d2ef181780b9a099ac06ef0f8a7f3f764209720"sv,
            "f652f3b1549f16710c7402895911e2b86a9b2aee"sv},
        test_vector{
            "b91e64235dbd234eea2ae14a92a173ebe835347239cff8b02074416f55c6b60dc6ced06ae9f8d705505f0d617e4b29aef9"sv,
            "63faebb807f32be708cf00fc35519991dc4e7f68"sv},
        test_vector{
            "e42a67362a581e8cf3d847502215755d7ad425ca030c4360b0f7ef513e6980265f61c9fa18dd9ce668f38dbc2a1ef8f83cd6"sv,
            "0e6730bc4a0e9322ea205f4edfff1fffda26af0a"sv},
        test_vector{
            "634db92c22010e1cbf1e1623923180406c515272209a8acc42de05cc2e96a1e94c1f9f6b93234b7f4c55de8b1961a3bf352259"sv,
            "b61a3a6f42e8e6604b93196c43c9e84d5359e6fe"sv},
        test_vector{
            "cc6ca3a8cb391cd8a5aff1faa7b3ffbdd21a5a3ce66cfaddbfe8b179e4c860be5ec66bd2c6de6a39a25622f9f2fcb3fc05af12b5"sv,
            "32d979ca1b3ed0ed8c890d99ec6dd85e6c16abf4"sv},
        test_vector{
            "7c0e6a0d35f8ac854c7245ebc73693731bbbc3e6fab644466de27bb522fcb99307126ae718fe8f00742e6e5cb7a687c88447cbc961"sv,
            "6f18190bd2d02fc93bce64756575cea36d08b1c3"sv},
        test_vector{
            "c5581d40b331e24003901bd6bf244aca9e9601b9d81252bb38048642731f1146b8a4c69f88e148b2c8f8c14f15e1d6da57b2daa9991e"sv,
            "68f525feea1d8dbe0117e417ca46708d18d7629a"sv},
        test_vector{
            "ec6b4a88713df27c0f2d02e738b69db43abda3921317259c864c1c386e9a5a3f533dc05f3beeb2bec2aac8e06db4c6cb3cddcf697e03d5"sv,
            "a7272e2308622ff7a339460adc61efd0ea8dabdc"sv},
        test_vector{
            "0321736beba578e90abc1a90aa56157d871618f6de0d764cc8c91e06c68ecd3b9de3824064503384db67beb7fe012232dacaef93a000fba7"sv,
            "aef843b86916c16f66c84d83a6005d23fd005c9e"sv},
        test_vector{
            "d0a249a97b5f1486721a50d4c4ab3f5d674a0e29925d5bf2678ef6d8d521e456bd84aa755328c83fc890837726a8e7877b570dba39579aabdd"sv,
            "be2cd6f380969be59cde2dff5e848a44e7880bd6"sv},
        test_vector{
            "c32138531118f08c7dcc292428ad20b45ab27d9517a18445f38b8f0c2795bcdfe3ffe384e65ecbf74d2c9d0da88398575326074904c1709ba072"sv,
            "e5eb4543deee8f6a5287845af8b593a95a9749a1"sv},
        test_vector{
            "b0f4cfb939ea785eabb7e7ca7c476cdd9b227f015d905368ba00ae96b9aaf720297491b3921267576b72c8f58d577617e844f9f0759b399c6b064c"sv,
            "534c850448dd486787b62bdec2d4a0b140a1b170"sv},
        test_vector{
            "bd02e51b0cf2c2b8d204a026b41a66fbfc2ac37ee9411fc449c8d1194a0792a28ee731407dfc89b6dfc2b10faa27723a184afef8fd83def858a32d3f"sv,
            "6fbfa6e4edce4cc85a845bf0d228dc39acefc2fa"sv},
        test_vector{
            "e33146b83e4bb671392218da9a77f8d9f5974147182fb95ba662cb66011989c16d9af104735d6f79841aa4d1df276615b50108df8a29dbc9de31f4260d"sv,
            "018872691d9b04e8220e09187df5bc5fa6257cd9"sv},
        test_vector{
            "411c13c75073c1e2d4b1ecf13139ba9656cd35c14201f1c7c6f0eeb58d2dbfe35bfdeccc92c3961cfabb590bc1eb77eac15732fb0275798680e0c7292e50"sv,
            "d98d512a35572f8bd20de62e9510cc21145c5bf4"sv},
        test_vector{
            "f2c76ef617fa2bfc8a4d6bcbb15fe88436fdc2165d3074629579079d4d5b86f5081ab177b4c3f530376c9c924cbd421a8daf8830d0940c4fb7589865830699"sv,
            "9f3ea255f6af95c5454e55d7354cabb45352ea0b"sv},
        test_vector{
            "45927e32ddf801caf35e18e7b5078b7f5435278212ec6bb99df884f49b327c6486feae46ba187dc1cc9145121e1492e6b06e9007394dc33b7748f86ac3207cfe"sv,
            "a70cfbfe7563dd0e665c7c6715a96a8d756950c0"sv},
    };

    for (auto &&vec : test_vectors) {
        INFO(vec.message);
        INFO(vec.digest);

        std::vector<std::uint8_t> plain_bytes;
        auto r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.message, std::back_inserter(plain_bytes));
        REQUIRE(r);

        std::array<std::byte, ivy::digest_length_v<ivy::hash_algorithm::sha1>>
            digest_bytes{};

        auto nbytes = ivy::hash_data(
            ivy::hash_algorithm::sha1, plain_bytes, digest_bytes);
        if (!nbytes) {
            INFO(nbytes.error().message());
            REQUIRE(nbytes);
        }

        auto digest =
            ivy::bintext_encode_to_string<ivy::hexchars_lc>(digest_bytes);
        REQUIRE(digest == vec.digest);
    }
}

TEST_CASE("ivy:hash:sha224", "[ivy][hash][sha224]")
{
    using namespace std::string_view_literals;

    // SHA-224 is not available with BCrypt
    if (!ivy::hash_available(ivy::hash_algorithm::sha224))
        return;

    std::array test_vectors{
        test_vector{
            ""sv, "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f"sv},
        test_vector{
            "84"sv,
            "3cd36921df5d6963e73739cf4d20211e2d8877c19cff087ade9d0e3a"sv},
        test_vector{
            "5c7b"sv,
            "daff9bce685eb831f97fc1225b03c275a6c112e2d6e76f5faf7a36e6"sv},
        test_vector{
            "51ca3d"sv,
            "2c8959023515476e38388abb43599a29876b4b33d56adc06032de3a2"sv},
        test_vector{
            "6084347e"sv,
            "ae57c0a6d49739ba338adfa53bdae063e5c09122b77604780a8eeaa3"sv},
        test_vector{
            "493e14623c"sv,
            "7f631f295e024e74552083245ca8f988a3fb65680ae97c3040d2e65c"sv},
        test_vector{
            "d729d8cd1631"sv,
            "342e8e6b23c1c6a54910631f098e08e836259c57e49c1b1d023d166d"sv},
        test_vector{
            "cbf2061e10faa5"sv,
            "3aa702b1b66dc57d7aec3ccdbdfbd88592d7520f843ba5d0fa481168"sv},
        test_vector{
            "5f77b3664823c33e"sv,
            "bdf21ff325f754157ccf417f4855360a72e8fd117d28c8fe7da3ea38"sv},
        test_vector{
            "10713b894de4a734c0"sv,
            "03842600c86f5cd60c3a2147a067cb962a05303c3488b05cb45327bd"sv},
        test_vector{
            "006470d57dad9893dc03"sv,
            "c90026cda5ad24115059c62ae9add57793ade445d4742273288bbce7"sv},
        test_vector{
            "6f29ca274190400720bba2"sv,
            "ac53157947aa4b2a19089182382a4363d182dd8e4ca79cd8571390be"sv},
        test_vector{
            "17e8556176fcca2addbdde29"sv,
            "cc6ad0488db0222066f740557b5758a19b30372b302332295d8c3aff"sv},
        test_vector{
            "dbf163601db9a122a4026824de"sv,
            "9849845f4e47e1ece9a1c1e01a0d896ffea61c6c8894a75a11ce5f49"sv},
        test_vector{
            "5e1ef2ad86ceaf5439fe87d2ec9b"sv,
            "223c5d5d4a0116b32cea044f9af0fe44babea1c5ab201502591bcd5f"sv},
        test_vector{
            "65f3b9866fb8002b53cfaf806f702f"sv,
            "b1e0806a218d593821fde8e9eacc44ab5287c32209a94f011ab66b75"sv},
        test_vector{
            "b776708ffb91b3515ac46598ab9fa796"sv,
            "427311b1d7ab2488791c4deeb4251d783fe5f9806bfdfb5188c5443d"sv},
        test_vector{
            "a4bc10b1a62c96d459fbaf3a5aa3face73"sv,
            "d7e6634723ac25cb1879bdb1508da05313530419013fe255967a39e1"sv},
        test_vector{
            "9e8f3c6645c1749b55c50d2018ce40dc2427"sv,
            "2f5a583bf588c8988a572d128a95bea5ef1b66780a7d4be9c29efc31"sv},
        test_vector{
            "2db6d207c0b7d9117f24d78ee59abf2f316978"sv,
            "35681fce28307cae19522c23cbd4a77969347f7d8ee4a3088ba90ada"sv},
        test_vector{
            "3df5e7f399f6dd61a12a9d4e9464fc4997c1f37b"sv,
            "a3e68076e30751085a843a6cbfbf0f3dee63d9c4219c914372e50b28"sv},
        test_vector{
            "65781d018f27ca0c72a9fa9ab4648ed369646dd3ce"sv,
            "d15ef0d872d02da6427b8d0349dea2f204e67133b7365b4b150efc3c"sv},
        test_vector{
            "af48eeddd93fee69d1bd7de428a63986011d10945eaf"sv,
            "b89d428ee42e397cf11029ecbb27baddd036c8938f51c8ab56b875ac"sv},
        test_vector{
            "df2bf0d5f9c994ac69d78baa0d512eceb74d8a047531c1"sv,
            "db8e1ce68c8c6b84d6db755c2b8bf54f3c4b081a881efcddaf303294"sv},
        test_vector{
            "48d2f20955ea2d13433c20bc0404eb2e6ad79ed28f7cb4c0"sv,
            "3617cc3179f8b59adce181eebeed5e2763f62650949224a67e53694b"sv},
        test_vector{
            "218f74a42d3a47ef3b806601fba024b078cbff4e4b85772e0e"sv,
            "b5f40b95dcc363b97e9d00b67c5d7c37f17ab563297d2d67a4df20c9"sv},
        test_vector{
            "ef55b1e797000b04fcdb9b3021b09327e3b4e269d20cabdf418f"sv,
            "827b223d51240c2e3271c534c19c5637b6fe10083e85bcf06761ef21"sv},
        test_vector{
            "96df4387dc2c40297043bea36483f65e4eb1e07e93359cb7e68610"sv,
            "98e430a63fcdedafc9419010f7f59a4d816a45b4f973beb62530ff8c"sv},
        test_vector{
            "3ec0aa8d30d5ed825b77dc7095f421b1e608158797a377ff8bed641b"sv,
            "3108321eb7ff857f6aae69101b937f32a51ea279a6c14ba5232ac8c1"sv},
        test_vector{
            "8b0239712039f077ce323b35f4e306787b9b35270096e57735cff45d84"sv,
            "a5c740d3ce46bb2e0a048488f2b0605c6d0ca0ea2f382d043d13db97"sv},
        test_vector{
            "044be30167a9758c46c727921dc4eb4e0dcb965623423e6fdd44e7a4ea52"sv,
            "6eb78313c743ea8769d8340f284dda6ded64a1db64392f21abb82c5c"sv},
        test_vector{
            "57f6118bacce47ecc31ce8b0c083d3c9219e0dbe9e4fbea154537c41231acc"sv,
            "0dbb53c866d63af44c222c76c825df0e379dcedfb958db03b6fd29a5"sv},
        test_vector{
            "fe1f0fb02c9011f4c8c5905934ed15136771737ce31c5859e67f235fe594f5f6"sv,
            "bbeaacc632c2a3db2a9b47f157ab54aa27776c6e74cf0bcaa91b06d5"sv},
        test_vector{
            "14fb01ae9d6015ecb3e56d6ecdfa4bc0533186adf8457f5e4a5c57c687895f3db3"sv,
            "178272c7d7cc71b15074c27e3b7997d4a3ba99626986a1a16cf30030"sv},
        test_vector{
            "ff6c49712f044f4063c14125c0cdfba18ed8b7138453768a45dfa2d82a05f1e84227"sv,
            "403284c888a7280bc8bfc25f0c34182cd378306a21a1404d4e1c40cf"sv},
        test_vector{
            "f900bd7e0117247f97c8fc7a665c76a35f571c3366571d6c4a3ee5d7fb93f1d1f726e2"sv,
            "48235b9820d66d8885faabf6a9ede63ba2a21b6177e987a33242373e"sv},
        test_vector{
            "42d38188ac49440cfefb77db975e083e6b22348c4c67f0f8692e88ad140d861dc828d595"sv,
            "615344f890e5bcf71b5efe39de1fc942ba1fe30dd9e9146adb6a41bf"sv},
        test_vector{
            "74fdd7d958b8ae7c2c3c5cff4266dfb2b3b842c9f59ecbbcaff575edcbcda08ccd6e08b764"sv,
            "66d7d6c54fc7775a0ba845ba3e11719fa535b9289f20b098c5f7a342"sv},
        test_vector{
            "934416dd0581e22f2bfbece7bb64afe820451fa21342df7e6f9fb37c4103381a1f7cd379bcc4"sv,
            "fae8f1aa22def4dbaa814c5b0babdec43394951792c937050d2963a6"sv},
        test_vector{
            "102401c84a716ae72579c6ae79c359ea309ffd95abffae4c61884c03c9e99df77b6c92e492cacb"sv,
            "8f34812d57a16ef8a51ad987660c5f8623e0fa9d89846e28d46d14d9"sv},
        test_vector{
            "79bc8fb60f85d15a2386566e3e7314df284533085add1c7bb6ead3ff760c86d5633a66404761b544"sv,
            "65c54014cfa30f0bc27d1c6efa96ae8481f4c2505bff272956eab0df"sv},
        test_vector{
            "db3121ea71294983b185207a9d8de3e484a66c0431bf07c962eb82977c4f834b7c3f1e7931a4a7f7a9"sv,
            "9316d2f021c2913d63a7e66924c87c161c3cfde0ea7ba07f54772862"sv},
        test_vector{
            "0dd51aa660c5cb4b7f78c46852c1db8707ab451c1367b6187388c8bb3873a1aa4210d0414cc6792a29a7"sv,
            "31989e7a62a5132a5070d77250d8904bb82d457dc63469d06b50185e"sv},
        test_vector{
            "487fd2e5b694b7071d3789a258a51e8604dc0d3e8f5d62f39131968e602abe1ddf6b0278962a512408b553"sv,
            "e798683438284626d710877d9eea3a0e02f349fc43acb7f9f8f9e81c"sv},
        test_vector{
            "11183bdebfef58e4da5b1cb73be0d30b20da304d8659d921da2e270fd14626799537e4d12119e809ee97004a"sv,
            "96870657d6cb668be3995aa8bd31df77840d1d1915d72482e83b6b2c"sv},
        test_vector{
            "a239de5c8e2644e8f030d94d98f1a30664e6fd961dc2977a9c08be5c31d8de89450945a53d79299ea2a1edde7f"sv,
            "e99743d4fd26c8800c36a67b6762247c29da6b62794123c59de06dc0"sv},
        test_vector{
            "917c4577aa6b0f9df49999fc1c958cb09b7fd5fc80be949670f03545eb27dcaed052076b24f96f5e0f2e2f4527c0"sv,
            "7ecd693d4d9cf43929464698efa0bac33c2e1424f816edc769260978"sv},
        test_vector{
            "c3f1e735a6741aa481ad577a98dbac1f03cc80ea0dae1b94db2369ed4e93facd29c64e4e77b25038279120bdfa3715"sv,
            "86f0d89d8e14fd8b6606412d71a7a54a347b304ea5d49c208f2266ab"sv},
        test_vector{
            "de4fbfd553cdf37019f25afa82dc6b9970f4bb1ebbc37f80d3084c88a70722cdc523a9e3c2afbad0dc0221bfdec9a2f9"sv,
            "4c5262acb4a2a44eaa9bc6757024fb202ef4d5a7a16fa37252a422b5"sv},
        test_vector{
            "db2e2eb636610cf42e9b33433acce1b3b925949f297dd83199f45d2861d64cd910c2db74a60b2089045e22cba0a536137d"sv,
            "16bf4e45bcdc60447c68dcb30e6b08f55ce9f4124a29cf1f9a9d065d"sv},
        test_vector{
            "a8e729d336d5d6ac50e1e22f0b193b66e26042fc6459214129875e740ab2b142918c138aaf941863ad3b7e6065450613b273"sv,
            "452bf2e5ebfc4e451cc434bc09e2a10032eed0b7627cf55e7e5ed0e2"sv},
        test_vector{
            "d05317d4b535f9d10f739d0c2dedf3ffb090c1ad9d205089b1346693f58273c4925c0face57ba45ad6fc687c66a88fc78878be"sv,
            "4f03c439e097b51b00e314f675937c4d911505859fb7ab16adc65e44"sv},
        test_vector{
            "26bb4ed4f0424c60fe4212ff8c955e89e2f553a7d7701be59416d2089af59fa1074724e214e919b1e30f33fb78374b4b055bbc9b"sv,
            "e7c899e27009d4dc77c2d300f191b757e52c9e7eac4b023bfab2b52a"sv},
        test_vector{
            "f015ec83944f03292463c4345fdb1c26d1ea07645facbc9520ae244b6eb191e53dabadb4ac0fb15cda4ed77dfb9e1193abfafb1b81"sv,
            "459e40b3fbd612912f0217c60099379ce077cd02505871b0c9c14e7a"sv},
        test_vector{
            "0786706f680c27b792d054faa63f499a8e6b5ddb90502946235bf74c022d772c809cb4171bfa4791539aca1abd91900e53ba93ca0efd"sv,
            "fadebab7c3d0fb8e97e429b79083087735e4ab385a789521260ef3ad"sv},
        test_vector{
            "445e8698eeb8accbaac4ffa7d934fffd16014a430ef70f3a9174c6cfe96d1e3f6ab1377f4a7212dbb30146dd17d9f470c4dffc45b8e871"sv,
            "4c7ae028c0fe61f2a9cada61fae30685b77f04c6442576e912af9fa6"sv},
        test_vector{
            "52839f2f0853a30df14ec897a1914c685c1ac21470d00654c8c37663bfb65fa732dbb694d9dd09ced723b48d8f545846ba168988b61cc724"sv,
            "2f755a57674b49d5c25cb37348f35b6fd2de2552c749f2645ba63d20"sv},
        test_vector{
            "5fe8c2072d8900287ccaf07f3f66b0c22acd3e0bb91d9573754e19e373ac35271d8b43443436ac0c162850ef3d7f281409ad29a9bf716c77d1"sv,
            "42909757f6e229f69f04cc7a863c4e70e48c7c3575057b455c959775"sv},
        test_vector{
            "e8064d83f3d643af8718c87e3ccd6a9733685eac61d572a22ab943f232fcb04f70858e8984449db14a76bb7eaf2458efc3ed2a32100622c52b7f"sv,
            "1a1d8ed54cb45c97bc970754b43eb93d9eabde4c7b07f76ad82d8ede"sv},
        test_vector{
            "87c9a517e28d1bb54ad20fca76460efd894d7786e68ee8d746b2f68208682157c8ad06cc324ad7a3189e09c6c39d4c768719c0a49a41669f2767d5"sv,
            "605977cf87b9b309bbddaaa64e528ace66b04df9f72c0e7ec88be1da"sv},
        test_vector{
            "59fdac3b6b32039291801c7d6f46ede8d26dc5b7a192e007116739b617569f2523680b3c0b6631af453e55805aa760c6970833ac06963bbc9dbd455e"sv,
            "e9f0cb1dc8337e906385892f2348a8ba4412318ecad9b96e3711531f"sv},
        test_vector{
            "30350a4df0b58ff49c0fa09e426fcd7007b290c760c825c1855d9b0023b82caa51e3cab4c60cfa61492be50568e5ac0f6db0fd468e39e4536403e3809f"sv,
            "776cc6636c02408fbf65ace73ae80017108b917c16c5a912fd860241"sv},
        test_vector{
            "ef797a0d43c30b4fe1014bdb9420879c2ff845d27e73d55a7df22930c8ece73253d8bb265b4ef2ff9c69455cc56ff25229b4126bb7bb26ee2c9ff36187b1"sv,
            "f5b9ffb102affac352a4a535a00f89b06c268cf4881d712668906025"sv},
        test_vector{
            "716944de41710c29b659be10480bb25a351a39e577ee30e8f422d57cf62ad95bda39b6e70c61426e33fd84aca84cc7912d5eee45dc34076a5d2323a15c7964"sv,
            "61645ac748db567ac862796b8d06a47afebfa2e1783d5c5f3bcd81e2"sv},
        test_vector{
            "a3310ba064be2e14ad32276e18cd0310c933a6e650c3c754d0243c6c61207865b4b65248f66a08edf6e0832689a9dc3a2e5d2095eeea50bd862bac88c8bd318d"sv,
            "b2a5586d9cbf0baa999157b4af06d88ae08d7c9faab4bc1a96829d65"sv},
    };

    for (auto &&vec : test_vectors) {
        INFO(vec.message);
        INFO(vec.digest);

        std::vector<std::uint8_t> plain_bytes;
        auto r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.message, std::back_inserter(plain_bytes));
        REQUIRE(r);

        std::array<std::byte, ivy::digest_length_v<ivy::hash_algorithm::sha224>>
            digest_bytes{};

        auto nbytes = ivy::hash_data(
            ivy::hash_algorithm::sha224, plain_bytes, digest_bytes);
        if (!nbytes) {
            INFO(nbytes.error().message());
            REQUIRE(nbytes);
        }

        auto digest =
            ivy::bintext_encode_to_string<ivy::hexchars_lc>(digest_bytes);
        REQUIRE(digest == vec.digest);
    }
}

TEST_CASE("ivy:hash:sha256", "[ivy][hash][sha256]")
{
    using namespace std::string_view_literals;

    REQUIRE(ivy::hash_available(ivy::hash_algorithm::sha256));

    std::array test_vectors{
        test_vector{
            ""sv,
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"sv},
        test_vector{
            "d3"sv,
            "28969cdfa74a12c82f3bad960b0b000aca2ac329deea5c2328ebc6f2ba9802c1"sv},
        test_vector{
            "11af"sv,
            "5ca7133fa735326081558ac312c620eeca9970d1e70a4b95533d956f072d1f98"sv},
        test_vector{
            "b4190e"sv,
            "dff2e73091f6c05e528896c4c831b9448653dc2ff043528f6769437bc7b975c2"sv},
        test_vector{
            "74ba2521"sv,
            "b16aa56be3880d18cd41e68384cf1ec8c17680c45a02b1575dc1518923ae8b0e"sv},
        test_vector{
            "c299209682"sv,
            "f0887fe961c9cd3beab957e8222494abb969b1ce4c6557976df8b0f6d20e9166"sv},
        test_vector{
            "e1dc724d5621"sv,
            "eca0a060b489636225b4fa64d267dabbe44273067ac679f20820bddc6b6a90ac"sv},
        test_vector{
            "06e076f5a442d5"sv,
            "3fd877e27450e6bbd5d74bb82f9870c64c66e109418baa8e6bbcff355e287926"sv},
        test_vector{
            "5738c929c4f4ccb6"sv,
            "963bb88f27f512777aab6c8b1a02c70ec0ad651d428f870036e1917120fb48bf"sv},
        test_vector{
            "3334c58075d3f4139e"sv,
            "078da3d77ed43bd3037a433fd0341855023793f9afd08b4b08ea1e5597ceef20"sv},
        test_vector{
            "74cb9381d89f5aa73368"sv,
            "73d6fad1caaa75b43b21733561fd3958bdc555194a037c2addec19dc2d7a52bd"sv},
        test_vector{
            "76ed24a0f40a41221ebfcf"sv,
            "044cef802901932e46dc46b2545e6c99c0fc323a0ed99b081bda4216857f38ac"sv},
        test_vector{
            "9baf69cba317f422fe26a9a0"sv,
            "fe56287cd657e4afc50dba7a3a54c2a6324b886becdcd1fae473b769e551a09b"sv},
        test_vector{
            "68511cdb2dbbf3530d7fb61cbc"sv,
            "af53430466715e99a602fc9f5945719b04dd24267e6a98471f7a7869bd3b4313"sv},
        test_vector{
            "af397a8b8dd73ab702ce8e53aa9f"sv,
            "d189498a3463b18e846b8ab1b41583b0b7efc789dad8a7fb885bbf8fb5b45c5c"sv},
        test_vector{
            "294af4802e5e925eb1c6cc9c724f09"sv,
            "dcbaf335360de853b9cddfdafb90fa75567d0d3d58af8db9d764113aef570125"sv},
        test_vector{
            "0a27847cdc98bd6f62220b046edd762b"sv,
            "80c25ec1600587e7f28b18b1b18e3cdc89928e39cab3bc25e4d4a4c139bcedc4"sv},
        test_vector{
            "1b503fb9a73b16ada3fcf1042623ae7610"sv,
            "d5c30315f72ed05fe519a1bf75ab5fd0ffec5ac1acb0daf66b6b769598594509"sv},
        test_vector{
            "59eb45bbbeb054b0b97334d53580ce03f699"sv,
            "32c38c54189f2357e96bd77eb00c2b9c341ebebacc2945f97804f59a93238288"sv},
        test_vector{
            "58e5a3259cb0b6d12c83f723379e35fd298b60"sv,
            "9b5b37816de8fcdf3ec10b745428708df8f391c550ea6746b2cafe019c2b6ace"sv},
        test_vector{
            "c1ef39cee58e78f6fcdc12e058b7f902acd1a93b"sv,
            "6dd52b0d8b48cc8146cebd0216fbf5f6ef7eeafc0ff2ff9d1422d6345555a142"sv},
        test_vector{
            "9cab7d7dcaec98cb3ac6c64dd5d4470d0b103a810c"sv,
            "44d34809fc60d1fcafa7f37b794d1d3a765dd0d23194ebbe340f013f0c39b613"sv},
        test_vector{
            "ea157c02ebaf1b22de221b53f2353936d2359d1e1c97"sv,
            "9df5c16a3f580406f07d96149303d8c408869b32053b726cf3defd241e484957"sv},
        test_vector{
            "da999bc1f9c7acff32828a73e672d0a492f6ee895c6867"sv,
            "672b54e43f41ee77584bdf8bf854d97b6252c918f7ea2d26bc4097ea53a88f10"sv},
        test_vector{
            "47991301156d1d977c0338efbcad41004133aefbca6bcf7e"sv,
            "feeb4b2b59fec8fdb1e55194a493d8c871757b5723675e93d3ac034b380b7fc9"sv},
        test_vector{
            "2e7ea84da4bc4d7cfb463e3f2c8647057afff3fbececa1d200"sv,
            "76e3acbc718836f2df8ad2d0d2d76f0cfa5fea0986be918f10bcee730df441b9"sv},
        test_vector{
            "47c770eb4549b6eff6381d62e9beb464cd98d341cc1c09981a7a"sv,
            "6733809c73e53666c735b3bd3daf87ebc77c72756150a616a194108d71231272"sv},
        test_vector{
            "ac4c26d8b43b8579d8f61c9807026e83e9b586e1159bd43b851937"sv,
            "0e6e3c143c3a5f7f38505ed6adc9b48c18edf6dedf11635f6e8f9ac73c39fe9e"sv},
        test_vector{
            "0777fc1e1ca47304c2e265692838109e26aab9e5c4ae4e8600df4b1f"sv,
            "ffb4fc03e054f8ecbc31470fc023bedcd4a406b9dd56c71da1b660dcc4842c65"sv},
        test_vector{
            "1a57251c431d4e6c2e06d65246a296915071a531425ecf255989422a66"sv,
            "c644612cd326b38b1c6813b1daded34448805aef317c35f548dfb4a0d74b8106"sv},
        test_vector{
            "9b245fdad9baeb890d9c0d0eff816efb4ca138610bc7d78cb1a801ed3273"sv,
            "c0e29eeeb0d3a7707947e623cdc7d1899adc70dd7861205ea5e5813954fb7957"sv},
        test_vector{
            "95a765809caf30ada90ad6d61c2b4b30250df0a7ce23b7753c9187f4319ce2"sv,
            "a4139b74b102cf1e2fce229a6cd84c87501f50afa4c80feacf7d8cf5ed94f042"sv},
        test_vector{
            "09fc1accc230a205e4a208e64a8f204291f581a12756392da4b8c0cf5ef02b95"sv,
            "4f44c1c7fbebb6f9601829f3897bfd650c56fa07844be76489076356ac1886a4"sv},
        test_vector{
            "0546f7b8682b5b95fd32385faf25854cb3f7b40cc8fa229fbd52b16934aab388a7"sv,
            "b31ad3cd02b10db282b3576c059b746fb24ca6f09fef69402dc90ece7421cbb7"sv},
        test_vector{
            "b12db4a1025529b3b7b1e45c6dbc7baa8897a0576e66f64bf3f8236113a6276ee77d"sv,
            "1c38bf6bbfd32292d67d1d651fd9d5b623b6ec1e854406223f51d0df46968712"sv},
        test_vector{
            "e68cb6d8c1866c0a71e7313f83dc11a5809cf5cfbeed1a587ce9c2c92e022abc1644bb"sv,
            "c2684c0dbb85c232b6da4fb5147dd0624429ec7e657991edd95eda37a587269e"sv},
        test_vector{
            "4e3d8ac36d61d9e51480831155b253b37969fe7ef49db3b39926f3a00b69a36774366000"sv,
            "bf9d5e5b5393053f055b380baed7e792ae85ad37c0ada5fd4519542ccc461cf3"sv},
        test_vector{
            "03b264be51e4b941864f9b70b4c958f5355aac294b4b87cb037f11f85f07eb57b3f0b89550"sv,
            "d1f8bd684001ac5a4b67bbf79f87de524d2da99ac014dec3e4187728f4557471"sv},
        test_vector{
            "d0fefd96787c65ffa7f910d6d0ada63d64d5c4679960e7f06aeb8c70dfef954f8e39efdb629b"sv,
            "49ba38db85c2796f85ffd57dd5ec337007414528ae33935b102d16a6b91ba6c1"sv},
        test_vector{
            "b7c79d7e5f1eeccdfedf0e7bf43e730d447e607d8d1489823d09e11201a0b1258039e7bd4875b1"sv,
            "725e6f8d888ebaf908b7692259ab8839c3248edd22ca115bb13e025808654700"sv},
        test_vector{
            "64cd363ecce05fdfda2486d011a3db95b5206a19d3054046819dd0d36783955d7e5bf8ba18bf738a"sv,
            "32caef024f84e97c30b4a7b9d04b678b3d8a6eb2259dff5b7f7c011f090845f8"sv},
        test_vector{
            "6ac6c63d618eaf00d91c5e2807e83c093912b8e202f78e139703498a79c6067f54497c6127a23910a6"sv,
            "4bb33e7c6916e08a9b3ed6bcef790aaaee0dcf2e7a01afb056182dea2dad7d63"sv},
        test_vector{
            "d26826db9baeaa892691b68900b96163208e806a1da077429e454fa011840951a031327e605ab82ecce2"sv,
            "3ac7ac6bed82fdc8cd15b746f0ee7489158192c238f371c1883c9fe90b3e2831"sv},
        test_vector{
            "3f7a059b65d6cb0249204aac10b9f1a4ac9e5868adebbe935a9eb5b9019e1c938bfc4e5c5378997a3947f2"sv,
            "bfce809534eefe871273964d32f091fe756c71a7f512ef5f2300bcd57f699e74"sv},
        test_vector{
            "60ffcb23d6b88e485b920af81d1083f6291d06ac8ca3a965b85914bc2add40544a027fca936bbde8f359051c"sv,
            "1d26f3e04f89b4eaa9dbed9231bb051eef2e8311ad26fe53d0bf0b821eaf7567"sv},
        test_vector{
            "9ecd07b684bb9e0e6692e320cec4510ca79fcdb3a2212c26d90df65db33e692d073cc174840db797504e482eef"sv,
            "0ffeb644a49e787ccc6970fe29705a4f4c2bfcfe7d19741c158333ff6982cc9c"sv},
        test_vector{
            "9d64de7161895884e7fa3d6e9eb996e7ebe511b01fe19cd4a6b3322e80aaf52bf6447ed1854e71001f4d54f8931d"sv,
            "d048ee1524014adf9a56e60a388277de194c694cc787fc5a1b554ea9f07abfdf"sv},
        test_vector{
            "c4ad3c5e78d917ecb0cbbcd1c481fc2aaf232f7e289779f40e504cc309662ee96fecbd20647ef00e46199fbc482f46"sv,
            "50dbf40066f8d270484ee2ef6632282dfa300a85a8530eceeb0e04275e1c1efd"sv},
        test_vector{
            "4eef5107459bddf8f24fc7656fd4896da8711db50400c0164847f692b886ce8d7f4d67395090b3534efd7b0d298da34b"sv,
            "7c5d14ed83dab875ac25ce7feed6ef837d58e79dc601fb3c1fca48d4464e8b83"sv},
        test_vector{
            "047d2758e7c2c9623f9bdb93b6597c5e84a0cd34e610014bcb25b49ed05c7e356e98c7a672c3dddcaeb84317ef614d342f"sv,
            "7d53eccd03da37bf58c1962a8f0f708a5c5c447f6a7e9e26137c169d5bdd82e4"sv},
        test_vector{
            "3d83df37172c81afd0de115139fbf4390c22e098c5af4c5ab4852406510bc0e6cf741769f44430c5270fdae0cb849d71cbab"sv,
            "99dc772e91ea02d9e421d552d61901016b9fd4ad2df4a8212c1ec5ba13893ab2"sv},
        test_vector{
            "33fd9bc17e2b271fa04c6b93c0bdeae98654a7682d31d9b4dab7e6f32cd58f2f148a68fbe7a88c5ab1d88edccddeb30ab21e5e"sv,
            "cefdae1a3d75e792e8698d5e71f177cc761314e9ad5df9602c6e60ae65c4c267"sv},
        test_vector{
            "77a879cfa11d7fcac7a8282cc38a43dcf37643cc909837213bd6fd95d956b219a1406cbe73c52cd56c600e55b75bc37ea69641bc"sv,
            "c99d64fa4dadd4bc8a389531c68b4590c6df0b9099c4d583bc00889fb7b98008"sv},
        test_vector{
            "45a3e6b86527f20b4537f5af96cfc5ad8777a2dde6cf7511886c5590ece24fc61b226739d207dabfe32ba6efd9ff4cd5db1bd5ead3"sv,
            "4d12a849047c6acd4b2eee6be35fa9051b02d21d50d419543008c1d82c427072"sv},
        test_vector{
            "25362a4b9d74bde6128c4fdc672305900947bc3ada9d9d316ebcf1667ad4363189937251f149c72e064a48608d940b7574b17fefc0df"sv,
            "f8e4ccab6c979229f6066cc0cb0cfa81bb21447c16c68773be7e558e9f9d798d"sv},
        test_vector{
            "3ebfb06db8c38d5ba037f1363e118550aad94606e26835a01af05078533cc25f2f39573c04b632f62f68c294ab31f2a3e2a1a0d8c2be51"sv,
            "6595a2ef537a69ba8583dfbf7f5bec0ab1f93ce4c8ee1916eff44a93af5749c4"sv},
        test_vector{
            "2d52447d1244d2ebc28650e7b05654bad35b3a68eedc7f8515306b496d75f3e73385dd1b002625024b81a02f2fd6dffb6e6d561cb7d0bd7a"sv,
            "cfb88d6faf2de3a69d36195acec2e255e2af2b7d933997f348e09f6ce5758360"sv},
        test_vector{
            "4cace422e4a015a75492b3b3bbfbdf3758eaff4fe504b46a26c90dacc119fa9050f603d2b58b398cad6d6d9fa922a154d9e0bc4389968274b0"sv,
            "4d54b2d284a6794581224e08f675541c8feab6eefa3ac1cfe5da4e03e62f72e4"sv},
        test_vector{
            "8620b86fbcaace4ff3c2921b8466ddd7bacae07eefef693cf17762dcabb89a84010fc9a0fb76ce1c26593ad637a61253f224d1b14a05addccabe"sv,
            "dba490256c9720c54c612a5bd1ef573cd51dc12b3e7bd8c6db2eabe0aacb846b"sv},
        test_vector{
            "d1be3f13febafefc14414d9fb7f693db16dc1ae270c5b647d80da8583587c1ad8cb8cb01824324411ca5ace3ca22e179a4ff4986f3f21190f3d7f3"sv,
            "02804978eba6e1de65afdbc6a6091ed6b1ecee51e8bff40646a251de6678b7ef"sv},
        test_vector{
            "f499cc3f6e3cf7c312ffdfba61b1260c37129c1afb391047193367b7b2edeb579253e51d62ba6d911e7b818ccae1553f6146ea780f78e2219f629309"sv,
            "0b66c8b4fefebc8dc7da0bbedc1114f228aa63c37d5c30e91ab500f3eadfcec5"sv},
        test_vector{
            "6dd6efd6f6caa63b729aa8186e308bc1bda06307c05a2c0ae5a3684e6e460811748690dc2b58775967cfcc645fd82064b1279fdca771803db9dca0ff53"sv,
            "c464a7bf6d180de4f744bb2fe5dc27a3f681334ffd54a9814650e60260a478e3"sv},
        test_vector{
            "6511a2242ddb273178e19a82c57c85cb05a6887ff2014cf1a31cb9ba5df1695aadb25c22b3c5ed51c10d047d256b8e3442842ae4e6c525f8d7a5a944af2a"sv,
            "d6859c0b5a0b66376a24f56b2ab104286ed0078634ba19112ace0d6d60a9c1ae"sv},
        test_vector{
            "e2f76e97606a872e317439f1a03fcd92e632e5bd4e7cbc4e97f1afc19a16fde92d77cbe546416b51640cddb92af996534dfd81edb17c4424cf1ac4d75aceeb"sv,
            "18041bd4665083001fba8c5411d2d748e8abbfdcdfd9218cb02b68a78e7d4c23"sv},
        test_vector{
            "5a86b737eaea8ee976a0a24da63e7ed7eefad18a101c1211e2b3650c5187c2a8a650547208251f6d4237e661c7bf4c77f335390394c37fa1a9f9be836ac28509"sv,
            "42e61e174fbb3897d6dd6cef3dd2802fe67b331953b06114a65c772859dfc1aa"sv},
    };

    for (auto &&vec : test_vectors) {
        INFO(vec.message);
        INFO(vec.digest);

        std::vector<std::uint8_t> plain_bytes;
        auto r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.message, std::back_inserter(plain_bytes));
        REQUIRE(r);

        std::array<std::byte, ivy::digest_length_v<ivy::hash_algorithm::sha256>>
            digest_bytes{};

        auto nbytes = ivy::hash_data(
            ivy::hash_algorithm::sha256, plain_bytes, digest_bytes);
        if (!nbytes) {
            INFO(nbytes.error().message());
            REQUIRE(nbytes);
        }

        auto digest =
            ivy::bintext_encode_to_string<ivy::hexchars_lc>(digest_bytes);
        REQUIRE(digest == vec.digest);
    }
}

TEST_CASE("ivy:hash:sha384", "[ivy][hash][sha384]")
{
    using namespace std::string_view_literals;

    REQUIRE(ivy::hash_available(ivy::hash_algorithm::sha384));

    std::
        array
            test_vectors{
                test_vector{
                    ""sv,
                    "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b"sv},
                test_vector{
                    "c5"sv,
                    "b52b72da75d0666379e20f9b4a79c33a329a01f06a2fb7865c9062a28c1de860ba432edfd86b4cb1cb8a75b46076e3b1"sv},
                test_vector{
                    "6ece"sv,
                    "53d4773da50d8be4145d8f3a7098ff3691a554a29ae6f652cc7121eb8bc96fd2210e06ae2fa2a36c4b3b3497341e70f0"sv},
                test_vector{
                    "1fa4d5"sv,
                    "e4ca4663dff189541cd026dcc056626419028774666f5b379b99f4887c7237bdbd3bea46d5388be0efc2d4b7989ab2c4"sv},
                test_vector{
                    "50e3853d"sv,
                    "936a3c3991716ba4c413bc03de20f5ce1c63703b3a5bdb6ab558c9ff70d537e46eb4a15d9f2c85e68d8678de5682695e"sv},
                test_vector{
                    "4b5fab61e0"sv,
                    "fb390aa5b70b068a54d6d5127df6a6227becc4d6f891fd3f6068b917a883c9b66f318fddb6384d10be8c7af0d3132f03"sv},
                test_vector{
                    "dad95a4b4d37"sv,
                    "3a2b40f453925bc3ce17d640757ee0e899390b4a8d984d0297c1bae6b60b9f2603bf71c323fd171011372335e5702e40"sv},
                test_vector{
                    "121835fe3700b7"sv,
                    "7bd06a94acba7beb3c5a9b9e8769c3da6691c482d78b1e5c7619b36630eba4e596d11c410a4c87006f4716b6f17bb9a0"sv},
                test_vector{
                    "de60275bdafce4b1"sv,
                    "a3d861d866c1362423eb21c6bec8e44b74ce993c55baa2b6640567560ebecdaeda07183dbbbd95e0f522caee5ddbdaf0"sv},
                test_vector{
                    "8d45a55d5ce1f928e6"sv,
                    "de76683575a050e2eb5ef95ee201f82416478a1d14bf3d96d1fd4efd52b1a28fed8dfee1830070001dc102a21f761d20"sv},
                test_vector{
                    "5c7dde9b3894d73cefe1"sv,
                    "f31b22115fa7178e78223e06aae870547ab93c6eb3c3910b0ee16e6106db55935d6c0eb820132a2078ece1067efc81c3"sv},
                test_vector{
                    "967fa34c07e4945a77051a"sv,
                    "f8f24d81c4f8f23ecb42d76ed5d2b34c9cbc1f0a97234d1114804b5999759f3131c741d5768cc928163503c5f55f594b"sv},
                test_vector{
                    "2209112ee779bf6d95711105"sv,
                    "09c54bf533a26c7447caa5783db2ec7ef5e55752da7f2a2c4e360982a94ec1ca2cb6a157d34eed28de978b4145e17ebc"sv},
                test_vector{
                    "20442e1c3f3c88919c39978b78"sv,
                    "50bc95b036e0f54d833032a80d45c2ac38b3d29e9c7f72a2eb14781e9241d2a4b8e8dba6ee6f4c9e46a758d5712dbd39"sv},
                test_vector{
                    "4d43702be4f0530319555d7f1a33"sv,
                    "83c8f0bb762801eb26cc5115abebb657c18ff811de500b32b7a568a220a287e900b6c75224fe7429169fbd534cb588e1"sv},
                test_vector{
                    "5e2a79a544af85f150f7a9d209fd44"sv,
                    "8051ebc9cabb052cabe07e4023e2140808b77d25b07b96d2e3c22393f71b116c1a1e41bf62e57f73ff67871da7c93cf9"sv},
                test_vector{
                    "e1bb967b5d379a4aa39050274d09bd93"sv,
                    "3b04f96965ad2fbabd4df25d5d8c95589d069c312ee48539090b2d7b495d2446c31eb2b8f8ffb3012bdce065323d9f48"sv},
                test_vector{
                    "bb84a014cd17cc232c98ae8b0709917e9d"sv,
                    "85227ae057f2082adf178cae996449100b6a3119e4c415a99e25be6ef20ba8c0eae818d60f71c5c83ff2d4c59aa75263"sv},
                test_vector{
                    "c3411a0592f1f4fa698815238997db356418"sv,
                    "233ac44170d9f452a1a0231622030b15c104ff8ecaa3fccdb9e9e5031fd5b4220186a8edd032849c8b93dc183a5c8627"sv},
                test_vector{
                    "e831b739e8eb9f787f63c0bb071ddcc9f44cab"sv,
                    "91722d4b7aecc211bb8a5480c6855f3b71be4647e1dde0380c23afaa03f45c642606a24506e0317bf51506a483de28ac"sv},
                test_vector{
                    "b8a7bbccde46e85f1223237d9353b78c3b19727b"sv,
                    "28ba69dc05e6e29de91924114d6c9fc7612f6d2a68b07fa001df059bcf98f7aa85389caeb966eaa299c79fe1fd1e40e3"sv},
                test_vector{
                    "cf391b8aabec6f81288c8b7b92843be23d2e847574"sv,
                    "121e5ef697df491a53d7bae121416aa653d759a37db9d0b993031b18a0ef160ed98842a291e1ba2cea8b998bc5eee0b1"sv},
                test_vector{
                    "9d65d88bffed764c286f34894f991600d1a67d622382"sv,
                    "84b6e0d6a45329daf47a793418ed5dbde01336b4b9468bb69e5da61c42b691e6794e6ed0e8fb1b8e7d4cd3cbaadc520a"sv},
                test_vector{
                    "bab6ea46fb717f73f0628132a2b96be383774f1e5d8b6d"sv,
                    "e969aca1b50e928cad29a0d756457f6de8d7a4e589fd41e53a1e758c3b20f9b81b36bf098a49102fbf869651ca9a98b5"sv},
                test_vector{
                    "8853b00e869764adb527a07b073c85a24e6c208ba47eef4e"sv,
                    "09ad44e85ac190e2d1c3ceb4efbea10ded34d0de961fe4ee268132c48e38660e6cf585bfffb8f7b00b0fad1514312b63"sv},
                test_vector{
                    "71fe1ba5d299495d2a56039c64032ec6263d437f55e3f5bedb"sv,
                    "b41a5d3b4af6d4b9c349e0788538e9a0311086894df7b72cf5aaf4091a7e039e4e89cc77a123474e6d1bac438e5e9f88"sv},
                test_vector{
                    "4ebe07d03c93e849b4bbfe9f2d2294bf6ccab457f76d3f99cb64"sv,
                    "beba464d7065185587fad89bfcea9635bf0ab775c3eb8c147b5b2bd8601db6dba0590b50dd1068733f20dc68e004a090"sv},
                test_vector{
                    "918ac0a97ec1632908489e5242ba284bc811aa7197242cf7226fcd"sv,
                    "c4baf6397a4c6e26492b63a4aab7dffdd0051d8f51938ac24cfd8dae2f7afed1a4aa2430d7aeb0be2a72b21a6c50198c"sv},
                test_vector{
                    "97477f7272d8a89e38e796c533e9f8a8ae4c92ccaa5d907ed26a1a15"sv,
                    "d1ad524ebe908d7c5aff50e6cb780fd3a70e87c914a36b93c4e35f5b2cb03850b122b480ef8587d4a44f22467f4c480c"sv},
                test_vector{
                    "718e0cfe1386cb1421b4799b15788b862bf03a8072bb30d02303888032"sv,
                    "6d8b8a5bc7ea365ea07f11d3b12e95872a9633684752495cc431636caf1b273a35321044af31c974d8575d38711f56c6"sv},
                test_vector{
                    "d3b07f0fd5d4cd3188aead8dc8338de42056e2e8487eca51ec37ef2daf27"sv,
                    "adcc2e954c91db3db2d71d0dee1f030e723bee1a23816fe003ac5dc862a0872ef51ff386c18be6ebcaa493f32d1195b9"sv},
                test_vector{
                    "695b9efe1809abd5d44eae957ddf9c2cd3c75fae2f522855712a07c639c0b9"sv,
                    "3bb95d164d94595a1187f77fc26c280ffbb08e74ec7947aa3e5b38bec7c6f8115c4d880788c2402dbb3e5b94afd130ee"sv},
                test_vector{
                    "be01e520e69f04174ccf95455b1c81445298264d9adc4958574a52843d95b8ba"sv,
                    "c5cf54b8e3105b1c7bf7a43754d915b0947f28b6dc94a019182929b5c848e11441c9e4e90c7449f4c3cd12954f0f5d99"sv},
                test_vector{
                    "98ef7301f957d73d4e821d5873e8a9b5970fbd219d5cf74ec2291b8381181391b4"sv, "b2564bbb159c3aeadbae0e4a4437f7c5111020e9ad0f4eb508147a961ac22a01e1a26df046b14ee5e8a49d9ed22b8cd1"sv},
                test_vector{
                    "873ba7f8b71517ec50297b21cf94cdb7a58abbb8829206f0d3f328ff8a6939ac1d1d"sv,
                    "7405fdd557d2b5d4f65e9e85f508d3791495f1820d37cabfc8dbb74d7b41df8613d995c612d378dd88337e00951d0280"sv},
                test_vector{
                    "e3bd4bc3a60cddd26c20aa86364bd44f4a07f3302825ad0ac127881de4eafbccf988cb"sv,
                    "199d5423a0e26438f4cea0081a89e0b6c84ca93f7c3120c8104b51c6edc04e0f6a203bb77d59973a7411a0efbe93a09d"sv},
                test_vector{
                    "8716e4b86deff9da2a8ed55baa43582a7586ec9cd38ac3a933156158cd8e5b7887585e91"sv,
                    "0d2306d9c0a8ce57bc7869b439376c07ce352a41d86ab6cf4a5654cccd5c724fe1b62b2c1101c986222f5264ab3fdd66"sv},
                test_vector{
                    "f8a503aaa25ef2cea25e31930c3a90db468cd3a862f4a93aab5de2777e82dc905cb03ee23c"sv,
                    "773ee958fe93dfd1b73af24d27ddce33144a9249d5a671682a56df30d0bbf92b9327130022075185d396de752959304f"sv},
                test_vector{
                    "669025175ea917cdd7a71ff4ec0c45bf1246d2a6f031c00b71de701e17939bfe92128b21911e"sv,
                    "9ff6be3f02c7c5d0206f4b944c0843cb68bea8f9b7c8cc0b729503db5005c7cd5cb14e3457d8f5eabf733fca9084f16b"sv},
                test_vector{
                    "b35fb2262edfa14938a0fba03eb2a25d377974b11f556491a781d0ba2b3c0ff3e42749925fef8b"sv,
                    "835b05a4bf00c2594c3c8c13da6c273a0d9efdea0da72b71b19d326bf5ce968c2e577a7d99fc0f985afd23b46423129d"sv},
                test_vector{
                    "9d86b45df8d7dae0cf6b0bc208666ee1163a39e6116d6d240c9dc1c3a3c1db1dd3b1c6680fe9a196"sv,
                    "a84c469c24696f81d7df4ee8cd76aa584f8c9960eaa9908d3e3ebc5eea7d0b50afdded39deb94fd59323a21a6539e93f"sv},
                test_vector{
                    "414f5619f6dfd45853bbabd224cc305d77350ad253358910a74f3a4381a9b86680b3c4068c089849c4"sv,
                    "848d481e3bbf5dd726f625cf6a444d995b36262c9f80d583b77accf1707e3f49bb3dc480a560694d769aa1ce65d69428"sv},
                test_vector{
                    "e2658324821ae7b0faa0cdd63ee9efb9fcbe82092d04696feb3da92c82521dfdc98f6b41b3ef365d219a"sv,
                    "3ea5d0799f1a4dcab9149a40ab74bec9c8d76d8e392c1e63e080ddec2ec535f80be9f00927be281ec97ac0c882bb0bbf"sv},
                test_vector{
                    "7e80271bb5f2cc7ddae4158658e4e8d06e04a39385da0ecac1cb8e91d68a9bd21ddb7320e79d10e3110758"sv,
                    "fa00bc0359a642dcb3559656094eb2fd4f63bc57f0d34abff26df5c54cc63dbeb4eac75905296e7fb69f871e134083f6"sv},
                test_vector{
                    "43d2828e86f7856b78c66cfa3d602387c290975afd021a8b76af0918069cac35dec45de3cc52edc4ba14432e"sv,
                    "6c9e367e066032ce47ba2575565932002cc786f533c5551656abfe7391e7dcb5f9d9e047adace23d32f8acedfd0cafc5"sv},
                test_vector{
                    "3f49bb645cced7530b8b82e6cf07fbf670f7ef0ba0583d16debafc639bdfbfc99b8417249f7f5a05410aa3a71f"sv,
                    "2b301a14647a67429cc3e7da02c4093a739640f7b47a305251d2855e75e09e60e262b279a073077d1fb6d0f04788f2b8"sv},
                test_vector{
                    "31aac06a59b74bf478617c1637fa6c5593df168b8d58b1e923bf3e3d80e55d7170b16454160ab29ee1f7412ebc05"sv,
                    "ddd245c9b29ceac60506fb6bd6e8037889cb73d6ecc669fd129060a8a8f58971ac572d3ec9b44404f81381d0fd35a649"sv},
                test_vector{
                    "c10b2852054d8034e07906c7fce3ce99402321a648bb881f13fb276afc224c6aecc64800cd767ed2429db94b95a9c3"sv,
                    "a44640fb4ce6dfd4a10290a0aecdb453054a9b54f2583e97bb7dc2b005e5fa2b4fda17b1f75902f51c18c0caad35833c"sv},
                test_vector{
                    "b1eeef324b499f19eba322215fe3ce19c9f000b698d2b2dab7145015046cc86d049ee15ad59dcd1564f30112e06444cb"sv,
                    "38742d18bfa6e918b888d68d1034e61f65dec0759172c2dbf08cf1e132b217eaf4ec29e15db7f4b07e08a70cc5662012"sv},
                test_vector{
                    "790dbba09965c9774dd60a32e010c50d6d518968a220141dc33e7410f2da6c08ad0419bd9864d5327d2c5c44914b2e83f9"sv,
                    "9174958bc8f4ed4731eced999bea2f63032f52bc8c46bcd903232f3fbc5046f0d6c203d43a078b822fc05101404f2635"sv},
                test_vector{
                    "f7b577f1396b23c27eb637e53d3d92460270b001cc612fd3b4d68bcdd09c2d50571ea4350636324cc2428a087e7bd8785f82"sv,
                    "80afe111e44ad9aff9e39c4cf9e6b4c520072b4550e62b1740160a04f8d530612dc098917a556b44977d0e73df518bee"sv},
                test_vector{
                    "7370d9b453936955b9c9d336f4b283237986232de007bf412fb426ff5b4093c80c428c19a12e0b187484dc6d5f4746537fb1ed"sv,
                    "6cd29159820d34e5706dd867e6363758d312660d4daca8d2abf677f234746e97a0a6224eb054066a0b74e18c70965368"sv},
                test_vector{
                    "e8620170f0f39328bdf8888148cfd17730f314ea68d8fea02d16d98a3cca61484139d3ee92b748091dc841dda08483f1184025ce"sv,
                    "29c408a6a5045f397b56dfb5292c7c16028c63f068e699b86a891ff8501208ec9398dbaf463c00f39af7b2cbe45bac15"sv},
                test_vector{
                    "75d4216bad77943bfe82be216157843b0da0fd16eeee847153a00b43e707b2ffe2c898168081f0bdb3af58f214cf67b920c385aca1"sv,
                    "0170357385a20865a8d30c2df39406903ff88c7f70fa1a7a5aaa86711d64046c432a1b139668ae5abed637d1dc4107b7"sv},
                test_vector{
                    "2fa90c2210e3096caed122b74eb9559977120e5d9a97eeb3f99bcba6c19cf8cf791ac6c8a0a94ae49246611dace7f24eb05673a36b3e"sv,
                    "6c2ced3fae94dbd92f4170b63f1ff6fcd8194f60937b22b4f3c95fc9e104b77148f9bc6c16a890de86d9ef1554c96fa0"sv},
                test_vector{
                    "a8de55170c6dc0d80de32f508bf49b7046aeea4b975a511b5ea3dc853bfa4b1e0138202d6785f6b3017914a86f824288d586dc0e8bc924"sv, "2bc3b10c148200f7919b57afe1d7db773ffd235e04fec6897dd94f13ad9c437ef50900a40937f82a39daf2aa2b3dfd68"sv},
                test_vector{
                    "accd9d05fb7ef3043470836137554af117440b3ccca7a280285494f90dfaea60dcbf40b230271932cd3875b1d3dca60d38865ff874180efa"sv,
                    "b9e02df93e9254180d6a15288d77088b5a5ce517644109b4e2532be315f08dee43491608a1127dcdf69397406e23d231"sv},
                test_vector{
                    "32b835c180cc4006c11a61c65b03c099510e1d4f0a94b63d54bd6bd5a8ab207ab0f4639206564edc3fa6af03280a67744f68106dc51ee35723"sv,
                    "df97a1c5dda6f9dde749f213e429db84f0dcd481d43bf58e6142968d629ecf05b262830a7dac87f67f4383975f3e821d"sv},
                test_vector{
                    "9409f9efadbf190b253367629f8f368c9d5ac262e94ab86f3559f9a1fe1a9b44b64e313121b34d43001c430bedc62fc586ea398acd8f17c7cfa2"sv,
                    "e1a69388ee6b6d234108ec29402cd0afd74957d990c7bdb544cf11e8eb2ccd170b6b5a74431be70364d7a31b926ff53c"sv},
                test_vector{
                    "289586baf8daced50db14c936529a0a6438eb5da8d3d469172b6a06f4ff3a956d4f9219563ac285cb8e70074cfcc152c02593a97733c36f4a9e97f"sv,
                    "509e996c1e11611c243021b8b78f2ad90c5a9263bbf35910db7c8ec102aa7c518066fff8ce88562fec2cd6dfe04056ae"sv},
                test_vector{
                    "156530cd6ed3baf1fd7232c7ff204f3c7d4d106016afa3bdff3786e84843ec556115626fdc84b2e874f1074e4f7d53e08079ee9fd01f80a8be7f20c0"sv,
                    "7b8a598029ca0ed475a72c0644ac81c63d72fd51305dada07b0ab4a29e47422f59e12643179269ca3d7d10446b372b2c"sv},
                test_vector{
                    "30655a6b5a5965db992e7248d24141055e988d726abb8e729dc5c21ffcbaedbc0b1b5fea35b8751f6ec6625517312fff2234014176269b60959723787c"sv,
                    "cfaf443e95deeb3cc1910771a2c0692a54b18b3633dc5414e71ae08877f0804818f67f7196c52e26b762dd12bb7a86ca"sv},
                test_vector{
                    "345c3c022e20144e135604078762ef5c8a8f038cf1b1d6a91709b59dd068396a9e971ab628f74886e765384a23607c1a1e6e973f8fbb0ff55104c46f5db3"sv,
                    "bfb1d5ee3a0b629058ecc521c706c2f9241c48cda3dcfdba660a2601d832a7a872a2bb840f3b98d21c37e28f9041a5b2"sv},
                test_vector{
                    "0b94a0f43a92408963a59ded01a9338283a6ff1bfbacd9051a0104445c7f041e8037afde3b5a87d22d5a4796144cbc944091d6cc47b5ffd1f997ab1496ca31"sv,
                    "07a045c9590e9901d0598e604c4649554a823de996fa438cc81a634344eeb98e5f3c0c234ba30e2285a4d7ab568f2610"sv},
                test_vector{
                    "93035d3a13ae1b06dd033e764aca0124961da79c366c6c756bc4bcc11850a3a8d120854f34290fff7c8d6d83531dbdd1e81cc4ed4246e00bd4113ef451334daa"sv,
                    "8d46cc84b6c2deb206aa5c861798798751a26ee74b1daf3a557c41aebd65adc027559f7cd92b255b374c83bd55568b45"sv},
                test_vector{
                    "bfb94dfbe0d9a509b78d164a722050054dad91c9a8e260545d037eb450321aac48ed4459fdd8a41572bd6c9c84d18b6ec339247482cc3ee52a1bbd6bd4ae918216"sv,
                    "13af0be02986ea3176e8c65534ec9f32c23b53c93a73b15c26b9ecbd8a1181ae184a372e9f5e0596cd6606849aeae8e0"sv},
                test_vector{
                    "1c8924a16fa7c602aff5ee961798bd44fe53798bf44c3d6b0d13ef837de07377651c1e94ed236eb79349f86ac31ba151a7e711c5407e65beb63824f6ec39754b58f3"sv,
                    "5be6482851ddafde582f2804071a702ae39bacb688741b7c37bbae99821ce4d3f47d5b097fd8eefa0ef9248a34f5d3ce"sv},
                test_vector{
                    "184215145da49db417e8bdd573d6282de073e674c3dea86b6c78591d4947f5655a9d9eb0e5f4ed046b1d8642da9aefa80448a299504160a1b000c9b4d3c62ab69b3d96"sv,
                    "8995cd7fc0956e124075440686beece17a6256b282e7988a0c998f790e3995c974383179893477bcc32d1f114129b496"sv},
                test_vector{
                    "ca7a63adf41f4da33142910c967706b5c8a093350eb3e6d3aabe69a46a2872f47a39bbe77cdc1160daa450225b0e8e36f506978ce3ac9ae5f75721ef30da46b28f07242c"sv,
                    "b89cc12b11e3afa58908580c47b25407abbf584f8e8d4b5631e9f450464c7e53cfd7e9f9d3cf35e587a6f02957ce4c28"sv},
                test_vector{
                    "1da41a0af202b079521deb6109e312c2ade48544d2b498c07e91a102dd4650ce354f3f201b3ecab8e85e21d667959b43d08f4e90fa18dca2ccca8f6ff5e9a902dc8bf5c5da"sv,
                    "5c297e20c307aab7f325939fd4e2883b034fd547f1dd17fb6b97ade8b148e06ebbf3ff60cbf469e4933d5f48f0166cb7"sv},
                test_vector{
                    "dece42c8849be40c78b8de6da96c2a8d7e940545b9f3f39aa1ca03ec60a85471aa84d8e29f095874f331b90a4c157da9eb048d2c8fd235399672707366c766f10bb833f02183"sv, "bb509e33e9ffcc4c01233146226ee9364cdac5658132460a76edf617a035b197c86434ee889438581458102618769382"sv},
                test_vector{
                    "952008ebdedd480449bb96a025576c5f617bbb8307958a010767e0d736ffe5a196ea4467d8a5d3ba1f5476ff07b6410ae659dcef520a2c14e3902f8b399a289f41f5fdadb502dd"sv,
                    "9b63d9145bc714a8253dcdb8341b2f5714eb58b9d4b22ce45aae07f51297a3dc9c5b691a8a3cd438dc5bd18be400af21"sv},
                test_vector{
                    "100132c315bfc9c4fb93023f5d3500d7208a68acb4d2c63096232c361a161c4c67c0a74bc3e4d72c11664b1d970321d405401924b3a0f6ce2b1a2899e7caa9a55ce725fc37f55d6a"sv,
                    "b6ca04467ed3e623dba36f2e0248cefbe134cf555fdc14731175eaaf08e244ab0b15fca2f173a0ec98feaf359fb84a11"sv},
                test_vector{
                    "99cba4019f5478789e674e08fe5d6ceadd698b0757ca39c605457c22c3d3b8ffb797d2be8f12960f099a5606b908d47207b2636a779948282de3661bb08b1b37ee576590800a492730"sv,
                    "e5378c7c251ae96f0359a30b3134fd77d61d0db68c42a1a2aac293195a596df42f677e6cb98abec90d6722baac63fc86"sv},
                test_vector{
                    "bb327a0bcb2574df47080e8c0d8a45ee1c0424ae0414dc0a9b8717d9f27d8ac987c7c9ecbc946073884d1fb96dbdb583aa758186b16fa429dbf15b8d5bb48cca71469e7ce0ad8e7fa14d"sv,
                    "0f75e65ff8494ae28d9a0a2e65959653275fc34b2fa27b9e10faafff07c45addef3b8f25953d5a2e54e31ebe6d429d26"sv},
                test_vector{
                    "7fd9eeb5ff368040d299fd17a943b21d65deb2eccf6128d18a33eb174693538935374c32c333a867821dba08636f20022c2ce01826c7b7e41640ad186f90ed0ac647d47086744867e5c54b"sv,
                    "007251a2a577add048b1edc79d96c7df8fd5b5fa0d7264f122e4cb54c50bc316a8bc5f4f9dfd4469e29e9b030f563a6d"sv},
                test_vector{
                    "7ca9e369e82186984d5fc729e111a7e5d8ec19c5d74e13b5ab22e4993b05c88ebba6ba72237389a6e0722e12c96c5d6a54515ab00ad80efb38665a76e831abab0fa5cf020807078441585de5"sv,
                    "3ee8c4184de9ceaecd0d3aea16271835f3d45c873358c93a515539c38e819414ea63b08d0a109346793d5e0f703125eb"sv},
                test_vector{
                    "5b4d945d55dea22e37821ec396476a4bfb617d2f392ad93afe67bcfda9cd9b725bc4ccdf516a83fd71dbff5a22b005fc61c58e471240bd2193ce13539730e63232f70f80308be48dab7266a1dd"sv,
                    "df82d242e4cdc2eb40bf3db6a56e1aa0a66e553f1914bedc65c8cc6ad9564b6e85df59f4c443cbe4e0aee05986f7d690"sv},
                test_vector{
                    "e865f4a42bbbd0b73fe275b8ab90d3a9fb74ec5070192d38f60efef9564498b9adb716f31d50cf77c20ae4b2e85515307bb8d95fbeb9ad964001ac550dbc60cf213fd8a522edfaf54e5b1b93b2b2"sv,
                    "091fa9ae2184e2268ef9ef23c7c809efad244536e00aa9e8b3a6c228d90e31da051b40f268a13bd6f62e69c91ae8cd2d"sv},
                test_vector{
                    "1d408c7b68e168f41bb46f9b2e9c8b04f968e4080252546814cc1cb2917dd5690886a9600a09c2673aec0329a4daf655508b06fc1646ef3bb3a472191d964db214a96a96fa89576ce4c4f6dbf1d176"sv,
                    "7e23472c03431925f3b4559d886e8d5d837b3d39b8efe1b7a91e61a13810c4dbc2439634d7c6ababfc66e9b18e6541db"sv},
                test_vector{
                    "54ae030a4e27a05c1ea4a12e69c67544af9b4044cf157dc8cebbe8b2d49f9bc0779077603c90c5c55b891d3ac33b87b65e79e1b19695813718191b3bd8b7e42d5583f7cf1e60f84495b8f869f3719969"sv,
                    "cb65f823585773cb8802b6339182f13752a82864c898b445be5a11a969657ac2dc4a3bbeb87ac0abb232a2b124171096"sv},
                test_vector{
                    "f73cd386f73d0c6ade05771b33117117c602e52693f05b47e90032eacc39295f9793258fe6512eeab291baa0be222e143295a28e8697e42fa27ec02b44217f32a1edae2f4f35213562ca37b6d6cc5ef72d"sv,
                    "f665c4d17a83d65a7ff16bfce279b58558250d76af68b8eb943753e411a57ceb31c1a131e54bcb7672584416e3d5719e"sv},
                test_vector{
                    "1d259612e6867e7d788c71d03c5136864ad6d84f24eaf913a34e69333116f812395288d4dcee6665e6d7dabd005ffc6327e3ca305cab78569d1107a115e619fc90110436317925066726774d1da3639c31a6"sv, "5dcf512e2b93d6ecdf7c3304534554ea79d22392e59bbe90df21e978c9fa3b34ff82e6dcfe8fe2236aa4af4e662e2a9d"sv},
                test_vector{
                    "da8734414c45fc1d5a75a3cbacadb1bfb523d6fc391f882d0db0eef21f9ffd78b6a1e14cfad09e71b65cf7b05d7e8f2f4bae4e454e16068d65465639c729cfa92738563d37edc9676b7be604ffbc68ec3b6b72"sv,
                    "8b328a31adf67dc7aeb864a359628410d5814a2f0cc683303f61432ce32177e1f538feead7e5000343916c7042f8b3cd"sv},
                test_vector{
                    "b228c75903d80fbc6d1cf629ff1d14a92ec4bf0e121fd97bd306ed265efe7a5d5c5d8fc764af98ed6f5978f88d7cd8bcd71cbef6a58261d201de3cb15b3161287e6a104cc2cf882d839f1da0d3f68b426cf08ab9"sv,
                    "fc92ba4eac9a1bf120a7b6c2cc30335b9615b1a98e55d14854ff872966e71040737401c6bc08db5842ceace14cb7e7ea"sv},
                test_vector{
                    "c90d473a6fd30be9a98bf442a9ad65a697d4629c33cd517dbbed02710fa8ee991360bc8e557b0a0bf0b869e6b0c3a9457607580edec3859f2060c9c0340289d53a5d755918ca54876599045a86a9bcb8163795ea8c"sv,
                    "807582b2520e990cfb74367343268b9148b2519b9e7cd9182edb3db9ae7afebedfe8ca118130e2ef9d31af9081da8222"sv},
                test_vector{
                    "6df8c5c28d1728975a0b766cd7de63bbe7f48c3db3e6fd5a4b8df6e3905cef0351f3d973b4f2a7eed80b0de5b85c877353fb9e930ad2679149ad4cbe69910e68d5500b096c5abdbf27d684fcfcf1a57f02769283d5a0"sv,
                    "7bda57d21a4434aada6758e282e612a4c0f41b242f9c790804d5bee25b81a821dc6f2a0ba56f1b3704802c9a6e153d85"sv},
                test_vector{
                    "2cfc76f88cb6fb90927b69526ad5f03d6bd335f4f75b52b6a3c21e8f989ab0d03acb1ebe07e68a87c1b5607acf17d976e10ac4a3d30a8561d49a5e7ec720edace9f5f632b4bd63e104f4894a79caad2e1c31c736453485"sv,
                    "e16670ea837c259e418d3c0e1eaad4948c3457e15b1573056e24da25bff5c66b7e95d24c6bc1b8d6c2b812f64adc9553"sv},
                test_vector{
                    "3f05108c2f33d39b3aa9e73efbad4b011b4e9e9fba409b7611e7e03956b2f3e5e0aa86f68c4bfada5f9223a66d574b08f9dd797cdda8f3c32d8e01921711f4870dec676027ecc56fc2010b496e95cfbf071c820f21edf25b"sv,
                    "b272bab680f3ab27de72d94df384323f8555f1d17facd2588ac8648def2451f82f9b99c05ead8316fd181a2cfb97483a"sv},
                test_vector{
                    "1ffbe1aff0a1e7fa3e68be31a74612a1519b59397e7007ef61fc015f316d55b57be528cebcc2f09a2f22e3c5e4a6ae9612776b17ae87cd763c1a9eabe6846c5bcb347ffc99f10e3b5e64b29a9bd71a5e9b3c01a802715de2a9"sv,
                    "f08bda9d6762607519d53fecb0bffbfd3ff2924854833a759d631e910c42ca86741fc2e290af42e94b94898609b91390"sv},
                test_vector{
                    "f755d6b5642378f2847893901d9aa91c54a4b7abb97c5c7184063e8f1e97aa2de4ad7ac927dd3cce770c906921e2d298f67cf9844e61bf104db803b265b86b821c5f4d901067d07b38764e3f6c95fd4f28e3cfe48d8a9694a8f3"sv,
                    "f85e98ea054455242280854e97c4ed399b85ee7bc5c5fc3d62910a76f3a9600c3d904c832b70b58d7d998db8dc978135"sv},
                test_vector{
                    "773577f3a642c4f13b1cb1f4103e9f6b2da86268a52f449cbb174c8349e3c0dc636ce85c373115a337eee26f7b70ba1060a79a1c76fd186399e6a5255db80f83b0be4a34ba876f7908840553ead380f3195507729d067ac2ee8eb4"sv,
                    "cc27869cd7e63695d19082446b068b77dde4e8604f8c0e9ce20a1b71aa9eff1460f32d5a54476275bdee8e7621491f46"sv},
                test_vector{
                    "651589927e17e1aef780690f3100a377f0179b18b31fd5b4418c84038573fc559b496a782beec3dcf6e9faf5aef676e10bbec34b1be5888fda49b91e02890d2524c5b369f8a54175f29dedf8156fff690cf186ec77104a798315033b"sv,
                    "da846042fb908eee5f5defd1055ff3e57103708278d379a8681f58bedc6ef89670b9f957c4e0edcaa42dfd8cd49df6ea"sv},
                test_vector{"678652600eee42580f73623412e9c011cc02dec4d4cc1b79b27b6f9939695bf2185b2012ab0630f317d2e2de95dd69890e430783e99d7ed121c7c8da9ae70780b5aabf9022d1435cf5ed6da6fc6692c050c2b5f22b24fb1cf9135f9cb2"sv,
                            "8a6ae41c9bccc16eac4860bd5fa91205a86fbfd09692578f7f36b3c135d96f9e9001c192dbeda975f7375bd43a23ba68"sv},
                test_vector{
                    "416d3fb7b401fa5e78cd96d479d8860df147eef03adf13fce1c61131fb89cc2ebc63289745bd7db9bef14571a55318496572dbe52b9b349ef59f406cecd68909f364325380bb75f3aa62503c84f47a55aa6b9c9b199ebe414409ff3964cd"sv,
                    "c5f20542e0c0ac1eb433de6229fe5baccfd4502e2c2275439385efda6374a1d0fc50cd9bba4233d470ad91a3356ea315"sv},
                test_vector{
                    "6f09e876c0b83c9934ffb777f006338c5142a31375e9b21cfea9a7de12998c4ea6708ff1fdf5a8ee6bb67c675ffd8209a10064e2d758a8734eb48f07f7cf3d43b09f1bfdc5d07a52b77079f23cec28bf863bed97c859276df7f7129fce71eb"sv,
                    "b3c968f3025f87dbd5cd3d364bf673e62827c35889532431becd87cfbe2cc75b7ef45696d19cd3452d0e7c2b69d09544"sv},
                test_vector{
                    "0dc2b226b3431c69a76addc018fcbda22bd72c8ff01ed6549596798bd950f361c489a09c95cee2dcfd6401208ae6368d6630026b417cc4718ccb8b42e8872937de66895fd09142c42f066bf0ef3ab2b03803a8185fb65fc7148c376ddd4bf58a"sv,
                    "aa645a4f8f602411260ace24d381f3f5dff0000c246343eb528e3dd027cd743815737906ac5c74ea83c2755e56b99509"sv},
                test_vector{
                    "8dc71c84c8772753c86ab6afd80e8d1df9b0d7e8d69ebe67fa883a82412c26738c3399cab95573b4d3c4367c85c81852d5a6564c0fc7caaafe16c05e62aa06cc9fa542ceb35c88fb6ab82c29d5dcd530f807d3f1c3bcb3974421101d1aa6ac112d"sv,
                    "12239813097124e6248e7dbec985a6a25f622b1d07295cfcfbaff33b847df7fd94265e439fa535f3becbdb576922ac41"sv},
                test_vector{
                    "3df3edd9fc93be9960b5a632e2847b30b10187c8f83de5b45fcb2e3ed475569a8b2ed0784348f9dacce7b323c6b65071abd8b32d1022b1e12787bd4989d3c5ac329d576ccd7608dd336716532e9b4c7f825826fb2e343623ef85c6270619bf5e3b27"sv,
                    "f36590f5211a9cf84eeb0a3b2e5dc1164e813191cda7cb883f3f4a074605ce6780cf2f1a105658706fbd2829db8a2a58"sv},
                test_vector{
                    "ced0ec6500cb891e6433d104ba5b1c1ebca397f3a5eeeaa0f0561729621ea50d4ae7ff1d0827178dccd84e4ca06d9891a90adbb7a9f4994ac947cf6296e71d2f49b826d64b123a7bf86f339fa4679caddbdf19cfed7d0206aa5f527a6b0cc00f52aa2b"sv,
                    "c2c2d7d65d0b9108648e3233d15fc4e4cb62ed8fee9cdd18ab44b8486e2100fbe45ddcf74f46c15eb77fb1c893c12202"sv},
                test_vector{
                    "afa4a2c4fbaffe838dd149c78ea7851ea9396304b41806a093a90aae59c0c5bdb170cc9a7d22b90cbce52cc1b105108942df20c29ef3a913223b915e7ebc98ef135adefaa0f0a6441ea05920e868ce9d1ff6c8fe4dbec06a4849e5e55ad0627f9e09dfcb"sv,
                    "cf6ef91d8567414f5c7f0b1f4ad09a976afc7c8327a382fca90f5a136b19be33100a664390a377f8d8a3015fb882125b"sv},
                test_vector{
                    "00f65a485bfd381113d6e79bf9e0d5e518c891988c073ba198ac3a20f25c2c81619723e88a3c0ed3075075fbbafb6a7b61d3bc336a5e6d6f08d166c4861e6a3bdc2e49b2806b567e7e821a55cb674a6cd699f7dc61a7054a8ff3dec73eb667f59644346be2"sv,
                    "809c6b5d41da7cd10df90b02b193ac7d40cf2e46c139e9dbd208a988da2b25002cdbad1db2ecc1322da20b7d054e5fe6"sv},
                test_vector{
                    "b9ce382e1e82a873cc444248a3008c2cf64d18759057abe8f91c9d87f5dc83aa4eca0c51d30829b9a1d2712da1fac31f52942d77c9f20c2bf6d3751028d7d4f0d336d3dc92b27ec368caa4444b3180c1e37e98b58f25e647a9a6361f0b04cf78d17955766168"sv,
                    "18cd10b3ea907b3770e8eb91c974666e2da2525afe7020b872b3ec6689e5e1cd0059dd4fd49ce44d75dc4c8430c322d6"sv},
                test_vector{
                    "6778d82f3a98eecdfac55ddeebc52476a070094fbd65831801fdd60f837d80d23b90d472c5f4e5ca6273a50f40154ea8fb94013f6310ad18800433a1d379c84bdf799a99e8c7b676fbcd29cc2ed66552297de7a6e565179bb42b70d48299e0925a1d72ca2c792a"sv, "71f08d9333df5cb885fd23d6cbb1db84f9b55908d069df50fa4795cc713a18439bcab8daca078356f5c75a619f2f8782"sv},
                test_vector{
                    "ba3a54a77d7c2b8ccec6d53136cd48827c87acdd1cd86ad1f56e862642ea2e1dcb4093f85d60f7bd77160738462e6c3fd3dd9d3a7c5cf7e2a1d60f489f84471902179f21f656ce0fff089278ea14441e04e7af71891622565f44c428044728fcc686212a32a5d809"sv,
                    "3cc154f0542d8e3cacf9d79f231416816178a7ef2275fb257a48a2f763ffa2e15a33c27b970a416a057925aa0412d268"sv},
                test_vector{
                    "7eec4f4f491b4eeaeb1cdbdb95e9511c2872372bf64a1f61cda1cd8032729c8beafd1edabf78036d80023c814ad8606106cb4e7f33f214c3e69c0f230e885474fd594f7f2444aa581e0c70ebf13073d89063eb81a43c5f608b2fc99fa1bcf5e2bfe62a6802e70c52ce"sv,
                    "2f8c5682a07438043e55f0b7759fe7eea5d9bdfc8b0f89800ebcf777bc05a941ea7f3cac45d4659de0f505d814590b6b"sv},
                test_vector{
                    "f3715b9e3ddd7862e15ee87aa23f1aaa0580591e55cff3fee9b49b42aa0c0cc8cfb8efa3eb96ffb72ab06b83d7b47b3d22a5772421cfc51214005150edf532af10138ad45758add459908601eccc3703e810002a2e4c6202e98d84281475d55d3de9f3d98809cce1f665"sv,
                    "04e7d55b0eb4bc3d3a21cfd2941dbb4dc44706588967186b40da54902aeea97b262c97f75e37ebe3cd60a804e7b9feca"sv},
                test_vector{
                    "dfd7d792e162bf7a889109550a0fc4c415232af0c0d72dcbc2595299e1a1c2aeae549f7970e994c15e0ab02f113d740d38c32a4d8ec079cd099d37d954ab7ef2800902cdf7c7a19fb14b3c98aaf4c6ad93fe9a9bc7a61229828e55ad4d6270d1bdbca9975d450f9be91e56"sv,
                    "08e5ef57d0c2aa23edfc75cfae39e6bc1a43b5db08b2e27bc9823114edf760367db9cf3cd9c3779755f6d39e219b7079"sv},
                test_vector{
                    "ffbc7c47f52e69f5c667f4ed578b46ff4592048f789081f3fb39321d0aa4627a6d4f261905649410a53a301c231fa787ae55c04f615a8f84196cecf2844d23007b44edd892649fc8ed10a2e855bf23fe8afd0b9edbb33296f5a7cf89f94634d9d1a2b8cac3b7f4e546f2329b"sv,
                    "be10c7baf94608408a0accfbc8ce95e159d08d8ca75dd6c273f935947a7ec3463e10a58d3ceaa0b2198b0887a3a24a29"sv},
                test_vector{
                    "a39f202d866e8e96765fbb53b6772537dec043322f4a7475247036d7495c987850cef2a46218d3fab36e3bcd595c0aca5e98b9db14fa484ca8c9772dfa3839f7ac3066727a50b0d5c933d82f82f1220720e8063f08bc283f199c8a4f85c70043df4fe55e751d0d71df36fa43d8"sv,
                    "3b2664ccb555a1b1f3ec996860146ea75ef7f3bd62028a19c26f63339399f4275a07f3c064d34766ebe8e4dd532f6629"sv},
                test_vector{
                    "b2c8261853e2218dfa135cb5387810352b8962e94e9fdc8695b41e7dba6ad122d14fdd0d2360dcc039ccce8b37fa0ead6ccc85bc26261d47cbaa78b925c6e380fef1856fed31dc616fe16b2039b1ac85cdee4ce04c0497998b41321868db08e35f358606585e0bb8c3da9a3be7a6"sv,
                    "45b286f49fd05c45c921b7bfdbe2cb024441c372e07394dcccae0de834cd541f13a79dbb3e5078896e88438542bd2f12"sv},
                test_vector{
                    "a04f390a9cc2effad05db80d9076a8d4b6cc8bba97b27b423670b290b8e69c2b187230011c1481ac88d090f39154659494db5e410851c6e8b2b8a93717cae76037e0881978124fe7e1a0929d8891491f4e99646cc94062dc82411fa66130eda46560e75b98048236439465125e737b"sv, "e7089d72945cef851e689b4409cfb63d135f0b5cdfb0dac6c3a292dd70371ab4b79da1997d7992906ac7213502662920"sv},
                test_vector{
                    "f419494c3c6d0727b3395a483a2167182a7252f4fd099c2d4b71b053f94bb8b3adf3b51e8460cfec084ce9415c95798fbae4975c208c544645b54c44d2b97f2ecfce5c805be61f5ba1d35dcc07afdd51a87baa990506668cf710e18be9b0ebf943f366fa29c69f7a6616de72a3353b66"sv,
                    "aead8688c58c6ba4e9cadb4756b465dce0fb06f1cfaa478197f2ea89414e47e9572034adfed160703c79b82b3fd7ab78"sv},
                test_vector{
                    "aaf7584d53006cbf2d2040e51b7feebd2bbf1e9f6d817cd8062a6a9680e7f10464eefeb50b07cb46b14b9b3fcb2caa3b9ab664490115d5919456613bf172b58c5388fd52646a5783535b88212717ef605314b70b8a085024d4ab1fcbe2be74609e4cbdec0730fabd3cd77151d647a3767b"sv,
                    "e6e79d8c61d0ea9fc70dd4dec1fa432849c3396e717b170badbd87a4c7974efc590ab8c1183a6232beff14534f004b02"sv},
                test_vector{
                    "a467f77369730201f2812204fd63ad0d2757be580d937dfeb221a06b21ed3213531d936152a0c1f09f0ad5fed19fd11e80ad982c61203e86b2508279d91d99fa483e2e97a3d6a6ad2548a8da404dddb58344f4bdc1c9ea907088885e4f532d9c4c73cdfda43c3a9e4ce5a2809096593cfac1"sv,
                    "5a7508c2cc096bf65a4d4d337aea22008edb9a3bae869f94e09fb526a52c3368e9b28576fb950f078b7e43b5562120e6"sv},
                test_vector{
                    "01abc90e9180fc9bb8ea67a405073ed6848bf33048076566476c55836bcb19d3e55e9400c5cc657bc7a95f1d703c390f5a8687e7cd7fe9138ea3837bfcadad6258a3eb8d65121fa8319bfde532aec0e694961bddd2b673f284124be5781100f40381b6ff99db92ea9cc82a4372e53924dac398"sv,
                    "96b209d7cb2c2033b38350744767fa5c253e1bfdb99fe8418bff83804df02248140fe3b77b0bfd4a79b51f70405a434b"sv},
                test_vector{
                    "b555d99056362bfc2bac2a1bbb71ba112d644e50b82b015e5a1ce3d9cd5e90b8b74b08d32119baa62abae251fc0015e400051ada4ecafce3681e5de727c20d47f5cadc663d46ac682022ca396a4b7ed1c413e0b72bd7eec4a0dfdc2a2185abb5d99afd50940528ca75ad89daeb9a1b61e15747f0"sv,
                    "abd39f79d72c6cd2e8e130f3603032fe3cef4177c3563930096df1f10b87e79cd4e2059cf1b9f8252184bb26f659a5da"sv},
                test_vector{
                    "14fb01ae9d6015ecb3e56d6ecdfa4bc0533186adf8457f5e4a5c57c687895f3db395d06ae7ffbd67ec41452009550dfc1878eec0df2eeab09e8665f7e59f9148a86b2bc695b36521a55b2302f2e869aac83f14d6feafc9e587322c3c44f052ea1c0578884f84f56307bc6dde31ba48118a0f62b6fd"sv,
                    "fc9b9a95a8ce1c157720cb63101a7594df24f4cc74baf735b0ccf6abb925478ad507cd048d30cde1c788806f43ed3a81"sv},
                test_vector{
                    "11ae0cbfee7bb3df90ce585f09b9cf8ff5bea69a68eeb6c22553f8ed118c9a61e7b752cc2c94f38766e63e1b891dfa05b23347b617d42fde3ec17eea6e76d3cb640bf8fa46586fb9dd5c4d9bfee04c4649571b781709f848ad7081afb6e2c746f071a551251050fd5df72ee65248ecdc24f2cbe74ed5"sv,
                    "3214b5feec925059149fa852e3ae285a6eb377df926504e2f824572a3aebd2050a20144e7bede7e7fe238ee83e69f72c"sv},
                test_vector{"a5c4a47a04f4714269d5d922ba4694060aa2df49193720c819fac93bb8787ec55a107ac9a6602f0045fd2cc8e66744bf863ced91eeabe60e7d2c1d80276ecf3bbe91f1757096cf589214f3569c2c48bd74be7f8befddb28395814780a47c180a58b0d0276a7e9873d682f473e27de7275c925ede23b6cc"sv,
                            "6bd9e1303579d815f58e8c6c9855850133251778a632f7b312c4b3163b29b5ef6cb9511a08a31a237d9a7604afbfa056"sv},
                test_vector{
                    "75264774af69ec7ee3125e205bd6d1cb8fdb22f7ea6dbe72d1f2c0f7e2205902796d75e379c79b11498615c21c9f52b8761a885eecc69d132b2b48c63bc074c3055ee5cc13f51d6c987e8188b030b837e8f754d40122b451f15b28cd2bdd576920e1de5806593a36d8e1e89b9ef3caefee5acd80b3e9c9d1"sv,
                    "ffa9e4e856d06227c1ccb959be558309cc10633173f4b66ceb382923b52b9150acfb08a7393500477a6ee4425a827e76"sv},
                test_vector{
                    "791a36d748695e62db5003a8ad367df1f051c1ac6a21d711823e8e069b546e3fa06ceeaae06de70a1de249e1dffd0d940edc6acac00c4c15504c02d4b0933658005423455f00023b01cdc5b681b6083379c24595518a47c654f5e1a10947df10c05a3d716b2a973faf98e1ee3b675816598bb8d4c2a3b06eb7"sv,
                    "f8d33369680524ed6c6a716d4c502de3eac891f340f40e82e37501be1a90fb61d26e5daafe92bc6ef5ec0ae431168842"sv},
                test_vector{
                    "74cd86bbed14d895301d8a54b2956b1c5cd1451eebab620b978d4ecef2cbdf7a14367381f5ee79281a773337740fb9f5853f4253c4b19f684341081d8f561b2ad773224151099588da90e04dddd565f67596c536d64c5b87e9480ad43601397507ad1b61ca0e349fb88f19feb48f770676fd562ee8259f50d0c9"sv,
                    "73ee8d29c30821dcdfa4441639f037fb6ba3a9ca596dc43428043785756608f6207d80b7f78e573174fb9dfd42f0b8cd"sv},
                test_vector{
                    "46612e1a4d016c41700a3b5ccb38323353bd8da5b9942c9c9212df40b4bee06be362a15dad62c8b2924d789168b2d325fe35bd5100e1e29f1ac0fa7a60a94c9eee5a70cccbc75ac214b1946a5679cb523b378d5c690751b7a7a3b80d413712feae7024ce71d6295a3d5d16515c3622a052eb862ebdab81ca7fe3a0"sv,
                    "cac13a3784225be03d526f9abc1eb50a762e72c0e01172a15d578801089e5c9f26e53cc00ff755909453e2964d7df838"sv},
                test_vector{
                    "1a857c1f105d068ceab0b1e12494890ec196362a48b0200a0d75d712b18fb14bec6bb5b68a33b7e0b4fdc5b77142c29c6d9151b9f884f59551f476e5256986a653d4a468f28141ed954721f2cd02054df0438738194545ed70234173ac4988b7d62812de4f2feada14f68e3b41bc9948d4139f4fb4da5939f26c0261"sv,
                    "b75d92b5cd3772846f7b68064a3567ae43e5010099f2b649f35087e7592aeec176646fc8c2629288944261cd35b5fcba"sv},
                test_vector{
                    "9d36818d0c5a008be7904d1917aa3eccb5ef4f38cecb8c4e63c4b2e9b4b091a3bf25b4ed0332445f894c2a4c258b749afa17fad03cdd41713a869f899ba9a085e73fa9474a58db7a950d3a2386b60f79495d8bf73e72acaffdbf65e1989ff9cc206ba8c46a368d8512bac7c7c191d713aca949d45df297b1b6594a1a33"sv,
                    "a88da844579f3e3725d00daa8e1e287da4bfbb2d931ebe8d3b16021154be503341d6371d382ada744f86f5bbb56cdcfa"sv},
                test_vector{
                    "4ae50ed626ee60dc5ea556e3ce0d3c18b9e6225b5620814e8b9621acf8f939dd370ad9c7620d85e6d67a229f37f517d1b580acae8df0b5c9d29d756f6d5ebd3b63b554e556469b5b4f8e7113bad1559fb254ca827fcd00425d18b0be7f2b48c2544c48d90982ec624f490be65e893fa93ac9467f35a0a8e1b56d9a403748"sv,
                    "f8cd943f429333c7c8d30a8576827f92c92ae18e0dbcae770601b79687bccf8c23e9e589dfeb45c3b9bcafdd545e45e7"sv},
                test_vector{
                    "dbed7612448d46cbe0a384d1c93233f02ffd1c984ba765299518656d3723b766c1658d4b1e7047cdc729459e366ef9349efc40cbd990f2a9a24db7a5045e1dea12dce8f9d9f2aaed933f93031e7b8959ac5e7bf6bbbdf30b48f7eb783f8fe292371a2f245c5c94b4acae160767a20ce7c0ea7723d97691d8eedda9efd1fe2d"sv, "fb531a1ed181c732311e56f4b56ed91dcacc0dd6bf1eb4a44be6f87dd7cb1ef9dfb0310f4a79eaaa3f32bf3914d8624e"sv},
                test_vector{
                    "3bf52cc5ee86b9a0190f390a5c0366a560b557000dbe5115fd9ee11630a62769011575f15881198f227876e8fe685a6939bc8b89fd48a34ec5e71e131462b2886794dffa68ccc6d564733e67ffef25e627c6f4b5460796e3bce67bf58ca6e8e555bc916a8531697ac948b90dc8616f25101db90b50c3d3dbc9e21e42ff387187"sv,
                    "12b6cb35eda92ee37356ddee77781a17b3d90e563824a984faffc6fdd1693bd7626039635563cfc3b9a2b00f9c65eefd"sv},
            };

    for (auto &&vec : test_vectors) {
        INFO(vec.message);
        INFO(vec.digest);

        std::vector<std::uint8_t> plain_bytes;
        auto r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.message, std::back_inserter(plain_bytes));
        REQUIRE(r);

        std::array<std::byte, ivy::digest_length_v<ivy::hash_algorithm::sha384>>
            digest_bytes{};

        auto nbytes = ivy::hash_data(
            ivy::hash_algorithm::sha384, plain_bytes, digest_bytes);
        if (!nbytes) {
            INFO(nbytes.error().message());
            REQUIRE(nbytes);
        }

        auto digest =
            ivy::bintext_encode_to_string<ivy::hexchars_lc>(digest_bytes);
        REQUIRE(digest == vec.digest);
    }
}

TEST_CASE("ivy:hash:sha512", "[ivy][hash][sha512]")
{
    using namespace std::string_view_literals;

    REQUIRE(ivy::hash_available(ivy::hash_algorithm::sha512));

    std::array
        test_vectors{
            test_vector{
                ""sv,
                "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e"sv},
            test_vector{
                "21"sv,
                "3831a6a6155e509dee59a7f451eb35324d8f8f2df6e3708894740f98fdee23889f4de5adb0c5010dfb555cda77c8ab5dc902094c52de3278f35a75ebc25f093a"sv},
            test_vector{
                "9083"sv,
                "55586ebba48768aeb323655ab6f4298fc9f670964fc2e5f2731e34dfa4b0c09e6e1e12e3d7286b3145c61c2047fb1a2a1297f36da64160b31fa4c8c2cddd2fb4"sv},
            test_vector{
                "0a55db"sv,
                "7952585e5330cb247d72bae696fc8a6b0f7d0804577e347d99bc1b11e52f384985a428449382306a89261ae143c2f3fb613804ab20b42dc097e5bf4a96ef919b"sv},
            test_vector{
                "23be86d5"sv,
                "76d42c8eadea35a69990c63a762f330614a4699977f058adb988f406fb0be8f2ea3dce3a2bbd1d827b70b9b299ae6f9e5058ee97b50bd4922d6d37ddc761f8eb"sv},
            test_vector{
                "eb0ca946c1"sv,
                "d39ecedfe6e705a821aee4f58bfc489c3d9433eb4ac1b03a97e321a2586b40dd0522f40fa5aef36afff591a78c916bfc6d1ca515c4983dd8695b1ec7951d723e"sv},
            test_vector{
                "38667f39277b"sv,
                "85708b8ff05d974d6af0801c152b95f5fa5c06af9a35230c5bea2752f031f9bd84bd844717b3add308a70dc777f90813c20b47b16385664eefc88449f04f2131"sv},
            test_vector{
                "b39f71aaa8a108"sv,
                "258b8efa05b4a06b1e63c7a3f925c5ef11fa03e3d47d631bf4d474983783d8c0b09449009e842fc9fa15de586c67cf8955a17d790b20f41dadf67ee8cdcdfce6"sv},
            test_vector{
                "6f8d58b7cab1888c"sv,
                "a3941def2803c8dfc08f20c06ba7e9a332ae0c67e47ae57365c243ef40059b11be22c91da6a80c2cff0742a8f4bcd941bdee0b861ec872b215433ce8dcf3c031"sv},
            test_vector{
                "162b0cf9b3750f9438"sv,
                "ade217305dc34392aa4b8e57f64f5a3afdd27f1fa969a9a2608353f82b95cfb4ae84598d01575a578a1068a59b34b5045ff6d5299c5cb7ee17180701b2d1d695"sv},
            test_vector{
                "bad7c618f45be207975e"sv,
                "5886828959d1f82254068be0bd14b6a88f59f534061fb20376a0541052dd3635edf3c6f0ca3d08775e13525df9333a2113c0b2af76515887529910b6c793c8a5"sv},
            test_vector{
                "6213e10a4420e0d9b77037"sv,
                "9982dc2a04dff165567f276fd463efef2b369fa2fbca8cee31ce0de8a79a2eb0b53e437f7d9d1f41c71d725cabb949b513075bad1740c9eefbf6a5c6633400c7"sv},
            test_vector{
                "6332c3c2a0a625a61df71858"sv,
                "9d60375d9858d9f2416fb86fa0a2189ee4213e8710314fd1ebed0fd158b043e6e7c9a76d62c6ba1e1d411a730902309ec676dd491433c6ef66c8f116233d6ce7"sv},
            test_vector{
                "f47be3a2b019d1beededf5b80c"sv,
                "b94292625caa28c7be24a0997eb7328062a76d9b529c0f1d568f850df6d569b5e84df07e9e246be232033ffac3adf2d18f92ab9dacfc0ecf08aff7145f0b833b"sv},
            test_vector{
                "b1715f782ff02c6b88937f054116"sv,
                "ee1a56ee78182ec41d2c3ab33d4c41871d437c5c1ca060ee9e219cb83689b4e5a4174dfdab5d1d1096a31a7c8d3abda75c1b5e6da97e1814901c505b0bc07f25"sv},
            test_vector{
                "9bcd5262868cd9c8a96c9e82987f03"sv,
                "2e07662a001b9755ae922c8e8a95756db5341dc0f2e62ae1cf827038f33ce055f63ad5c00b65391428434ddc01e5535e7fecbf53db66d93099b8e0b7e44e4b25"sv},
            test_vector{
                "cd67bd4054aaa3baa0db178ce232fd5a"sv,
                "0d8521f8f2f3900332d1a1a55c60ba81d04d28dfe8c504b6328ae787925fe0188f2ba91c3a9f0c1653c4bf0ada356455ea36fd31f8e73e3951cad4ebba8c6e04"sv},
            test_vector{
                "6ba004fd176791efb381b862e298c67b08"sv,
                "112e19144a9c51a223a002b977459920e38afd4ca610bd1c532349e9fa7c0d503215c01ad70e1b2ac5133cf2d10c9e8c1a4c9405f291da2dc45f706761c5e8fe"sv},
            test_vector{
                "c6a170936568651020edfe15df8012acda8d"sv,
                "c36c100cdb6c8c45b072f18256d63a66c9843acb4d07de62e0600711d4fbe64c8cf314ec3457c90308147cb7ac7e4d073ba10f0ced78ea724a474b32dae71231"sv},
            test_vector{
                "61be0c9f5cf62745c7da47c104597194db245c"sv,
                "b379249a3ca5f14c29456710114ba6f6136b34c3fc9f6fb91b59d491af782d6b237eb71aaffdd38079461cf690a46d9a4ddd602d19808ab6235d1d8aa01e8200"sv},
            test_vector{
                "e07056d4f7277bc548099577720a581eec94141d"sv, "59f1856303ff165e2ab5683dddeb6e8ad81f15bb578579b999eb5746680f22cfec6dba741e591ca4d9e53904837701b374be74bbc0847a92179ac2b67496d807"sv},
            test_vector{
                "67ebda0a3573a9a58751d4169e10c7e8663febb3a8"sv,
                "13963f81cfabfca71de4739fd24a10ce3897bba1d716907fc0a28490c192a7fc3ccb8db1f91af7a2d250d6617f0dfd1519d221d618a02e3e3fa9041cf35ed1ea"sv},
            test_vector{
                "63e09db99eb4cd6238677859a567df313c8520d845b4"sv,
                "9083e5348b08eb9810b2d15781d8265845410de54fe61750d4b93853690649adc6e72490bc2b7c365e2390573d9414becc0939719e0cb78eca6b2c80c2fda920"sv},
            test_vector{
                "f3e06b4bd79e380a65cb679a98ccd732563cc5ebe892e2"sv,
                "6b315f106b07c59eedc5ab1df813b3c0b903060e7217cc010e9070278512a885008dac8b2472a521e77835a7f4deadc1d591aa23b624b69948a99bb60121c54e"sv},
            test_vector{
                "16b17074d3e3d97557f9ed77d920b4b1bff4e845b345a922"sv,
                "6884134582a760046433abcbd53db8ff1a89995862f305b887020f6da6c7b903a314721e972bf438483f452a8b09596298a576c903c91df4a414c7bd20fd1d07"sv},
            test_vector{
                "3edf93251349d22806bed25345fd5c190aac96d6cdb2d758b8"sv,
                "299e0daf6605e5b0c30e1ec8bb98e7a3bd7b33b388bdb457452dab509594406c8e7b841e6f4e75c8d6fbd614d5eb9e56c359bfafb4285754787ab72b46dd33f0"sv},
            test_vector{
                "b2d5a14f01e6b778888c562a059ec819ad89992d16a09f7a54b4"sv,
                "ab2e7d745d8ad393439af2a3fbc9cdc25510d4a04e78b526e12b1c0be3b22966872ebe652e2f46ed5c5acecd2f233a9175dd295ebeb3a0706fc66fa1b137042b"sv},
            test_vector{
                "844b66f12ba0c5f9e92731f571539d1eef332e1549a49dbfa4c6de"sv,
                "c3f9c5781925774783ae9d839772d7513dfcea8c5af8da262c196f9fe80135b2b0c8c6ca0a1604e0a3460247620de20b299f2db7871982d27c2176ae5fa7ad65"sv},
            test_vector{
                "6b6cc692d39860b1f30203653e25d09c01e6a8043c1a9cb8b249a41e"sv,
                "2e5263d9a4f21b210e0e161ed39df44102864325788647261a6e70ea4b1ee0abb57b57499bc82158d82336dd53f1ef4464c6a08126e138b2cc0892f765f6af85"sv},
            test_vector{
                "ab1fc9ee845eeb205ec13725daf1fb1f5d50629b14ea9a2235a9350a88"sv,
                "72d188a9df5f3b00057bca22c92c0f8228422d974302d22d4b322e7a6c8fc3b2b50ec74c6842781f29f7075c3d4bd065878648846c39bb3e4e2692c0f053f7ed"sv},
            test_vector{
                "594ed82acfc03c0e359cc560b8e4b85f6ee77ee59a70023c2b3d5b3285b2"sv,
                "5ef322cb4014ecbb713a13659612a222225984d31c187debc4459ba7901f03dac775400acfe3510b306b79894fb0e8437b412150c9193ee5a2164306ebb78301"sv},
            test_vector{
                "f2c66efbf2a76c5b041860ea576103cd8c6b25e50eca9ff6a2fa88083fe9ac"sv,
                "7978f93ef7ed02c4a24abecba124d14dd214e1492ff1e168304c0eab89637da0f7a569c43dc4562bdb9404a018b6314fe0eebaccfb25ba76506aa7e9dcd956a7"sv},
            test_vector{
                "8ccb08d2a1a282aa8cc99902ecaf0f67a9f21cffe28005cb27fcf129e963f99d"sv,
                "4551def2f9127386eea8d4dae1ea8d8e49b2add0509f27ccbce7d9e950ac7db01d5bca579c271b9f2d806730d88f58252fd0c2587851c3ac8a0e72b4e1dc0da6"sv},
            test_vector{
                "9f8c49320af9370cd3db20e9b50d3eaa59a6232d7a86fb7d472f124508d7968b05"sv,
                "81b002f15c4d48be8517f7ed89df302fb1435c9435efefed58f3eb8ea11910623f1eb9028a66e02121a7f08a7c604226f2324f483e91548dbbd2c441ab704ce5"sv},
            test_vector{
                "4ab9aa069475e54b25e5688a52dd4acd134169c858105f01a0a1b134c72d4af51f8e"sv,
                "48ba5a63aba7e7bd8e420475331125a947928c67fdb00f65c4080d9a0b99c0672424e76a1ba6bd76dfe492c730f6f9adccaee7bb11571aadb31f6bb628cfa933"sv},
            test_vector{
                "f0c1d3407de92ef7421e42df5c9ab31d2ec0a750a9522869cbe4cabd66908d5823ec04"sv,
                "9e75c5bca2c2af1d7739787f46e1d981c4f98e493d0724b5252c2fbae3c526719f1d27e6ccd0d705240281e8fbf3db75b9b3205c1413436d3b5d140004b8cca1"sv},
            test_vector{
                "ae8c9f8fb41b519b6d943833fe1c32d1c4292fb1ddf1dbe2eb227d9e14d31ed74ebaef12"sv,
                "042f9fd0a4ed3d9fec3655ae11011c6f2bc7e457e8812b6d8be2cd45fc6c432a94558c88f22c01439618865e8e49e509c448b342ca914b120344aaf7bcbdca18"sv},
            test_vector{
                "da39fb86237f00303844e61fc6cfe779e42af53349839590bcd2f0e4cbbc279ec0b7e885d1"sv,
                "ecb43de8c233a731b38e30c5696f8876761b7ea72efe283fd07bedf20029f47c6d2a4427823e100fb087abaf22d7eff42a951c97c3dd05f48a20163fa4367cba"sv},
            test_vector{
                "3e7271d2070ef095394620c4b016576c150f34bea60784613a0f660d7fa5ae56872b88c58398"sv,
                "8154d0da634ab2266061acc123acb407650ffe9164a22de3fe29bf05393b2aece92cf4db00ea5b4341c31ddb7de151683c8a71b5a44d5c3175790feac67d18ee"sv},
            test_vector{
                "311fb67f6a0784bb01a2d5a3f3092c407a9d3322319dff9a79f894291c5fac37319fb408402e18"sv,
                "1870fe913abb0a4b4f53b6581ae18322cd05328514556607f3f4d7b6a2ac8e9185d94d947d8b9c88e0efa66d89b59f7439c75fdadd1816f7412306ab2b59d664"sv},
            test_vector{
                "7651ab491b8fa86f969d42977d09df5f8bee3e5899180b52c968b0db057a6f02a886ad617a84915a"sv,
                "f35e50e2e02b8781345f8ceb2198f068ba103476f715cfb487a452882c9f0de0c720b2a088a39d06a8a6b64ce4d6470dfeadc4f65ae06672c057e29f14c4daf9"sv},
            test_vector{
                "dbe5db685ed7cb848c094524c1723519d49dc66ef9fe6d57e6862a6435750bfa0a70f104f5d396e61a"sv,
                "2fa6e5b2c443a68050f093e7fb713bd6b18f6274c061ed61d79bf0688a61dba1940bcc30998276860943ab038902896d0fbf59b88b07c80de927037097150c40"sv},
            test_vector{
                "9fa83e96b2a6df23fb372895015678e0b2c9cd18a8542c3eaa2c435a76ae4dc9bd5136d970daff933acf"sv,
                "3a2c0ec88a3e5347cf0ea9c078838300ef7356f9a6c342063277c106b880a00ed2be205c13064097bd372fde38007bc306561eb4e74bba2bb20bd354aa690ca6"sv},
            test_vector{
                "8a5a45e398bac1d9b896b5a2b4e3566b91d80ad20c977ea7450ff2efb521d82f65019ee762e0c85c6cc841"sv,
                "3c704620f4066d79c1ff67752980f39ef3d9c1023fa5a213a5265376b14a15166ffe069b51df7710d8907fef9406bf375d502ce086ac82aff17229aaa7a5a334"sv},
            test_vector{
                "49cfffdaf4d031e33b1d28a447450545f6c4293b38d5afbcb9883976c014f080576ec691ac1bff70b742efab"sv,
                "8bcc4f1ea2b7862ef1591bfa73916665de8faf65439ddf5cc1be43cebfd5f60f205e835a2b186b675b041258c5cff42669316ce25b46a2f4d4218e102f0f5d6f"sv},
            test_vector{
                "2ff845d85efbc4fa5637e9448d950496f19d8d57da99b7bd3df7474822f0a790586736416714e364c6e1fae04e"sv,
                "236f6f4ed6e858c02d51787e60c578f731f694f8e52b5df4ecd5b04dff14c78e56bad1028d6f626c29d85aeee151a2a2846d3eed5cfafa9854a69fea8af6d04a"sv},
            test_vector{
                "cfca05fd893c0f005f5ff796f4da19ba27a1e729956b8b715e67ce4b2d2a382a72ec7814f2f507b1825209a20fcc"sv,
                "d80969284a4565add4dad6ab9b3bdf53446142f84aaf92d4b23dd22ee7241e6c81489ac8b246edcb6df9bd7b23d91a0c517f546feba4ed5790a2be6e165c1709"sv},
            test_vector{
                "cfc425759a9c36bb9f4b32eed7767af6566f68ded0adeae25c7a70ca78ec09774d16c8bc357f6d6f7bd441bf62d942"sv,
                "b587a785cdf455cc9c544e756c1e306300aa3c59f8725012e68ab4d54020b6d227a164d9f83c905e86f8cebeef708a69f976d6e7b18b9bf78e9b98cc4a5cd1b6"sv},
            test_vector{
                "097c9db919515242c99d973acb1dc4ed482768f974eb83b465f9f6c82503372006e4490835e2ec8f92301130bfb790b2"sv,
                "ff5a376f938e73014caef7fe3962944a7230d020b7087869ebe7ec70302721cd06fcdc981c893a425d05e2f99fe198e4db50a088aee2bf1263212110efed422c"sv},
            test_vector{
                "77e73d387e7bc80419ebf5482b61d5255caf819fb59251ff6a384e75f601ea026d83ef950ed0b67518fb99dee0d8aaef1f"sv,
                "c4c89cd882ec945cc888fb9a0127d35e585ecc14a75e4b5b3d8330538d22da28cf6af1ebec96dc247f109cd2aaab9756e6946a3d80db8363a4da3e6ddbb510a1"sv},
            test_vector{
                "317e5d9ac73ed0633fa18ebebbca7909ec3a5ef790478f9c38cacec44f196d895835b425774483043341381e7af2d383e51a"sv,
                "b10bb04491b9c0c334709b407cda1d503efb6b63ee944f2d366b6855e6e63e5b80115be4be7ff63edecdfb5923792e68123976d79212b3884dec2179d1fcf382"sv},
            test_vector{
                "209461f20666a346fedf4a530f41a6fa280c43665767be923bc1d80bbcb8c9f8f93ad75782ea2689c8c5d211d2053b993145a0"sv,
                "67b7a328d9444056a52ca2f695c5d3f3baafb625a14fb32eee8ff26a40ccb296bec1771a826b55f7ddb6170d4caf7795b612448e66a0f19356fe505927149b47"sv},
            test_vector{
                "5d61aa45c446f3bf93604b0511313b4e2f306d6b046fbd94797b926746836f2e1dbdc56124060c6ca9c911b1122192d112420827"sv,
                "d3931bde2bde8271ed18ca0b9148b12f6f16161e637e376fc961f65bc33bcacf2f6addf26a3eaa81b196653cc37e8a739ec5b3df870d8c38c8f28691c22a39bb"sv},
            test_vector{
                "9288c795bb0b86c0419d9c5637dcc37b39bfa18d441e3fbfca75bc0306e5432e8e7b3a5627b5bc7fdc424a77520abdff566e7f2bb8"sv,
                "e363d0e95d8cd18c384016ebeed6d99c4fa2768e2bd58fca019c5108b9cde1cb46f3f884028a55ce282ec310a10037faa1b16b4a6a669957f0b00f350bbd63d0"sv},
            test_vector{
                "780427dc164b2f69b8c7d569266f461e2d30c88c4cd6057fb030a6cf636f24e3c0d0db742a7b6193fdaa15eec50dfb4fae6ec7653c91"sv, "2964b009fb1bf996de12e030b9d6e0608ae8b9dbf2acfb9beb76fc5361cc104ee85c2a46fb7b4cee90848312da302de49afe61c546477e2b25d223d5e3d33560"sv},
            test_vector{
                "ec2a92e47f692b53c1355475c71ceff0b0952a8b3541b2938270247d44e7c5cc04e17236b353da028674eab4047d89ec5dad868cfd91ce"sv,
                "c83aca6147bfcbbc72c377efa8d53654ba0830c5a6a89e1d2a19b713e68fb534640deb833ca512247166dd273b5897e57d526f88eef58f6ff97baee0b4ee5644"sv},
            test_vector{
                "c99e31ad4e23ac68e15e605d0b02437f8147c44f5445a55b68a10905276cce8676481c33e8cd3efe322bb13fe0107bb546ccbec7b8b38d10"sv,
                "52992d45a88221d972958e9f2854adaa9a21d2bf7051e1f1019ae78004da50c5b55c144a02afffe539d753949a2b056534f5b4c21f248a05baa52a6c38c7f5dd"sv},
            test_vector{
                "9aa3e8ad92777dfeb121a646ce2e918d1e12b30754bc09470d6da4af6cc9642b012f041ff046569d4fd8d0dccfe448e59feefc908d9ad5af6f"sv,
                "994d1cda4de40aff4713237cf9f78f7033af83369ac9c64e504091ea2f1caff6c5152d6a0c5608f82886c0093b3d7fbadd49dfd1f9e0f85accf23bc7dad48904"sv},
            test_vector{
                "5842512c37312511a3d8ae41f5801df60cd682d58b4a997342b6e717e94006c214813e6c63e75591f957a7ec301779838bec8ae3ed7febad0805"sv,
                "9763c43331ad0eb279d704c5f6e97e02da8724115026827f889e9fcda21f60fd230894ab35abb719890f3afa51afd31bc6852183b9c51059910af460abd2474d"sv},
            test_vector{
                "ca14e2ea2f37c78f78ef280f58707ec549a31a94361073e37701bfe503e4c01ee1f2e123e00e81a188f08fa050825709128a9b66bb8ae6ea47e41d"sv,
                "4600e022a02258739f67fdd367cc1e662631fb087918768352062b9b3c8de8dbca0e9ec751b91f284694fbddb8d325c0637bccb21dd2efa92e48dbab2e5e9c26"sv},
            test_vector{
                "647629c779b24c1e76f41744aba17159487532a0156a7d8264db50d645e9595ff81e0c96a850f2aa56c844c613a4b892727a9bfc3d3e20386766f805"sv,
                "5bc842fc2d3b7eb31d2d3044df3ec32af114feaa7cfc27ebc8630f46ab6f0c543f59b812e776e5303861d17da3f1f16097641f3b808d4d5cb3e483946409746c"sv},
            test_vector{
                "1c5dc0d1dd2e4c717635ff3e9b67caf957aec0f8f63c1b1e221e800a4c14848f4ea06e644e5d3e1de592ef5a8007fa3f07171b24bd07578d68963e5cb1"sv,
                "cbf1ea86fa5b3dbf67be82fac41e84cccd0d296c757169b37837d273ccc015eecd102b9ce1cff68fdc7f05d22f2b774734f62ded54c8ee0bf57a5a82010d74f5"sv},
            test_vector{
                "8a555e75477d065b3af7e615475f37c0a667f73a4c7af5e4a69f28a68d9f4434776a8f90eab7f1d137eb4b22643c0a0d6a16fcfaa1bd62f2783546a9695f"sv,
                "c088e4a3d7da2f6f99a8f3f717361108872b8ffef921b383c24b8061d4e7c27fc56f4f20dc8f952a14043c5650b5a9e777c49c41cfeb3f2de97ee2e16b2c3924"sv},
            test_vector{
                "ebb3e2ad7803508ba46e81e220b1cff33ea8381504110e9f8092ef085afef84db0d436931d085d0e1b06bd218cf571c79338da31a83b4cb1ec6c06d6b98768"sv,
                "f33428d8fc67aa2cc1adcb2822f37f29cbd72abff68190483e415824f0bcecd447cb4f05a9c47031b9c50e0411c552f31cd04c30cea2bc64bcf825a5f8a66028"sv},
            test_vector{
                "c1ca70ae1279ba0b918157558b4920d6b7fba8a06be515170f202fafd36fb7f79d69fad745dba6150568db1e2b728504113eeac34f527fc82f2200b462ecbf5d"sv,
                "046e46623912b3932b8d662ab42583423843206301b58bf20ab6d76fd47f1cbbcf421df536ecd7e56db5354e7e0f98822d2129c197f6f0f222b8ec5231f3967d"sv},
            test_vector{
                "d3ddddf805b1678a02e39200f6440047acbb062e4a2f046a3ca7f1dd6eb03a18be00cd1eb158706a64af5834c68cf7f105b415194605222c99a2cbf72c50cb14bf"sv,
                "bae7c5d590bf25a493d8f48b8b4638ccb10541c67996e47287b984322009d27d1348f3ef2999f5ee0d38e112cd5a807a57830cdc318a1181e6c4653cdb8cf122"sv},
            test_vector{
                "8e8ef8aa336b3b98894c3126c71878910618838c00ac8590173c91749972ff3d42a61137029ad74501684f75e1b8d1d74336aa908c44082ae9eb162e901867f54905"sv, "41672931558a93762522b1d55389ecf1b8c0feb8b88f4587fbd417ca809055b0cb630d8bea133ab7f6cf1f21c6b35e2e25c0d19583258808e6c23e1a75336103"sv},
            test_vector{
                "52761e1dac0eaea898e0b07cd24f4b2e6bb7bc200ea4b0528842f17b87154559a2ea94459a0e480ae0bdf9f757dd4a335aed0e510138b024a04ed1d591b4323234dbd5"sv,
                "b826fe80494e19c51b42f2582b2d080ba6b90512f35f2db67dd7fd5ee532eaa16498afba08b4996cbcfdf8d1a2df6b1da939e8265115a48aefa42f38205db436"sv},
            test_vector{
                "3804ebc43cbea80c2bd7e4fda5c5515500cd2d2b846a1378dbf218d5c377138606eb3cb8ac88f9076f6ff4436f90717427c9df1ba052acbbe4585e98b6e8e0bf800f1946"sv,
                "17dd6d87bc6773051e52047fd444996afa8124b0483fe121877f98553448772bd0e7751fc655e9cc2d29830211015d310f191474ca6adc0477a187c03b8fe252"sv},
            test_vector{
                "2249d698c4d807a8e7b4de21c485738959a0d67e5d2ca6f77983dfccb5dbf47931261e1f1537f3cbca253afb6bf4fe5e7672e1dcc860b3d6c8d243afe2d9758b375e955692"sv,
                "6af44563fc468d51182f6c3be58d45932af1d985c6f283976c91a9ff421f383fe21dc7322f397ccead583e26b3e3fda067976a7f34665df25a2ced7b4b09cdec"sv},
            test_vector{
                "32a9c17033658c54f22c7135ddfc879de94d79593ef2dc7d3041bfa872738389864eeda27801794ccc4ff1fcb5ef3fc48833801d6fe959e3627f8ea1536ad00fa9c7d7d9f043"sv,
                "6a47699dd3ada2f11bc4ea42072b06cc20857bf164497df1285400c250f5848b6f71957dbdc845f5daeab913036661f69387893fc2d61c25fa59b9d85b19f401"sv},
            test_vector{
                "3d65f69a590a5baaabcd274fe3ef9e88920ffc7adf05c16d7b0f4d18d72bac1e94c3b3d83b8f4c552eb80e9fde3911403f8b000579816f02e1716fd627946031d0af0793e7f3e1"sv,
                "ffb2d9450943c24b5933c24812459b75d3d9f380344c9bc06fa3e17ee448eca2f98ff79f7e2235ccd9f9a8176f68a2254bbc9b834d6ac8d2bfdbc1597c432c9f"sv},
            test_vector{
                "76ff8b20a18cf104f6cdb65e2ba8f66ecf844af7e85e8ef2da19e8848a16052ec405a644dafb5ca08ec48f97327ac52c0e56218402c72a9a6dc1cf344d58a716a78d7d7529680bae"sv,
                "f8858144c6d709dd0689a526a548a43f17494950ba2ac20544799e8ea27201d78bce5b921e29a7b4029278e68341ef2a0ca4ba3894566b3c8f8950e3e545a689"sv},
            test_vector{
                "ca88dddfc876a12f45f19562bc9ca250f43267ab251a7f345c3c022e20144e135604078762ef5c8a8f038cf1b1d6a91709b59dd068396a9e971ab628f74886e765384a23607c1a1e6e"sv,
                "4f3d9eeef349ca51a7e419af1686f42795abde58a85335ce68d496e81e4436a80a61dc143a4300008c23a3e71f4ba98743195a3694a8d02fee11bd314569abc0"sv},
            test_vector{
                "0a78b16b4026f7ec063db4e7b77c42a298e524e268093c5038853e217dcd65f66428650165fca06a1b4c9cf1537fb5d463630ff3bd71cf32c3538b1fdda3fed5c9f601203319b7e1869a"sv,
                "6095c3df5b9db7ce524d76123f77421ce888b86a477ae8c6db1d0be8d326d22c852915ab03c0c81a5b7ac71e2c14e74bda17a78d2b10585fa214f6546eb710a0"sv},
            test_vector{
                "20f10ef9a0e6128675340171cd248df30b586557620b615ca39a00db534315a9012dbdbfd6a994986eb829dbe6cdaf3a37d4f59ac27298742c8f777b6b12677f21eb289129579868705f27"sv,
                "b4ead3f860eabbd36c770d66c7356f8107acd1485c7c94178c2eaabd50266d7645d009972586ef83ed43ed92882137df5117b88f35231b894ec1741ae7501145"sv},
            test_vector{
                "995c8f747ea418f7d63aba2260b34ac3c7dceebb78438ca4b1f982b7db9798ec1a7f32622264cb024c0d9e60e955a6e1d677c923518851990a459b767d0f13cd803460f61870db3391b44693"sv,
                "a00a601edeaca83041dc452d438a8de549594e25d843c2cf60a0e009fb92d87abe28a72690ab657c8d35b43cd02d22ec0755de229d1f922fa6ca18a6d6c2aaae"sv},
            test_vector{
                "0feb23c7e4a19bcbd70bd300d76ec9045d696f8c9687f49ec4154400e231d2f0862495150cf250b6f12f172a7d130f8fa5d175bf2f25e280172ccdfb327951701165302728a619aa2f242631c9"sv, "eeb6dee30c119fb1e1eb5c15ff2b32d8b9c7464a4e4cc6815cd251a6bae29b49961dd5c2fa9c44a9b142ca062c7072cbf3db04299b767789040196bf0c06aa76"sv},
            test_vector{
                "ac59a110623f1a64666f160ed32926676cb5be25dd9d962f441951b0efcb5d6a67ac1a4eae473e49c6257860728853ff415c5e8ec76a8a462ecfd343eeac22dad820722c597332fbfd94ebbd32c6"sv,
                "f65ea942ae0a47e73b02b1442e5b26083db79307f64dd34a039c476faf18d5c514bb77a2c412a6074a7afc326ea66c74e5705fe2abbabf274333325a15b61fd9"sv},
            test_vector{
                "9e3e1077e1333a1fb1aa633ccf2f746588ad426489ea08dff5511438b5f4c0b110d1a4d47b540a12b21ea2aa070578ccfa5c22fe0b743ec0cc621c6b3a03b75f4d3eea5dce89e03269afcd9603d0db"sv,
                "4b5c5df80c344c12388c723856cd06965b2190af652480476747dc2195ea3716f87c1762359583a5f31522f83f7833bec30f1f47d14540417dd463f5d258cd4a"sv},
            test_vector{
                "e881e3284c79d8f5237e699e4fbca84090c664bb53229f58cb0842b0436710c9b329d98191b8f030e9c1df89b03858c1569c6ff49a7c07c4a23a8a434b0fde13be4f94cb44ee629d5b44d336090d3de6"sv,
                "147d8071c7871ef9256cff32aa63ea031404fa5ee4ec09c56afdd5da919b0cc84a9d35d142c417715203316011cc620cd6855bb117063a5e52867facc680d5f4"sv},
            test_vector{
                "e58521098911503de84311387d375c25929e6e55076eb6934fd8f2b1bb7b9667fbd76d5ee204828769a341b1f716da5bdfece6c62a9f4d4f988267fce1f5615540dbe375324eef607c910d976b45a5ea5f"sv,
                "f97ba056fa41f43b8e1987072a09e828c71c5ff6ad4e37f9ab6b89e2a078933dd23052fa72c6615b613904259e9ff9b55ef7b923b89bc8752f6babddd256e117"sv},
            test_vector{
                "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c9f133e5f8527f1b184830e07c3458c83a8ca9f9d9c6998760e610668ba0f22e22b656a737e978b246a17840b7dc4091da85f"sv,
                "c8a466199acbcbc93f2ce042968508c046901631e3118a2d0bf39a9b42b4197a379b3a86cdeca9df2de1a3eb71b79ae9bf2d6575eadf1878029c4093133f54d3"sv},
            test_vector{
                "9af608d031ccf309d7273c607a8e5e36840d449b55db5b13f03aeb9af49fa7e7cf1383ee2ed9c5a8b7515f16fb1c7c84a681590bf90f56597b844db5ebee223d78109b72350772f7c72ea996603e1e84f2ba5f"sv,
                "f0ded9495b4f64cac585be8a737cfa14247a4a81cdf7f01ebcb134ace71f5a83df2cd72e7773fea1e82beae17e13857372792c8231e2ab9fbeb633e399d5f0ae"sv},
            test_vector{
                "d0df1bdf1df6203241722fb9c9c1cf7405017497ae154538ccf9224ad752e6ce1d4ae948639aca70cfe86b2b06543cb9914ebd3085aa3e2963f6e9b93d0b03a31ae26fcb9ca974eee016c091a6fcac37b21cc1d7"sv,
                "c2da3ea3c8a3fd88a5bc5dea2bc076f861abedefae5a5fbd941ddfd1c41cc3312eb2dc826c2c0f65414fe72ebee447d2f9b1a6a56302660d1f86632ee80a175f"sv},
            test_vector{
                "8cbc9480553acef7bcdba9716ea8d66b4131780917de2b0b048045fcb32b5cac054808e1fce6e94ad851ecb47fe6cb802225d3551e08ea122093d0078dada564212eacf1d6394e0007cc62a1d595ab14ca08a284bc"sv,
                "63b39b88ceb848188b37316e04560e75a5340ab8d417932d231c997e892b41daa69d9fe3e9a14dd19ccfbbfa01488c208e7b946cfaf16ca2b1bf7c8d8da4e6b2"sv},
            test_vector{
                "38f184448f3cf82a54cafc556aff336f23f9149e612134b3fc00c8a56455653d88640b12f69062b8432c4335ad8f7ab4ff66cb7eb54f332561a36f024d92c3e26276f4fd48619628cff88e4b8e85cf14ca4767ed990d"sv, "9a49265fc641c59f1a91872cdae490d3da73c0c60fd59648e1d17dba1a647a5b95629392bb4ff5163d1a3cb45427c1437a3b2e1d9f030c0a8bcc5ed22da9e2ed"sv},
            test_vector{
                "70900618b1e9e9db62296fb6c6590c9f10b0a632765c489c887f1ab7c07791765a62e38465e1be281b1d396c6e080b7ee3e6fa56a30b9799d0e629be153ee76f81bc6a3295aa61489bfa87d53a8ad24248a6ede0dfcfe9"sv,
                "1c8c3357ff1f8d6ac4defb3af462a73e09159e3a20c6506edd8cd3052df941c81f68c5fbb893912619e28640977fe8eaae8e9d5d4e7d5f132552cefab4540bac"sv},
            test_vector{
                "4e6ddae0d805afcd10a055bce584c848d050fb29fe8f1c64b18e1abfe46b65782e6ff536e89d8d40928b41ed7371365c8080a9647f7532ce6c6d4ac21cfb0c8020783851ec9a7dbc3948f8fca7adf8b2a78c04d898d31ff6"sv,
                "5c2f996c779b91b3c4639311f54fabbdde7e2212b53dbae4828c8399588fc00d3b2ae60918aaaf6bb48bc757e52b2bcea84f5d15bf4ec25d5519fb54f6f26e1b"sv},
            test_vector{
                "696825f6d6ea8173ec47d0959a401c4ddf69f8f08ddd678a4d2ff976e3a4372bb39f4159845cb63585e1d4108d32e12fa7c5c9d7ce3508a7f53aca2b4bd951adbcd8984ebb7536563f5884c90bc5023b3316f7e4dc6958f743"sv,
                "3ce940ca96b00011375daa95c65f66907d69b3eb3b8d779e6fc971afcc05e990bc4c541f434590f6b18b68c080d0f24475a3e764e9cb85343301314ee2fb661e"sv},
            test_vector{
                "79ecdfd47a29a74220a52819ce4589747f2b30b364d0852cce52f91e4f0f48e61c72fa76b60d3002cae89dfc5519d3430b95c098fa4678516b5e355109ea9b3745aa41d6f8206ee64ae720f8d44653b001057f2eba7f63cd42f9"sv,
                "ba3d0fe04470f4cf8f08c46d82ae3afd1caea8c13bebbe026b5c1777aa59860af2e3da7751844e0be24072af48bc8a6fd77678aaee04e08f63395f5c8a465763"sv},
            test_vector{
                "9263fe75e8f6c7d5d642e2ca6a6eea4f44e9a0f249513ed79c9409ffca5526ca4491aebb1382057cc7c36722b0b6c3b15123cde312214f25353abfe30bca170568a8e1ba5408917403a01834080ab607c56a10d0265082498fe0b6"sv,
                "7736d7a7fc1eb05857ce7d88abfffa87f58c670bfdfc0a8031f60f379e4b6ad94ac8f13ffe28c697809b5cfac7f13be01e7496a85237c4025539051fb2e32fb6"sv},
            test_vector{
                "78c17bfe0e02eb526d1a44a1ac127be082181452b625394bd6dc093a2cb432e6ee59c2f8b5503aba30dae41e1a1c6702697c99b2c94e94af48b00caf53b2e0e4e1bbee81ee282c7b2b35f58cf421a07e828d57a6622626af25835399"sv,
                "b56b6e343166328523e0d1693e5174da643ae83cf69c85a7b3c3bee247b77b84702069d9e6b4cab03bf17fe612009bf4239683ca78ca7e876aca7d07603ba714"sv},
            test_vector{
                "298bb304a920f960447d8fd38b061bf8fe4ac1f871d8a0feb4549feb72ca694a5a41b6867d94cd5af77d468ad2f315d127b6c41a862800f3985e573e037740298e2c5c6186a9fb83609be2d49f8b4c31f96a2e49b56dbf09571b38587f"sv,
                "34e3878627904ffbbbd85266cc973c34f931e3cab5d4c31f841c553dd69f84838206067df4f9f3b9102001be19267151e673f5c2d4c2f8438a6999a0a325487d"sv},
            test_vector{
                "a3cf714bf112647e727e8cfd46499acd35a640dd393ddd263cd85cf6225f59890a0686dad1c54eb8d809b81c08a98dba131bbdd6fce8ff59d95db824d8831ea480529da739227a6e0f62b603b38c35cdc2581f614a31879b8be54aeefaa0"sv,
                "6f230ae4903ddbef0ba384c2e3506eab318bfd1a46ea76099f65a3fd529c91bc2865b9fd943e346de64626b8529f9db1377bf2c5e0129c66b50c6a5cfb364b3a"sv},
            test_vector{
                "0a427ae55ef3a7e6044a08cf6128cbaaabfd776c4e9374708f2ece246fd73603d2f54ac3e01d16cfac2bdaf713920d66e8f0a3d54ee68cff64267d5528cdf2f295f474d10f81173e0143488ac53fc503c444ed23dec63a080ce90c2443dba8"sv,
                "f6bbe5d0cf13ddf41c1436748a5d1ccae2948547b452c2171c7c8e8b66c6ae4de3c0e8b2962bcb60d3de3608479f80e455c9024d9716c38f6f1206861ab1eaac"sv},
            test_vector{
                "2cbbb87511f4948efec3a61b511ededb1dda8b6ecfc0210c11e43a77ee32dc2e374afae4268e3d30427804868232a966b56006d3214037076bf6a265b72135af0fb2ef7909fea2dea412f7717446b276ff153753662b4d4148c02347e3259169"sv,
                "76897b87a8a1cf835c434f6d391c9e5227351af9d3e20a3389c796b98b424281a59068d9c8d567ec2bebc435b0126b059e2d86394a9854d6611e1c922f385496"sv},
            test_vector{
                "2b23324c9992f60a7fc010159a03cb9a2b290df4fa6a82359b9af602f0a403a5ef33ed5da5b2caf87b77e6a4b93b650348ce2a7dbc08f8da9203d710b587ba5947c65e899f4a759f8e2b049ae7850a8e3e2962f6ef93ea4c631de5d78e729ec5bc"sv,
                "3beea0b373ed09cf1c919c51d86d642c9125e0ee81698dc4cbadf02e9e6925efb562fd9b87301a6377ca192be79c4118deabc450b54639000c2e312945451fb5"sv},
            test_vector{
                "4022f930c7033b00d986c65ff6bbbdf9ebd0e58c52844ff658df3893c3202dc533f873d4a7f5a5f944419fb5528c9b6788479a1e891306acae7995fc06db70a59baa95bef7da79f5e793f2db7f2a55825e4fdb4a34884af881ded1089fd5334502a2"sv,
                "0358775bbb733ccc49e78f544aeee512370d480d0e13c7e8d5c444c423e592146b45fdb91a1b694d35e36b60e4bc8397fca8bb9790e619339778b9cd1abe3fe9"sv},
            test_vector{
                "1cb77ba43ce77e236b9fc925f589b1c070780a84f99e8f50c1ff846ac92599cfe91612c8178325bee642a34f4dffdba2aa2ebcf7064339829b26f27993e1106c139c70d578cc05f0e1a777cceddb10a2c67fd9675e4a009df8037d6eeb38f5fba233df"sv,
                "6502f46551a3fab3a96428fb97801d7a4aa2f17fef6603238df84e17c74309ed3d9489c8b16a9384ee634a3f86d0b3ba9a4dbc9c51ec8bd4bf8d61de6d3d87d7"sv},
            test_vector{
                "52167de2d6c502d99fa10c27b2ab6203bdebc2cafbbfdef15872a43dd610c2362f796ad9bcb5528d95870058fa454453f1e6065b315d410a3f2650e5d71e69d78d9767dfb4accc057fd2069266b0f180cb319e30ded7535bbe52d24be151de4bb598fc5c"sv,
                "25cb3ed3983a91b4cf37a65193916c5e3e211b63e943e2f7b50a85d349a463b941aad33eff16561bdfdc92fda06a4e1d94b162de48f06d3c626940b31020925f"sv},
            test_vector{
                "cede6697d422ddaa78e2d55ae080b8b9e9356c69bc558201a2d4b0b3190a812c27b34bbcee3a62b781378b1bf636b372bcbae1fa2f816a046a0a649a5c555c641fea4ccd841cc761f38f777972f8c91b0324e71c333ce787f04741439bf087ef5e895011c0"sv,
                "0be42a25d77ac6ad995c6be48e783380bad25a61732f87cefb0cce1a769cd69081f494a1a12d657664ef2b4d9c41f2ee83f6e9a84327d8756af9f985595e7d3b"sv},
            test_vector{
                "56d18d3e2e496440d0a5c9e1bcb464faf5bc70a8b562124f5fc9e9deb5fee6544b945e833b8b5d131b773ecb2cdd780cd4e1bb9e4f1e3cb0a1d64d19cf4b30e44e6c2d0cbcb4e284ce50db7a8a8062ddb63f981d9026c532bf8eeddf8af5a43848a32262178c"sv,
                "982dc61c91a93770582eee8025aa55da8e9edb966bf5cf70d4a6534c0d53a2789a8c4fb65b7fed478cda02ed1e0d198d85c5c735b2417c5fab5d34e969fc8e7e"sv},
            test_vector{
                "25a7320dfaec5af65da4d0f8688e29e8e95532ecc16679ea8aff0f407d898db6922855b0e8901aa9681aa3dca617cb440764cdc7293fbeaf7f585b593c2b0531738e0ade7c8626b9995f4a84d9fc9b593d6bbee01abc53c5be14bf6956fd2fd81000dafc7c7686"sv, "749c928c3d5510925bfe98659025b0ed7c01acd4d59a9bf1c54863a088091771dc9d407bdbf83b0f44b0902e10349ba79c84d0981d5e8c4f5c733a117fed0790"sv},
            test_vector{
                "3d7177b28ffd916e7e0634895833ba0bd9e0653df2cc4202c811536a005aec853a505e75db55d3c7107579041099e382a1feac80dde65d72368e909ab85f56d88e68d7c3c80c38f85bf8c2b36959409cc34ba8e3ad94fe8ee1927612d672d92141a329c4dd8a88a9"sv,
                "14a331508cd7d94fcce56a66bf65f20870a281c8442f8dbd4c2371454a2b66f8d0994a0b67692e771efc6a5e0b887acae7d6f4ec7338e1aa89f2abc7034c4e4c"sv},
            test_vector{
                "c033e4a512297caecdbead892b11a9f7007af9a74bcab89e0bd4ffdd542ca03ea12e17a06c42bd43fc5f3f757fce4f6f5831997abac3f95676e1ebdb11ca43e11aa31e5ebabe18ce8d1bbfd8b02f482e1ce581b532e307e6960eb97441506c2ed299e1282523f41527"sv,
                "95ac9b7d22aa458921874c4b4331e7d64761853217c3f83c601abcbccd7e2eaa6ca6ce9a22ebcfe5046d52f8a09097f043ab8bc59243fd770090bb432c3155e9"sv},
            test_vector{
                "69fff0f1a3dbfb36e32f025819fa99ea9a0edaef73145bf7fcd05d8bb0a646cb3b5d5256d524856acfd2e44d6b72e4ebf1ff23c0ff6c56f821e782d5a15f7052a1445b06668eeb4af700679ee7ae26496fbd4640c06aa149964dfd6011df835ac13b73c8ff21151e8440"sv,
                "45d4daa652558d1c12beb0f5662c712f325b4c802fc6eb9ee039c949d002bb786f1a732712be941f9c5c79b3e5c43064d63a38578e5a54ee526acb735b9ad45f"sv},
            test_vector{
                "b2c439c97ab7c63736b3796324d68eeb7a471ed142bd9622684167d61234fff82f93f907537a909bc2e75a4bcbc133cf57197662c1af746ae8b81e5b83de05d9b589851de25d3c99c004c1dfb12d93bf50d450af49c428716f5b90ef088e3b6a6b2c46d3ce67b379599018"sv,
                "c48ec83be5fa669e6ec8db90aca9676cfe2ec0d5e8e7a2431687bb953c0a300be3db4075cca3bac4dfa4d971baf0fa1aff46639db4b238856ff36d1dfcd520f1"sv},
            test_vector{
                "c016f522f26b7470e922b9a287e6d45f6c28813b68c1457e36d9ba266708272f9cbc5411f8db9d8bd5a9449fb6eb0cde7d4d03e5df019f2814a90ceed377c59d7d92623899bcb0268033073559d4d8de488686cbe3d67796e6df6ad4276d0b52cc62c49ebb58d7c95287aa6c"sv,
                "7402f1a99b47e102b3b73140c6771b07ee6c33b3715e9c4027c441bee40511b735d95e508baea78da26fded9b7038e9a53defa58448aba40dc1e62d7ec592107"sv},
            test_vector{
                "a766b2a7ef916721f4677b67dbc65ef9b4d1bda1ad4e53fc854b0236440822152a111939e5ab2ba207719472b63fd4f4a54f4bde44a205d334a2d72cfe05abf804f41841b86d36920be6b0b529331ac163a985556c84511ec986439f83e1d7311f57d848cfa02df9ea0cf6b99a"sv,
                "ddd60f93a3babc78299cf763e7919d45ac6f479700e1adb05ab137acdf89c1521ecb9dfeacd091e58ca57a1db964a9c3cd1fa39192cc1e9f734caa1c5fa62975"sv},
            test_vector{"10f2be77a4055771a67007cd8630e3230e38288499cb160380290174d66da57455b6baaa9785c84c8a663de41ed3bd544055b9170cec43cb3eb120eceaba1fe36e3eaa3fa4f99b425cd2519f09bc0282bada52d14ce625b1ded3b24d86b1dad342d2b7be322b775b04fc6b86afb4"sv, "a872fa33d463b3343cec57c20c66979c33e1ad067bfc703454696aab5dd0003bc194318f4a8ebbc74503feb7211a472dadee991efe3e38f21a1310f8a76eac80"sv},
            test_vector{
                "324533e685f1852e358eea8ea8b81c288b3f3beb1f2bc2b8d3fdbac318382e3d7120de30c9c237aa0a34831deb1e5e060a7969cd3a9742ec1e64b354f7eb290cba1c681c66cc7ea994fdf5614f604d1a2718aab581c1c94931b1387e4b7dc73635bf3a7301174075fa70a9227d85d3"sv,
                "3b26c5170729d0814153becb95f1b65cd42f9a6d0649d914e4f69d938b5e9dc041cd0f5c8da0b484d7c7bc7b1bdefb08fe8b1bfedc81109345bc9e9a399feedf"sv},
            test_vector{
                "518985977ee21d2bf622a20567124fcbf11c72df805365835ab3c041f4a9cd8a0ad63c9dee1018aa21a9fa3720f47dc48006f1aa3dba544950f87e627f369bc2793ede21223274492cceb77be7eea50e5a509059929a16d33a9f54796cde5770c74bd3ecc25318503f1a41976407aff2"sv,
                "c00926a374cde55b8fbd77f50da1363da19744d3f464e07ce31794c5a61b6f9c85689fa1cfe136553527fd876be91673c2cac2dd157b2defea360851b6d92cf4"sv},
            test_vector{
                "9159767275ba6f79cbb3d58c0108339d8c6a41138991ab7aa58b14793b545b04bda61dd255127b12cc501d5aaad476e09fa14aec21626e8d57b7d08c36cdb79eea314bdd77e65779a0b54eab08c48ceb976adf631f4246a33f7ef896887ea8b5dfa2087a225c8c180f8970696101fc283b"sv,
                "3cd3380a90868de17dee4bd4d7f90d7512696f0a92b2d089240d61a9d20cd3af094c78bf466c2d404dd2f662ec5f4a299be2adeadf627b98e50e1c072b769d62"sv},
            test_vector{
                "fe2d8ae200e6657fdc7494af5a12b2ae940348f1f983f0ba98febbe99c80d115126d57dbf37296765ebb5990256696588b3851d54c8fbe7ade98a6faf7c20b5e4f730f54a7f912ca0ac31bbb53d17949ef69aa0de40c7bab12a871a9b90f68813ca87af4256422a268f4a1d8ec3aa1a947fd"sv,
                "8025a8608df0f6a01c34cdec012d4cb25852e1b100b68172fc4e86ac8b7126b64859cb9e767a7e59060989cedbd925afc475ca7369bd43f85ae590e224e036dd"sv},
            test_vector{
                "dc28484ebfd293d62ac759d5754bdf502423e4d419fa79020805134b2ce3dff738c7556c91d810adbad8dd210f041296b73c2185d4646c97fc0a5b69ed49ac8c7ced0bd1cfd7e3c3cca47374d189247da6811a40b0ab097067ed4ad40ade2e4791e39204e398b3204971445822a1be0dd93af8"sv,
                "615115d2e8b62e345adaa4bdb95395a3b4fe27d71c4a111b86c1841463c5f03d6b20d164a39948ab08ae060720d05c10f6022e5c8caf2fa3bca2e04d9c539ded"sv},
            test_vector{
                "5af8c0f26db4e99b47ec2e4a01a786e77899e46d464ac337f175027b61aef3149848af849d76ac39b9b0910fe6594817859e55974fa167518ed72d088dae6b414d744d477974fb719c626da792f981233de24b7579d8acca510a266d73c0ee8ee1424343eaf6ffcc59c86c1becce5894072c6c11"sv,
                "09da284d5b6556508be54c8ab6c97bbd472995c6bbd585917ecdb54ea9167208daaa070a7b2b7d8e93ce1315f0d1ef8d69667429c44dc5ee1499de57b229a398"sv},
            test_vector{
                "49cd0ba0df5bb3f43f68464e3e83e9cbd5d5ee077ffa5591e30f939cb30c93f7d454fb3fbf8bb05327a89c08dc4baf1eef50237317a405775357f1e0d1f31d9f0f0d98124019d47bf18363b1ecfbfe155c10cbc83300e01bc9ce0347c596b35f411e6d8229ad2855e42022b0373ade98663c6d6e9c"sv,
                "30cbf0679a97c871574d2fc05d7aa760c6bc8a864b7d246c39b9e812f9b7ff7b4ef5197dd5b69493306688b8564de1ad47d75505c913ba6a78788f8caf5788bd"sv},
            test_vector{
                "a8a37dfc083ad2f47fff468738bf8b728eb7f1907e427fa15cb4424bc685e55ed7b2825c9c60b839ccc2fe5fb33e36f570cb8661609e630bda05ee641d938428867d90e00744a4aad494c93c5f6d1327878078590cdce1e647c9820818f467641fcd508e2f2ebfd0ff3d4f272393478f3b9e6f806b43"sv,
                "8e1c91729be8eb40226f6c58a029380ef7edb9dc166a5c3cdbcefe90bd30d85cb7c4b248e66abf0a3a4c842281299bef6db88858d9e5ab5244f70b7969e1c072"sv},
            test_vector{
                "36af17595494ef793c42f48410246df07d05936a918afe74cd005e537c586b2843701f5df8952242b74586f83339b48f4ba3a66bdeb457ecdf61784eac6765cd9b8c570dd628dbba6ae5836b9ac3dbcd795f9efdb8742a35bca232abf36eb3b6698b2933965802277ba953a6edcacaf330c1e4e8c7d45f"sv,
                "158bfc348a30b4fabbe355a7d44bdc2122a4c850444c03f289003ce01bfc1ebf3ecc0febb6a8ff523d25db7681b05bdce048d11943ab476c1967cf6556c4a120"sv},
            test_vector{
                "42d66edc5f22e0c13c25504c5101a5d172d2db7209e461efa323c0bfaed27e5f808042ea9c3838ea31f9b76de465225ccfbd0c09ca0d9f07e9a43e3e46c7693e00a7e1d483900ddb0a629d5563456dbbf299ac91f92c3d3c17b05d180e6c87c6c93194c39d90273fcf4a482c56084f95e34c04311fa80438"sv,
                "061afb119a3c60876e04c10f12ad0f4b977593dc5a2d21096a57e7d3f7d4d44fdef934b2c17d7530674e4f4a1c176dbdcc54811a22e1b8712e4192fc2d4bf8e8"sv},
            test_vector{
                "f91bb2e1a9c4cd96bf250426b3a6afd9b87ac51e93254d2dae3b16ec686ba80fb0bd7a84d218660e9007593075bc4f4c66567f0c7a5fd2010c999a8a0efa81f89ff5bfefe0fb910f0442e6d4a7c55bbb618c69a79a2ddd82a0938927f6fe3a80f04beaeb7c7636e3435d12dcf1c6bb6ed0a4edb69c9657fa93"sv,
                "6e692c8c694ee0a3565f37a299e0006b85ab4a821b20e76798220229f656efc6a20211a4e7e4ed77facde0d70e4d5d95bc8ed1d7a56d8df1446d562f044b344c"sv},
            test_vector{
                "d1eb961ca6a8f67c49b61e4d3ceaa2a1de6f0ea927b132bf987abdaa725b0e1e274e46830e99a2f75af608964df0dff9a99024fc6839bac5acd10202f921ac71a27fcda681aa3109ebf5f21ee3a849098ea3a551e844fae4b48b5c5bb97ccc802bc5520d68a14cb7e5fc056b67d889d876efb82d0e9a9a2499f1"sv,
                "39b2c76ec207120de4b320c7fe069e602c9c38f257596da7369395e87eb64b3acff988c1839ac269d5012c093f9edd4b7cabf13bdea7d42e969ab108269c6ab0"sv},
            test_vector{
                "adf2263200f376886ba7b6f5e4411d5f07f7d9d101590c73ace114bafbcb0fdc99269e87cd2cead2a1cfe5744394d333aba408a07e21f30233b65b907472e9e3c7d6e7aa6d2c47a08a1be7bb877913a6b5604c723384478911c339e3b5fe527c7e288705a89c95d970b443347897e79f6c522bafe62b11ef8f3135"sv,
                "3c23d2d8cf4db6ac6a42e27208180f37668bef5ee0a3f879483c8e604e7f42583f202037b8d242c04a87345b8be6dc8b121d6484b9edad0d73c894c1288f5cae"sv},
            test_vector{
                "18e75b47d898ac629c48e80dbfb75dae1e1700b771165eccdb18d628bfc4063dd6c3839a7ec4cd1255c4821b078cd174647b320bb685541d517c579f6b8e3cdd2e109a610c7a921653b204ad018d0340d9938735b60262662016767e1d8824a64954086229c0e3b5bd9ad88c54c1dc5aa4e768ff1a9470ee6f6e998f"sv,
                "01c756b7c20b5f95fd2b079ab6a50f28b946fb16266b07c6060945dc4fe9e0d279c5b1505b9ec7d8f8f3c9ebf0c5ee9365aec08cf278d65b64daeccc19d3cbf4"sv},
            test_vector{
                "c2963342cfaa88ccd102a258e6d629f6b0d367dd55116502ca4451ea523623bc4175819a0648df3168e8ea8f10ed27354807d76e02ee1fdf1c9c655ee2b9fd08d557058dabdf8dcf964bfcacc996ae173971e26ea038d407c824260d06c2848a04a488c4c456dbcde2939e561ab908c4097b508638d6cda556465c9cc5"sv,
                "a4d2f59393a5fea612c3c745f4bb9f41aaf3a3ce1679aa8afc1a62baa4ed452819418c8ae1a1e658757976692390fc43d4decf7d855cd8b498b6dc60cae05a90"sv},
            test_vector{
                "85360c3d4257d9878e2f5c16d3cd7d0747df3d231e1a8f63fddc69b3b1101af72153de4c8154b090c9815f2466e0e4f02f3af3a89a7fd04e306664f93e5490d4ce7fc169d553c520ae15dd02c7c613c39b4acd00e0c9a3c501566e52cecea11f7303dd1da61abf3f2532fd396047b1887255f4b256c0afcf58f3ae48c947"sv,
                "e8352ddcac59e377ea0f9c32bbb43dfd1b6c829fad1954240c41b7c45b0b09db11064b64e2442a96f6530aac2c4abf3beb1eae77f2bce4efe88fee1a70cf5423"sv},
            test_vector{
                "c13e6ca3abb893aa5f82c4a8ef754460628af6b75af02168f45b72f8f09e45ed127c203bc7bb80ff0c7bd96f8cc6d8110868eb2cfc01037d8058992a6cf2effcbfe498c842e53a2e68a793867968ba18efc4a78b21cdf6a11e5de821dcabab14921ddb33625d48a13baffad6fe8272dbdf4433bd0f7b813c981269c388f001"sv,
                "6e56f77f6883d0bd4face8b8d557f144661989f66d51b1fe4b8fc7124d66d9d20218616fea1bcf86c08d63bf8f2f21845a3e519083b937e70aa7c358310b5a7c"sv},
            test_vector{
                "fd2203e467574e834ab07c9097ae164532f24be1eb5d88f1af7748ceff0d2c67a21f4e4097f9d3bb4e9fbf97186e0db6db0100230a52b453d421f8ab9c9a6043aa3295ea20d2f06a2f37470d8a99075f1b8a8336f6228cf08b5942fc1fb4299c7d2480e8e82bce175540bdfad7752bc95b577f229515394f3ae5cec870a4b2f8"sv,
                "a21b1077d52b27ac545af63b32746c6e3c51cb0cb9f281eb9f3580a6d4996d5c9917d2a6e484627a9d5a06fa1b25327a9d710e027387fc3e07d7c4d14c6086cc"sv},
        };

    for (auto &&vec : test_vectors) {
        INFO(vec.message);
        INFO(vec.digest);

        std::vector<std::uint8_t> plain_bytes;
        auto r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.message, std::back_inserter(plain_bytes));
        REQUIRE(r);

        std::array<std::byte, ivy::digest_length_v<ivy::hash_algorithm::sha512>>
            digest_bytes{};

        auto nbytes = ivy::hash_data(
            ivy::hash_algorithm::sha512, plain_bytes, digest_bytes);
        if (!nbytes) {
            INFO(nbytes.error().message());
            REQUIRE(nbytes);
        }

        auto digest =
            ivy::bintext_encode_to_string<ivy::hexchars_lc>(digest_bytes);
        REQUIRE(digest == vec.digest);
    }
}

TEST_CASE("ivy:hash:md5", "[ivy][hash][md5]")
{
    using namespace std::string_view_literals;

    REQUIRE(ivy::hash_available(ivy::hash_algorithm::md5));

    std::array test_vectors{
        test_vector{""sv, "d41d8cd98f00b204e9800998ecf8427e"sv},
        test_vector{"a"sv, "0cc175b9c0f1b6a831c399e269772661"sv},
        test_vector{"abc"sv, "900150983cd24fb0d6963f7d28e17f72"sv},
        test_vector{"message digest"sv, "f96b697d7cb7938d525a2f31aaf161d0"sv},
        test_vector{"abcdefghijklmnopqrstuvwxyz"sv,
                    "c3fcd3d76192e4007dfb496cca67e13b"sv},
        test_vector{
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"sv,
            "d174ab98d277d9f5a5611c2c9f419d9f"sv},
        test_vector{
            "12345678901234567890123456789012345678901234567890123456789012345678901234567890"sv,
            "57edf4a22be3c955ac49da2e2107b67a"sv},
    };

    for (auto &&vec : test_vectors) {
        INFO(vec.message);
        INFO(vec.digest);

        std::array<std::byte, ivy::digest_length_v<ivy::hash_algorithm::md5>>
            digest_bytes{};

        auto nbytes =
            ivy::hash_data(ivy::hash_algorithm::md5, vec.message, digest_bytes);
        if (!nbytes) {
            INFO(nbytes.error().message());
            REQUIRE(nbytes);
        }

        auto digest =
            ivy::bintext_encode_to_string<ivy::hexchars_lc>(digest_bytes);
        REQUIRE(digest == vec.digest);
    }
}

TEST_CASE("ivy:hash:md5_hmac", "[ivy][hash][hmac][md5]")
{
    using namespace std::string_view_literals;

    REQUIRE(ivy::hash_available(ivy::hash_algorithm::md5));

    std::array test_vectors{
        hmac_test_vector{"4869205468657265"sv,
                         "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b"sv,
                         "9294727a3638bb1c13f48ef8158bfc9d"sv},
        hmac_test_vector{
            "7768617420646f2079612077616e7420666f72206e6f7468696e673f"sv,
            "4a656665"sv,
            "750c783e6ab0b503eaa86e310a5db738"sv},
        hmac_test_vector{
            "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"sv,
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"sv,
            "56be34521d144c88dbb8c733f0e8b3f6"sv},
        hmac_test_vector{
            "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"sv,
            "0102030405060708090a0b0c0d0e0f10111213141516171819"sv,
            "697eaf0aca3a3aea3a75164746ffaa79"sv},
        hmac_test_vector{"546573742057697468205472756e636174696f6e"sv,
                         "0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c"sv,
                         "56461ef2342edc00f9bab995690efd4c"sv},
        hmac_test_vector{
            "54657374205573696e67204c6172676572205468616e20426c6f636b2d53697a65204b6579202d2048617368204b6579204669727374"sv,
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"sv,
            "6b1ab7fe4bd7bf8f0b62e6ce61b9d0cd"sv},
        hmac_test_vector{
            "54657374205573696e67204c6172676572205468616e20426c6f636b2d53697a65204b657920616e64204c6172676572205468616e204f6e6520426c6f636b2d53697a652044617461"sv,
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"sv,
            "6f630fad67cda0ee1fb1f562db3aa53e"sv},
    };

    for (auto &&vec : test_vectors) {
        INFO(vec.message);
        INFO(vec.key);
        INFO(vec.digest);

        std::vector<std::uint8_t> message_bytes;
        auto r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.message, std::back_inserter(message_bytes));
        REQUIRE(r);

        std::vector<std::uint8_t> key_bytes;
        r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.key, std::back_inserter(key_bytes));
        REQUIRE(r);

        std::array<std::byte, ivy::digest_length_v<ivy::hash_algorithm::md5>>
            digest_bytes{};

        auto nbytes = ivy::hash_data_hmac(
            ivy::hash_algorithm::md5, message_bytes, key_bytes, digest_bytes);
        if (!nbytes) {
            INFO(nbytes.error().message());
            REQUIRE(nbytes);
        }

        auto digest =
            ivy::bintext_encode_to_string<ivy::hexchars_lc>(digest_bytes);
        REQUIRE(digest == vec.digest);
    }
}

TEST_CASE("ivy:hash:sha1_hmac", "[ivy][hash][hmac][sha1]")
{
    using namespace std::string_view_literals;

    REQUIRE(ivy::hash_available(ivy::hash_algorithm::sha1));

    std::array test_vectors{
        hmac_test_vector{"4869205468657265"sv,
                         "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b"sv,
                         "b617318655057264e28bc0b6fb378c8ef146be00"sv},
        hmac_test_vector{
            "7768617420646f2079612077616e7420666f72206e6f7468696e673f"sv,
            "4a656665"sv,
            "effcdf6ae5eb2fa2d27416d5f184df9c259a7c79"sv},
        hmac_test_vector{
            "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"sv,
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"sv,
            "125d7342b9ac11cd91a39af48aa17b4f63f175d3"sv},
        hmac_test_vector{
            "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"sv,
            "0102030405060708090a0b0c0d0e0f10111213141516171819"sv,
            "4c9007f4026250c6bc8414f9bf50c86c2d7235da"sv},
        hmac_test_vector{"546573742057697468205472756e636174696f6e"sv,
                         "0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c"sv,
                         "4c1a03424b55e07fe7f27be1d58bb9324a9a5a04"sv},
        hmac_test_vector{
            "54657374205573696e67204c6172676572205468616e20426c6f636b2d53697a65204b6579202d2048617368204b6579204669727374"sv,
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"sv,
            "aa4ae5e15272d00e95705637ce8a3b55ed402112"sv},
        hmac_test_vector{
            "54657374205573696e67204c6172676572205468616e20426c6f636b2d53697a65204b657920616e64204c6172676572205468616e204f6e6520426c6f636b2d53697a652044617461"sv,
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"sv,
            "e8e99d0f45237d786d6bbaa7965c7808bbff1a91"sv},
        hmac_test_vector{
            "54657374205573696e67204c6172676572205468616e20426c6f636b2d53697a65204b6579202d2048617368204b6579204669727374"sv,
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"sv,
            "aa4ae5e15272d00e95705637ce8a3b55ed402112"sv},
        hmac_test_vector{
            "54657374205573696e67204c6172676572205468616e20426c6f636b2d53697a65204b657920616e64204c6172676572205468616e204f6e6520426c6f636b2d53697a652044617461"sv,
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"sv,
            "e8e99d0f45237d786d6bbaa7965c7808bbff1a91"sv},
    };

    for (auto &&vec : test_vectors) {
        INFO(vec.message);
        INFO(vec.key);
        INFO(vec.digest);

        std::vector<std::uint8_t> message_bytes;
        auto r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.message, std::back_inserter(message_bytes));
        REQUIRE(r);

        std::vector<std::uint8_t> key_bytes;
        r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.key, std::back_inserter(key_bytes));
        REQUIRE(r);

        std::array<std::byte, ivy::digest_length_v<ivy::hash_algorithm::sha1>>
            digest_bytes{};

        auto nbytes = ivy::hash_data_hmac(
            ivy::hash_algorithm::sha1, message_bytes, key_bytes, digest_bytes);
        if (!nbytes) {
            INFO(nbytes.error().message());
            REQUIRE(nbytes);
        }

        auto digest =
            ivy::bintext_encode_to_string<ivy::hexchars_lc>(digest_bytes);
        REQUIRE(digest == vec.digest);
    }
}

TEST_CASE("ivy:hash:sha256_hmac", "[ivy][hash][hmac][sha256]")
{
    using namespace std::string_view_literals;

    REQUIRE(ivy::hash_available(ivy::hash_algorithm::sha256));

    std::array
        test_vectors{
            hmac_test_vector{
                "b1689c2591eaf3c9e66070f8a77954ffb81749f1b00346f9dfe0b2ee905dcc288baf4a92de3f4001dd9f44c468c3d07d6c6ee82faceafc97c2fc0fc0601719d2dcd0aa2aec92d1b0ae933c65eb06a03c9c935c2bad0459810241347ab87e9f11adb30415424c6c7f5f22a003b8ab8de54f6ded0e3ab9245fa79568451dfa258e"sv,
                "9779d9120642797f1747025d5b22b7ac607cab08e1758f2f3a46c8be1e25c53b8c6a8f58ffefa176"sv,
                "769f00d3e6a6cc1fb426a14a4f76c6462e6149726e0dee0ec0cf97a16605ac8b"sv},
            hmac_test_vector{
                "0cf2198c31376f5c8915660137725f2bbc180a986e5a7bda27fa81593a4a339bab92cbc39fb2b8581108ee48c794812d845a72ce8008c9e915d9e330bbb90e9136aa53ba0e6693dd4046d6b03362dfb9edfa04c887153cc5de677aab8c7839d517035879679c29727e96c5426324a2575fbe678d6cc7fef5eb6cebd595cfddef"sv,
                "09675f2dcc4783b599f18fb765583668a0fd8ae4096f6fcdc60d4f35b4130fbefcd542ffe7459d2a"sv,
                "6b142d4dfe217f1881aa0e6483b271dd5d43f70b85605953a0fef272ddde46ca"sv},
            hmac_test_vector{
                "3fb301cb4092f9623aa5ffd690d22d65d56e5a1c330b9c4a0d910c34e391c90a76d5401a2d3caa44b8c5d5aef3e928b90d2ee233e9f9a2cec4a32cd019d06a0dc1fcb1125f5746a4fbd32169ed7bf0e4fd065fa7c8ac97c366380484495f5c5b6850dd1c9d8cd6694cf8686e46308ed0ed1f5bdf98cd831339771db63de5a7de"sv,
                "cfd4a44910c9e567507abb6cede4fe601a7a2765c9755aa2cf6ba4814223811a26a8a1ef499cebd9"sv,
                "20153bf8ea2953c48251ebcc4161f8b6e28499e5c76c24014cff4a9e2f62d25c"sv},
            hmac_test_vector{
                "1c4396f7b7f9228e832a13692002ba2aff439dcb7fddbfd456c022d133ee8903a2d482562fdaa493ce3916d77a0c51441dab26f6b0340238a36a71f87fc3e179cabca9482b704971ce69f3f20ab64b70413d6c2908532b2a888a9fc224cae1365da410b6f2e298904b63b4a41726321835a4774dd063c211cfc8b5166c2d11a2"sv,
                "5448998f9d8f98534addf0c8ba631c496bf8a8006cbb46ad15fa1fa2f55367120c19348c3afa90c3"sv,
                "7e8cba9dd9f06ebdd7f92e0f1a67c7f4df52693c212bdd84f67370b351533c6c"sv},
            hmac_test_vector{
                "4953408be3ddde42521eb625a37af0d2cf9ed184f5b627e5e7e0e824e8e11648b418e5c4c1b0204bc519c9e578b800439bdd254f39f641082d03a28de44ac677644c7b6c8df743f29f1dfd80fd25c2db31010ea02f60201cde24a364d4168da261d848aed01c10dee9149c1ebb29004398f0d29c605a8bca032b31d241ad3371"sv,
                "9da0c114682f82c1d1e9b54430580b9c569489ca16b92ee10498d55d7cad5db5e652063439311e04"sv,
                "cdeacfcebf46cc9d7e4d4175e5d8d267c23a64cde83e867e5001ecf26fbd30d2"sv},
            hmac_test_vector{
                "44131187c07a8e3979254b0c1d1cfa8081f0beb8890633744932af3f6987c7eace6e153876f639dba46b1e9f3e2a7fe673b3a954a00082cb7516ca9a54d9a1f1f924499960192ee1e3b623dca4a9efc92a6608d34f769efb5912db5267f06a6b0f5d3610458c74347e2ee32916425213ef2f649d5c1090ea3d4f6bcf6b752a3f"sv,
                "aaafd08fd89bebe239ab65bb190b86d49c5d39faa50b1109f7dc8b179bc693f0810449c36a68041a"sv,
                "0c19ab5d4ee7b64396eff7b2ca9efa5ca7369c1a1ed14952445d2fb5ece9473a"sv},
            hmac_test_vector{
                "32b45fbcbaf262bbe347360bd6076c43dc26ba9573fcabaea14595de886ccc793b09157dd0a85d74b6ccab9c49335446a45c6e7cb64786e6997c96ef1e4e3123ad6101db4c6a731dfd36b1be4deed1c92a994b25f5e2b171d81b9a335a83e03230c40b2056c00c7c5f8d2fb70abe4b9615e53bd756569217072d8bf362923f6e"sv,
                "b06f7ca7a5dd8baf2ca940811edad87a33da666dc427bcf4d54a8e03520dd5c399e9729d39be1494"sv,
                "a9c9d3993fe7ec4c2033ccf3b73b3407cd999d67455b43a75d6ba97efda3be63"sv},
            hmac_test_vector{
                "14890f3b2ee63746c8249909013571a403eb54273760090db5959b06ff59acfaee6d0c4aece58b5964d10b4b771dd90cf1b63d947bee4f6a12220d67b79aabbd68b02a3850352cc33b10072d4c28182df2855aa418b236239c659dad036155be6b9c908bc09dc38c3329b538e81ed710ef9fd3de7671673f3da5745f4a785204"sv,
                "2dff35c2fe5039123d4c5d9feb7d5167e3e959b31841abec1e5b18b0ece2ef25e04d1f8d030d9b1b"sv,
                "468d8498d46afe74a0ffb541b847bac724faeabd48c41322bf534b284c4e9fe0"sv},
            hmac_test_vector{"3e8a9030eae1bb6084cffdb577623c4cf94b7aee3d3ca994ea94c12acd3e1194cad6d2ef190e0219af517073f9a613e5d0d69f23aad15a2f0d4e2c204ab2f621673325bc5d3d875984145d014bbcb1682c16ea2bdf4b9d56ce6da629ca5c781cfce7b1201e34f228eb62ede8d36cbfdcf451818d46721910153b56cfb5053d8c"sv, "9794cf76aeef22963fa40a09a86bf0e2ba9f54f30f43bff09d44f9d28cfd7b7a45002797cc1437c9"sv, "29973999c4ec891154b83ebe5b0201cf29205d68e7be2c1d59bbc81658d6668e"sv},
            hmac_test_vector{
                "97d29ac5ede94c0a5071e0095e6102123d1726132f9dc102672ab87b1cec18abdb04096c21d3fdb129742d250389460fe63b5f79c77c2f912a8f7d4f39cbd758139c872366cac35a40fe248322825adf57481d92832e66057f80e08964be993de6a0fe31e45806cb3c17ad6ae4d2a44a374647a88c3acf260d04c970c74ec720"sv,
                "c1d60814376aae39c4111246353485958f95558fa38ffc14e4a0981d76249b9f8763c4b3e2ce4ef5"sv,
                "50db0ecb5b31524a6914264930abccae0da07f01a2bbb9408207156f8e8a340c"sv},
            hmac_test_vector{
                "8734e49e3e629deb352c77f58ff4dcce2af3b1182e7d896ae68619f6cf66ed69efd95913684ab1484d51bc06b47a67d70d48b7f9b27901bdbf8c5d2d238158f1f7e0e9740ffca742cf7938b5400c0dd063824c6bc6040e905499cb2671ec12cc47507e085a01e5a163acd2495b32367fd6aa5ab492a518ad50b54b28e23084c2"sv,
                "ca5f3eb9308604f9fcc2af1c6a3175cd8a75045593b473bd7ae37933c345ddb0982e2dd7180db31f"sv,
                "a5772a3da86365b46638f1e97037fc0d8351d2e19ed929f85448ebf4e8379a8e"sv},
            hmac_test_vector{
                "61c5be972faa61f67bcb332542c0b8a7c74ef67cdb95d6f65c8acec8fca8bd6043e31677d8de41e6fc5d3ebb57fd8c8cf723490b96329adb1b014da2648cbd6043e9f6ffc67e1a2bbc72046374612a50c854c8565af03b6a1eedaa2319caec1368bfa65783f4b46dc3f0cb4622545c9c43c9bb86b237804a6c382e72a2cc1222"sv,
                "808d7aa9aba6a40d1bc43e9b932ec8e9273b892ffc0a769e4f7255f3b83c224bb090b23952ae9616"sv,
                "5f1b8de0e3b07da6f9ce1a494be5712e54ac16080bb4f6d5373620d86d5ea5c7"sv},
            hmac_test_vector{
                "b31d11cb4f5c572ccf3405c65cbd218ee8abdc08b6c82e5d1da2baaf8980f7a9c29b915a718b0d43e000adae01b29342b29b28d53f63bf81281c76fa252f5d1e6896dbce224c4dfd4802ef0697140043d6bb21db5b84ffdbd001318937be64f52c76b5d06a875e8191a4957627cab1b8dc758fc3121334949cb9b303c6155153"sv,
                "d8b994bb8df02d7803ca2e09d601b918d6b5bde90b611bebf70e078d1ac7b152bc4c2528e60b70f6"sv,
                "8e44d685fa79395b4761cab89688e37509e69ad007a2794c8c0b4152b67036ea"sv},
            hmac_test_vector{
                "3ad17308cd259688d5b52c32d01a3b868bfaa4758bdaa5ceac34a1f908ca24e71a39224924d17f00cda4d4d50fdd716b50549e71cf5f271c42ea17d5becac32fd64e0a1b0717dc5f542af9442d44fb8f956e97b384d020458aca4cb0b6413b2ab637b5e73f9fb48cb06f22e6f2f6e3dca27016a272d89830ccfdcaf3b9d895c2"sv,
                "a89bbaa86a339951ddcd37799e21b5d1688e4abedbc72daf7cc9b5adfe10be34c00a504196cc7bac"sv,
                "905d55da5d290d023f6940fcb904c50e70181c95000eb1e6a33aa01077692736"sv},
            hmac_test_vector{
                "46eb5059055d3345c1ea84a4ebd2d7cc53361707eccd70e7cfd86bda83585bfe7c7ef937e1634b7e93f9ca7c6a42c357c2bffecc362c9e7eab6a488d91bd876b65376feb7a74819bfa88cf542736610fe763d6fa80c94ecca0f08855a05a485909fefc9e58f99e44fe7fdc55ab17779dcc08e9bc530e4a79b65274593a996671"sv, "a9560fd61746d7f986b691f070c920256a535c21a64ab5a2bd771aeeab7119681bcc4761e68ee230"sv, "9045dd3fa6e8f2ef7c57b03932d244186caa1bc1d4b694c47e1f2901d9eba193"sv},
            hmac_test_vector{
                "53cb09d0a788e4466d01588df6945d8728d9363f76cd012a10308dad562b6be093364892e8397a8d86f1d81a2096cfc8a1bbb26a1a75525ffebfcf16911dadd09e802aa8686acfd1e4524620254a6bca18dfa56e71417756e5a452fa9ae5aec5dc71591c11630e9defec49a4ecf85a14f60eb854657899972ea5bf6159cb9547"sv,
                "b763263dc4fc62b227cd3f6b4e9e358c21ca036ce396ab9259c1bedd2f5cd90297dc703c336eca3e358a4d6dc5"sv,
                "737301dea93db6bcbadd7bf796693961317ca680b380416f12f466f06526b36b"sv},
            hmac_test_vector{
                "f9660fb784c14b5fbec280526a69c2294fba12aea163789bbe9f52a51b5aebb97d964f866c0d5e3be41820924fcf580db0725c7f210823cf7f45a0f964b14e5555070d1c3ddb2c281a80c7fbf72953031a4e771d7e521d578462cafae5a02ac8eb81f082e173ddadc8c41d964bbfda94f5180c8da28a8ebb33be77b0866fa798"sv,
                "9fe42dfac92a4a136fa7c9f6e331b5d3a61aa73035b53a8d2517be43721b31b215a96b9bd43798cb5e8febfa97"sv,
                "7786c155d10c741b63ec650b7b1aa3bfd71ac71881ad06ae98fb082f17e0caa0"sv},
            hmac_test_vector{
                "64a78a4d6fb8ff3813df8dc022faaf4415e4df2949e16467683c6c47242e5a6b2c02610e5877528d2766b2266ca41000442a956c4b73dd6b10260570c6f506673cc541f50f0f5b021e864a753efab03e2f7c689acfc35f928ecea6c522cbc5687c38518bfa48c19ede887d33ffc23806be21803a3c9793e5ca7c75cfa1783f77"sv,
                "98fff7b5f77326c24471bb9c317490be1febad28e2e825afc41c3b97cc03c963405ce3ec68dcb7b19523b76e62"sv,
                "c02c6022ee0de099e3027850be95a29ce800118ed3a97757dd8ab9e60f69a005"sv},
            hmac_test_vector{
                "a7734a0739d51af0ac2c4039dfafa86f36fc06c2355d0f654d4ae938f52fe0a5fd6f5ac71fa80dd2d8396faf76016ee6716a62c1fea640afe23910e684b8a14c47d07b98168915b441cc48668724043074c14275edc239dc09b4d5fa2255652b2c9e94c046019a608ff0b3a83b9ed015e6098d24273864b769c120bbf68f9408"sv,
                "8d649e5ccbb8bb0032cdddbbe44ed0b5bbbde78a30c0f8437bbca985fca5ea08da15c34bea9b5086d2550ae16e"sv,
                "13e0834e4dd72a2ef7872249bf895da4432329c6e8ade8665d702ba33bb677b0"sv},
            hmac_test_vector{
                "0b9a58cd96351a135c559d17e82ede3434a0caf0befef5dfdf138ec5586793fb2ebe4114b9e2cfbff7a25bef261b253a9136fb7faa72f4cc59e4617f947c01ab308974bdf67ff25ffaf83d9c28fad44520786a94441b96100e42ccb0a8478c43b604d90f7695edb90c602b651753551d886dff77b4804472a835b7a2bc509c8d"sv,
                "57958d7e4c73fa606ef405d77ea4977ac96b8813fc1210483a037e7b6c502ceed8f7b22bf6655aa37e38d495c6"sv,
                "cd251e66c421bad1b37cfebfa3c04ef30b8be4e5526b10fc48fd5bc5d6f04bb4"sv},
            hmac_test_vector{
                "e5804b099ee4b351843adb9c9e3c231773256e6a2070d697a9e29e258dca677f9d88a7970d4c58cecc20ed1811298a5b37297419ca49c74fe216679dafc938a656cb92bafb78efb31f24e71c2d5b5f994f6dfd82862adfd2faeb8c408fd22aabb852f2bb90f1e2c6274cb1f0195c089766f9efee7d9c86e79a69f557526da555"sv, "6d32ba0c063774bf8d0621b208d72095f684faa33ca6f3dc62fbdf95ff0c3733720c6c34d3027b6f2a2bc29cde"sv, "9d283d8e8e473a16162d186e96355b1885370e83954dbd08622dbe64f0aac695"sv},
            hmac_test_vector{
                "8b1d4523b6e457f856e5f09875d389eb6587223e53477ba01f49878c6c731ec9f365f28f1cb9c4ebcf89d8648732a6dfa958d2c0152b5e52fae81f69eea26d463e421fba82cdb78f75e5d92304930256a54376a6ea107a995642c45c6f1530a914bdb4ed11a696abf100dc1b147b0518014ff639fc80373ddc605fac1755cdbb"sv,
                "6b97478fdafd3a85d0d9b339971a70c2fd24d542abd3e20eb2bd630f67b86668719df258204bf66201ee80acaf"sv,
                "6ab8f69868b4c87fdec9a031045b34b66660212f687a83d561bc4f9caad59fff"sv},
            hmac_test_vector{
                "ff8662e9af3a38d3efc0143138fa619a57d569f61e29b3895ae08f2d055befdebc11787c7379d9cd672b5cc25442bafbe804348c78c5df02f30840a114e818f0dbb681783de43ac81b2140bc71c69effd07185cf0eef9f003c60a144d89520a944bda563774103ccf3ece8a9f64fb3aff564854646719b8c1d2fdb9db92cac12"sv,
                "89c77d79de98df18f0cf29a9316d6dc46b61eb7af7f1e2de2f5ca6c525bef3c996338194193fd85b9c6e66a811"sv,
                "4746e6f151caf29b3534b2f493f7cc1308fa119116d251481572a1b53a8a1b3a"sv},
            hmac_test_vector{
                "33ab861f089bac0e5c886f66adc568ae7ba331655a371de7475e269138ff2725f7904c702fdcc62ac703c31d70c29d8a7af451c8ec59342ed397e133da7e76d41b90003635c1338d9f7b5f3c3ce59f3e2f6554c4f064d11f9f5158e199e8463f4ab48aba42d25bff8af92b0b38b7d69241fd20a28fde5e84539473e39dc4fe2f"sv,
                "08cce7d7f3ccea0212cf0299f27f3d3f393a97d3dd71caf1954e67bc8d9a26db5edd7ac23dc7693372ce9b040d"sv,
                "2c723282159ceabc5b367b95cd807f249f1dff7f9ebf5ba179a43081454e1b05"sv},
            hmac_test_vector{
                "5a2240f64fc704ce9f8ed33d019e4155cb46747a659e3421fe6b42d67f44eb84bdf3dcf1f31e38886f27e85b8b503368df238e1bb511b515bd59fa2c032bddb31d0ddefba97f8f19f7daedea027ef055a52c61d00bb1ec2668c57677e632b180e339ed1c5931310b9d718af34d70a3a4832b96a04fc702db65785ebf12a18c73"sv,
                "1a2e86f6ab2db235e5d7f00cf438680fe5b442dcb1f8c3ae7730b92f097a1a8eaa9be8d216f2576ec3aa321567"sv,
                "22de07c3055a8935b52bb2c85a9a6b7ffd4038b5db4069c07e9e86ee1b171d25"sv},
            hmac_test_vector{
                "f407f815a33cd450c0b72a378f00762788f91bc44f09f93de67a41d2222088935b3c1b6a689f935bca13a90b28f64b7ffc28ef278b28271b1a7975a45f4b61fe3657ca5c950b7a2dc2e7fd9ec327b26017a222aba3f29183efd5d33a92d36136eb21acf412c6b14d0efccef849d9d451412e5d587fb060fdcd55029ba401afc2"sv,
                "3270b4e48d575f0312659a6202adbc4e877d69298de4090ed47278b4433fff95802e844fbd73fd4ad5532b9b97"sv,
                "dd1a8105ab753d83d90ab39adbc748940fefda05bedea7eeebdbdf54b02d9ae1"sv},
            hmac_test_vector{
                "dbb84fef130f929805b0876cb4646a046330bc33ab1cf1e9ca3869573ee1a1549341ab007915dba719b3c4e8a94b62163e6d99dee2cbde2ae74135467b125b417c7544978d50c80c694399db77e878109f59a8335df3a326135a0d50a4bde6fc3e5c03fb7747bf919c68ee8f45c312bc2dfdd279411ba7a5f78dd9bfe16baa4a"sv, "c704d5793539ef3909bdaa7c29e9c0a0c441814c37bcd062325f6e2e16107be4a2aa3949cf4d14b0f8f8df283e"sv, "441c7fdaa40e50bf1eba073509769b1c0942f3a16e1e183435819d3b5f8538cd"sv},
            hmac_test_vector{
                "1de00288a6e93930070183de9d9ed0ce86f6cc0f64b7bedb5df8af24676fd06fc2e516e5c5e827a7dec07963d5a4b825502d696f9c0ace8baaf6092058e78304f2888f51f9ea4bbb2376c720a2276a61a9f691712d9578abe95f5e69a490e4d2b6b1b7f3c9576e12dd0db63e8f8fac2b9a398a3d9ebe86e3201df726d2d1ba82"sv,
                "5b2cced47045bca47512fe226c1f415ef127a209bf885b8a76f5a24f9c6bce61e166bc3ca75471ddc14a001c7b"sv,
                "15c62ce7a3bfd5b3b3856d6f47cb19bb7030dc469e35a27807511f81ea83091c"sv},
            hmac_test_vector{
                "2937aa2ff7c942bf7dcfa670154e988c28177391969db4995804ba1a647acacfd0ca56f63b2e7fbc6965d8f62d066d118c14044c1fd2a224b9d951104a67216f03fa6dbfbb1e5f0f9283b6b7d452c74620c1c2bcc9e637fa7cc8d97623bc81330aef76f1403feba1414fc91bd1daaf132b4737495b7e7c01e9fbd9b3b720f303"sv,
                "0d4dd35f90f0a10d7d8030e9919446f3d5e2532472bcef0cc5db84bab65c48dc46086f2768d89ef912b8a23d93"sv,
                "d5596bcc39af2782df1cd9fc8c37a8f96789275422f511280971d8429a8cb661"sv},
            hmac_test_vector{
                "dfa3b06eb1e30b47ad9f0bf0f441fcd94856ca8b1f4cb88cf6795582e860ad9c7f30bc2eca8e289bb0942f78831addeed934836097fb664e4e91b47acb5fbc49e9a15d6baa25bfbe864f42700361b46586f9c7d869dcc2444df17685b291743ac5fe7d6f78303a79d8d82d209c9fe804f9ae7d39be7435359ca385ecc57c3d39"sv,
                "5ef946b64ff80e4df8ee98a357f07c825c3acc434d0f994069c0b88ccc0ac5e192a469d93f19d9615fd49f6b69"sv,
                "223dfaf583140a769c805c33f1f30bfb2f0926b088f55439dfeb4f5a9ceeedf1"sv},
            hmac_test_vector{
                "ed4f269a8851eb3154771516b27228155200778049b2dc1963f3ac32ba46ea1387cfbb9c39151a2cc406cdc13c3c9860a27eb0b7fe8a7201ad11552afd041e33f70e53d97c62f17194b66117028fa9071cc0e04bd92de4972cd54f719010a694e414d4977abed7ca6b90ba612df6c3d467cded85032598a48546804f9cf2ecfe"sv,
                "992868504d2564c4fb47bcbd4ae482d8fb0e8e56d7b81864e61986a0e25682daeb5b50177c095edc9e971da95c3210c376e723365ac33d1b4f391817f4c35124"sv,
                "2f8321f416b9bb249f113b13fc12d70e1668dc332839c10daa5717896cb70ddf"sv},
            hmac_test_vector{"6dde9ae867e2feb367008a975d7853ed8f89690f3c87a1107f2e98aa7736f477a527ed64956f0d64c1b23361b261de78688ea865fcff113c84817e5b377e829cd2d25bcf3adbc06762cfda736f5390d01a49079d56e969f03313e6c703e3f942bb87ed0f9c4d9f25120085b5dc75ef5d6d618da0926d3293568dd7d8238de3d0"sv, "ceab398e4107483ede64ce107c9270e6022778b61f6a258d3b7045d4ad8506d32ece0a738d2cb948a562dbce8d7b66f30e6694d65ae439cffaa454af09abe449"sv, "2d3a760595f3fb19293cc6d23651222a9f5a4f02284457a9c1ed4c43ac993ca5"sv},
            hmac_test_vector{
                "107bdfb55c601e74f6505015a5cb87bc0eb0b2e7cb04594fbeef8e0fa5072007eed21183cc854a188a128ecf2062ad8604dffa924236fea9cf5b6e001acd5bb0e51ba95e53a7c21b42aa8b89da78983f66069c6f63a923c6d7208394e5d50f2d9d608f8f194ded45c51f318bfe94afb2df2b7fc657e42e6f7f47b3152ba7a547"sv,
                "6a6155dc4d59c6bf46caa3de09666326da308c51a23e6ec342bd12b227376e8a1f11da906b58c8c515bdaf0d84dd48904dc6fd614cb79f5ef4285757e30adf72"sv,
                "6dc2b05619ad5458ee3de70b0c1649b3788e1a5312e8924b5486905506970881"sv},
            hmac_test_vector{
                "f62796faaa333dddae596f98cd4de3931ed90710287446604a158b575b4901fd8d841e8697b4df85131c555c246060f75ddcbbbade3a38b7c0444d25b4f6d00de6d8ff47288bc3a54ca1366ed1b2620ec3ab4c0bdc6a313bef880f3587766705cbcc4124a4dd72a7228f1ab61c6a704017eec2ed692ab7549f8ad86f1bf14e4b"sv,
                "ce97ded47e101a6d0aa1041138093586046524f54345ec9e860550c9415bfc002d2c0d7beaa4d4dce985d71d89bf19c680429c637d1023350c963c28b93c7e05"sv,
                "837ecd647e03fe8df9a92c32dcbc87d0734851ffbc17376e03218cce9cbe974f"sv},
            hmac_test_vector{
                "44e9a1f1437791963c1a3e0aaaae24affc3b405844d16a5233b6e5a145c4358b390c305bc4bf585f864f68333dd12d4139a69789105a109e92cc0cf1ff8fe2527891dab4b4fa8731f457574e39f8687fb4969dee7e3af27889590cf8d74415c9e9c0c6867bf0c5146e7c32e306ec7c7055557a0ff738b7e700a70d3e33a975f7"sv,
                "554e344537a09659920c19b40f2850b07235c3c7209993a6de905c82db1e5faff148e16f2883ce087c6da219e0bb892d8272c591515b5163bdb0c4ecbd1c7730"sv,
                "9cd24a0efa26c107738f5335526b57d8c93e54fef8c1babbbbb2d42f3a1d03c6"sv},
            hmac_test_vector{
                "0ebaefd2153de2c70537ceb27e5ee70105ae85bd4da38462b4abebed11dbcd36ade16d808f3aa54ffda5897a3fd74780a670521fcd2ebf231f60ef7d999e6e94d1b81be038ec89b49c5ca65bf1bf9a675056f2464021fe16355477ba5605652e8327401797bb569fea456c7f1b7da85d0c48af592de60ae3fe6dcecfcf767cab"sv,
                "76d8e0342011d2bca953b26ee200e56685b721d50ed4dda7cd3a05633a50f153884998e67da901528004fb7df4090e1ec4c0b11f3f10bd4727842215044fd9ef"sv,
                "1cbd4f923d683ca38aca6cd0ad81151062fd642b155b2a950eb551ca8216b0ca"sv},
            hmac_test_vector{
                "d98557504a21fc3a434c780c328ec239cf8d7c26f58d6ad7b23329c79a8e1e176058aceba778aa1215cc14e5a92600714f94d4d8b2e5b7f45268453ed6f787eea3342264ad13cec78d990aecd5e30f79a069024a6d846d132d2ef0479a093439cba4218205f951a2d53ac4ea5bcdd599e9956c45cd73767c6a0c92ac8ecd0d40"sv, "731ec9f365f28f1cb9c4ebcf89d8648732a6dfa958d2c0152b5e52fae81f69eea26d463e421fba82cdb78f75e5d92304930256a54376a6ea107a995642c45c6f"sv, "4f2501d2a88cb13046a6549f90e4ea924773408bb684025b5126a8fc21f48670"sv},
            hmac_test_vector{
                "6e09febed308baa41a8b6e0f7fab61808c9c8471ea32eef178a4888e9a910a77d44026e2972c02ac5ac0ec3fed5f4ab90aa7cf4b2ef7f5dea62ea7fdedb63def35c2ae2344d301d2818105df4f78420299c12f25ae43a60e5089943f07c5f51abc15004982069e5db75721b54cff33a261700cc8151ee9c89c3bb91c92c51942"sv,
                "cc38826523a9097e0f7d075a3a039a70ca1e2b5590a6443e820ba1c16c3b89dbe2c65f37794074ad37e81f0a4786100ff19ae1bccab2eece281c6786d9bda3ac"sv,
                "83b1403389173568588e5b6b8cf9da180408c79f91d054ac5cd99de0b728ff66"sv},
            hmac_test_vector{
                "7af390cc4edde0f3d496137d0cacd0876b54c909dc5ce36705619742cb42989418d4b6fcdbd80256512a338f843b48b711c06f582dac2607ea5ca038b7126a5726a54e14f37778fe41a6d7532687c6166a50ec638c14600006f51134d29566dc2dcd21bb9ba289122b74c870fc7992cc006a07d1007cdb79e192b4dd25b1d34c"sv,
                "62c1d149567f05a0b76c4fd32d1f365d170cb165cfb38f922f1716225472eb36a127327007f8f5c08479ca7beac4b0aee26f3bb130bbf1ff390ef344c2a4e0b8"sv,
                "2f1a4c2bde7c8bdd7d8a9b6315b19ac654266120c652fc24ab19e00ac11c5461"sv},
            hmac_test_vector{
                "75ed3ae9085bbf2d034b864d7f87057c2d0b12c7395feb0375237903b3ebd60e724e0c8fbe3a200f518a4f61fedb971c509b794f6e62fe6f4186f894d9ea8ae50d16ea51628d66812f5aa50afeed30e634253025f5ae7ae0428dc86f64f949db8e6d5d96befb996ae4e312b04664d8c223d2c0b396e9673dbe6173fa1cc21cd7"sv,
                "af81e327525f3a9104b7282959a0f6600fad7efae7709bb8b33cde34b12f830c1770a342efb6abe3250a0ce7dfcd34590cfcbeb840b3e59cbff03f9cd89aa870"sv,
                "579d35cef5b6f8468c8285829861e93587c8dee5791208406a7f4bfafb70abfd"sv},
            hmac_test_vector{
                "7809e59ad48aeb2c6f03de775b1371b7f86926ae0b87098e10c69e19d29b18073818cba862b6e4caf45158ddb2741a554ed791507d2649795004e92cc25065db8ea774b0432a457399816daf062025108dc8b210d75124d284a8434ec314c7af20bdc7f99e6e74ef069a07347e9df8b05d4571353e91026354b896c9fd6da64c"sv,
                "17a5baecf916634433dcf133ddc2dcdfcf4a680e088928985138c01d1d09eef3b437cc6290614f14079814c72bb75c45eff255968bb29b7421a1feffa00086b2"sv,
                "810d7bda3421589a7dd60597447edf2b987f1e7283f3c65890248712c80969c1"sv},
            hmac_test_vector{
                "4745100cec0406cffa146350ee12213330d192123af4a1bafdbc5c98801eaf6ecb19724a0346a7b9d6b1fc381ae798ebb0501392afbfc6b8be48462dc2522bb7baec1605e665f2e42f1679b6c383fa1f00a35a01937b5aabe1f2174da6e0d7afdb680223de886fb9cdeee1b1320dd236e6716f492f4fe3fb2c61d8df73f03bbf"sv,
                "e09ad7d2ff8d559a26e0454bcbfff844e8d2415b07872bc59c93e73698f308483bb8f3212ac29050c1cc46f9aaa92732afcc67accc0e139689acffbe878f01fa"sv,
                "055ee0ade716231bcaa0a7d18161004127a37e7aa12773433a376073474d3d58"sv},
            hmac_test_vector{
                "91ea78334108ce6261ddee5d98045bb307a6e8f3d0ee65c1d9bc7d28cd9edf3264fc9cb6e592d072e9238559616cd42eda584d5200729adb619f5ee5740d632dda67f5dce34b89a054fda301685df6f31416cca78f19a8a7124a2a22dd7834847a934b4a451940152cd20ffdb4bd07273c4a2b9a86c9d94e7323a9860ec89860"sv,
                "fd013d615c6ca959030a520e148808a07e27d38a215634d53486ae8be43a856f3e5dc6eb4fd9874a8a6570276a9e7b25585af7e1ce39d325bd7d195f2c1bb951"sv,
                "eb5aaa4ee702ff7b5324bc72c98fe87df6d9cc342b053ebce6cbf27fdea0eabf"sv},
            hmac_test_vector{
                "ec638734d336b8da6dfaf3da9e18c7131494fcc0709cd3a9a6618e9ba62533153c958e44345a7531c3eb503a22a5d8bf7c1d1e1d0ab5cfe07d6db7349cfc859d2e20cee81a325462cdfd8747dcd04c7dead2fe82cd96b2a4ecefc070eb067f6c8ba94f09cbe6ddd354d9a2eb13c2adb7285aa3d8ff68045cbc8faf35dd6aa9ea"sv,
                "62e3a735edcd87fca0dd1d2797cc0e574160da9ac23f60e39501a5b77688d1287f947a0791922556f5b50afc434818bc83433968931cd752c9df9f04d8818531"sv,
                "26db47a48a10b9b0b697b793f5c0231aa35fe192c9d063d7b03a55e3c302850a"sv},
            hmac_test_vector{
                "ac4756b851fc8866b9adfac2d02599148e0db7757a62b1e06d26cf8c99556b79c91a5649ea437752cbf3b5f121961821ce1a2a4c635da461e3e14626cac707d04dfb6ed1e4ac40f106ff5ba03304e28a38e99a6daf6d9427c5980d1440a99296c05168f5441e2a6af13ab4760f55407855e0cf7f667ccb5d9bb2eafd03e455f6"sv,
                "abc9ccdfbd92b6919a5d6c6b5a765a39662ed90080d3549204dfaa5f6d70d48e1af8c84d53369d658765ef11d7b38510d9f431f99598f8cfd4da73d59b3b75a3"sv,
                "0e445d77789a6947da70848dc4da5dc9c125869bb6945b04304bde93829a75d9"sv},
            hmac_test_vector{
                "ea7240529980076d3b028a083ebc4e24efdaa06c9c84d76bf5b2d9fdb842e1038e487f5b30a5e010cddb4fcdb01ffc981eb0fcbc7d689207bc90ad36eef9b1ae38487a6dee929f3ff929f3357cb55253b7869a892b28f7e5fe386406a2776ed4b21d3b6e1c70cc6485947f27e9a5d8bd820380b9eced8e6b865206541be39fdc"sv,
                "c09e29071c405d5e820d345a46dbbf1e0f8202e92de3ed3e2d298e43aa4f846866e3b748990946d488c2c1ae5a6e99d32790d47d53d205481a497c936bf9ba29fa9c2821919f"sv,
                "49ae1c4a7a570fde47f7517ab18898b1b991d03cfcf8c45bb3615b5f755da682"sv},
            hmac_test_vector{
                "93b7ef0e470ddfac6aef93c0dcd37b8f1c4baf5eadd978e3bf0512fa0baeb099ff9ec1061b6172479b5674db5606ffa7e6b5173309370e1647054aafd5904816bad5e1523032cccd4d786505e241ac83a484911189666f287553d6a8164e8dcb0c85d75c4e29f624c97ceea64a2c8b0c9ddfa560f70fa3ff91183e4b968f88a1"sv,
                "bce50cdfff843885d4f364d69f93bf58a2322c707b82e878eec96d11e5db97bbb54606a3a3ccc3bba716261070a6f759a70ed3cb785fd1354fe56648df11863669b70c803b7a"sv,
                "37f9f32918308210849dfebf8dd456804babd6845af07218f9d9be9df9743d55"sv},
            hmac_test_vector{
                "21063443bf02ffe9f813dc6688920d036041a2a3a63a9956fc254a2c05ae03472537ef3489c93c7c68517c7588094c5e033434ab4b0ecf9e6c032c17911f73adcac6ccfd0ca57c427ae85127e2ad41d98bb94e5f2e6aad2e42ed26f87cb1bec6971c9446517c0966b6402321a06834997f3ab66756377a2f064d0277cf4e2bb9"sv,
                "0cb35a02ddc8c7fb7c93aeab77b9318118b0fd449524209d879a1cd69d5439e192741f9c5c64a353a774e28681c58ced576783ba20bea51ed82ae50e30e6a147843130900dac"sv,
                "5c258ba6241f65c2ee5356bb47332236baea227857e29506165861a4c7379c51"sv},
            hmac_test_vector{"9724c0d5c989e5adafcd7527fee269ea14c0aec3ddb62596f3fdee9b0993e6c689466e877c0f6fb4aba29bc40343f53d3edb936fc04ba263bf00ac0fa7c816cbbde4ed09025ee2405a9d9229ed360b2ece058c20db7d8d28e43cff000fe2d5627a24c3c1231c463805e3e4c08462b5a50b65223bf4f1edcda8d872d6078a2c73"sv, "cddf76f985d6797c9fe3830c210567c5094fb979343fd5a1804c239a2ebe9a0e8ac283b0cdbe802c42e2cc5da800c4c1d89da72ba7489ab80e2aef0488dfa69ebc8434b95c11"sv, "3c5a9ac2a0fa2f58825233ff676bedf93d8845a409a42a05a9ae5218cea14680"sv},
            hmac_test_vector{
                "12353bca6b0f3d545ec4b470c69272f72bb5589793e6ca769a226018c5acde83145567a1d6fbede5c150ec3142dc58f81246d4a00acf242a381fe51432447b7eaaf84c8d43222c0da3a0175aca442680a21cbca1d7f70097e82491db7f7d75a5fea552555a8de0122c3d9eb105d1c4d802c17963a1664706d3bacc345360b240"sv,
                "731bdc9fb219f3667c9a135ecf34c7f52cf638c39c554f1ef1691ae84e5a71ace915d9e91043a8ae6a7b6a6780b684f77b0417072f7e279d597cfdf02508c97bf4928c505be5"sv,
                "f15a210fca2cefc4d92bf14ff572d021463bcc28f60d034e87222dc6076eaffe"sv},
            hmac_test_vector{
                "df073817d8687293257d7ed1816803afe292d779f34e14b0c5ba6e0ac1e6c3b9e239f4f02110f4a430a71e906a3dcc7b0b7325bd9cf63600b25d4544d8556126cafb3e61e4894095d935d647a8560929ccc9559cb393b77472c707fbb7ab8838ff16be71091c7fee8aed4d0022fbe3428f5b0e1f216ebe946dc05d3746305f79"sv,
                "85806ff2a642f729d28ded0734aef4f6a3f0bb32771e77729b4391cae4b49bd0a15089fe74071e576099a44d22a0e0e3c5d1450f717f68628460b4eae3945f5893e39c5e8347"sv,
                "6c63bed6c6082bfb085cf2426ef3d0dea97acd717a57ff0aa624d0b803f2ea14"sv},
            hmac_test_vector{
                "cd3f17355a1e254b9821276141a850f0b71cb3cf4824a803b01c71d8dfc31d31fd33ad1cac1776a98d18c6fd0598caa241a3af21772208d36f5270f4437570f963c8a323dbb41755d948f72369e7672b843eb0a849799d448ab7252e8abb496d05e44074715fd2f6849b02fbf6fdef3488d6fc8b45922fff0832d7af3efc7234"sv,
                "f13794e5ea5e27507a7bad638f8eb8b86ca5ad73b5a17424c63c74ef494bbfea084189c6fff5dfb2b6a5967cce3a81f9d9cde7a86f6b33927e15ee74e10beb20344bc121e754"sv,
                "d08563dad7c32c02b305b87fad504918fd566c433e98a1367a3dbbadb26e9b64"sv},
            hmac_test_vector{
                "934dc1ef76993aa82061cf67aaac7714f12e25aa8f6f54840a2ae3d84af32481511d300126db7dc612a5b2ac0fdeb9c47eb316541846781e270c8ee5f6731c2e86c94e4482594c7e75d70ec43bfe7250b6778cb2c2fd3d176abf07ca5c051ffb9a17c4c0735bd059b2bd8db81553c94100412dce73dbcaf63a0af58f63f15571"sv,
                "e3d0c3abdef069e6e4fa35015797bd8a9d64bc9b75f20b028b12cca04a4fe80ff1bbbd88e9ef1003564d499fec88df4503671188eec5d7d089dd18b812c41db43a3746f77b97"sv,
                "5717fc337916d66b4e292e69d507b1c81663d8140536670f3e70e33b04c83ac3"sv},
            hmac_test_vector{
                "c84394086457d8fa900a57f18ea50a93be16f06fc28b5532de40541da5959bb6d2646ebe7491ef644ee39cb87d1219625b213094a4ed163dd707ef80dfbf9564f38195cdbb657babb4015071d58260c973fb418562fc10d95d67fec8a77f0bddf342121b82f906368b0d7b04df1c682ecd4c2b2b43dfcd6f370888df45fd8689"sv,
                "51bbdf37124cee0cd5830e9d8f4b0ecfa44c8b1bb86a6433c18f6ee961ab694d74f93316e5833c44c5e83a039e5d1ed104f246e36e17f4c5445eff423982c883dba9707b68e6"sv,
                "3e0212e7982f43fc303d5e8457d2ab630aa257302ac489c74976cc5678823931"sv},
            hmac_test_vector{
                "36bda8d33b3bc10f367caf71c5ed387fe5f1493c1d3bd2aaf97ad78cba3cc5704c0c02ed78dec72a5bae329f17639720c8f91817badf7511d99e257c68bca5aef6e0102a8e36f01f2f1553327be0227db32aafd8e31d8d575a1ca4145da7842e1d7ffa11e60be1f898fb3bb15b2b81a08fca370702bbc285663b7edc02c50cf7"sv,
                "e95751c99e14bed0dd9ba102f48e5e440519c53208e03ab7133613dad99042db7239347f5a47f9a8bbcda428ef52f5d7408235e4f3246268864c8c4135d27f1dc302a2d57695"sv,
                "d965907e6d0f926a7ea719464b1034a5879c865a00d4df0342b2d4f4bde0976c"sv},
            hmac_test_vector{
                "3722eaa433830abdbcaa9177e373bab05fcb8fd82fc3afa581e34f08d3c07f5f58d0aeec9d7e71866c7a808ef15301251b470a9c455a612c16a586e8a5f1f3efe184a2e6313bd0a657d901319a9f44eb241db807a9474f3f49cbd2c8b8a225859ce5cd7b36e3af8545701a482780086a42f4a1ffa2b30144e3fd3b9052fc9e87"sv,
                "9dd10a4c713776700f7e7e0a710a014b923bf228234daf5e807c8eb3e26cb97fd6c93d6cee2a5d7ab63c2c46e91c5b8be5044fe95d2a76e54ee5dc323412f92f7db6ceb03ee5"sv,
                "9c22961d48d0651bd592fd369129e44822ee22d35c142dcb6b60a725bf177c85"sv},
            hmac_test_vector{
                "03074e714d5eefdf5b714381d80e694ef37c2647b374d8a38a6dac2a2e1d11dfa43c6de19d8b0e93061563fbdbb46c683cd86f58c284ed981399d4adb457f6731f21ba04168011db366bac3acfc66dc8f3281b7fcde159c5343cd9d98001cd719d3e9ea25e47e1ff13fc87055d4a53b741f592857c94067216dd23763a227e21"sv,
                "36bbb59925c6432139c7cd1bbc2b1b05c4010e09645f797e230131b2ad3468e7c9f2369b8b4f790dcb14dffcd6a941b262383341c80fd90d6d46fc8a81a25c47edba482c8658"sv,
                "a6109ba372c4564f4ed8c875619ff5bb64d503225197ee9259dd50264eb1f4ea"sv},
            hmac_test_vector{
                "739f460034249e805aff665d6248a594250695835aa24cfa5d9c9b962f7d374abd0d163f65c51cdeb687f72b778d4854eba00389548a180fb6cd5390dd9580b6a1ecd4f8692d88b3eebbc77c42f2cab5105e425e252bf62e2fddade2c5424ed6a8a446d249422a268b029df9c96075de1baa19a8d56f2d8051357234ef6ae7d2"sv,
                "ffa63ebba8239b6896bbec6af1c7b87b9c69257a0d146c0d5c4e8b8a99b43a18633f1f11b6c745ab05c5cbd8895dd96ad89cd87bb9fee30c373378ecf42274dcc02f3ef06ab9"sv,
                "c580c8e0f6a1f36403322f7b0ae3d06dd2dfd16ebc6dddd205704e97dc2998e2"sv},
            hmac_test_vector{
                "082e7b4cde8914bf07c288441be643e408f6cb5ca932f67e9b975bd54ca706885468708009afaecd4d9ee846ab6c0d70a364c5a24131a766f558ad219e06e4f7e80c68e9d8289040a586662fca865ab459c037bf92465596b4281178133e7a806b214dcd747b24e0b681ea459fbd9276d31108fcc3f968d781106f20d3d62fed"sv,
                "30be326c2ffff6d031affdab0a27d5a8cbfc4ba9dec626ad522615f77307e56d9e23f73e53c9f2c78cdeb5b84d2390727db5b3b4f4dae677d5fa7b161eec81b27d743bd56609"sv,
                "a51f5988a8f0f3992f549ea7f8c370a06d5ae8d65880067997536385d632b206"sv},
            hmac_test_vector{
                "892525a0f02aae7f2264cb024632f11e8adbdbecb7d0c7080832e2373c94014cea02914c1542d1d000593fab43524fcd1f3a63670f6ff8509f1b1da881fb2abbde65ae27ea89a942bbf7fcb65b611d6e1ca20fb62b00929d68ae979e7595f6800d55637b98869f9cfc43eb6bb5e9c2ca281cc720340bfdb70bf5366340edce65"sv,
                "19fb88775a517bfedeb2cde7c9455ca58d40d150b0a47ffbd0288e42e4725822c48d130eec98b13e7cbb044b846026f97f9f18531df9a9fe464a99c75bf9ff7ebf72e80796d6"sv,
                "974752b18d0dcbf29cc6104295e041259622cb7733cff63dbcf6808b15a5ad45"sv},
            hmac_test_vector{
                "b9e944e0b42d0ff454f7f8aa24f00e9ee039058ce4094111e39731b6dc3ade2a4acec4cf9c5be078e4f10a72d3d685c1e5e4d5abd92cd07b64dff87f266f0853ddf1cd61d9c637a9b07ab0be32ecac119faf827218b17ad4541a27519477f76ed918089f54b63d0e1e5a92982979ac187764b5e989e066a61b1065340e9cd203"sv, "81574323c973540719d192833ddb51f13a52dcbae294aebea51be5f6aa47f3571f5d97facdcf0c7befbe809f44bdc73963d8514e4fd559774bb96087ef8eda6e7c64275d6d96c42b4e4e"sv, "514bd18495f6de0e237054b8e3ba1a74c3fada4279ad6b8550f3a14712c528df"sv},
            hmac_test_vector{
                "2ac0bb0524c22b902de34ce64e6172d1b2074e159f517ab1abd152622cd10669f03aed8e2eb51c65bd0f38d084e288c532724e512fd558ddd257d2b1d41c5eb6040767803ddbb18b95a035c5d8492d4d35936b7b3630ee20f625b70f8e71d9dcd0efd0e3387d138c1f5eedce32dd88f223334b9a9eab65017f04aa8442179f62"sv,
                "44f71c2317cde52151c84260d1d3c04a28cc15ce5b3802b2e5357e2bfcaf10ab15d77dfaaad1a3883bada502939948234c559dcd95e7e158338fa12ac6fd21874ec2ffabed051416ef77"sv,
                "ca0053d51f6cf6f9998ff1e0db00b90e82c7b18cb5377acc8ebe9afe20da1c3d"sv},
            hmac_test_vector{
                "f5aff283b3aaa4c71b13c590771d8bd3358d76988ecd1eae653c2f9d72c9b2dc9fc08e44b2e34ec52dbd245872332e342b5cf945e99344da0bca069ee221b2c913b7b9973cbf50fadad7758b6a962cc7ce640f78f38f0571b19b527ef2d9d09b173b7b64976633cde909be13a56d0df3e64ec019f2eaecdb1d571b27ea1994ba"sv,
                "7edeeb6b63c3b9c836c4843ba46bfebd8ca9a6e205c7ed68a29f9710f50c65ac519ff17ad494d9b0a5041f587b5cd05e5f0de4e8b28566e5715fd5e9b8d6c9388580d921bf39bd8d775c"sv,
                "5131ce486de164491b4bbc84e7e461a874a2cfdd769355584a063e306960acac"sv},
            hmac_test_vector{
                "c0bb12a5da628363a71f1f5c9ce715ce8995e607148d772b669f6532242f9830a1931bd952bd2a44821a8def46b92504b4b0c5da50bc43bfc727cef5e0ef81faaf24390c0c92a4ed43a09be40d78b204bf680db0c288755f439eaa9d2b3efb5352361547ef2919e65479f142d86ae35714856692523b359442cba333ef662ec1"sv,
                "6e1b663e808a6986f29956b7b9708066696f9dfe0d7bcdb55696d8bef9b3b7c052c857884d2499fb86039d4eaf604079330ae3e818fa6f742ae49593560c5bcb545bd46d89b22e7f2b7e"sv,
                "665344e5618e0c1fb8758d049409a484fa69b89b009746067ea036bfa0ee8a37"sv},
            hmac_test_vector{
                "854b32866273c6eb110e380b8f3bfd169cc87a6f6149c75e5667b305637b0895465c10c134745773c31ab3be071c8215fb9a33ba231b087870da199564619d03765965d6b8a1a9fbb79d0726a3d1c90cb0ae67d3bbab4cc63198dd4e2d2fb81de0ed39ad362043e9b6403d2aab825a6481ab1ea271221eaf614a0716050ee14d"sv,
                "208f91ccc87965d365cc325d3262b64277f6112b0b9371a4174cee721c2eb32638735ff2a5f8abbc82f24c71d6dc1b9cd2b473375666dac0b789e490c0495569f6a4864e20da0a97071e"sv,
                "42680195f431e71b592899686af630e15996dc718cc29030163d677688a33021"sv},
            hmac_test_vector{
                "99494422460ec858a24394f603b1d9b940a24ad9c6a3d1e9e88781fe77afcd139389f7acc057cbba3d328cbf914e2f32667fc7259afc412594645162d4feac10ce45780cf9a400c3237ead50077132e421dc066bc19e176c5f21bd312e98ec29f384af8a187dd13afc2fddf08ea34a971ac0eff36311bd86f1c8acb5ac03f627"sv,
                "915794a6c6540f1ce9958c2784cefcc13772198cabd4fa17c88de45c281d648dcbd59a100cf4d8c8d3106c960db7b91f59578dd0045bae203897b61570e6210a2f11a5aff2f3c25163db"sv,
                "2ca1bb808448eb29085286594de21e254fb3416f9ab01e99ea33ca83c1d14dc3"sv},
            hmac_test_vector{
                "d8efcb416f237c7e05bed9212c543011c39e6a5f25d7e2cba065788a29bce1464d8041676be9fb91216cc76d049806ad943e534a6fd45b10c41bee5d0b005626f3c0e73a9c50d7cb07fc502acb4ec4d2093181a8a1568581a6d793e5101b8613b1f9e6446b20b9349fb69bdfe83f11880ac11b00252508252fe18ea9a0d41a15"sv,
                "b1a95aa80bac5acb7a18332fc03067600610f376d99e77a272be96063ac5a0ca8d316e6cbe978e575cdca1b8b4a8008d9718a6fe5eb34af12aa0cbd97116d1ceb613b2e3975192b40d76"sv,
                "988d4a6fa87f8138d754c5de9d176c45eaccf8eb8ca1799d87c8f04a966b6f4c"sv},
            hmac_test_vector{
                "1a0223261ab437a4ac1701b4780776c43f0f8949b3e7a1618c3b4ab6d8ae2aa6921f38a2772b28d415f32905251fd3bd1a235bacfac00a486dceedb8143acdf11b4b611f1229c346f89f21299920b56b1b08f7f4d32511965d7693f0eb326893dd0c096492b6f0427ea450e87d1203146748c3e9e51d9e9183baa42806a0e3d5"sv,
                "9e4ba7d72b76edee6a6f290ed318bedb0ad88c8411f9c449bd4ffb3a661b7e41e32ee662b552ec4283e57ee6c7c712bec6773ae2c578789b7afa5425c1b6adb3901a4db42da6c0559e96"sv,
                "ee6492a669e22bcf19bbdfc45495cd0efa9c2f2ef5d42831e3f13a545cbcd6a1"sv},
            hmac_test_vector{
                "faa6ce40d931f3c0cb4538a82a22f0d4f3221f027b99d3d85dffb729b751e57496b4fcadae5c72404fac2c54949e4c4cde664b948052479abcf59e1aef84bb9f088030473e9505c603c350ad33bb06ed928c1196757ea3e5bf3ec97e0f3c43f638529394f2a65459cfd1cd3d7041c6bcf8db9a91c1e58ec24e2461dc81412580"sv,
                "8fa12bc017bfeb6c894020e420c5f76f9080e8733b998ef3a7d0b6563063b66afa3200a82a21f6ba56be003a3924dcbdac1f3610d29079c19213e4e14ae0e009c1ef919b5e60ab4a9819"sv,
                "9611e838fb1d816a0ff9cd269217d93258c34df9e26b74476fe4da0f7dee2335"sv},
            hmac_test_vector{
                "28b18b862ce9541ed6daf81199f9a331133b0ea3e48ff486c1acc6d5c40e9f8f063b7a15704ba3d3cea76b222511206d47e53c93a49edd8d639b7551b224c3f65aa802189648607e259ab1fa9ea665910435b7dc9a4c28aef8f32cf85f3a23e94a7e8a5945e9736702383261aac15ae571b4e8466da1bd31a83a5291745ba7af"sv,
                "c18bc28d496beedb25ca42d1b217bc81891d4c2bbb35380e5bb9bf7e3dbbfd37fef70ef14407763447d6c06e915766430277f124165061236b9fcf057d785199b4381e49a2bcf3ef85d0"sv,
                "0bb4127d89d9073ea425c303adc3f9db39e40adac23ea61fba8b6e251d79390f"sv},
            hmac_test_vector{
                "80f20152d12b0a5993a2b17d1f55cfc0c078961ed00cd1c21db36d7a92c339691399eafca830621fdef232b06acd5d33108a5fc8c35a6d5b0eb2ff1bb2598c2d91c094a1ca91e4a5268a16f8b38c57a2aeef6de3a619f869df4ff7c5f5ca8f20c10e082a807719543215653f41ba45746350c855c170f85459315f62a13ecaaa"sv,
                "dfd4faa6b9ebfff6eb33d4b536f3f18785fc33e82ddf3908735d0fd94f1f09666fa8f2667f876611a8d17d3256ceaa7e3ff3e224a11000a5cacb68e6de4dea84d53bea67c3e8be9a5cc9"sv,
                "109ebb4cb2ad746762b6652fc63b99019857ae89acfe9807648c3cfa151fed42"sv},
            hmac_test_vector{
                "b11389c7dc20ffd0c4a5f887f2576bdc302c7d2af7089a012799c528fa7f2ce23bb10071b31c83d9e58d63e6fbd04670ff1aa6de4ea4dfe94a9986a35032fdb7ea1f44f2452a1202e517257e97ced627a7bcf06e5476c236819f73daad0d96722527fe527891d4d42c0ce658af97428890da04e1efc56c6f337534d7fb57209b"sv,
                "c96c04a3bb0816fc47e05913a715fbac9a3ad09db75b48e8013d9f27bbe8532d7e63dbea88bf968f575602f377552e35987872a4e3155ddb8e5cef30aedd08504d4b2123bd7f3af62bbf"sv,
                "b53db6bf0c8317586ae6c1a1e2857f241bf55dddd1b423578c6949d4bf014611"sv},
            hmac_test_vector{
                "57e1d3ff5fc4785f9370df2e5abf454579752ea934d2a9bab568d5aeb22ba43e4bc7df9f31366bb40d91ca822026e4e426cc088081732ef993ff7f676c571704a5b809278b50a3778108f4589fa18caa9f0283b3fad0bd594e406b950329d5242e5e5880b53aaa0eb57c66992055c4ffabc0a72ae712de42add2a321c0ca6808"sv, "9319838432ca096960e2196a06398134ea06e4e8799ba470c54f0512cabb9045f529b6c4e749b6e27626c11df4595bf5b47c04ffcbe218351485f49077405ad96a3f17bcb7b3e21e80ca"sv, "4a34bd4dfeef7fa1dc739280f16a3fe1281a51311c10a920ab43d406d4ae3370"sv},
            hmac_test_vector{
                "6b8db9acdfd24150808a92368596557181d445e5a04e91112db2812b58035d72378d8bc00a1ef75ec373b81dc6f1f0a2ed96f302cf2eac8f42ca3df11e6ee678440a28b0dfab2a36eaf35bcbf3c759a71e47120f6c03292a3d6b9b111488a2259bead9a5e7e2a180fcf1c467947f59271cd0e8360035ce8b287fe2b3c3b95822"sv,
                "2914da23e86a603cda1eede153be2431c2947cdaeed6a1ea801d18e2c218220ca682e40f0a51c4c13a31163cb730f83437bb7a88ecc903160956f0d483137d1d145ce948866ad57f2eca"sv,
                "4de7bab7fe9a0a9bf7b51a7cdf7d929f2b1c6ff4575fd527baba1efdf4254890"sv},
            hmac_test_vector{
                "138efc832c64513d11b9873c6fd4d8a65dbf367092a826ddd587d141b401580b798c69025ad510cff05fcfbceb6cf0bb03201aaa32e423d5200925bddfadd418d8e30e18050eb4f0618eb9959d9f78c1157d4b3e02cd5961f138afd57459939917d9144c95d8e6a94c8f6d4eef3418c17b1ef0b46c2a7188305d9811dccb3d99"sv,
                "4b7ab133efe99e02fc89a28409ee187d579e774f4cba6fc223e13504e3511bef8d4f638b9aca55d4a43b8fbd64cf9d74dcc8c9e8d52034898c70264ea911a3fd70813fa73b083371289b"sv,
                "4f1ee7cb36c58803a8721d4ac8c4cf8cae5d8832392eed2a96dc59694252801b"sv},
        };

    for (auto &&vec : test_vectors) {
        INFO(vec.message);
        INFO(vec.key);
        INFO(vec.digest);

        std::vector<std::uint8_t> message_bytes;
        auto r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.message, std::back_inserter(message_bytes));
        REQUIRE(r);

        std::vector<std::uint8_t> key_bytes;
        r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.key, std::back_inserter(key_bytes));
        REQUIRE(r);

        std::array<std::byte, ivy::digest_length_v<ivy::hash_algorithm::sha256>>
            digest_bytes{};

        auto nbytes = ivy::hash_data_hmac(ivy::hash_algorithm::sha256,
                                          message_bytes,
                                          key_bytes,
                                          digest_bytes);
        if (!nbytes) {
            INFO(nbytes.error().message());
            REQUIRE(nbytes);
        }

        auto digest =
            ivy::bintext_encode_to_string<ivy::hexchars_lc>(digest_bytes);
        REQUIRE(digest == vec.digest);
    }
}

TEST_CASE("ivy:hash:sha384_hmac", "[ivy][hash][hmac][sha384]")
{
    using namespace std::string_view_literals;

    REQUIRE(ivy::hash_available(ivy::hash_algorithm::sha384));

    std::array
        test_vectors{
            hmac_test_vector{
                "f41380123ccbec4c527b425652641191e90a17d45e2f6206cf01b5edbe932d41cc8a2405c3195617da2f420535eed422ac6040d9cd65314224f023f3ba730d19db9844c71c329c8d9d73d04d8c5f244aea80488292dc803e772402e72d2e9f1baba5a6004f0006d822b0b2d65e9e4a302dd4f776b47a972250051a701fab2b70"sv,
                "5eab0dfa27311260d7bddcf77112b23d8b42eb7a5d72a5a318e1ba7e7927f0079dbb701317b87a3340e156dbcee28ec3a8d9"sv,
                "7cf5a06156ad3de5405a5d261de90275f9bb36de45667f84d08fbcb308ca8f53a419b07deab3b5f8ea231c5b036f8875"sv},
            hmac_test_vector{
                "e0bcacbe96dad6f60e51129f35acd03e12276a91fa13fc15037c75cabb0aee3a19253bb8b35cc0e63208867a032c8f4150a066642f6ff9ea197dab7e9d6da67255c16e051a43bce174a489e85464693006f11a4c61135dce4187040937eb4d1c7eda6e2c315771f0bc6f4273911a07151c63afd3f8c8cec963e4a8f5ef4b8b3e"sv,
                "f86902e5e5db478ec6e278692728a812c4cd8745f90a7d9f7915f5a94345fc12d2770a3c94b01ffb9e0412999eb6261d11a0"sv,
                "4bb4eb2db2cc921b159b78a2bb9edc1608bb2a1ca9873b411ae30a63386e462f9f69d9f5fc838ff1818748aab74da94f"sv},
            hmac_test_vector{
                "009b71ebbb05a82c8c13388f65c4b3b57164c5e5ed0eafae3970f7ce2fce5e53b34913008b6226f272ba3f84671b906f4bbc0836abc1cac1db02dd47440d2572fe6e92bea22e97235999def339443a05d24fdd1303e8cb6113108e9ffca22b110a9156eda571e2056b397ba5834797bf2da4aa459d8a99d70923729e01945a78"sv,
                "0d729c7e630560f8e8dbeb745e021f1fcab754bf6c8af0f4f4bc37bdf3ec653f90f2fac27c0c56fd2838859335e05cdd2cbc"sv,
                "a7d1b0081f4884bfa7b30903251fa2803d96822a2160772a473568d41ed3adc77c1b4950a22a14ef5b335a2842046cf2"sv},
            hmac_test_vector{
                "3a7f4dfecb333e44f0258d33d69ceff70ac07e948f5c6acd2ee3ec450db17ce48394447c2b8bd4ac37cd1705070d866cfbfd9bbf851189bc82852a1ecaa13908053cc1a7c17fd514c2eb8277999d027279d132bd45a6470ded21bc3bfb738ea2697facc9b697a9423986b5d1b4d4cbd12c452cd4f8a30b8a19bd8ae4c9b0e578"sv,
                "5405e1f28569138976b03675efcb88ce373e92c84080fe381ea0a853bd2ef8b8d4596eb115557eb5a7009f32d5c49f8a08bf"sv,
                "6cb9ff908bb72c45f49da3f4a8c5265983c1f4d811dfc098cc3ffa58aa6a6d0ba18183455cee62992b4c3a6cb53c5c16"sv},
            hmac_test_vector{
                "0b9b52ec0e46793a179589513f117a956fcf98717339373f5010a268d8b254cc7b996b6460255925b59bdd28436d320945bc868d7a1bad87799617fc45f3ff852137e6f5a56c403c12a26d8be334eab9a44cc9e607a95e5e35a03cbf6261605fa47cabe805a49645d7b221c247ed0c6e35884ac4436cb38b2c38f9fb5886dd21"sv,
                "6c075056122218f595bb28753e87ae6334a0adc24336e85bdb8202545cfce30490ce5e067988108c4e158bf6c0eeb4a4818d"sv,
                "58c6f879aa6ee523dc374a01e541f02f4c3dffb948b071468d2b242bbd358d8614aa7fad660348e61828cea1b758b91e"sv},
            hmac_test_vector{
                "64a884670d1c1dff555483dcd3da305dfba54bdc4d817c33ccb8fe7eb2ebf623624103109ec41644fa078491900c59a0f666f0356d9bc0b45bcc79e5fc9850f4543d96bc68009044add0838ac1260e80592fbc557b2ddaf5ed1b86d3ed8f09e622e567f1d39a340857f6a850cceef6060c48dac3dd0071fe68eb4ed2ed9aca01"sv,
                "d137f3e6cc4af28554beb03ba7a97e60c9d3959cd3bb08068edbf68d402d0498c6ee0ae9e3a20dc7d8586e5c352f605cee19"sv,
                "c550fa53514da34f15e7f98ea87226ab6896cdfae25d3ec2335839f755cdc9a4992092e70b7e5bd422784380b6396cf5"sv},
            hmac_test_vector{
                "07b2eb870ed51f53b01cfbe9b01469d96f69f7cc03aa8768884b4774bbf0e43dc5e9fb3854f8d1e8f05b43028d0600d05c31f5d3e71209ac5dec0b3d2a48e80bcb8222e6e1887dcd84dffc0cd9e70ba7e75ffefba62bbaa35955b2991e53169dc48846dbdc0d8dc813b00c4dbf3df427b7fdf98e687d0314259026d1bf97d4bd"sv,
                "5f76d094bd102281900a1c1973a1d7a6da880c85f8382022e0885d24ea7d83e458637f24391c7afe8eb9151aac47f324e1eb"sv,
                "9891feb5bae6e7f517783322943d7074275f405e54412f3f27ab11f5fdfb57bb54067852789b55dd21107c11a809836e"sv},
            hmac_test_vector{"edd19cc1f4e6185c362787bf3cc3e93a2a21990c4f2380cbf9ad88068e615328d8ad2e9fa108403e807807e0f258064fac58b76430c5c76f9f0b82d8ce788d079b42d38282ccc1556b25b6b7c6d1b5eadacd5e635acd1c2b3ac89f85c0b68dd1de9974b0bfba0469a0357b8ce1b760d55c877452f5c3e6c872edc6548ea6bc4e"sv,
                             "e339f1caaf630eea2e88fa5989bec005c780464f935d2e6140edfbc20a0975279f3b4e8dde740584defd0bce32b025286b68"sv,
                             "af65cf67f4b82a1f1ef5ff3c7b99266daff3e134059f5ff26261fbfd6114f13d69c1cf1195b93cc9814255ca9a3e9267"sv},
            hmac_test_vector{
                "ab6d18f2c0756519657eab694e73f35969a324c279d0c994008214c863a12395494383e744c3f30db3136ab1e7535b0e84a2375d2a34349055a44d43828c89f09454816275471c6c00ec9eda1f93e1a8302ee172ae627f2c0dde7e13e5ecd2e353451a4b9d2a6b9e14b04fcb3f5c0d8f35ef2be94b8332cfefd7b11fd4e884d8"sv,
                "fa0f033f3293389e5a9312c6046757eee20fda34e4c8b4d8a1ddd2a8f8c0b0bbf7596362fb9edd5c19646d49bd74856191e2"sv,
                "b075d10aaf54eec1a14ed42d77a9d961452b480bc6f655995dbaf7acab6befbe7bf6945fc0b28ec1898b3a912b539a7f"sv},
            hmac_test_vector{
                "5ade9519477fbfb754e1427586417618202ec140cfec68ed2517a4414888f988ec7dc3757ef312aea257b78ce05e9f1b9a32606942ce12fdcaab60a55b9cde7549f69e0c47f5fe19d75bc31e055a6c7455b4c090fa21aa8448f146c86cf23c3b5b944d65084b2bffda358eb55dfd842a38ff083d5fadd78b89317f34e554b9e3"sv,
                "19d7b971259fdbdbb4cc0977adb704917d2b3c68b97796df05c26d75349fd777b1e229751db1bdf239def25f0774754b2c8f"sv,
                "b50fa5b15b51f7b9eace48a79bbf3e392150fef2448500e56a1b25cd5f41b3b159872985b67a6170503e0ace217e6ad5"sv},
            hmac_test_vector{
                "9185eaa99394c63ca43f67b663c0f03ea998aa85b1264bfc7c79794473638ae54fce8e4436538fcf2609177b979bf3705c9e3702cb2182e41d858398188fbc2aebbb861e59d356a719c13e1a7da2b7a1531b9d3409f07bc514d209bb9179f3a22df3dc21433ca32c450ee57a5dfac33a54b8e502b2bcbe0827091294294ab6b5"sv,
                "f915e9a8a822b2bc5e67eb34cae85a5178118afff44a130732cc1b862db31d9ecf54ea6b11d0ed3863eab2f34e15aadf58d1"sv,
                "c19115c8ad223d0ca3e2eaf0ee5bb33900b29e9a8bcb3d93b9511f3186b2c23e4dddd1be36cb4d43cbb40936126f19ac"sv},
            hmac_test_vector{
                "268a1505739c34e36179ed0b316a33311cd89a28f361189c032b0d033b7882d1917baeeba16bbf5f97d5d7525036543abcc9544d879e7838f8ece91519a899971856cc88ed20d6763d2526a97e1383585df8961c5c55a9a4c075aeabaafc832784c374c39ee302901910e2f7052949f3c60c677dbcaf3147236d64ac379dc7e7"sv,
                "35c28f3bb2ca3a2d3a5089df8b6f3ae27af3f9072dbd31d98a6a216aef2f3253240788f68e11d706a533784aa804bc9d8fe9"sv,
                "b5385799bd110ac1152146e295331f3ff5c2d676765b933869c9b507e0715f73ef00dc42807e2e9313d973fb114f74c3"sv},
            hmac_test_vector{
                "24a5551bf8c75913467aa639ee6f3a6203c0d8167dd45b942e824b15f879ae553cdf205095382649c78ac011f2be3d860c4a535a8d0d6f166c7a4a5e4bf9bd76ef8ad605f2ac2b19bc7d6e6caf2242e6a2694d6291ad8fb592fbf2ec0b7222d623f09ec376eb108a0d9cdad18d63f188ae67eeb7ef7d627a894b6a49a1ad847d"sv,
                "c1a8b10b8f7cb5ba8e9c4198b04176cfc70161c8eda3809b233282ca38504f2eedcd48315678d03e83350b766050e5a5da11"sv,
                "c4ab31ef4d7b1872a0f926db17ef4e4e816c16815d7163566460afca3dfa3af3c6846eb1f6e9aaa08dc14754977b08bd"sv},
            hmac_test_vector{
                "935848ed0f2851a4b225e6866d10127d533f21bb9e95bd4df3b157f3e766c92fc16b399c8154408122c14ba61dd642662eeb68065a84834531833c34519cc8482b0ac8ae0a806ede5163e2730336e43a8956b289b0e72cd2f20e3677f6605ee05a9bbfccaf9424e4c571719629183f20ede8c7eba7a6011bbcba0d64df9bd11f"sv,
                "8c185fbb4055ddda5111172aaf518688cf85727343c51d718e5a3eae3680b0e71b6a6c860687b50f850492baf7928162e369"sv,
                "a0eadb390b490ad39f883482995c790425dd7d01c0e1f86252527f2fa92b948e320a54900fe2b95c588d3464b6462a9b"sv},
            hmac_test_vector{
                "e735bacfd46777da284e5ba4aa638c08dfe8d002dc70ff5b58a1b989053954d1214f18ae47dcaf768c79d73bc76a5aef30dc40cc2d6b43f368e7835425587bcdd33f3496e01fb8dcd23ccaac94a3d4130d6a2184f7394c79ef0af95dc82cc2aa258e754f473a7c5423209cc0b815e3bb9f1bdbf0e4e304351490816e97be12ae"sv,
                "1b08c4bc3cb4d58339073f6575ea0eca0b1ed58eb28aabfbe4d374f43f2e62722b72bb35bd72dcb3db897e55a17638e2046a"sv,
                "2fc24f969c156e34093dc51a683a489a12afe0ef1837d8729a3412ac41de04940331ba226fb54e37c9425edb8fcdeb74"sv},
            hmac_test_vector{
                "5b7a078f980bb8919743bbce52fd0ba3c22083d2b0254e28c8d3a05def4da33bd64fb502cfb5d00ce03d49ad168dbe5d1c784a190c7dfa0685908558fe1e37725a4b2f4ebc7eca209c1f5f361b9f2d2393b9911c73f87da24a7a256221f3fb590ef4de3b066e8e16f3726432063a403d4f6dc2a48b9fbd443d17e84200d6d737"sv,
                "bfe6bb4c9b171b93d28e9f8f86b88dbe509c66eed41818a1986d75b616fee4460f5456cd23667c8a9f1738289601519d33716a534db235"sv,
                "e82eeb7f4b7415a4c95dc82c46bb59715fda4e0bdaf64a7fb3af3c7058ec7d2a172b8293057b72f9664454e7dee11d95"sv},
            hmac_test_vector{
                "d22f194a1af33cd8cdffe9967f677acb68500d6cbbf77a3f34f58840f0c16044827641dc43d6767ce98f85dd5cbeaa9fc5b28333e7f20df8b281cfa4156055e61555e04a1ceb5c5c93ba9210b2e89f6197f0a53996a2c091d16c3cd908d7059ab2545e5a4c39d6c0f10778f82bee43590993da4571107c51b83c35a6702e56a8"sv,
                "4cf54eb8cf7bd421ddb0586ac4fab9c478cdaedd89cc5a195332211f7571b9988419843300fa1ded868d318f48909078bbf1839c8fed61"sv,
                "830b4a798f85c448b3d54abfee61b376597f65666d83a21052cb3f4466f44747431607bc659c91cb520308fbf4fcdb58"sv},
            hmac_test_vector{
                "ee6922e96d48b6e0b52a1cc6f3ddfc7b1cd11786237a1fa36ff6cc7b8cf02cb221c43b2622e3b376467a25c2bc4e49b21aea93ba96fd069c78fe4d2ecf11085d632e6472ec80ee94810ba98e53925dd39bb833bfd1657f201f6bddf5156a7f769ef1c55433e50e414e27cfb32271e2e58981201f3e7a31384c87a359c6c13a03"sv,
                "de281ac95941750111396ad0e152be30ebf8c47c2dfe8bd5562b56c5d7f54c223c8c7135bd1ad51c10490c6d8207dd1c46863e9f0d8681"sv,
                "a136b0ce4e8239ae848c5dd84a708027b0083efbb61589b2c30764def27e2e1b221ffb4d18af81f27c35dccf0683bbeb"sv},
            hmac_test_vector{
                "4d53f5dfe10ca3880f44fbc2d9bf3d1f97479f4f49ac3f432c085d918b47c71bd8e794ecfbfd1562ea7909927aaa10e87505630eec22c10fce07adbdbaa9d65012b0d74f517b20b49e64344ba145165b953bfd889ea94ff85d80cdb5c2be0b52223524d28a8eabc5528de5b4f59735bf2071d785084ad14401fc27414d8f4f6b"sv,
                "790bd8d4e9ff691a6bb8c0cd64d68d31195961e2c46363b9e63f44f3dea37590bb3297fa4f4533e784ebab80f42f70c3b39976266ff7e5"sv,
                "856c255d058752cfde3e74df1c4a34986e6f7ad5a34a06c157f59d8d6b2039918c4201dd8b2d58617911555802d7c799"sv},
            hmac_test_vector{
                "929172a72b23de3936037ed526238ba70aa8a695c04cb4c56500ec5f0fd52e94bf1c97df91cc528a3625b175a4937c130cd589b50d5e8c0b27fc31e772b7684590fc0089e0fb7807f44d12c846da288b26fac30bf7da6a809b2238682cd43287ed77d72d8076f8b759c3c95872bf391a405e7f1fe786f52daab92a6aa5f43263"sv,
                "15e3ca0e8d9f69f7bc8f63f05b57586c1cb59eb33ac37d3f54769f667b6bc23a8ae3b0aeb50c71fa6582bf404415ba6f93cddd68c42b31"sv,
                "8d6895c2b80f3ff362dfe355252578dfa31596c8f8b028ee9cd1a689807b6207a216e3516f3d782234d65eb1f9597f68"sv},
            hmac_test_vector{
                "004c38356decec9ee5ac710126e804b8f6d947eaad0ce6a4f5137b2cce7aaf92d5fda287c5f78e723e1ea7c1c56e457d0d634422746babadd19b8df66da12d404ffa650fbd65493aff970740663012df8dee7fe3df20af3d595e2e549178dba0f8ac49745c1c661bb17f5a271016b20600e1bed6f514377a16c525ee43e700d7"sv,
                "dfd7e7fa6dc49cac285dac27f0968e1a8bacdaceea5cabffad5247ebe9e402a828005fbab16379e50a0ca140bcf1498b56f021c2b4087f"sv,
                "fd24205b960f569042ec8db3c4e442eaa4eb38272aa36f40327ac9f5e3753d3ee8eeefa28ec5fb1e4e1c8815416515d9"sv},
            hmac_test_vector{
                "a116269f6dc522d0abd6cd740c0879e7902b4a0b9bdfea334b9e7f3f09fdda085e2ef1fe08ca900f4ecd4bcb90e3b7c622e2da3e5b97dffee8c50e82666bfc5c3146ffd77697e5d99026e60e9187d6421e9ca00f815befb9f9d12e565467b332c0653771dfb48af619e88484367e3f232c6183c635b3822a25cbc601fb7a6750"sv,
                "8c354e398566a4b9bbe215f1ad7b40d70e698710753365fe56d301fa311503de0940a6c718f80e19c163ddc3c68b01c4bb03cfeebbce56"sv,
                "36768ec85d992222b67619454918b6ad9369bd002d5a1d08079dbefd35c3ae6d28f0475747afad3c3c64981b20e7b665"sv},
            hmac_test_vector{
                "c050ea6c6b8bc0971c1c64e8b8df91397ab8f0a1f2823c004d48a4d8d6b8705fbdd4e8d217eb710b27c8fa56dc2996819a736a323ea3ca7d5c889fb6ca300c8463a0513705c7ef5cdcb50d8ee3091a8fa7a8b4974fb5c8ebd9bdfbb2a6314904391aec453c0880b1c34b6437d566638b29c194772d9e7e724c4e807371a57107"sv,
                "1fd04f50423cccd5f884f5954c19c8a4c6efbbb0342ea24da92bd28ac79687c83af583cc8550f812012ea86422c14bacfd5e3107073028"sv,
                "7bfaebd1b8b1950274b815c26d4b7d11bfebf2fc1e0bfe4ef0a3e36196ebb64dbea0e2aa0f342c4fa021f82faeca66ef"sv},
            hmac_test_vector{
                "5802a9f1d0bf3b1af5aca2a16a6e3988f937b9add7f9a59e29d3e5aec6d0b0493fd0b0841dc661f86924bbd172941022cae9377c9255eabef7dc6d2efd0870c97cdf37d86712f8c45477852b39b92a33744fd91f0faa842041b3cb6d3874d79515158614390bc825bf8c49d8494650640050e7bed6613c6b0f404792ff7bad55"sv,
                "5a63fe74efdc2a71b87ba0557452386fc3d0bb31dd9e3a3684c8a584371d990ea9d8787d7180bf2f030c9480df4b9f4908831ba7b8592d"sv,
                "ab9aedb3bae00ba8a335bbcd2ca1c610a5e07fb090a5b7058f11ca5c5d884601068aafc2491ffae62db3d5799f6f7997"sv},
            hmac_test_vector{
                "503f4f781c453afef8a77661ca7c4f0d622d1959c27c1fb1bcc5dcf4c3836e1bfb15471c92c3260d53fc458d78e1d460c008a759c2792c7d81fd9f65409981e4985546eac7414ea84bba16f5b0eab0f1a68e70e856fce979f66417f79b56de0f4f84b3ab64ca9a17086b5062199ec9083a49ddcd260c3eec4481ad717cc26de4"sv,
                "52b3069b60f5375f4dc447f866090a3a44c902eba0a6d66d68a9ff9c113ec5fa30806a0a44b7a5f9d61f5283c5ec6715db8a2a2fa329a1"sv,
                "5173199b9551905447d3e220c3301f99006c61d2013a375601fd3b162176adc4dc1ff3c345f81f5b71de718b650aa3a1"sv},
            hmac_test_vector{
                "27852e97225f7966e2da1e7ec5e615853167878f32448fd964ba43ba14f7eed6a2e4a0952942e9d462f8e1dac6ab6b8df390bbe5517e16a2f548d93ac649bd16de4059fd335fe9d1769e4d793b55551e5b0a3b9a9687712d7d309d729094e392a34262b886de01c5e4746b446c0b58a02f7e0f94498728e4c41d974bb900e6b4"sv,
                "05b2305a6264fb92280197a579b4d336395d5b51148adbfec2a3671589641b530490feae24e42ce6744a355da150c02839d87466b31118"sv,
                "e36f45d68614266248a91553c18818c049ec60e233cd5ba37d6cad12d9333962a5d3610f2dddd3c0343cafb82953dd9c"sv},
            hmac_test_vector{
                "f00f1d6331110716041ba25d28fe48b7764238f7c372993a08bbbc1cc1b6a22c9281be9d3014fb3e7fd201acf85fb4c54eb2fe61516bcf3d126662384890c1c2f2f98b913e2bf595aabb0f2f691d499e08bc35ba514a8ee470557a3d541c0c1658a00da823fefc05ccfb534f42e10c28ec8a64698cbadf1769b28c93ad018a10"sv,
                "185f150008c482249b50548efe89c71aeb4e0fbecf6b98c02d6b92263daead6fdabb21809bd6f2e13fd672294e3107b7fd592ea3524bc0"sv,
                "47c51f9c7346bd7b4ffe38b2feca74f43df543916a001c880483ceabf6452f7a3edf9d80bf321e9de27dba18537de349"sv},
            hmac_test_vector{
                "bac382a645b43242a8274704a6a2b55905a2993ee59b295e503aeb7a12ed260ecba5973242db64befcd156b10e1a42d42f5cc89a1155404bf21cf2616ea985339798bbd0472a5b2eb58d6e84475dadf4a76a3b6d19bc90d00cd4b551ee35b7214523564afcabfddafae3f743fa73df029654587fa4d8ff34c6eb9e123d98a320"sv,
                "4a6e36f56637c7b8f445b4f096c544507e2fdd3dd31e823a575f9aa44848f39f844b0650f0cb7d4d192df2511c33fa35c485621bf391b8"sv,
                "f194e972fde5d62f56d5fb99137e0f2f942d0b2a6a22916951ad03bfee22b58287bbce5d24700c6c395452dfddce97ea"sv},
            hmac_test_vector{
                "2c97ffb7f1937176ba699943c435b3bc481fd8e34ddd75c5c07afb5ac656d8ad516fa73cf2dea3a2eea5627393a7c068ccf819bcf457bbd5e8f99a27608946567f6c1b9aa849b76894c9b24fa19b89206dbcda51a4f5d7a316a5f5030dc0e4aedd18c499bcdb9610d45df09d9718f52b53f2856b06beb177730472b675059aff"sv, "83fb4a69527469aee5d72be40f3c9bfcc94225c57a4bf9be76e9b029c4933260249dd76c5067ccd483d4b9b338fda00d34270963c6f35c"sv, "bc852d73029f8c2dd6115c5949598b9ea613be7601a5a224d46ceae0bdc0ca43a8cffb86d9f322cdc09361868e2345c8"sv},
            hmac_test_vector{
                "21b801681f2ba3d51ca2347f4bd2a75c5319f25901626459394a397b33816c9ce472cc1eb92652e78b65b2acd01f306f0e0bb546968e225f6edee4418f67c954506e11a423f4c9e27fdd54ac1d514b0d676387482ef07bd19bcac79d68160b9b4ff3f983c35dafcedf5043059c309f3f46688fe6315218b9a8570c172657db7a"sv,
                "1b0bb110ef8d7139773117d7308dac5d11ce7c756f071df11ec8ac05d9f35ab4e3cb2789ef4eee873ec5a2620799d7f01b6884dac95807"sv,
                "3dd9d0f426054cabd0dae1337bec5682c0778679a9a3f908c66b90e1c28814bc6b9a61d710b0041a2f17d576a69dd4e4"sv},
            hmac_test_vector{
                "5a81e711adfe5077dd8c8b57c95e8e1f3de39f4fc448c523bd3e7c72b1fdacd6e489dc0d2a34a39ffc6460c1cb962b7a94a30c04b5426a75ffcfc69f0c4ba934d3a3da2e7935d56d6b9079a2a97b016d653a35c2cc0ce19124f887a617c951ce4e58493b4209cc294f983cc20b16f63fd52e8451b1ad13bf5342275079818deb"sv,
                "b619d9d07461c11bc9fb66117d61ed90001366bbffdbff583556777584b0d65244af5c7bdbf3b7358d7c791b966cc809760e57398d1896ace72d26cc59a6904fcd92365edafb8af7986c7d90b2af3bfdbcdb1593c78fbe8e3378bbb0c519152bf9cb51c19a02a12a8fd35cb6f8b3ac337a828711d6c8e0c4c13e1e6af090cdae"sv,
                "8b4258be4c094aa4056f332ede8c733772664b088ba22ef8caae7cefd77eceb35e83af8d9c1283cbbffe4a372b699c21"sv},
            hmac_test_vector{
                "8561865ac2ce12832746f82584a4b98e7f4c3ae2410e18196f4e3b475c62ae207d3cadbb1d490096519888db2f3f18e13bfb86f62216015cab8ea491ea734cd3b791a7e45e4f8e0b98d7955bba77e0372d4738161e0d5d84765d9e6a0d05a88e1aa89c5defa864e9e349462e8f14b9993d7a78cb9dbad69aba0551582ddf6958"sv, "e48825a5503a6afe0bf9a240c67f27acd4a8f6993834645e03c80c72dd370cd2e10071a3ae18ef19bae9d697ea9a4118609190cd95361907a7fa1b58f499f3f5e79b935f12212f437dde399e3e6490244aa1f5e38ba9be2433b6ce924f6cc49e9f627321a5df9343fce1b59deb647d9a3ae00b234414ba7b4e020d67173be693"sv, "ec780a915ec7deeba2c8c9e2ab15c9762a3eb18fafa2d48a554ae1fe6c4459da1a54e2d58bdf06fea0740098eebbb699"sv},
            hmac_test_vector{
                "9b1d9b8060067187f19e09e2bfe17c11d0092dc1b7b25b06c43bf2e5786fa7153b65430651a43f230b8b92553af1c7c8e90852f14c8724d0f3087d7243ae02bb270c0f6203424e3dea6989bf1032bcbf82c96f7a140042dc9ba9cafb5d439be5c6daa69b5d28c65df600952828ed847e01162b65f964ddb225e78087ad769b1b"sv,
                "b9280949918c582fa63ac4c68cc7a0e74971435e08035aef12f98397f9e51713161146a736666a18918c95fafc536c325f5239b368ce3b8b81dd412300ca77c29ebc7f542b93c36a80130a81b4444a879318bd9b4b4f37156998b7c604f93c813870dad9d7db0c2d5a154ce21e62e1f0cd0df9a0194ff794b1730e076d380f56"sv,
                "f9bf35de6f3888828b0acf7da8d2a8848c292a5ab9e6e2e3475d665d398e5ab424183e9beb4db0d7accc6315c53bf0de"sv},
            hmac_test_vector{
                "8428a75382510207600c10573c4c0f6056a74a0b7cfc33d7c559b2d9821cbd017258299e357abf24032c932d9e7991bf853259cd6b914d00e71de7b76dcd514c7d702f991a4cbfba7e0625150cc20098868a414280f43de515ff012ad83fe5d50d3a644a1a112a5038636f166e6e8474c077ab72a46c2c0eb5c9a53aabb8a225"sv,
                "1dd0942297cba8157069925114b832018fe31f35450ef6e2e382f22a950abdde7a9c8642553c5410851e9abf370b0b04da05af9ad32e373f72e68eff2534b0b4ec67bfce0dfd545009c3131bce3e826f2e4a1baf90019f64e6deae85df2295f65d77674e29d44fdde6a024ee431a3e4975f888fce54ed75b25b27a46299e13aa"sv,
                "8d032500ea94fd8686b465634d74047db687a1d1e56fc17100f40ea0f7e3288efbd016daffd5d3c32fc6988996292788"sv},
            hmac_test_vector{
                "b5e3eefd6fabee2f84ac6b7b98ca4493a1ef4e49410b49ad54591376c5e31ca4a5481a637ebdfe31e45226434e77d42524bedfa219e2a8e3bb0950db484628e2c462699f48dc262c9d2e7e750a5216e16985c735804b93b58fb3de8c26dcda6d39b8447b76104e66d6c8dcd77365f20f99cc6dcb1f4cf26af8df255105ad1b8f"sv,
                "3dc1c0cf6978292dc6389fe87e96335dcbf493165b528940f867c45f68f0db4092498f2fe33a516bf01304d75dfb03f2fd8566093a8481acb821ae5e352a4d90b4ad08d1dac65c3f98b554410a3398789f07a7f27bd461a11935c80c350b0745e916b41b16c3529005451a8e4ee7fbd5f9a1df38e4e9300964cb9ad22f3c1345"sv,
                "40d43eb4671e6840050be5fc38c2709eccfcc47388c380afd52331dff3868227377550420ff176ff97088cbf9c5aaa05"sv},
            hmac_test_vector{
                "b5ef6f15a59e24f357e578b50c0f6016e9c0b70c54d1ca42a15b19c9ee125c0b4bd5c5001f8385a4c70f91a87fd7a66786318a1d7d4796531026719ed9111e89811b39d029f57dcf08048a3955b6e50d671741c61b9ffdd3764eed468bfc5dd09fea53ed8c9c155c63ebebd2555626efd4996171f58107a819fe162acc7a22c7"sv,
                "abcdb92d96bd11e85ebc4fbd6f17dc8417701e188cb59bf53153620615a6a7b8bcb4ad53231ce98afe49645144eb61eeeda84053fb0ea2abb7cdce333d23534b3a97efcdf5eab09d8c4b598180ca0733740f14b5324c3ce5ef70fe51d09e454caddd48421411af48912af7f72c9b177b340d64e73b55b2cb62a4bc09d7fec6e4"sv,
                "3802d4c92f767fda5187cfee5eb73ae20b6bce00e8cfdde4879a0f8906ccba8a0ae3d4fb4a7f1188c3ae9b1910bd07cb"sv},
            hmac_test_vector{
                "d292a7d9a314845d0468665d59ae01891dccaea88b59bd989eba0d903c8adba9ae40d89d540b7ece9701a38a1e794f8d45b5cbc7cbc5fa60caa7ecfaedb3b136dd376b79fc5ea7f05656eed1d5b196e4b40cde4335ef3b5e97f5ec41422dd41a46bf89f503d3a9cb29ae6dbbe41e8097501911a233a12798ebea34efa5bfee5d"sv,
                "56cac4042da649666fe7b1efd4f70f72b55435891687d728e35aee787a465be26a2bcd697a44e43eee59978e0536d39fc65d641bfcf819ac801123c8b605bfcf5a3615537d015fb6caf04dfb0e30a8d6f84537051dfcdfe33ea10d3bce73595aaf5563683dc601f88d139dddf6cbd83fd17473ef7e7c70f8a8737477c97770ac"sv,
                "375d68bebc28b9bd0d7108bcb9cb779df000d15090ce6b188ebd3b80cab50c431929ec374c693ca4f08654ab378f3648"sv},
            hmac_test_vector{
                "30accaec827d47ee5aa21603cf62b6c3ee29331cbb7d47e97334c5c614e437231f9cc6989dd15b78609b04a98b2299b355529f1609d76ce151458c80b59664d65fda158c212aa4a8b9de5761530062314f00c22d45a86a9246539da0655a9bab76d07f6f166f68a4ecd1d7d22ee458676698650776da8723d9c6becd2d1823ca"sv,
                "a797132ce5971a3a9152f0e3521f9de381406a2be0c53ae189ac1da7244d23205df66eb096d28b84d80f03d3103f2c6378ff454e53d7c206dd62e59782af8e2c4ee654d5a52fd48734cf496527945e3d9c7440565871f669b9c0f5edec29ef9760b1ed227b779d4c8bdced21ffe6d87bd7e780ca59597e11060d7631cc85ee6a"sv,
                "3ddf47cb13e49e5cf054bbcc073636f30b05dc9ad979a017cc8401d3f1f7f83e3034c41db41942a671b3d3dd706b3619"sv},
            hmac_test_vector{
                "989d5d6083c85c2b09be202c60f1277b8c5e471fca623b812fd05b218d42ab8896ac177e4437fd7c784cec64e1eeac701f4e7e682899a419eb152402346cca50d0486c0df11f7194d4519448a070e68592de12d7579ee56ab9640ec27eee22ac8d97e375532ac15965f4a13e671ddea32c388dd31e18065ee1e5a0c93370bb85"sv, "657fcef962db04bd269ae5fef2cbd5e6558d072946d235e8706394d4cd250796769a926fbaaa121b6da42cfc82808474dd672f9362756af252bd8cded78d39b9ddf4d99e24824844934fcf25d03e54df0d83cdda2563fb2be73b54b8b1c4419d429589cfc9ea0dff41a3b7c20190adee8febca47b6264e5bd8e8d4aa8552850a"sv, "4f1283e5446483a4f8237650572c8a7694d5f8e34086e81cea1f5bcfea345e33dff699a36aabaaf95fb2e0ec6e0e3b03"sv},
            hmac_test_vector{
                "7eaca1335bbbc419f930bb7562b9090f50023d84f7346cec26bcdeb98e4d08e26128ba42209b9a3baec7d19ea8a704cff94e8624a870cc8ff524217da5e89afd9ae64d25a676beb1a1e39aeeb972a8d70aa0fc7d6fac6eac97554acb5be549044e6ec0a5965418c6fb0b7e2d3e22db6eac810756f2ef3a8a33d7c36c584bfc3e"sv,
                "900f404d396f4bee526db4c9be701896a60d85bf1a203b15fb1cafc1189fd67d974a9f0497cfc2c6134bb8e109d70a4ddc59cf56f8a17050b4b1af867c5be8732f129635c6fa183e3741b64d98b2df9c8887b45bcb7fcb5e34312c13c8a0a0b6200f2cc80f17d8bd85f6b4f5b32a1ded703f3db7dce5d1a743452f50449587ae"sv,
                "52c63558050e88a4a934d389c9d973f9901f2db74428d642a750b67d890910d7028c26dfe1e76010a9b12c95dbdbbecf"sv},
            hmac_test_vector{
                "dce5adde1d996c50f9b061ff8777f9b0450343475d675596272bec73eef9520df7988580bf1f95a685624e3008b117f692c21a8c35eba5165594d05d67ed6122f7dcecb2f03a3fba183efd5dda3988844884247560b75aa88e1c9bd573642652e5982c9c37523134ba2cb5ac837914016f33ab2b65353cf3dbaa93059cc66a46"sv,
                "3a9f2fb04c0c42b2dce178313786aa7713256c96c90ce2f2be53d5ff22bf054cdee7c1c1c795b625186f05e21fd7980d360fb5a1f5a1d30ba6feadecf965961afa060ce83f688fa76608b46cbf4375144575cb86d3637f3894d63cf5bc89eadd754537ad1c4b7640aef2ad567d91e995289fd5cdc9198b807024aaa2d1f4a99d"sv,
                "27808650572d339546aba818c11e07274bcbb7ef5b46f5a2917d7a7014e3ba58b5c162d2dc4ba6986eedc62a061b981e"sv},
            hmac_test_vector{
                "aea7ac414ac7b81df547ed95800cb443ae141cc731a2710cd59c10cdfc3179574abea690a1f39c3850323f6f87376a770ffe3fbc37e1bc1112028f3ddfe82cb6c96d1cacd1d7984908455638014da36486e3b2028a7c34d15f7428a029adf512937ab69ec0bbd5d98549dbf8d59486907c8a1da110bb00d325bb83ffc1c8fcf1"sv, "407cca4b390ceabcb37c48dde081d1b53ef41075ca30fffd71bc1da2c72ac02ffb3ead290b53e65834fbc382ade6447a9405a65a29ee340c3a83600fdece12776630403691bbbf9bffdc5918e83d1f480ee90030f0b29bc641d052af98caa04d699aacf9340c1e8dfc673126fb4f092894ea48828942dc336eea1a0d5ee511b8"sv, "a205326b6c4959d91bcf22e97ed4b447977e2a3af7e1f7db41e493521ab6a218a868365ecffdda6701ea4dee48e61bf6"sv},
            hmac_test_vector{
                "09e7dd407526d478b4f6ff64e4cb92bde8cca38bbcd9254605cd193ced0003322d119b4e6be2c635c7e3c470b194ab14fd4514a23920142c3e6a6eb9050606ebbc3b99df19b7ac55efd3c211ff18d2811002f89f42056b199fceafaff3cb0cdddded3ebb27ad6e21534d610e7660f04b4a8cb84b7e8f1256508046e75e57fb52"sv,
                "c8d18c4707df3ce945f3d7d58d39e73ade087abddac37978c2fd3643a2e04432c9cf8b5f7c4a9be9ec12413c658e5893c76ee2bc3a8e581b1e1d3ef241fef9e0556ee5a7a1e8a332f645b0d6e5283cd82f99392f7df0b28c51f55e983d45757c53f8c2a7dc80facb4dd44c2976cf390ffdd2a69f0bb12fd145cdbfbcfe7d5d11"sv,
                "cae7cf6fcabf725e2c23915795a8ec62262fb7c7cfeaa93def9079e7dcdd7bfc82eec904312a588c03ee87c43df6f702"sv},
            hmac_test_vector{
                "8dccd462fec7423d3ae5abf8649553170019058b4a8a05d6cc1c456f2fa3db685256b34e99d381518864faa4908144272896ab4420069516326fb8d1db308a5e6928f94554c15dd0d12dab6ea5f0eb55379fec6beae73667a9d04033c443e395facb0f0fd798bd0440c9241b4ceb30e955bc30ca4f35cf0f59c8455f92e7ce57"sv, "8dbb6cdc769851fbe90dce3183cd19e9cdd7893076a6fb7ee0a874d3dff20806bd32fa5a81e5245370ca99c8d5968a38fee966628781450fdbae8733f705b8347159b78ab9e71888a0d1c67916ceedef381f17588ac595d280f1204144bcc09eb318ed92a806f4437e0a9d6ed0dc0bb68361e33b07d9b16fd95a87254d7de7eb"sv, "5704dc3750e47330ad7dab4910be0cb5f7f74dbe78f5c9a19ad718e71aea72171fa2a5dbdf7cb5579beb97930177474a"sv},
            hmac_test_vector{
                "9a121482c7775a8b5fdaf1c2fb7de1a86ef931b1a88cf23ddbb47fc9dcfd0267cb173a6bf62b7c68fb6ff85b2df93e2539d1013f0a491aa9e991cf23e98656a082cb95f87c1b2cdd0eddb51048f94ad4aeeb48a426165321145a9b4ec3e85dff0755ac8f20ee71d2e24cb14a13280e9e15709147c499a68da23868b232cc1f6d"sv,
                "74f41a6b1c4e5713499557d6f7e889f8a8ce2e444e8261fe6a8e5518769bdfa88188349a19b9f3a26db26675b3e40539c8c63b3a16286ddebbc539dbe817fba7866f9631204471cefdcbbf768cc9043006a6d4cb4ec2decf1c0c2ab35ad09f50ced0c896fa97d87e400aeb3f4a408ec5a993825fbcf7bdb8d48bb208956ed28ba0d4"sv,
                "b0da90c043493511d94f22fac35b5962749c49972fb43571b8478764dffc1c25e3a7523fd405338a048d38dd1b75511d"sv},
            hmac_test_vector{
                "9e1a5d9f236ef93f2cda60489166c82dce322327046644cc406b42e3005c2177f3b7af2a0159adcc8ba92f2cf413462e60b8db1ebb63de44febfa1b9adc87e79a480c0b814e3c17ac91c4f5eaef954ba929db6ed2c757df15d6d3430b66391993adb58f265f57c706d9d8785c7023df9ed497c3c5f8267fbe7dbc4f12213a100"sv,
                "d87fb6ba27215e5cb65c3b5b34ac2a32037f30e1f7ea603d5a9bff8a330fe74bc70529596132f6334f36c0952dcf9c4c664ceb48f74539f3768a65c1535902085fd4fe138ab18172f1341893185a139773582c5e2c4369e4201143d12bc0074ba5d57d0f2c08c8c0a43e8d7e7db757bb34893a4a1d4db7b95f18e0e140adbcbba3f0"sv,
                "3c7cee960221c9d9f7464aeb70d198bd60414dc3ffbfa7a2227a3a375ebb8f6448e524706e1e3ae95541bdcef2b31d9f"sv},
            hmac_test_vector{
                "c6cbf0916dce3ee4fdc3ade93875b2d3d6cb5ff627e52d7ff967f863bc154b95e4a1de7c8fc05da468836bb4cbe5e7a02aa16e1faf462160228ca8f80ee977201f604f1dcf9a08ff41378f8e6d662b827eb304f27821e50f1e79411213e174733fb04c5c7481c85d52871f61682004e19bac2957cc9f02f6b5d5cb981426ccf8"sv, "4710d3a0a835d5913a96ad54499f2a5329a95150251ff1a6b8e07ed200e51b336f24f90ec4b4e0d539310ba9fe62391de719013d625b66cfabc1abd8431e69629e62de7d1bbf88843a0af2a10a63cf93e01845af4ec78b2553c3b685d0b9d0823b942bf5979df425a4e70b4553ea123e7c6ec5afd3ab893219ae47e28ffe7a1ab080"sv, "b3b489532bd5b701e3fb7da0601ce9c94536dc3d8acafafe835c503b0be50a0e852b551456a328e65c76892f448912f6"sv},
            hmac_test_vector{
                "12f2890cdbd85863ce570fe3a418431bbbcd74deb16f3fa232cad6260e3cae588fba777509135172b2793d9bb43dd17a81da31fa67353acc6423000a3b2c7b1ac78f7dd69085e7e99cca1d7c885ab713d7787ab189f58643efafd03bbf58d37c6479837697b68392335397d045f75cae63b0374d40388bcf7d772f03c1481f44"sv,
                "2cb6e84fd408571c65af8e26743fed236cc3593383ecd41eec6f51dd4ea8c65d7683827f499cc163fca57ee68709886956ced8d542c022dbc1ccaa8159aa59da5bbf1014cd413cb9a89cab2e44149e8010ac1f5f8647946b5e0e95af0211fb6b433139174a3df0a4d15bbd0593aa56ee0025d5dc36cb53552dfdb9713127d39996a2"sv,
                "4c32a80183ca3f1b5d07d91924fec55cd3c8e0a3719beaaaf3b7824aad1c1ef49608d5ba7a4aa5b53c244e896fec2fc8"sv},
            hmac_test_vector{
                "35ae3481efd30d5d668dc0e147e6ff837d30a5465199f8ead6ff2b5f8405288023668cfeeb89f29ae9567498a4496f7001c487c664aaeb9c1829dac7f1d7679f2f0179fba63f155d1c4ce069f315df9c0810823adacfe188efd5453c73a8ea3223280360691eff0cc8180b3c7b75d86f59bc9e8afc32611522ab73a753a03a9f"sv, "1a8e273181755e05df15946114db513993803b32c4dd9610a700076dbb7f9db11e0c113ef54a4deb0ee02cb4c4cb81b023f85a434d1286941c99544109349e524d48066c46c980471b501162a36ed6f6834147289744bd82946b32a4eb704837f0678d233b99fe024e8fcad4796d58f4fb828a4b6a1c44c355a128fe27dba4494e94"sv, "432f310947c3ef1f146401824f451025f4c24be8b4c92587a5c4263457924f426ceb8aecb533e4788d20fac0b25856b9"sv},
            hmac_test_vector{
                "37b4cf3789e40a62aebde9cd9cab34eb846dc10d057644e39f94693f6acf201d089438e1d81330df6ce54a203ef2e0639e92fb63eff2225813ba70c024ecea5f650f0f85788eb08fd15b01b06ad7f1f2b8f6b777df9ead05225162c29d0cea3b366f79abd11d317bc366370589240c9dbbe21bd23b448ef1b2a366cb3df7cbb8"sv,
                "c4b6e18a87558d2ce6a5946c65a9446f66cda139a76506c60d560f56a013b508d6ccbbaa14e24ad0729dd823bf214efcc59e6932cdc860306687c84a63efb551237223641554940a7a60fa7e6ddad64a21b4a2176b046dc480b6c5b5ff7ed96e3211df609195b4028756c22479ba278105771493870372abe24dcc407daa69878b12"sv,
                "b3abb0141f7a5b4c39a1e5e6ef8bb64456200d42d15402be97bc516497adbe241cea596fc9493e91c84b9cb601eaa38c"sv},
            hmac_test_vector{"fed888f3b12b8f17d450b4303279ad29d90f8ed71ba7ed89d83244e1007396e253b756496bc08421fd0219925c12a5fa3e5b373fc3137d63b36d2c580710a216acc9cbec837c4113ac61b789046c971cec0deac54d1a7938d90a31b99363cc319175ae8100490d166ad555be3471cbba6b8f7014c0a62833d06f1baca9545c5b"sv, "9147253adc883da2d831a57b55e742aceaf10ae4db91088b4095a6f864292be46cef831e39523f818f17dc24763448d0e0e6a2b78dfe41622c7bf9ecc35e996e50bab8bab10bb24cc1c8ed7dc43c61fa05915775bd2e709a3743ea4532a11ff287d04e750b5b9eebb76f60f006a495129f1bb08634c99d1aaa12016b7a9ac4585758"sv, "380f48d74b2a6b7ab206cfee0a4c0fa5dde0fae81fd0240b3da352070611cfd45dd389f9e6b5570c8c202d4178cfef9c"sv},
            hmac_test_vector{
                "62c1d149567f05a0b76c4fd32d1f365d170cb165cfb38f922f1716225472eb36a127327007f8f5c08479ca7beac4b0aee26f3bb130bbf1ff390ef344c2a4e0b8fa81f6acbbaa7a620d945a22ecdd128a4b3acc2658b1cb41020809fab87d1f9a74b76624f9fd5c2e59a649f0b9d0229b5855adeccefbe60092eba26abf657283"sv,
                "da03db48559d2e361cdcf29eff209d75339c291ba45709848c76619745be76d35c556438e6c80f6517525bfe105ca050f940583850521b408021ce0cb6fa17f5f0ae13f357954cc0ced3e53acbda180a6531e383af73fdcc459a0f42247d2118bf9852404f0ccd8f6ae6f81cd7a4efe9a1f630c56ac1987002698e0138507e85f09a"sv,
                "35931980eb488506fbf05def3f501c90cd99e8d18a48999b4f9b1e95060d3a509050ac8a7a8461a9fb011f2fe6815a4e"sv},
            hmac_test_vector{
                "beace90b45258d290fa0d56c359ec61505083be3562ad93418ff466e2faf969d8256065e4e07edfbc111efd7c480740d18e75854a45de5260dc6fe2188549cadec4d7fdf0c0e1d9d4350df0350aa8c06645705577a3a348ae71f3f78324401e22518ba5724420557bee50bb88222f74914dfaab680c7afabcd92713cd687ab85"sv,
                "290660f114130cb1c66fe88b2d9a87969502a765d86f0989ce9e80580b2c47edcab79770243f799886d2da6d6168dd53230c7544673c325d68935b9cbca53ee83f2bc4514d60489a34aaa9f4e87d9e0df97c4a49c3e2114fe521a9c8f4c9746370aa0d1df63d21bfb84080a82ba173ccff9d51fd54294d6a2b9192651a5f9269457e"sv,
                "3ba6d5beb3b97d29c93887bffac37cdcfa407cb7ee9730dc0551144048dc83e4a2d24224f02fecdded21bc805cac3418"sv},
            hmac_test_vector{
                "893a7f54d8739af07d5ba0e1e4b911353130ccdee25bd663af1c57d6b530e506d40705f556d3ef8dd1e0928e4e23d8cbaa152b963dc23879ba1cf3275696dbb12dee7ffde14a417584875afcb8ce1e26146881a8cad3db0d8715e8ab9b5013d65a97f453c200170f5efaff1169e16aae37d0d7fcd1b74689e0ad3f4ccc8715db"sv, "033f79314407bcd35919d1ef3725fdc000540ef085c5faae0a3dc301491a9d5cb98953720ff9e74b05d079985b5c69441c0cc04b23e0cccfdd1e0b6951474bbd5d490e5142f6339f63b5d36849776a4cebed37982cf9e55fa6626acca6cb30b677e67275e4e5eaf4f831a908085f4600f4603a7ea9f78b85ea70a8d25190947090c8"sv, "7f0c42b9e080b844295fe9de114539436aa92d184b6a3cfcec1de8a84ccee0f38f5d16e34f955ad20d6a20b45ecf99a5"sv},
            hmac_test_vector{
                "ad3d83020142b3f1f07a3d02f602146749c9c3925efd0b4eafc3ce08bc2b2c6d4f63eb3b052b1c500a88fb0824bd18c5f377b4cf77486e5fd2561ff64f5502f3a4b218657411ca1e18fad1099fcf53bd13024e51fe843a722d81eab4b44ab4263393d12957de0682a33784ad70220e7432d259c1fe9bb8b149aaf9e9f3f4892d"sv,
                "2a452560cb1f964b927daac63b72342755a94806ad044dfa0a8252a41a66a6efad96a2952141cae61dc415c583cf88f39fc30990a55f73de5a9ce64911d561a4d92b92b6f528ecf58ca3fb91a2e3f4b4470791090ea41b7bc1b1c83f53a0481416ce9bc92e1d250046df043382f7a1f4b43a8579d21c6f57a888a38eca0f84c6cd45"sv,
                "21a02fc0c319e6d7fda5a5a0eadd1a7505a7901dc5a24b91792d2f5952982b20cea4b55fac302a5d4c99700992b8a7d7"sv},
            hmac_test_vector{
                "21a9873b258290a981be2548cbb026097df680b29a96f68c954d225c6e23b6951e4308cc350ace6cc25e25ab7341d9bdfda5e047be822716a127921143ce3e6c5472091f3fe554970d1d07ab86ced6d19a3fc3920056e43b1df467d62789d3588adf901c8b3aaa902ba3d22f4e81a6af1c69327ba837b663cf5ea992647cffc1"sv, "fd545d84fbc11f642910e828bfe7d548e422641d227586a11753f6776d0fc82b0c4245ba8d6cf655f2deff6295d7afc45fed545c3aae54cfc988798a56e68d040c9c27436685c4a2e76a19d10c26a81d7f0892f28d79ac9edadfd45753c18680652baa286c54b0d46f7edf7e0a1eeaddab3dc5e7021eb695b221afe64de7db267fb7"sv, "78bd2b27a34ebec53c0c15732a23dfea9030a6e63eaf4a9f1263c7a933b6beaa4d3b647e801e61d0c44bc9afcc021f2c"sv},
            hmac_test_vector{
                "267d99cdc5989ff8d200f39665437b4d9ad862e42b6056a8442bf40e92fe80999e13dafe5962ad87f71feab501847529a6d01ec45390d20e831d12d4d766a51669ef8a205e1a31f6baa6ebf578759eb8db92bd39ff727738b37a2fe18bc22da930199379fb49920ec29cdda0f29e5ec1de252989578faef07d4b327bd49b801b"sv,
                "225374e44a65dd0fd9cfa2f7bd90572f4b7a17ba92357528e988b502cbd43d68e5e146f96cddd7f3b4a2f49bff3813e0939bcf3b0fbee0d9fecf4bfcadbdd69f3af8bf59ee78ad83cc00e79d6aa4c4ee0089636bbfec9bda646535370c6d59574a1f47cb773b48074970e3c4a7db53a2bccb39124bc78f7342d2dd7b91edafd93cde"sv,
                "984f82f1b69bf49cdba99b702b205fd50a7f618b6732a48639fcc37d0f5b868b30b53411b5167fe7f09b34d5d73d2d1d"sv},
            hmac_test_vector{
                "6f55443fe65938a33f0749655bc60f4c3ca62b0622f38763f30ae0c1e06b82c846105357d2fd13bde919e22d473c036ce689dfcd21fe4d5f1949106558dda106b446304676b9e535bcf06b661c35afa38abce5df7c35cd6c4ca2346fdd66bd90819fe1f61ab635728035ebfa23d89c5af2e645267bc18ff353bc53c6b82e5dcd"sv,
                "552e518ddc1a518c2c853897083b7ae8136273f6354ff1ed409ef35cd481b02e8058dbdc298226e1b073056dcf07b45d44167b6b324931a2c42cff16138466d14ae28310087892b0052cb2e3f9ebe727b9406579a3250cc53d192fa21972993cfcf5d2913ea49ee174d7c75f4d8e1c2dba06ba87528a8a3ff48c0b69aef45257ddb8"sv,
                "a9cfee8005eb9dfba8a2f919a0a19925b1f8d349a9cd9fd8ff0feddcef9fe687c5213aa77c1a05a72ce51909c240b746"sv},
            hmac_test_vector{
                "eefc0676e9fb74262e727ffc07135b6ceb05e6ed48938a4d802665473170a757baeb3ccdfb82ac215c8d3f86100364c19beb533c9114f256a106f5fed3384a800e84b49eddd7b6e3722db234ecbd52a8d570279aef9890fc30dfdeed0b7db58ef1afb51218645a157b39212f22a7edc8a37ba82ee9872de43498663e61a8b12a"sv,
                "3609e874c69e9f3516387502f9eb82acd029be530936ae7be6dd43628062ff920bb759b25bf263a1a7ffaac68e925da15760b2a67d8f864c6b6b4461845a07a06612038570092160822de9a903d6a59329f9c3e3749e7c037c94b352ed6320bfd082ad960cca20992c2bfb53629ea0a9cc5ab9b2f74bd7c9a2c059e0c4649017d06f"sv,
                "33a941a00df494796c3ca14caaa71a7e95479a60b9d0274cd1f58a0e809168bcf1e6e5c47753237d4b777fc4cca86920"sv},
            hmac_test_vector{
                "77d3f3e647e67766e5f4cf1bce5f631bd575ddbd02f29643a0c64dbd92191f2ae68db3dfadc3b62d0920873e87d13340af0ca3c5da99146a4492c8b76267fb477624192960f72e85b7ed9e8318fc1668be46c203539cc1470641d639def1600d4e228c8b098ac9b817e17cb329e8f5dd2aaaa23c160283220f5dde09aec134c2"sv,
                "f05d561f5ad70403264c5e0a0edc12fd473b19c0b40f8cd85a99ba2a14987705876ab76359755b6c9ec54a3c93f6c4e68f55f3b93642c3c2f0d9f4919ad16e407ba3d4b279ef5b198c1cddbb744029f5a73f9e808e36f8f3f01a6989af9cec25b250d693220fad11d99a3e0e177fea317741419d22b3d27443a54099bbc299bb15b9e48fbc9bf95c6b8496bde67eaea3e8"sv,
                "724e5d2d51d98c15ce2e78f861d7b6f8952882e9d93d40850b78a23e632c4e14a222ab3726b1a0aa7c6b2cd66082ed95"sv},
            hmac_test_vector{
                "18dbab9f86b9d70bbdeb018f6a76ea7af23eb2ff111e9be3c13811795d8ae7d006c3e42b46547eb1f3c9e566565a435a8dbd42212e3fd0822d131f7300eaef4600c40f1d130521a388cb9ffe427f1bff19aacb9c7d0a44a15ce686a2469e3934d086365d36f449484498353d760cf9d15eac525a46a881a617584eed79cf4d03"sv,
                "95ece1c8ae5e94d16ec9983b1089a37395ad5b1d660916c13c87e4c13dbecf8f68c6611c324a679471def5487a93aaec86c935025b4518962884ac2cb04e66f7aa8e584b6860fb55b86c2b0a0873735dcd278bb525401f9ebaccd2beeac6830c26ebcf3c98c9d77d09194367014e872f306e641e0c21b241bc085e61354faf35a386cdd70aac83752d8d4449af4f6ccb78"sv,
                "2be1bd6a766e30792154cda00af97cc512e81413e0fb761698f39a26cecc3face6f9a98b7c49605126dfa5aa8de1ad72"sv},
            hmac_test_vector{
                "1e6a0d7f4bf483287255148e2e9346734c14751421a6c55121c35b0eae12cdd92e30d6cd60b14fb6f0625a7649d1e1a7aa103f817cb53cfee3253f3d9313bf24e543fb4a2994eb143e3d761e6157e5dc439c98815e6ef71f881bac10c95f10966a2ae2be4ea43f8a866ece4e1f2b3c5b3e40e8db70d7dffc3b2a0a51f9fea675"sv,
                "70200eb90526e89d3d139d4d057398534fbd7d91087dc0834ab270c4d27553d4b4dc38184d1e89227af82033074f5bfe6b1673afb272092cba1c029ba20895248044a2f8522246923899ac3de0e926d6417e7cd38c1784845f27f2993c7b72e53b1ef461fb7a4774debfea7176891f623abd40047eb35612a29be8c68919119e4c33d55c306db550d63281d9b2766e2abe"sv,
                "40497133e82d9b6a335002f71585340da2fdc6bceb03fd911c82abe8715191624671bf476b89de4d9a7bdade775ac744"sv},
            hmac_test_vector{
                "64c479cbb5c9c6167db18c881f23699354c8f4c67b4facf430b52f931fc3ad621ad89b4f6ae8ac9763c5fd2f3c9d8f631dc4fbbd78b461289d53a2558974cf5eff9fe1a9db15aa10a20f0b3e5d47685ca8b959ee06aba2f777eb66644d5897a73ba3b1a3af57a8c861171e7f2c27dac81e2f7621cb3b57abe605e3881277ed87"sv, "332b091239699daabb4df4d8d45527a288437b8c586791011420ddf5f1a159c9dcf17b6f0d3564cede325a68de0e783a2156a4e3db46c2092d4427f93fad518d8fd4d553894898eb575642b1facff09d9a0c2a671a2e14eef0aae8da3406a380f6ddff07d529a2c9cdc295fc5121f7929a6af35d3eaaef77045c06e1f733b96ab913e1d31a60e830e97c39910b8bf5e2c9"sv, "ad69ddfcbf2e6383eabb9f561d947eb5a6cdfcfc220f506e591881291ae85524394e4b4568eeabf1b0103d07fdfbb8da"sv},
            hmac_test_vector{
                "38bd6300e7ace5d0be9713b5fd4ece221bfde5b80c5b7e14c023b8ececc88e5a0c65ff77a9f26b2cd7be41450319d7c7eb519984eee166ec86e4372613d00260da60de634dd2676a8d5364342db8a758396e35d976b4a7faec07d053a2dea025a99848eb5bb39f31e53ec5a90391067c0b3b01de2d3c09e5f975cb9b215c3152"sv,
                "f87e5d9fa8d2745f9aa442686bb0e372c190db05d42fb58f2e80ebb4af7006e5d3190634383804665ae1164aa171734a9fc5d84092a92a8b31935c61a5929366d581887ee6802ee002a5966d59ee0f3f667a918474f95c8d6062dc22a0e4d019841eab27bc923af51b5788769b0fff79a019cf7a810894be4453339fa94d04246b5837d4c25670cc2c61b5106dbc8a1fd4"sv,
                "f8c0539edea90bd9cec6a408a9d47dc2e341a68c5d91fb8cdd48c13f7667836dc4b68c8ec2519121a3fe02e571dc5c74"sv},
            hmac_test_vector{
                "c523fd03e54f1046e7282756f87501c6ed0835a233e6c6af09a8f7f5ec21607b534142529d3918d448e06e2e6356f61c46e7d9f480324be6118a6282da9c287a5152efaa79f8e455129ee7b20cbd9aa7421db8d465f5a9d3365553bef9c132f2261eab1361d39d51ac283ae9dcf115ee49717b49ca7907ffe1d071b232d7a76b"sv, "7da3134a85e28a852e16a552aa34d3767d444a583c15f05b942f7c6a8d09d8d5107cca5ba878d48885f65941c62d009df8095ef6d9d9cf406248a49a058e842334e70c0f2244b9facd41087b3c58dbfd05541fca1308ac69a6406c2a9bf8c0ffeecc1f8d7db001830fe1fb2b941c2b3faf0dd6fd710a507d68011c43d8af551148134cfe402373cc52bd7a7757e0b70afb"sv, "b1fc08f7a1d0bcaf9cd23024cb0f9ebcb826d9facf84c50d644031ce69d99f5d07eb72e02e25b0a3fd395759552d0dec"sv},
            hmac_test_vector{
                "40ecdfd2810f09de02dee7a3d82ad794fe124af3f81d818cb78695a2bdd7ba2a81b9c437150bebb60631cb5b84afafdbc2f60e5ca07d56298814e9ab95766595fb1d295ea35a15f415a490af9b31d083a3c3567b66c5b762c796a2c37e251f1575be152f72230f02f7df3f8667d7c4f569222838911a2d9e3eb3c9d4be0c6f12"sv,
                "f85000b579f5db06206f5cf43e9f700e352bb6bfd37e7c76de10e903f0e77b45855eb50253251116da893cd03bf582994db987d6ee0b3910974b0252348c42d3324ffcd5d991d0cddc0929c42eabb7fd187020d88959f2f6adb2dd9ec0941f6025ad3ff8b243fe754f778b9abfc7f684bdd7e78d4b71907147cae0af3f07f93286ffe531874384545a5cc9189553267465"sv,
                "c2fdc5b35ee992e0cb4c761a705d8cdeeca26edc2e392419f81173b478cebfb3bdc8787273639da8882020150ef120a8"sv},
            hmac_test_vector{"cfb0db236306e51c6b540bc1797409ecce36c4fc1e0d2f2793086f1214043e9ec6218d05d3726d276bf92f1e09d2ca18f8ae7e661583b7d0d682342c8b4a682fe45b2182b4cd6718f442a7c00b1911f64de43effc11c16352714f19f392cfcc43247dfc18d7f9c9a0f17bace07b0a42ed5c04ceea94113fd8da6ed92a613c008"sv, "59f3efcd2fc8ab28aed9165424c8962fc883b232fa767649cc0019e1c224dd42d682371fbf85306b73dd0b6feb30281a260c289870215226e4ad352430378f6b98806ee23eb7954ca0ec216462c76d2c4e7a328299f4f3f4675798d592ad5990a7d7dd3d32c5a3723ceda25c11f52c0e47b31818fb58205c5ee8881d71ee01590c6dcc59251922864fc7d47e81232f6719"sv, "761c1b719e554e261f783334770d59879926a641bb5f8db8583b43c52554639d4f00e19e9e08bfef181a406e68a23a2f"sv},
            hmac_test_vector{
                "d3d906f23fd6893e241066457047acf137d3a374b396f8db89301be64d71e61749147f097aaa7f03dcd454cde8b5e4c25aa77227372dc6bb0f2a888f7f50ee18fbda9783ca7eb648529d720296be8d7efd3a44125538773017dc0a3aedbc8b9871941509383a74360fd4898d8448f04ca9f4e1dc14a054612ab0698f9e9e06bf"sv,
                "e6fae88e7e75e970646091815949c55f9554a5f85d82c1efd83389411fea06289351c83f737d20e7ae728feaf5ea7d5f15522ece8416f394391e9d88c19937de90f9f54c953e676c1178f262a88781beb92678a4eb28ea2a08000b010f87e85884fa55fd0b4227c53579a42f9aa9384428d76ac1ba400d78ece2bbf42e3e7f5c185dd299fe11c9bbc37d8c99f2d70ab80a"sv,
                "002b1474834e0bd0ef8baaf4b38ee412ab1da2cad89a54ecba9cc8e5a74b44a9f663e23f6dbf419e1009766c3d7275cc"sv},
            hmac_test_vector{
                "19583b5e7cc08b189ac798ca2f45a83c345d23c0070de066c603a26efbb2e3037fa6d026981494e865b6d9dc7cd58cccd1a8e1abdadc98cbf0747e50f9d336e5441739adb4a91c7c21121248fb11b1da30eaf9c9500092a9f0a655804c330e79305480dd18a7aa3e968549f6f41f9ed01acc7104b3a156698c432fe63cf9f229"sv,
                "321206ddf2d6f0c29b10e34444cad455a4324738824066446c9950e5acb4b3e3783b9de769b48f3b2f60d4d1826310875b261ca965fa11b7aefea369a342b1f13b2c1980721bd10bf94b344f2cf9991344c72aadce61f4c75c613a637a700f1de3ae24b0f555debc6399abc72ad29781c6dc1a4ca5be34fbdc8bbcda1986f6efd8896380daa8ddd6d311e3406436315a48"sv,
                "88aa8e3dd920ad7684acc468021c4a9fd782766c6d80c339e81efba5121b22c80f872895a544c42a849b9c9c2838344c"sv},
            hmac_test_vector{
                "6b1bb1f05d08e59fae3d06392a2d4d61f63432016a20dfeb1ef0e4bc48dc2c06459530ae75cd6a924cfa0d6fa657f3035b2cd76e1450c4de5deccd6bfb81ff407f2d94fc1b2a6958b597b3093a7bede835071550a01b17c7f90c8ae199a59a62426b45a3f725cb37109c5fcd8da02358ab4b536eb58ac0ee1dd881e6cb4de5b4"sv,
                "ec96c4a5f1b32b78a8439ef6e98dd93b712aa051360bad51f8beeecb19eced68ae2790ced6e202c44bc1f1973614fabc489e61b6069a063b640094929604fc3f23df22e23bd3d55d4a690535cdfe303b657d1e6b296ece144130b039b18215377249e3a9c78d8d89cec44298555365fff0523e36488fef25b6d6a98911701f9ac4ea3beb23181091ff581187771b805112"sv,
                "bd4cb2284374d21c465ff20ebb88616abcae7c8a1cbd0ad966352c7a47e1f153c7e0eb62be767aa56fc5bc78ee5867e0"sv},
            hmac_test_vector{
                "e68bd3073773cb4d15fef2bb61a0be80d5356a7a2f953f6092691141293d0c2d0dd6b13269c51b8adc3c87121e2113fee761c6cb0c87d176eb8d14ec0af1e4c1fb0c0c2483a01113fbb0bbcbf594d083234c23597515cd8d823abbed292d699078d6ecc7cc2b36513a0196ce2e217e76529f890bb267bc0c1d190d2cce172103"sv,
                "0b0d5aa89010d5ebe8e2144e9a7a82a5fa6eab54a967e8655fd33fd7e8d985386d92c576f576e6f22b1487d662072ea9d625a22035f995375e74868168dfe6bce6147aa969ee001b47fe154d9b7f56788d8cc6a5d49170c079004defdd456722fdfb3feaf5606c8ec306865c4ecc423da70dde8c8e17c02838df5ac84ff3508f3bafdde1a0b473742796a1aa82d53a1eaa"sv,
                "8cf8f7217e2e9c3dcd14322e19eabcac17de11128f944441385ed984f07df8f446577c789f8d301f37c62af374a05602"sv},
            hmac_test_vector{
                "66dd7620746dd224582e8857079059417407f79b06b11a114939d046e4fb1419cb69a327b5ad0b90d19005ffb49a04e4967a0e04bfa9cb343c4bdc61d6566dd445f50eb4a02fbecd18c4498fe657d55a82d2b7f916078b6c329383eac108ff263c4b5ed21d3a14619792fd128410f22a9fd79980f312813f93da6ee88c5af196"sv,
                "a0f93cfbd5f5ba6b0dfaed5310fa07eebfaa031f049b36b205a627e59d8ea0270ce1051fed1fe5f0691ecfbcabad67679378b89daf2ccc1667db7a1877a1803ea388d279a5a931546b94fe7c7ff2517fc76946d46ec0971c224470a7806a7e3accb54a0fe6dac51e7923239b7611edc0c14552cc2aa0bf4c9eaaaf999e30c3a2f13215eb96fccad6fd92e622775164b1ea"sv,
                "6a186858094b8e9ac0f5b519a309fe89ee659b2a919f7f942a83da674e5163c65333e56458aa3bb29a11c62f65ff1c41"sv},
            hmac_test_vector{
                "dbcab7759b3e83d957896333952ca4329671378e7efffa41b6ac3af38f9275b6286564f3910438a4304d25c464361d046919dec07c502e313f0265e45e3c2d6b73448652272a0d559a9f7dcf060a76769fd97d46bd1545bb3c3a2c43234509f3850e2579a5d887525d27753dc1d237efff581e088a77c54677fb97f827d591b6"sv,
                "a1360850ee9e253d9792b8736df334679990370ed58f6257dc0f67fa1e0b6370b9816f42a5d471872e7039a8a8acc9675ef4f320cb272a4e0d3e9250962f596e25104a17f4809f6c33431703b62c49e4eb4038d80e1827e4a087af10157d0e6ccd2bc255456857f58f547d09ff3b5de728bc2dbf062a02217d32e6bf02be2b75991aff86ff55402928eb9144eca0ad77fd"sv,
                "2789ac4f72d6fd014b1bd5895e4812da977b8c6b3473a18270db1d99ebb9c47392400cf6a3300d88f8774f52b405486b"sv},
            hmac_test_vector{
                "3ebada6dcd96f06f3c2eba9a5f174c428b60451afc6a55a474f9fcec258c9476d1ea917530fe083d92e46efbc544a3a439df2e2b6267cbf75e942394d874daa92c03ce2acbfb9ecee2ed6ac43691daa2525b629ee54ec4006615d6d7f95085eb962c1af46836bc097371365b7aa5fb2320fb67d94dc748a7d3a59d1d52c79e26"sv, "044514e7de18bad8a149fadb8ce93228089129b739a346a8d3d200e12fe596f99eb70055fd832590895e4e20f505db37bad935e8af28038bd9aea162845ccd7d9258a4da1444ced117c3f5afc0397ba29cd36cd531c8b1dbd9de8751d02f30bf8b2de8c04be41b63f37fd5d0160e7a2f5586b7c5e809b8607a689cf63ae726892f40b54dcbea760dc49ad8cb1f0b40a78e"sv, "1da79416a0f61ffdd39cee60fa603e70c08061dbd9c1693683c3f54d6fee4a20006275edf3bae040df289495a89a6b01"sv},
        };

    for (auto &&vec : test_vectors) {
        INFO(vec.message);
        INFO(vec.key);
        INFO(vec.digest);

        std::vector<std::uint8_t> message_bytes;
        auto r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.message, std::back_inserter(message_bytes));
        REQUIRE(r);

        std::vector<std::uint8_t> key_bytes;
        r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.key, std::back_inserter(key_bytes));
        REQUIRE(r);

        std::array<std::byte, ivy::digest_length_v<ivy::hash_algorithm::sha384>>
            digest_bytes{};

        auto nbytes = ivy::hash_data_hmac(ivy::hash_algorithm::sha384,
                                          message_bytes,
                                          key_bytes,
                                          digest_bytes);
        if (!nbytes) {
            INFO(nbytes.error().message());
            REQUIRE(nbytes);
        }

        auto digest =
            ivy::bintext_encode_to_string<ivy::hexchars_lc>(digest_bytes);
        REQUIRE(digest == vec.digest);
    }
}

TEST_CASE("ivy:hash:sha512_hmac", "[ivy][hash][hmac][sha512]")
{
    using namespace std::string_view_literals;

    REQUIRE(ivy::hash_available(ivy::hash_algorithm::sha512));

    std::array
        test_vectors{
            hmac_test_vector{
                "2423dff48b312be864cb3490641f793d2b9fb68a7763b8e298c86f42245e4540eb01ae4d2d4500370b1886f23ca2cf9701704cad5bd21ba87b811daf7a854ea24a56565ced425b35e40e1acbebe03603e35dcf4a100e57218408a1d8dbcc3b99296cfea931efe3ebd8f719a6d9a15487b9ad67eafedf15559ca42445b0f9b42e"sv,
                "57c2eb677b5093b9e829ea4babb50bde55d0ad59fec34a618973802b2ad9b78e26b2045dda784df3ff90ae0f2cc51ce39cf54867320ac6f3ba2c6f0d72360480c96614ae66581f266c35fb79fd28774afd113fa5187eff9206d7cbe90dd8bf67c844e202"sv,
                "33c511e9bc2307c62758df61125a980ee64cefebd90931cb91c13742d4714c06de4003faf3c41c06aefc638ad47b21906e6b104816b72de6269e045a1f4429d4"sv},
            hmac_test_vector{
                "74e8936d83bf3f16b8d03fb73384ed8f46bd32343f5df8358107e2fdda293afa103a2bffbd4030e75d96cc7ca6ec7c97188fea88d4eb63b7b14e8b8c8dee4f8de12e1cc6981d4e6e223fecc7c491924632c7aef45fd8ef1494bcfb06c074616b0f4cce8abd5d83f32d550661357b18e5bcede841882c869251db9a331ac456dd"sv,
                "7c98912c74421362e112a2f98fed9babe0057fc778b4453239aaf5ac724b725553539770a5bc8666b8e13d0e9ce36b2b934c8137c7f20b5f391f41cefaeed92e9df8206cec3049bcda0c05deb9e6549fada19aa2618ff560f892ce6e4782aeff41cf53a9"sv,
                "4cc28818486bb9b1b52e333dde71f73acc227488453fd907c6b51d349d67af1df29a9f225532ce04f50395fed565e98d78978626df93462d3f012f7373347298"sv},
            hmac_test_vector{
                "0c057a2b56cb7e651c6339e4c91a1a72d51af2a646de9dfd77e9e42c18b8a2b576f526b9fcedd90dfa442090a6e784bb614311793bb5fb39b8418842d586294746f1ea3c02320d6801ecf2ba44b13b60172d2d9693a158bc66947aacd7c5a14a0463905d6e80649db8c4770cac5e858a7f400da4568cfaae08498311265b50e5"sv,
                "662ca8f53b97edd9bbd43b1f9e4ea49f2ac14417faee257aff93608bc49a85abf6913def235a2e76c2241ffa749a5da489595d25c6a8a2026563e12f5e3964e0e518ac9c34e45a938a6f503174a613f34b08737afe5d6fde11a45344e64d23b33ca83c23"sv,
                "c0d6e13c5746369d49bef107cfc9a465627691320b8203233359e6a49659025ac96a6db6c4d460224f6aa1cb7a6b8df311e066f6109bd466cd9aee3058dbc5f0"sv},
            hmac_test_vector{
                "c532714f570982993d4b22c7d07a1e79ff5a75c94eee75dc1fa222b630cad753664b30f3c99826b5cfe17c67dd875b9d0bd2390028e6ffe9fef36a2fd6adb13d3ffc69670cf4a67e9c0764a15e7925579315dbdb561f07b7da892394f4693e51d9abe65228034a1b2b26a01d5a3ac5cf208b2301e27fd86e3ecc159090e8c3b8"sv,
                "0cc5bca2025bd6030fe0818e0a61ecc730b2e5526da942c0d7897fa97bc1a8fb5dd77991ba9fc50890b014ce6118907b334f2265db6ad86e7b918a214ab3bdfe9378c711017834ca19aa6908081f87779ff0921c9c75d32e2bb77a28ac28881cb792ec4a"sv,
                "c34bf0931b2dd2e41956dc86996e1427379d0c89739b1c33fa3be5b0770673a20c5335c6d22c766826009938fe1f4d478b882b59a3b19fdf25bf18f043fbb3f7"sv},
            hmac_test_vector{
                "eabd8db90e6d67a41f096e4369f77cd6ba23da4fcfa459120d9c9ef9725fbe9bcad80bce26292d6a8a927450e6946cab4756b2764f47073fe305a32a237ecb389f55a6c9c7874d60a44e21a7c64561b37ecdfc884db0a3e09b052328ac54f2ccd1fa07b4dbceef0fd5041e4ff3528374c5525f8eb028567d9f64c7fcf62a59de"sv, "7d407fda74d3a127b2ed14c727d0e81a04f6789d20eeef629b670abdc18b1f41318e5eea3e86579c957dbccc20c4687d2b8ba16fc6af9a936ad33cc1dfb226ad5cb3f318f1bfbb43224fdca9d5c9faed6e0c44123849f9ea07162bd11bbdc49b48dac6ca"sv, "771bf59b658cb17576761d078cf6b1474db746a2201d30ddf289fa708366a27d6a53959bb7eb2b963622b326edaae3dce086dc364c93c874e50089b69c5cf52d"sv},
            hmac_test_vector{
                "1a7331c8ff1b748e3cee96952190fdbbe4ee2f79e5753bbb368255ee5b19c05a4ed9f1b2c72ff1e9b9cb0348205087befa501e7793770faf0606e9c901836a9bc8afa00d7db94ee29eb191d5cf3fc3e8da95a0f9f4a2a7964289c3129b512bd890de8700a9205420f28a8965b6c67be28ba7fe278e5fcd16f0f22cf2b2eacbb9"sv,
                "c367aeb5c02b727883ffe2a4ceebf911b01454beb328fb5d57fc7f11bf744576aba421e2a63426ea8109bd28ff21f53cd2bf1a11c6c989623d6ec27cdb0bbf458250857d819ff84408b4f3dce08b98b1587ee59683af8852a0a5f55bda3ab5e132b4010e"sv,
                "4459066109cb11e6870fa9c6bfd251adfa304c0a2928ca915049704972edc560cc7c0bc38249e9101aae2f7d4da62eaff83fb07134efc277de72b9e4ab360425"sv},
            hmac_test_vector{
                "2485736977ef55a55abeba3b8e857ee2fa5beb144324e46f9e12625be26b25ede28ca30bf92e45d1e6e8d234daf52be5d0383a781d7d25c64802c7901b366065fac08bc574c3718618603d778a7dd044d6c5b59903f0578aec4571334b5dc79b172914df1037438c9830e14cc4a6d3c5b30c44be1e06e28331e44a8b9968c059"sv,
                "52d3e26c59df9bf3f5c01e311fd6611b895dbf6e8e918ff16916fbfaa6981033d7af119e880511d775bac09afa078684ca22ce1ee462a517c3a483d1d5ed68202f512b4e7f130f62420d98a137529d5613139dcf76bf57a81e6e944c5b8048b8c281d982"sv,
                "b1c34ea9d837b4e0b0771792384fb5f5b9bb5af7226d461b5ca81ce8079c6472c5c44624a640f01960c8a94f6aaca5324c0da2cbfdbcb077cbdca7f6c6a38e75"sv},
            hmac_test_vector{
                "e2542c06864dd3a0499493e144a97fe04006b68c83a5dd4ceac3cde238e48895ae17728fdc7bbe84b6022694ea75df7371b8ccc76450f2d112222c504f7d1fa20f5b712d33e436fda234abae9c5e278d4bb14efa9b3a88114c89b28946b813db2caa91a73391245435b38cf8016d3f77f678a6eed06b8852c181c754c49d4a88"sv,
                "ce1e3b693ce203166bf045472fdd1457c8f6591a0ad41912bb30f6e63df8f90f6ca18ed5cac0d07adad407b5c9666f6253553c77e56bda3aff3379b1dd0fc95a5685021d04da287fa5e28d18c11697478ee7c3241052eab684b5c467ffe1aab45370a029"sv,
                "d5e6ef77772459874a73fc4f7665cd3ade20468bea1ecdac41142ff32350b8cb15828612050046299f08ccc486acef0d0c04e0f8feb29045ef7e3a3db093d512"sv},
            hmac_test_vector{
                "9186eaa3b8e785dd2a747297bdbdd4f5532a47b7008c21686ff7f8d881d464cd383205f6d45dc8203bb267ac9eb12f415a5406be1c9fac73497941909dba08dd12856aac03d83e0d916147404694fe70f8fa929ef0cc2edb4cc07abaa2236405e62820af8e806d0af32a1b3afb8dcaeaf5c4f43dc4392e074075aa3ed93601ab"sv,
                "3699d9cd078a20ec0c96eb01aa60df6cd5dcb554260eea8e2e15b7c00b6943c638611854aaf8d3dd18d020b49a77e67275eabf973557ab74fd2705481c3cb6a9e077a825af7e7e2a53bc822396a9dee40f4b10483bfd9818d06ff32f4deecddcd6e57388"sv,
                "d8bf5ff4392938534a7962c64985f163ce7c95e6c05f93cf704106f9bda7c9ae963f5ea87f73626f67ed3146e8611ca62ef2eeb4f9a13847dc6e7ffbe3d851a4"sv},
            hmac_test_vector{
                "debcf190ea6ab2358636af5cfe4b3a9bdc1bce160bf350aa3cd3956b897e255158cd3e2e83481ce3b6f778d418764f992d48e4f7fb6d080e6b3799d3f35949c17241a0cc5ba84597166779e6a38ce45681ad944cce7c432baf9cd8caf2b33125f2c12052bbb0b3b76f2cb97be9b4813a9ff1e5fdcd478769d0ab5b36cfb466e3"sv,
                "b0c0a896096bc42bd0c5ea646779a4f1ce541f9cbc04df29ef20b180c069e10efa50ae68ecb8fb31bdfc473f0034dff988b452037ed6261eb0fac9492ccaca2c0ec349b796f1ad077ef995898a5d106160fc100d9ad81c451a1c46269d5e5d90932163cd"sv,
                "2180018c7e9c3287c3d2928fdf36dda80be4fa21d3a879c0f617eb0e43c58836b0cd714a8081652f8dca9a01925a4f3ec5dbe07b5160be7b1ac58ea623952293"sv},
            hmac_test_vector{
                "46ae8403ebb4c8723652b9848fdaaa537a50e3191bd94442f9702bc602db98b5cfdd8f142aaebee7cda8608f6d436156f743c3491a30404605073155722fae3be3aef74d2b9d128331d9b6cdf1fc68aac38ce4f6e072be0322ad49ec0b47b82609888358f86b0d6de94e83e722ed077666910ec9768506a4d7ca3d33d60bfe9e"sv,
                "1ce7e20abbdcd1154d4b536714ff534a01b8e88c78da34d653638c39291fd80ad01f3df02067fa3bfae7907789ad2641c8582b5a45d03dfa24344a676614f5c56ce13b30b6a15608f1e7e18c31033eab7b76351686a9dd9ac2dec0ad9a663a47f61422f3"sv,
                "d032bfb5a538197385eb70673cf8f93e31fdf9c22c0e90008a454ba4d69bdc2232475a41723c8a5e3b29c6de929a7a1e87b64beecda29683d0d925f00ce23b35"sv},
            hmac_test_vector{
                "251cff72773e93021e816407edbdf5c1b0dd9a0d633f41e7a25e932d61ae3ca5ddc78642d2c62da3eff06fdd8799627a89458ac2b20cab390143dc686c58dde0d1feaa7d2f8a50e8169d005f5c0462b912dc2ba4b6faf232aa8a4094e5f5e625e90993aaf554a5d77bd04016d4c69d8533eca53dd8d0bfda867ae638364dfe7a"sv,
                "d7148e81b94a9a902b5980e751a5a59ef4a2397ad4df251240443e30e598bf7ae445f65227603dfaf4e42cfcc23e0dc94c0f90a0e52dc1b10beb36833e9a8d93134f163e84e7cce08a498a3eeeeb7b215b98d344b970bc70b63093e6a5b355fd8cb9540c"sv,
                "9f783389d7905291a0446004816233918acfbbc1d4443f4dfdff55f3c151a7a1ad20d0a18d0aafc4bde38e3bbd9c7f672f8b1d14649e8f41c47a0fe1ca051d8e"sv},
            hmac_test_vector{
                "98b0c5e030490c0de1cb08d49ab64560693160acedab1a450ec2ab52eb6459d114344823fa2f94cba48f9d73a3efa22f47b19206139d1eaf6fca13989dc2e72deee1915636fe9e417d4e8263f7842cb9373ddd549f9c39141b319fb40f20b6068d7f4880ccff54f8d5cf5eac80bd0a859f9fe99d79f193fe7abff6ad28c6ede7"sv,
                "c821be1cce09579ea899899d24f8329994c2c839cf0084e27857c688837fb5c4f4f72527eaf7bfcfdda75b37248eb153ba4d31dd418d2fea473643c0c9e1f0ebf591838e349d3ef868f1b67772777a71f8cff5b0654696fe31062ef2628a99095355a0f8"sv,
                "8af5f3e56ba1a151975f4fc6a784aa050572f7b163a93f24f016395ab4688f39172bf20f1bc246d73b971d022b3d49d1b31f40b0a121b9c3a66e09096d4815f4"sv},
            hmac_test_vector{
                "18aa61677eb2e25b7f5738c929c4f4ccb69749a1038b1a6644545722f7cb8a06164badf3915b3f3cfb8d97ac83a677cb27fee45d242a352cb1d96b07ecfcc00b152a8321fa4222c8b25289158eb7aaf74d865dc08f2b6be18d50e5f50601bb027d0d89fba1afb4890d6db60a3141b6db90f75cd22dd6e30f3f8270f52c21a273"sv,
                "8ad2ff9cc9e5979ab79e2122f2b6c0d75f0f19da6bedb79a9762aeee330a7f6169f93ccc7ef2ed2b55d931a9356b29fdcb2d91f973a23060b3c173f908a655e1e6888125faebbb90b2177cc2ffc8dc825a27b55605f906509317952aaa1ed996086716cf"sv,
                "3aed2fb463393706efbbb98fd426075af837a8eab622b95da9aeb0393188665336f0d46bd20773fbea36aa289bd702d6ed4d6080449b680c92b0355fcbf13ead"sv},
            hmac_test_vector{
                "f3bfa5c1f1055281a35b48f86fa3ae454c03eac56ff064cf268b8da20431219b3e4da9ac55714309f5a6a5241e0060dc817562f12deebfbc6a9fb11de594ddb40e8dd754bf0ce9b41eac1068c4b448101fec09d014b01200e94265246365931e2b2739a276fecbebe51690acaaeaee4aed12e8486e5be5036b1db39fc4c9cb41"sv,
                "bceaf34d50c1f202539233630b16dd048ea23f093c9f713b8d3a385b0d5c2bb6a4dd14f91bf59947dcf31c89f931df0570476c33ae7f34cb51897623327062b8a3cd7f0af53b4aae3e0a209e58385dd32d9cc6163265241332c332af4de4b99b4022fa29"sv,
                "bb714844b6be93ccec0acb8780996b2a4778c42a8a76a49eff87ced3a258815d76685dce4c8ae37c244229f17e0503de089a043368cd300d13f842f28c111fbb"sv},
            hmac_test_vector{
                "88ad812fd34e55c809e817199604b4a7f7feae42cdc4c9e930db08e845a3d74313db8a57926706bf0551be758a0fe239f004d237c849d9e4bfac18292bf9c0c3e37985ea54b94f30d18c32ad2b53a059827cddb95a49b4bef1d369ead14eeeb4a18e592e40ca96e515a15908a05a57cd5570b611ab4ec23f7057e1725f29c9de"sv,
                "13fb1ed6389f32d1de3139cb04bcdd53525c9889b85379d3535a25d290351c95938a3d0cdaf38dbf1d5234bf7965c8ddce9ace1b66247e60d74ec7702a0f931a3cdf4cb465ca9fc458c380004a3a6e7957f1f813210b8038ba663fcdc42a8965d6a252b5224bf249552b2575bf64568db4091d58323006c3c34994d3a5"sv,
                "a481e713cdc81ca5afa0efcb16e35cd20d01aa449958fd2eaede2e25a5ba540beafba2fab4adfef2e146b4c1b2a1832e93dd373d63fa90bb61490f6568191f65"sv},
            hmac_test_vector{
                "8d2e68d7e9846cfa30d931a38efb59bcced53a14164b3163d2653888eeb0bb1448e1a80c65bcc6eb633447e72ed4a075f75d980fe2b19f35ffef62b27ce09c2019922faedb427321057fce19448d85962a08d1baddc936d1110e108e33d46f97e7882445b5df1ca4ff03edc237efaf264f1c0d9e705d9b3eee076ba57c56db82"sv, "fd5070362296c40d65b105d5ab4653fe34e0200516933f3eeae03ed0c5d9f6016a8560b4bd86ab2f7bf98b22299ed3e54a394602d538aaf3e6951f2db4feaf5dc33426f15bb124da388d709083a28f5701ef96c28b3a3c75bef9332ef373b90771236af5e25d589504345d28a19ab0dbc1c9b74d1ee21c4bd8d423de6a"sv, "b6cad1ca5ba505498a8f66a9422bf539426a8a55334fab9c6b9e08e3a5179d157d1efa0f91d5c5e26ffa43f5c1cb7ca5f906ce4f0efcf4e871820b8353e890e4"sv},
            hmac_test_vector{
                "3173a712ed715b321a849279be6ba237fc90dec0e48b0e1290e81cb86c2a10eed50f3e05e616be098e3f1da2e6125238c2e2f45a8753aa613c1ae250e304c6ff093a6b799cc34ca2fd4af81d5622076c2e8752593a27649576e12ba075ccf3e8f57b9635b77fec448e2e89b2b5a3a81d65066285a70e24f868c35f6ecb8981f2"sv,
                "0f67caceedf8cce39a7223d32ae1b6badae2c2ab01bf75f543dbb8a408514c4b2cd81801f9eac1aa52257c7830f120b1536380b23161f734330744ce204a98bbd9dbeede484e9b03937c26689526597c8edb98e6191b72c95fbc76468b8d48437e3af46bc36f8176c540caab3fc989a3f511b54fa2350e3d31c2f6162f"sv,
                "e913fccc25f84a1390ef0b0f86050fe9e61146c1b4ef0b4b60d187baa36b61458bbda925b96e99cf8a4330291690417fe194cb9bc2b6232d43e046d13f038ed3"sv},
            hmac_test_vector{"7f2544a478961dd012ac705e12e74587e2df6b2ee1406a0bfe13b908853f31eb384fc236a275654ba8cf51d461ea726228851754ef97ebfbd69326fcfcea04f594d177633c4618c38b4d64f7e98025eca4c62e7a40634b8b0e317880002c51d0bb34caf2ee0f207ee2ae108f53b1466b7f2d18667cac7403ce75175d390e00c5"sv, "5c8589b3845970145e12b34713455eb6b5ceb132242024e42fd9a886fc9a30d3aa15703b3467e4dc99a915ae3ac118fd837e571dcde5945983c52a2c849296b4c96f3763488d52f818b459add51f6db2468d21db3d958196bf3a531f65bf9cf702bd66928672c14f235c08aeae0665be472397d43cd9f3822b5fa9c767"sv, "be73d57ab3a7c5bb305451922fa9b11d0ae938bec886fac5645a8b72de93770d96465291da2bdd5d11692f2cbfae69adb36ae714f1e5cf43b9d2841ddeba4ca6"sv},
            hmac_test_vector{
                "9b2e0306e73662c94377e5a99fa9b63f034ba91c8b95645eb0afc381c8207b77d089fdfa6a939d212226f331f5dc81b614b597d3e50c74d74bc9ae1027d9d4e41fdb511d9e1c93851bc66db39f54ba248149448e7422e470c589eaeeddcbbedc0d8cc8198bba8e1eb7bf1cd6a3fba9b1d37177f441c0fb53a4563ec1f2e08387"sv,
                "7198b12a22014154578f5236b5a0d4cbe29aab3bf289bea2220a4a13c9677ddd8b03400cd0f954337400a069c192c9929a4d04e0f89928999b8b081af909ff1c7b2fcc36e8f2a0a32103764610f6a3ebdba4aed05dce6164d603204ee1c37ab1a7e4feae5335502bc6627cd38b17895e0d6852130771918b3d393db76a"sv,
                "165e1cc47a7433270f1673e1cee581001708195a3471c9ec71333ce87299e72abc5dd490d43676d10c1375125d4e7fc673adb15342ddb7e72eb24ff36f54f82c"sv},
            hmac_test_vector{
                "1e6b0799e857a1efb3cb0aaadf74c78c31d5e1c72547dd1d863eed463bcf6892646f78cfa6fe136dc2042ce06d3a2a465c4c994a9edd1f482ecbb2b2c9b509b2fdbb501083852057ce87ae33e483431e6d4fec3b09d87282e7678c1e9423541310d8f82427f6b2f4feddfa6bed57fa5b8c6642641141bd15d999e353442031ff"sv,
                "b5ffe36117589646ac097327e4147dc9fb13f7346fd97a6fb756448b32bbd3e25edfb3a14ae194844e109fe1f9070ae84b1cea2924fc4957fd8f794bd622a74b6fc4b38dbbf040f7926d0729a67370bdb80f63cd0cc85bb61a83bd1c86a4692f52768e2c53cdc226e1ea5206d39ed6d1abb309290d87d81776fab9072a"sv,
                "5702e9898e96cf220c6778cc1d07469f13ef0c2ec0e335ddd1791ff265ce865d200e4e6238b52eaa9be880adfc9076202625bf83bc07d861401a17d4a51d84a8"sv},
            hmac_test_vector{
                "29aaac0418f6bb3890902888219720b59878f226d06c7e8bcb42e9c0015e96f4f802520a15cccf3fb280540e7108b251cfb97aa8fcd86d1eea5d340aa3f65234e14f5639d89155315729978e0fca914732b513374138c3c01f74cab36964cd740a1b1f59094d3554a6115ad2a6e5a3e2ebf3269a479367b692101383faaff1fc"sv,
                "b8100034c20a0b6e423c9f6c541fe9fca08fde8ce7177191db6f0929344332fe0036926e3a2720ea9fb3909cbdfb114d4da73b85c22b43f42248721015d6d5003cc235b9c35349673ab12b0ea0a70971c1a81d33c310df3cfbad795e057e7b3813bad05b8514f1acba3e580a26ac3830a59a6bdc0f50ab310da506592b"sv,
                "9c87ca3b8ee3849224833b477333d4fb1bea3d8167fda445dc677d0d70b9ba85d0c4fd79909703481fbfe99caba7299da8c514a7a0799d59ab6f9e3f1f63a372"sv},
            hmac_test_vector{
                "ed3dc5fa63bedb28c12a423320ca6dce3afe8f72712ebbf61304495fdcd87bf9bc6c61a7dbdaf977dbca9b795de894d2c696120c43dd40b00aaab117e337027de7c7bb57889677e6d210450df1414d60f28db77dcea1c89fa4a94e7046c33f1ff7b49db373db9c9f30630796e4bce0a0474b476e1a609cea6869e8130c667908"sv,
                "d62dd5837abfe25749b0371803cc47ff4b386e840b4c7ad115a06a76066a765e70074f0238d7a7dc3c4ef41f394871a5ed9d662978f6aab608df665fb51d1b31aa41e766867d04db02b791c5d3dc959fd27741675a827509f17b861c2bbb3fcccc0425172d59ff3de1129671972fbad542ed85a57897e4b2189f521330"sv,
                "1e5b3b965cf268d5f9529d0a256e6512b41bf2726d762e9447e40af239cf3673fd91257edc6739cfcf9e6893bd9955e4166e4fe2eff2265c1534abaf5b11544d"sv},
            hmac_test_vector{
                "e76411d3a1a72ffae3f9363389548084510d61fae1c251400c8e4d70517e7a29a4490ce2723b667edf738220524e94403323eafff33117b74dd550d7cc0116ee8ee9160547ff4d3288226f6a42128a978628e6ea8e4431730106d8ef7341b8e12bacf6e42adb2b3f696ec5ba6189aab0d0841e66bbe3e69baf88ad604d27a82a"sv,
                "ad9e1d5774ee7e882b2dd772c7867eaadc56299b7583f67b430fae7efbed4a49f913b6a929d0d6852760c711a5be67450eb9178e684abfc37f25135a408e15dc636edb964da6939234a3c4c58432d78a8196d54f2ed9728e6e5f4b006ac16c0d32d81586a717be96981e58f62b8dd6617f16ec488fd716c65ae6ddd641"sv,
                "313a41518da2a8194451b01a4365b1d99d9c46498c24160b51a4b494f669dd2d0bf0922dbfecc703255f7ab74fff2d7bee9ff2a4823d26874f7532594b96847d"sv},
            hmac_test_vector{
                "56958d7871c6b901f42910b8d7a17c3172fd25454b77e5b52d81fd0f7ac206d6c15b2add936e93f493e124c6cd5f670249a0c7b9dfeb00084cd9d3f6865bfe9f4dfe6b93036ea879bd3997f89ad667d754e6c26214b948108b143d643e327e112108ba6168ceded79abad114e70f62f7bb567f0893eba71710bca120494e6f72"sv,
                "054221b2a3507d491e5f0da4292931f63d105152316217100adbdb72146b5f88323abd5072ea9d22e41cab4bad3549f9576280576bd93df84262ba7918163a626267f94e6161634cf8308484bbe1c01e486dc45fdaf3bc151b45c6363aa6a7c43ed2bef39a3f368e01a37f977bb6fe2eb93903b3a537fc2f3e8be9f0a3"sv,
                "6d3c6a9a076d3a3506d7beed3845521f50b2df2c5f3bce0707d66e20bcf09445ff13e0e445190e7f2c43fc73ae2bf68858a721a67feb8083dfaf498c8b86e192"sv},
            hmac_test_vector{"2679ac7911bf024295d61b25aaa0ddba9328bbfe6ee3e5b10a36bbfcf8398857e5bb6daa27869bb295e3c5a79c61d61c8fd3df263f9853c0cc1f766ebaa9132147516d5bfc4a799383694adbb8e3cdc00257b672e40fba25cf83ffb63b1ed192f64596d4bb9604ad066d910927ad2b6f95bd2a2986d083a6bc6a3e1ca2202a71"sv, "42d6852473735ecc21b107e3e0a5d3c5ad2cbccf7e2d9dc31d1ea26e35cafc88ad51d9db367093a82002274672c7ea0c3965777120acec72411e56343baf69eb85c975fa0293955cd5732a4276d287fd051fee1fdf16bcdd7042e0b848c0915d704741c9fe65ccb0e70bf620154c1462b9620921e3f274cd5091284282"sv, "cbc45d2f971d684010caef1c2d6124f56be8dfdcea7050c417b0a26f6058f6401542f8cd4cc9f81a28f4a4eecdb8376d8fd5cb15ca6b2bcafe79e1e209f55c16"sv},
            hmac_test_vector{
                "2b0b018d11c1ebeecc2a1b5bd17e3be5b87f577987ac3eb733b82b00643aa4d179a6a350cc7cf42537bfde8bd7a905a70f9d8f18d05d86cf187d7d21b918771a5cc10415c5bcad89596f9226dd13522180af2bcc1c476369b950e640536830efdd446d0285eadfc33a2081536b24eafa129d73bcf2d29c2d1c1aac86b89398f7"sv,
                "0748bc683d617a4e668f36907eee655e454d6c19e43f6309d3701952fa36582417686a335f73db6768aa81d446debaecf52ff5f3f72174b43df350734b2cbd2298768fd833affd7eac3ea5cc32a3d174f952f99cdfc289924ec1e3bb640aa8cd5119343e013e67fdfcf1ab5f8a65325c1a6080fbe4492a0bce83dd148c"sv,
                "ee313f4f6c8ad4764f81873d4baa3e9f9b7f6481904a01fb1c827eff0314c1305cdfb0595a4cb8a6f979d9b7ab337e38eabe823742b89c2a1ce88c5260896ac7"sv},
            hmac_test_vector{
                "b6d8212d828dfaf27a03b79f0cc73ecd76ebeeb521058c4f317a80bb5afd41dff5520e525358df7851469bef2c358b2a97df0f5c0ba68f985fd8b5369831d97802a1bd6e80507b1620e0bbc8f2f229d11beb275f25f8be9d994fdb1ed0b8b87f064c9848b07db7140f5781f20606020a29979e84160302a508695a3ba99bc43c"sv, "ad8db0f165d0ed6e9495e6e53ea1e0dee4437cc156db2e83c999f084298738b8b1ca2706b82503edcebee76d0725b92597925db99f6e2876471598390ddb428c5d4d0b1361b99b271af00fc0b04aee9806eb5f8d924599476cde9a1ca4531c30d99ec5e107949e62a1b1c6a5e018687f5ad6ea07d667673f5f7b60f697"sv, "9210b074bb645c3a36ce46f9921de1d6cd8c9f37829c251d1d999c193dc30467d44c830084914cf19f4ba761a0e774c103f092b51ea25beb1b8277e99fde26a3"sv},
            hmac_test_vector{
                "b04bb381f91b8b64478391d6ab6ae306e796020418bf917899e85a9423ef924739625eb4c39496e2f9f0c8b5828e801ddd04c68d017fe9af40127b56714d9db44e127dcd1ba41c2b890155d3d9721b79446002f09b6900e42bb375cbe3806a19b90316b34973a7a7bf5d3f1af83e3c82e451bc152fa66080207451ddc1b081df"sv,
                "484babc2861503d442448c3c43d7569a380eadd9eed2eeb4c651997867e9a80bb0471f2df0d0e5fe5a3fd2c0b39009df4e1e882e3a08fd74bad8db27f567ae77effbac8fb8b38c17e4259bfd3a450c75b1898f3e984a2627146e34d3cf44d7b9efaf786e4587a127c73c0e7a41bc06644797ec906e6111d6bc59cc035c"sv,
                "0d73f53c10c029069a2d1d5733bc6be8a1e94de821573daa290fbee3f897f44297cfbd515cc5ddb0df7bf44ef58c446ec085c32c8d6ef779a5518e63328de957"sv},
            hmac_test_vector{
                "e8ab6a06dbcd52a245152d3851ceac0551d5c3780da0a44f5a7fedfeef222050755c5550dd262b6af6e704ce3ff37215fa51ec2781c1c65cddfd60e195409d5ec40596d56e1190ce14b32cb1864bde73f909a07459f7e8a8e13ad7f6e895b3ddfe2e975d37956915c7020da9cfded866778548293d4bf27c8dbdbb0391294b17"sv, "c4e631adcca6a4735c905c369c7cd44d132200b362f29a0a3240b7e06ba49d32a6a1d82ee6848b3d828f05416657941b7c6b9e716436edae6b274b4abb0f4f4bcfca760c2a21753d49efce2bd683a2411e46c6cfc59d190bb811145664aea2c026a832cd02450b7a97f6e07b52c7a5c7e3faa156edefc3290248398cce"sv, "8d9725605aea94c3ef125b307f7acd5a906d10b4908fcd21f678937cc4605b0800af50480ef6ff1439cca60cd1986f04a1ddea0abed6c8dcb9b485a923288c03"sv},
            hmac_test_vector{
                "b82eefb2081bd14dab0e9e345248a34ade73f3291886b91ea3e8cc742fd884f6ee0ccdaf4c9879f4db12dba58cf491af2541a1d5ef6cc8b1af750ef5d8559ef7ff9cd56d8f599974be3aecd8c0f4c08f3ae50d86f9f822a1e4ca39fd2f0b4d78d22630733a24d8d63ecdf9555411daf205a761c39ef46ff6292e74129bc13a7f"sv,
                "e9e4480d1c4a621e0c4e1505992556347a7ab34fd2b289910474766cc969116f8040d96dc5f66cdc4454fa7bcfb9f838af19195038467ab8a16e1cbc12e598e6fd250e21b2145f1e2e859cf73400be12a0c69749f7100847429875351d5a76970b9ccf700c2ca3ad72e9e4c0f0840e8cf488158136989b0891f867211350134a"sv,
                "90093bdcc45da7338bd2efe92e30933b14f75582739c747f7572b3270b104f33af0c939e3c8ae53b2066fc8c97ccf38785cd2ec3d79e6946499d36121e44a3e7"sv},
            hmac_test_vector{
                "b967c7d9c0a941f02e87723cf282eada4347b28193d3e0bfbeda6985886a37e646cc7b1cdbab45cce677528b3a0c24a08f8f580b779935c79398814d067298592a6bbff08248b5a2f0b48b0d28e4b6a2657763ac5ba00a8d6c86464b1eebe44ccd0c395e9dc9b9fbb306c6caa551c6682ec57869272e889ab26e6189b91f4248"sv,
                "d3fbd6fe4e356ac1c8c120d432d7204d9d579b2a5a5d0c8b6016bd1eefd38dda735cf2f0ab873afe0a0916865e8b58a0af01fceb6a3765c9bfaceacc47a4916bea791afa003240d9b6563bebb3038949fc3aee38157dba596a9c4a20edccd187fff95904945d04b8925298e97b643ab24cab7af9a55890a2298de5022872d697"sv,
                "bc9a83d782e50ba5a801146f8da39095d92387d759eb4ad52bbd9e99d9f68f4a0f6f6470c653c45979c2e19543804ced592ee9c53eb68a5b1b7746ed403ebe67"sv},
            hmac_test_vector{
                "fd13a5e109ee583bda183ab64e4d27855bfaec17449f14991378febc435c33b8bde5f79106d11e98b6a821362c9f71e580bd0b7fb93c4dbb403208f49571d62d41abae530cdab5c16fde570a4c6897f2dd18a3bdebe2acad40b6f4c65e6029d471adf1af83cfc6beef0204ba187040b45a52dc5a4159d876f94cebb706f2d3b4"sv,
                "19d4cb1d72c73e2577a23006f31466ff777b9582fdfb25e8cbcd34649adade35f889bc20ebd5aa1ed7a2ce52a151d63d1592803585796013b3d5de2df2bd7e84876b643e554e1756ba5a8592b4a347b5482a27f624f6dfb28367245e51c8e3bf8f23cb5dfa590b35e7715dae723143ced7eb90ae209a2b2b012e10df00239750"sv,
                "b201720661830a671c94421958f73c8b666fd8a323629548a29afa163cee2ec24a01201d901ccf5b0adb1d20fcf0c5ea1c7483fa95ffa0a9590b970385d5779e"sv},
            hmac_test_vector{
                "18ec13250ee9c74c0fc4dd564b3d24a825802d5ae402a53bacace115ae3bbb329be79d1e5e42dbaf0a6446431145fe49b86a8703c7c41f8985d54f12e314c16ff89351d8addf66ebba2783f2d1a11965182aa0b0dd2de53586c5a695c6265c2b173958da648611090557bdebf11a1e042f089fe98e049f4796c60d26be38356f"sv,
                "69d9440047b29b8e1dea08482a506d9afad24fffe9ef7f18e36ff9ff6d510cd9e905bbaa15db646ed6bc9f65341869aea51f82178e341334079e94aedf81eddedf0ddb9a53f6964fc724b1500fab416e8afd41c9a75f35e6a7990e01a5f24ea4d45b6c7809eb7a5c798b05b4c3f33d0331d555bd3a572d564cc72f9fc98a7752"sv,
                "921264559658c2a0f948d13620312047326ba3ab84d1795c9e438fa76daa37ea5f16024306be804aedf8f91b586987254bc0ca8d64a79325c46b2f0b7371e3dc"sv},
            hmac_test_vector{
                "b85c46b5d55b896d67b87ac3313a97c7509984211ed80b0357d4615c7a1eaa4f7206c0e376f830fc2e0c868a17d8cb0028894b08b6329c749563db7880fe3917ba46b6dcf6392dca752091956e647613b2a3d3ed9003069af6c6188eda1f43aed844b1081dc587c1831c224efd85a0e73610a33975f4515cc426a004512ad0fb"sv,
                "9ecc24e4faa8fd520aa9a49cab88fee7fd39425e13ca502eef8d45d5ad794c9dfeceb763d8f84e9d6cb6e69c597b360e1f15e7c6d68ceac0204d0e5f5c87d2fa1cd67797d91f5af6e6bb81d2a3d77463f31a4e27f08913e2dda844e45be2b18ae02b8f0766e4ca6460ff9dc6f2635ff06192a008c989749e0ee80fea14529255"sv,
                "9088a7ca211f69835b3786789afd93f3704de4a34116ec5cd5ed0a43a3bd611ca08619718d9bf287502bfe07b6d79b3b2ba982f99442752aad29ec23856bfa47"sv},
            hmac_test_vector{
                "2b1f5c46d4b819bfa1ede55a14077644b642aa3963d177a6e823200bd065afa47a489f486f04d991f39de23dda6452d49dc2888bad319c69078b95a80987dc5e8480f15d12795d57aa5fe846718d0b0ad396a854d33ef9c49fc9c74e6879dce27052ba4c65208d59edbb5f3b828a8b2e8046745c7c0076fed8661dc594429578"sv,
                "aa42b41c544fa928b2f3c7f12c41e5c56c910860ca257cb3080c24e440470e951a2b4a694206fdc41a05b1d3ac55efcde2891078f93c50ee33f724a1cc55ce9d30642e0d6b4fdb01e13a726e3f6e2e76b1b6b9ea5608420ef168d09ce10ad60b53b70710b6716b666f5ab3cbced2ca4b41e0acc0c8d37b9aa929d0dc65af4f67"sv,
                "16d83f28f335f8d876b2fc85512159147f4cdcbb5c3ace09367d8f1b557bc977cc6cd31db4f93b144302f2712a05fd964f21f5fff11d28b703b9de3a01f87764"sv},
            hmac_test_vector{"1bfa498a685e81f725583a0f4fc5722fde8c01199b23139a3255db6a884286534ea76e95d75f918a262a41864cae1d01f1bb3ff830d3b790a8ea38fdcf6a12a28a7a6079fb8083b69ae4cfa7881883df20d5ff93cea7314424ab519e2a97cea1f6fc88fe7dc83fc4a90f4b3bac0b8e109361a8b31ea569fecf218c1741d2a297"sv, "b04cada1712ceb8b03c37c11034d7f6723c5d185cdfad3d80ae56e37a33a5418863d88046ad72048b6e94aee9fe08deb918a519ad128a26960c431d322c49b8e3fc0ee05ca916a08a1aa84c294ac31ecc93460415ef7c8325112e5da9d9b3d34a67ce88cd7814f338aaf347728d8f3d2916c0762be92cf99a57792365ce6c274"sv, "175e9a5606934304d65f5a2357d074b3511b597afe0167704aa457447a7015a02700f9c00aad116217b27daa9898c6c1e134e7624a7488c3515694b98a2f6ec4"sv},
            hmac_test_vector{
                "2ce3453903e4f074dfe57499a1506187f8287e79849e0e373cdf538e0d3151fdc33ed4b12dafb4b47cfd5861ed84791ba8da283ee75e13565a14048fcbb0aa6dfef09cbddb2f9bccce3817d66f58f5c15eb7900b71e7fe0212be9433e261ac24a3a2a546548c2c259d3bbae26ef25ee3e467bdc96c6157a22a850c609c6dde8b"sv,
                "95b203c6488fc5b5215aa58c6e34148dc277cd1552925b139f14dcbe55060488737e654dad71edd10fc9b069e2b7e7f8d34b391d52423aab391f325aae7d1fbc4aa3fd727b59449f26fab39d91cb4cef818ba0779f4b4ce92080c480a5574ff06048539b79eed307b016369043164a5a1260888a01569ab69e601a99043c9d0c"sv,
                "2302ffdac9cf3b7e284d80fc470ed42cac01d218fa1b851a4dc3edc80c2f3c8f239280da93ebcc249886bfc08c7f0bf22defb7e447dc8bdbb94093cb357cc5ea"sv},
            hmac_test_vector{
                "2bac5a6bd9dc5ee714606e2262bbd3d3ef73c9d578688321676370fa40f2bd673b741be63370c25fbe2bb5579e79486658d3e0eb22aafbfe02fb70a63524f74ccef6eb709f0b4f9b5c591095fe0889d766814c4343c82013350b4610337b01042a5a5571e6550e83361504444b119e0f62a69547a369fa4848bc7b9e019fe276"sv, "f84d1361c51c3e50e245848383fcd37b2f7b0fd916010a7fa41bb1f256228302cd0548ae2148ff42774d18c2d6d3e38b36bc4938da13bac3e04d66ec17cfb0df10b1df1178c2176ba4cc89aa6e19e606403519116ef635e3c9baac7471f0c349eada42537290701492493e03f4d6c332746fe82e79b2652686e9ec500c8ca389"sv, "5f3422f586e6dbae45ddc87f8d04b0c88152f90fabfc6dfbb0ccd7edae37de528460a7bf16c4c0521355d5f28c88ffeb55986f8b919feb756693b076c1690d5b"sv},
            hmac_test_vector{
                "1283748593d539417ff5701cac703c3dfcae39608382bd14ec005e26188fb45d093f6067ff5c4c14e04335c2dd74671953e9c8f8efa618ae1692776e848528fd33a294ae7ee792908602e5e6d56606d7419f256713e26aa669e98027d9fe54b457551a40599e921d39db8970da6fa2e18e785697375f3a63adae803b6021c1eb"sv,
                "befb1ce10b50a8dd71468834cc5c1174bcc1885f4a67e49ece59d6b3104e0730ad7ea126bcb410e1b2a50ad28380cccd0ac6a775ab5cbcf437df04ef0f3793b88d6f1dc69fc3b963b5fbe5a5def8ca9f2d8dc2d8629018fdc6300fd25788256e257598a8fca52acd43f1219424ed9353eebde072b72a802045f5ff462f6a45b9"sv,
                "6d4aa62658419fc842553c70118d90da3cb2a37539dae4b086b4a7f0303dd9c5f5a82d1a7700fbc1d5309a7f668bda06ac53f6f77ae26878bb9225651b1523a2"sv},
            hmac_test_vector{
                "41da7852d48d59fc2ad0fffb9d64b9ae213f3266d5d4a2c7d89445725eb50de1033294915f135467daee1a4dc1aeb81c9a93fabeb57adc94045ffa152c2048b8dc8303145e7be8466ca7a394441c8a50ff648bfe1ec663b966c811d2eef38bbb76d6af23fa4e7c63870482194e369712f7bbcc6e3741a0fea5da73f1c9b73db0"sv, "21251ddba377e48fa35ad148389c486a84e623f3dc49f9af281aa0af8d00f0f74a4bca77e087593d765198e87b6a15608d0af4c49a7736a445d53c718b408631a618c177319c01938ebb4b06852656392daa926e10bd6af68a57c6a47203d583fc3509c4dede63fab23ef08a9cf9945c2c6e06d786441c0dc04549328b706453"sv, "41a4c83777750d4fea58645cea32ff4456c44953b466a5980d2b8da234603547a6259188f8693649dcef85e85110026c006ab968297567ce823965a1df6701fe"sv},
            hmac_test_vector{
                "f91bc92f97c28b011e7bb1dce84cf9154942094ab908b49635b87906e2f14c51f42a9ac3ce46877b6a687ad6fc08db2bd2471bb97f7ff5dd381ff4897eb636f1fe4d6f87b5fa302a57b26a9af25f2e30ce32b6cc993ba90ec0379bc920a9d3b4de2c526393071176ad0289111278788c06aee36b4e63579095a875af10f2ae03"sv,
                "5d307655cf8f7c1b3b573bd75e2374942ee3e56b6b2578ec7793bbc067bc908d5a17261a094427b4a09633d0cdcf8ef1162a15cc6f9f77aa0c62a10f74ad7a99d7bfd12aa125934a4f3842c681e7a29d51b6b61de407ea4a3e98927f5b4e93587b3160cebdb729a4ef454f03a5f31a618890aea7f1e63b92b73e755945274491"sv,
                "9f5c4fd863f070b85d29b933b1379e7023335b74aac37186315e959473bf2b3c0f1f893e1feace27dffe35be6c607a22b02d695e41948b3b6b2bbf58ae7ae84b"sv},
            hmac_test_vector{
                "a32d2fba17f4b0f08a9ee7edeab34b1b8f7b12ad6e65fe248fa97a18c12de5358ca62e8467ffa1eb2bfe00f8c825d6da36d608b0afbbad071c651f9dcd33526b6c7665e334d277775ee8bc5adc31d08a4a2d4b0e22be957c2fcefcbf443803172de6fd61e637db990ba3439d90a1e234b2c8024e2d355f8eb02e36058e04a041"sv,
                "ffe01cbd0ef36a85e32adf18931c4761709fb382228b27bbf9c1938d816c041f57871ce03ca0c06e68db10b720399c5e8b1ad460c201c1f72698f3bef6f4bb1621199ac958c1f8ee6859190dc74d4e836c856827e430722da3c0a04b9835821b049f7dc18bde7ccd8cade363aedfc599bbe75620b29ea3271741807c8eb9c2f4"sv,
                "c1437541647fb134322fe4809de516ff2a9982c16132077dac91e0e06f14cfa943fc8539a22c91faa3fa8fe623dafc954ebee5c17136281e7f8a3038f58bd80a"sv},
            hmac_test_vector{"0f280564119a83a8482f57b7c20b247171a985d8dcc55b17157966c4eba613626095952a5ede370ba589f1ef08743940d9f41baaa2bf8c23150afc2946ee2a4b18103cebf5810f42c3e3cca513cebc069b725dbde67db5894a3fe6d11b0b03301ee12231404bb25788850f614be054cb9f68719811c57d4f9b5f4d44d0c64518"sv,
                             "d65a384d328a1c8908a53151d8fb1e029c6fc44958c2728bf314588445a73f2e71e777e475a710c7ffae4d61837255888a232c854debe27682750af176ac6eea5cc501d7e47f151110a9ce7e44e5d76d9cad53c1819317527fcd169051f01c6a3efcc06ea9999431e3a09ef143dd0c79791423451f4179e7912464a9fffdb274"sv,
                             "9645b0d953f9f91de98ca15845b7edc24434d3a247c1eccc99b71e9a3c3ffc79e94ac59ac7bb6ebfd10cc7645dd9c8449ce36bd1b4d1eae96de857cb04a76c0e"sv},
            hmac_test_vector{
                "b9c8680ebb44ac60adfb20716c23b7bd9ba54908f51e888de129355847e094f1a3a01d3a580d749a46569b5b9ebb6751f54c30bd98f3cd7020b4bf344634ad67f87811e9acf03039f4b44fda520d24fc4e378b58c7657a5c870637881a47c818dfd9ace35ff4c883de9ca4b63023d704ceefc0a2297d77973ea6031d6b21ce4a"sv,
                "69a9f4e2dc0ec5a720cb369e9a7ef804a4eb5254dffc1567ca06d2e0944e4ac72fcc2674a62fe9afb021221585cbe6bef09c7d1ca6465c26d60a53b6013608300ceca4659424ccb781f4d37dde102ea9e88d28a864ead78936504e62301914ef2890d57d4df75806bbcfb19e4c53b80db146b9bec2ee6dd8136129aac8ff564c"sv,
                "73eefeacc31d31fa658517504322a759664bc1a94c3f31ffcf333f678d236c743066f05f92c99b30141a13dd65d0fc9881145af6acc9bbc446e0194d68b64977"sv},
            hmac_test_vector{
                "61d91f317a902ea0944e11e92e6657a589e17abc027fcd869ff8b030e8870662f8a9e91ed3239cecfa42c0343d66cbebd1c2b771a25df7baea5cafad038424c97afb720e644e7d1bf5b829944ea2cec69766e68e4e580976de071c2274c0c5eb0e5421c9d51bba76ac39b3d009204680035771d9ad79eb02a3805d58e243cf0e"sv,
                "95af10920dc788269e70b8560b73135cf7f6f5b04a502c7bd61cb74f3b8ccd160701224922d865636a860d949ae755b970d3858c0ff37418a2d24b7142378ba11ab352e5c876da1a076642728b73916b2d24f8024876572363e7036510cec7f413ed28cec749ed33be3adf56a8bece597612d478bf84de85628367946df887f73dd92d6de7faa896d7276d"sv,
                "6e989ec9cbf010ad6691a672ff4ca90a00275f9ba4c81cd147cc506e1dbc8bc93b1d96a375e493503c0ac697f7c45e4fadf138242df7e06e677de245afa97780"sv},
            hmac_test_vector{
                "df24279bf8277ad1091972b82594d84677e54fe5d65786d19ab5b2c1ae0a3cc9e7abb67f9477145d575e196633200f0ce557bb5278b8902e1496233117a7df69660bfa87068aa73de61e8eeaffb179799f275086029f47c323f6569bd18dea15054ddafa73e89c3a5f61b98cb2ce7e554d5df4cb9d95135a70de33470744c393"sv,
                "27e6c9f270b9855c9658ad0e3d6c9a111a624f66fa64a49a0688a49b454733ca6230f451b0dd69b76b275cb241967e3c101b4fe8f2023d77772210a63157854b763239a061eec9df1aa6380f57c6911d23c0cd2edf00f63486218dbf35612a17ea5262878bd3edfb2b3f08ce8ae419dddab792e0c94517fabbede38e574d685546fa35ad37741d34275996"sv,
                "e6f6061275a89345f5463cfa198d528e14047d478f69ad7a73432f18f88bc68a1b8aba2c3b025c93b25deb8f403763a55024408a97a903e95f0cb6178e7be389"sv},
            hmac_test_vector{
                "b9a8865c3a6ba8f2c13f35730b39fc3c92405c06bb6e116851b84d9d32d20a88484d9ff5bb03922265b4ae7e87f155b0ba3917db28638321fb3b3c661670505603aec6a92d0776a550971ee52d68b15a8293f28f39a84231e050b6ce59316dddd31221fff940fb846830ac316765b940d29416a95807f7a0e73fe35f63dd0a25"sv,
                "59aa9d7f583f6ab90b472935aa6dea95e2fbe402cbf70c6e1992e61c96c49b63d0304daf0e4da7c889c7b857d92301a6aea3cca7c069c03809deb3a53155bf6e4aec984bbdb31c6e84112c089a63bb0eb0e5243d22d6c15c29d7b9c1529519162fa7275d4cbaf33264eb2e50d5743f57528b94cdd8873662e345a178e1cad2e2e729a1eca3a7519c921e66"sv,
                "d78285ab97dffed5f16d00a7f277eee9e9bbbc5eb14fab10c189739965fb3dd1c196fbe3b01363260bf688955278884812286dcf81c25f1eb17cd2503ffc7acc"sv},
            hmac_test_vector{
                "b96ac1ed835f1e58f5327338fd604fabd399bb65e6d9cdf716d57a512398ac86656e94d0be0142d4cec27327658aaa103e818290ea40429f0a32b2b9c1402d9969adf845853ed24af79fcc4974025bb23409acd0eee6370603c19758fa59992c2cef9352c060f743da9127a6100f8b191ef7e22dda14d5b26a48d236b42d17c8"sv,
                "51f9ab865146187fe650e49d45421fff28c45c3bd8c465458b762d93f199067e0afd14ec3a42022c9fe2f321a272bca3cc245022dc917b8c16b5d471dd3bed6684fbcca762c29f002451abed67a8860087848683b126795f4056963c46a8b4ea68febafff04e57c21da5f348bd6ce5ddfeebe6a6820bc584b1605d3a5366c5d35e0bc63e0c6e923c31e7b2"sv,
                "d6aa880ea45e0ab267e47374598086784a7db2169043073614c4a1917e90e8dd2818f63bf8a899d0bc615e60abc57d45e20638dec691b4750a36079a330e5270"sv},
            hmac_test_vector{
                "96a83010f9d4f0405826e8cc941190e07c33d933362db680e1b1db3a78ddc47cf9c7fa3fc75992cf4d5ff680e5dee89cf8a7c3a1662d04a928940a2a340a1adfa05aa6060bbecfcc39f52806fd96bd215ca4545cef62f2348969a1201af7717fd38abdcf8baeaf1f621306c7a4e21756f05112cc9976870a4c582986f34cd143"sv,
                "9cb3288f3b04f0442f2ca8cdc41b0f39ddf93f898868e312d509a422e941e4e64e3daf8b8d33eea4ffebdecce07fc18ca55fd47f8f9780b38155d4530fa53383d7804a8c14054539700643811ac607651901f01ce02d2b6e2200275ee732490be084e2d8f7015ecb2b84be339b7a488ee0e97ca9a0b24b096013848a9a6f8f4610525db85fb09f22d76d36"sv,
                "cb700e68aec9448b67ab8e15d491aa070242b4430d6c70db6b736ad66756065f417c0b201395e203c57bd7809272ce34d4dfe1972b7c5277a28d71c7f52c32fd"sv},
            hmac_test_vector{
                "ff24c8943c8e6d3db40c7ac16776f756c44803ee07a3c95bc594afb7c599d0031261ad0e4ed41f98495391b8d3416b7bcec2d1ce87c28e9e463a4b3d23ae05081cfbbe47654f7254ff794c008c631a3262dcffd1de9b67e4fa8140f8221f68c24478610627084cb8fd0515603be4ebc3a81ff3bfb4363d770cf4f7b06bf3e07b"sv,
                "cf44b9d057cc04899fdc5a32e48c043fd99862e3f761dc3115351c8138d07a15ac23b8fc5454f0373e05ca1b7ad9f2f62d34caf5e1435c451f3d927940e8a92c805ee3e754117c45fe0de0545e7d1b3f0b71912aa2deef5e5ee661a6e95a06f8727ea158000c91fa067b03a7378066619bd61f4ae33b7ec2fabbf1d0dc3078c3ab0a5919004c159f7fd79a"sv,
                "2c537d0edfa86126672ca6f0081e8c13fd161510d56c0bb6ef015ec35cdcbadf4fe68594fe70820ebfd99cfb83a1fd18febfde2743bf408479a52c3334e39141"sv},
            hmac_test_vector{
                "0f565c68deda3bc803d93246a1fc18ede3cf16d1e217adfcc965faab37eb39bbe48f895e883eee12f8839cd492587390dc3cc6dd688560e7fbf8c9aced97c56cd3ba1e5a5c61a39879c97162c13d718a132f22247d8799825c3bc663c520f8672400a3c623ec6242ced3385af4541bc1d0d1b30ee8c55fb536577936862fdb94"sv, "0f55624e40771d01c2643c2bef1c97d5fd0eaa1ede76953064e96874a92e9e02ae50e75c42f12b5b26e1cb696ef02af12a006c14465e7d9eaf525538b7f47bdfbb42c89403706e55e97f394d3e111448e97cce69d11d1e1ffeefe555fb5bb4e97e528e604a9aefd855650c3d26285dc082aa5985475c819c98e89f333a0c500a3ea9c027e117b5cab0bccf"sv, "db88ffd6256ef15c09d67244d84d4ba61730ee6eaa565cfcc4fe587e6a950cbf69a7ca19ef489b68f8dec772550795198462e87ae414ad604591d765b6a2ad0a"sv},
            hmac_test_vector{
                "f4d6aedd9a34e0a1822362714d4e81794b53b266417678c16a97887bbb612cc96bc5e532b3a654e5d3d65a5155427ff09569906381138cc49e3fc2384c5d33c34abd3d617c487b52ec6ee7b5105f41584b7eb5cfb512b8c31f3f338d5236e30398a8ff927e801c8ed7d14fc5040d915a737967d166ddc266f68023a357530431"sv,
                "a50b14fc2b1852542a497dddb86709c49b3285f26af9d93fef69cce2d0a3c92c6e91e2770e79155937d2ce1d5a57ae73b95b8b66815cb88f39da868860690aa4520621dd6ca7b20e701633632bfe6f0d5546863ab89f354495595728437bac3b1912da188ec9b1dd9fbab3977184d4fa389e7e5657ebd8c6c98e48abbfff37588a5e140fbc089b2d8a6957"sv,
                "0d99228df5e7ec538be55d8852a0c4ad0ca61befb94f988d2d3cb68c006c0fed69b867e1a4f2a50348890fc1ec82c46ba72fb8d585376037f94c9fd18b67a839"sv},
            hmac_test_vector{
                "acf723e38ad26db1560747fa39674eb6d8546af98625a677b7cc3f47b8b5fae79ef2bb817d96546cda5e9bf66297bf61dc3bdc2b5c5ecc93b9c8415842e410c4add9d0e950f6a42b945355fea6b5a4f16fb3deecc717b0b5e5873db91a656e0ac0f389a46dbb06f46c2e9e9b6d8ba46f7b0298c22f1afae823505cf3aa00bbc1"sv, "bd619ca4cf382df22b99f1310a6498633bbf0100220a578e011681727691b90645c57aeb5883a0a10459cb548e0b04f9ee41a39ad27e6feee651082d53cf82228540538232196f7c982bb19106197da69d0fa45858b1878f4a52805627677dea063bee1065e4e2ac6e756c9079754c92d190cbe1bb8de799b480b094fb41f2844f1c143efaca40590ea0c5"sv, "3c13c0897926d1d45c67f68a4e1c1bdbee8f601affa0edf8ebc6b567d920962fb9f14a4fc92276ee1a266258ea7f71e09241225053edbeb7f31130b36022dc13"sv},
            hmac_test_vector{
                "204058177a61aa45bd666bd0f3870645fd6b9330cb91a89ad3f072cfbbda04655926ee79de8e123c4e56b3af342fcfaa935aaf231f10bcadad22943fe9dd5d3747305a9e3c11bbaead1cba91a87de36c5211b39c20c865404a4e09f77dead6ebd1a6c10efab04fa3c200a701b73beb9320d7c82436814c5991bbf87fe1ca1787"sv,
                "107ed9ca1f16fbdcbdfa5211b1a9ec8d9e03bbcb1f0a468715e99fdb403695a80d742a37afdc4820715daf6b4be132f6b3d22316b5dc0c8146f59f6cd69bf0f0245de2b2569ddfc34994cdc526973d503698e594f7ef503f6f5bd4a1c043c50dfd42e8833ce32a8deca9926769537562ce5de98a0bca7b87372125c127b67ac83f2a24f28835904603a3e8"sv,
                "1bc503398a25223fb9e93ea65228d2727b201f0a3fb544a300dc804340b6ab3676862b6320bce35adabef387ec628da38277f7de6929c44810c47f2a7acbeb03"sv},
            hmac_test_vector{
                "20d04970ba4d2cfcb4c4e1cfa3fc6406011e8a31e869e4ac1b28e62d46e1489987d0ad1c2c938378bf272a9d4013587bb1e3bc14d8d8e1d540de562681be0c40c195ff3e7b8518777faa520c3a501dbd38630001e72681559e2849d4dd758408d70987002d8ab5a2fd36a3431b1a7a759e849d209fbc8cd53def095ee46c799a"sv, "8aa3ae798494805441b45a59bce230f9b2bbb960b15f2456727807636879983799241f48ad4fabbd44e7048b8d35e2de15605cbadc34a3e2ae21f0f9b087bbe73a312e7ee41fd95ee488a715971f118b5d96919eaa605a095abc468a45ef104414d0c362538a72fdc79e2a3f012725c933624053436daf921e4ff05f4f39fd15d00cc63b1f1ade63953175"sv, "45560765e5e4644476206c7af9a2744de1768b44692a1e38567390eab59b951e6c311576b8c33b8e8769149c9bed6b5e1698ee209a8b464e032f4bdef057065c"sv},
            hmac_test_vector{
                "6e3662888bd3f3d6d980684854d93883dcd2da3637a8e6aa5618779bd9ced347d5204ec4d4fc6b961d1f458136882d9ca82d95a702ec2d9e20c44b8ead4590a8e745c994a2d5130890744398bf8284d063f74280d6544757ddda24b32dfad3b82e0e9aa0fe463251b9e52935f8ad85469797aa68bcc87efc14c1cab260d6e49e"sv,
                "be0a902d7d0a1a31c76982a5a4612e27ce13c887656f94cae453017554f77e08bf888ec8e8131fc139a5180cfdb1314ecfb10781521070f723480b9b0c481c7b7b18978332bf7b8b3f6561952fb5554f7f85e4e053b976e06a64dfa8523130cdd802d3e7c3d6d797c2f088c8a2364334c4d7882acf30518fa2f1a2248d7b6206c08d697b0320daeba88fb3"sv,
                "4a87dba3c394b7f9c0d55041043e9637bc5713a9664daf4077a37fc55ebfadc5fd04a28a9bd13caeda0a7db4ea7a01eeb1410d35a821b1cf4e8c66475549f43d"sv},
            hmac_test_vector{"97449ca6c272484156c84d60b7afc1cc59546a46cba1df1bf56beaf89eed31003175151cc05ddb92493d09da385f13ad2e73375e0184a66d042be45a880371b7a25ca9812f34e9b01663f30dcd1594441f7d843a2cc88da0b150efc9891304b87463207e18dfbbc345a1d2a27db98abab4da17815454dcdc8442d3edaa05302c"sv, "6dfc8966adf3c592d0d2895f5ba20d3cce6d7eac6ef6e99d47a75bf672cb476f94fd27a5a7e0666efd117e69815a5eda41290eb5294095f911ad1b3ffbfd4b9395adfb8206e10f74e6589a3d8ea047ee9386f8f18dd1ac551d30a6662e70cc817af24d26505d864f959027000adb9120cc6e83872111f65871164591938c91e04bb664b910bc94a5129f6b"sv, "2ffef9fe1a0a7ecf39d730564020b8f0ccadca49e4c1780e193e1901a19787db8aec9d7af3b9b7d5a96c2d948f81d89b0b0aa23e4fa350c3d8983556e1be2f4c"sv},
            hmac_test_vector{
                "b55e84a2c74e837420120c9babb9813dff4d545833587664f4a71334d64e7c1ee8ed655b8b3150b0849d494e4f8ad4f8b66c2463a3588de233c33c430b8d26f3ef3e3e69918febc2a62cb9a93fb84f7a5711a1a7a8d6665ac427cd5c60ac6b35f3d9789580703cf2af47c473234bd90d08c533ecebdce914cf5b2c487db5352a"sv,
                "1f2855e133cefd5e9e295a3c63fcb3f9185b5fdcf56b7ddc2b69e5bf76ed5e7ff67011f13b0c3da89f842d18d88467f7cd2a27e9a9c40b5b2edc10af4d72bd9af3889ecb25e4f4ce4f11ab2fd9a78ce2fcccb4e2d4fb4519b6045b80c3544287bd77e6b71adb8a9657d7bdcb8feb8a824c4af0129d9592dd69eaae78021d530dfd45cffd427b67c95fde4c"sv,
                "61a87e67cdb1cc9b3aedc912b28f086c0a23cb5be98a51d60cdf1ab3e0d288fb443881a10205778c6f599d79dfedd4af27f1417131d9263d995748df45342d2d"sv},
            hmac_test_vector{
                "a781be58853dcea37c1a30ca5db6e16b9ebdaab800ffea2670c695652b667d952615d0b7adfde2614a902db6e81c9796389ff31254dcc923a3ae5a9bfc9ddf5b0eaf12c7ff80ef775395bc2f10ee47121fda6f12f610e5ab74fa34d10c54ff2726f68a71c2ee7d4c5fd2e41d1b5579471b1726a12dc2c627ac522cefa5b4665f"sv, "f37bfaeac36a4ec9d379ac509b49fe50f85a995a89d8c22f59b87178bf0455b78373177e423de3df142d25236bf890be3fcd6583682df2154bfd599eb8da92c313acb3d7b27e6f4e878ddd75ff9e7b61299573251b441d499281c477d87bad76eb4e555ddbd4fb0074be5eaf1de4c82000da4ba96bc44bb2766b3d6be790adba5280867885e88edc23eb30"sv, "66dadec2dd4e47418e8116d5ce4c2a9e85fb82ea0a96257b661d5ee9f4b8568a0021602f698f20b5c3c77d2533e1c6760eca0abaff7f1cb0834cc5064b19b102"sv},
            hmac_test_vector{
                "632afa8e79b14b2a3604f5855d2bf182d3c56d6853f21fe46271da5286065f38b31f751306b63c57b679beb14729c78f0040f7e2a0d615224dc5a693cd0cbec8f87117656d6b6029853ed72b85681a63183c3a6dfccd128afb0dd7e81d36f0231c69070b189560a88c9b697b81b0930701026190cf9ebe23559194d6de4d9a51"sv,
                "01b95a887927ce31b1242391bbd00965eb77a903d4b8399b72e6cebda9ae721beefa779145160b626b110cc554671da0d8dcf993a9ab073888e02fa9b803ed43b3f6a3aa1d20340df6ccceac13cb0797cf612cb8fe5fd513228cbd4de249d16bb77587dde98f71bbba1a124ee046f0d239ccea7abb1accb5aab021b00dca491c623fcb3191a9ecf31fc680b4a41e"sv,
                "210ad45ca2fd1f105c0a18f993774f933ece57ace4da619689e1cb8b491a189cc6e4ee1954a32201072e70f934837c0fb6e239b4fdfbd26ebf11b9a919eafd09"sv},
            hmac_test_vector{"67e704046f98cb5aa97da95b19147391f05788f811366b0ece44b12af2b11e0e05780bbfcbd90a950e0acd8e9d2a44e7957606eedfbff212fa1c163cfbdcd062d2be3259ce65abea6406e4292c64e9022cfe89155986ffc45b96d289919ff98d552243778122f68231d9b6d3cbaaa9093d57d9158674da4c781bacbabce2e2ba"sv, "61096f4fe5340488916de293be38cc3ae0c877670c713637b760d74fc18ac773b2e27d5543cf16aa20dd3d83ecb34edb8545bb6c8a4aaec81bf1f0a4e0cf09774d1ca944242046b33be807677f3de18c39d700af90cd68d34f50dcc1e999fe9fbb20b9c4900fdccb6af607e680c0cb7583e60dd825e2ab81dce7634de3cff0148355757f90841f19366f06a9f623"sv, "9a2d147e50827157f3866e868c1cca9f081579c92f25da8cebc9ed249928c82bead39d480ecbb5b5d0e0755029aebf3e0206984f3ea83f4d6372f4453390e070"sv},
            hmac_test_vector{
                "b53127b89772ea1ca6dd27277da80ed972e82f1232a73d4ba537118418c5f17d9a311329a61e5d6003456fd4e90ee3466561d3fafeb99c68997be2349a87d5604c0cb2c183a08caf80904c011474f73909072ffbbd36fdc41077cdd8805cba7c93680c667621ff72e366c7964703d01825834afeb546e5c7d2d3d958136e2a39"sv,
                "c5c06993d43f27e86bff96ca7511176974bb63e618bfc4b610e0854820a3a6e77453d5e134479ae95868d2babeed5efd79691c6d6d0816391915faa9b3c0cb057a1fd5b34872e69f66abbbe0a52eb998aad5de1b8a37f654972a12657986368e802c5250384773d23ed23b83535b8f01af068f3a97d4cbd13225b3c3997c504a2d8332012d4faa4988e439eceffc"sv,
                "e3a4d32f262c6cb0e99195c7439ad2731185c58811f40ecd32af214a21c20869aef5297cd951fec2a145e15f982266c46f7a60c9fb0bd0c6b16f5ee40fb44708"sv},
            hmac_test_vector{
                "c35b26bd02499cdb6b06bfa4b18979f0a472ba7c559dbd277bf78c611590c6e051f2a094adb22ade5c44d4fdeb1330c924d1f9a3330c55ac07035735fbb7c877b64527844f72ee7eb58817074f61dff8dfc1ca56ace9e782e06855af2f350699b9fbc37532b47023407992ab24980ee79de8337d0959fb11cecf8eb8f83108af"sv, "5860501208a4c922ad7550dbd931a19ac1434750e63d5f34f528a0b5eb1798b37c0338eeb6d293bfe2b9e306abc4cd6382b3e6a94008758f0d5e7ba981fcf0970aaa507d8ec456b3518c07bd18c4f37d8f7db8a7e82ac776c5f86b9d58620781c8ff9fa5d79f9965c397c5e869599c50b048c53325cade4fe39e7879b67063d780aa2d4fdb8ee53fff82246fe7aa"sv, "8c385547a8eba518e777c3593c5b7ce0bd7c859af6d67b6238d20a58b8d0d74d80d18ab358ef1c1218b928a026ae8c4e3b73bb5bc0914de905d499c75e6f3d9f"sv},
            hmac_test_vector{
                "c24fab7f7998c69063c2d1103e60a6c4cb03206add01d09faf75f1007a879e9047ee435a02b35257d1373791a4778d890c8f92d6507dd810be283eec3fa11fa82ea8c9aa6a723164aaa9e57a11b54127033ae6dd36e1682b0c5c47e420a4217e1e8525b8d95dcb7f9721c213afa02a66570c04c5b7b6e7b94219f430451a0cd8"sv,
                "fe4c83e8496a69b7a251228396a5d2b4849edcee0ab1f8dabd6d872a1da324d7c8c97cadeda05f0a041517e3bc65f807358538a870c1011704a3c5cf1216d2b57acb269e4fdc841289b0c750fa1e779184d59a9188fbcc4ca11492059326ec8d7e1a29c25866ff5699e9dbd2381676dad755a9b23ba68201fe8897d588199ae83b7e2e22ee85f95e9d89fb715e97"sv,
                "d725750a042de65607af5ef523e3c86d08be52427b7036ad514b9596c901e96d76b5e58f68907044282e695b3b875c09ad49ecd9950fe312a59dad691471c572"sv},
            hmac_test_vector{
                "5c74212dfc2a80d30c39d680327d2488838c35d6503c1a8c4366d7eaabe95c7115f1b7481c7987de820eb4d17fd65d0f58d2123b346044522c04f98ea167c48ad2a0f5a8adb30db0e65775b947fd6f4d470d4cc8dd73e001965a332ce63779ffbb0a441458e1f98f619d800032f8408b75c74b46f47dd5e2abf1eb8e22616218"sv, "41f4749cded6e44c11b8118c38cd71cb95a26f9eff01bbbdd716e44e3ed02867858a8bcee5eb2603710ac28048d6a53f0fb6ac7d9f6c9abefa3fb01184597e95706ef83c789ecccfd19df3325e1186ea243bd4dcfedab157914c115583f7d5fee8e7e46efdb87eb819b7cd2be044bdd4ba7b0e438413a89285852ea4a371d5abd63e77edde02e3c731a178f23838"sv, "7e983cd601ff5837e7d170f3092e914e076c21b31761eb7b9ec211e3506758d8d1395ea914c0350afdd6827c0283ea4af188cf30c1fdf075e41363fbdbb29eed"sv},
            hmac_test_vector{
                "99841c3e4a41b53c30267dc056e7e9b8f9994494dfbba363ea761c38ec2433d3bd10957d8b7c093472e9a3084c923ac5cb3a1dd2c5270259ce6f3fa80c723dd847a829ac409decbb44395ed20045b694972b4663f2fd658458b9ac7d3ecc65c260d4409110aa481bcea016e41a07446c86f5250f0f45b32aaddec97f293993db"sv,
                "fe27bbc87755aacc37f667f8ca37f8888fc9dc530fe4f8f38e8cd426e01307747edff012d96da707ee96338d1b11feba313a865fca115431dd8632268ff499224ceb69d31732dcd91e0cbd2b92bbd5b6b543a74735705daab81a0114b8a8f0be91d38cd3d8ad328cefe16c99d63c67c4446ca7d1f708f9a848d1a9b60238f6907420c3d9c5e48f67889ca7a1909c"sv,
                "562b5ef3d5cec882a2f54f8169612dba2b033325ce5ed924024e7806c745de9e7612dbcfcbb95ea3fdb93de9c6460a866bd412b45eaba5139939fe43d20f9315"sv},
            hmac_test_vector{
                "3ac2ffbc5b6b2334809232c0f8151ed379a8634d70d3f5a1963a7637c421ad0f082f34a8f872702046a4c69c95ad0cea8b683e6528aa731956810f28c1b9396de8a5905e751c1937c9c17c55dc8771df447575ac93a7c161e6967cdabb9930cc03ab7ba8796e07c23170bbb274ad33facb566eea5ad1c7c16f0127155bc77875"sv, "29f8eb9fc8ab58fef681f9faaf934e992d42046f0ccd2fc9ab23d42bd5f5aafda110218196eaa408137a1b66ee4db5a35cd7e4f31107a9e8a81e11e744c000d9784b2d2264696ed721e1362b60b35b2b4d631dacba95658179da4af109cad9687653166c7a503ed3e85d4f334aeaca9bc98fb8804e9febfae70086316c3ac01162cde4461fc89c642f977065f71d"sv, "38ca18d60f180fd2a40e342272190d9b84ba37bbccf59b29bcbdb08762a90e1f8b28349ca634a6f955cc08c96835ee70a2267444fde88b45b8e313b0daf6e12f"sv},
            hmac_test_vector{
                "e4b38e556aa285688979a55eeacd7d953f1ee0ab8109444c7cc068488eb83ae9aca1f783a59b944caba75d6e0f5bdc5b4cdbfc6147046e7ed5ea4c757e85fc2181a7580a17310b36fc873e422c4175b1ea24b3830750e50961ba7df9aadd5ebe6badf81148cdb4cd850192ffc9e6103d22e14f3a4a557197291945fb9a292665"sv,
                "345479ae901adbac7223f5f9edc419bb64665cba4e3684b7371e28ff07f3124087f0e89a21630cf9e8a6c0a3d8518e0d5eaee7f31b6d0aa7e59927aa0ecbc479e99e61a98b625736cf1506199d8f2f186bfc9fe2038f0e5b87754635b30888c063462b035581860b2f571083c4e5c6859338cdb09004597b2899cdc87f1224bdfcd08fcf07275f1f1156260ad5bd"sv,
                "24d2dd3d082e6556dbe27381640837a23e5d4a4d6822066cd09217a677068e5b8901c1eca7da77a9595be271abfa76f9d40656cfbae050ff6d8ddedb0f4c82ed"sv},
            hmac_test_vector{
                "acf624e86580af11d0d23c19df6969fe2ec2cdc737bfd00bc54dc0b2ab4421ffb58f44cfdf8c1b1bc5b54bc45b818390de850c6f0adfa2048ed48360bdb8c511860eec5ba6f1bcc51cb34cd8ddc35c23cad4e882df3bfea0ad99ccbb0abbfda707be461622773b16bd1268dbcff89dbfdaf789871d9d8ae80ae4c44afa1571cb"sv, "2aa1d94ec83ce7c3c75c6bc847759b085234fd44b407d8f80ddfe93c243556e87e4be8fb30b4743ef1169a24732fb2f5f416042b10c3371dd9d20dda29844d58370700ce69f7df5e69240df77b96027a0ecec71b904f690b875da854de05ef047c5d898d1c0d116c580e2a0906b271dec8e5b0dcdfb2550a40092270eabf253376d6eb01f0fff1afe55d5b21bd8c"sv, "b6e82d35182ec417bb33d9230a55690f8720d32191cb5cd46bfd591421911727a0f8ff64ba6e16f25aa10669a85bf2ba74d84a754ed947335b7a17af0297accc"sv},
            hmac_test_vector{
                "f3ac4422cc724378100d7515ddfbf3fe340002b7976c43acd69c2acf26c3b18173eb4eb6f73622540c6a73dd3eac5c4ea58cc34772428c6bc7370c0accc8c1feff4640d2cb416e2a5d06f35eb366ec69f5b9e0020923f6086216652318182ba93ec702be701a90c0abe9dee261b00b16cd9042318596e9494e401b62333d594a"sv,
                "cea946542b91ca50e2afecba73cf546ce1383d82668ecb6265f79ffaa07daa49abb43e21a19c6b2b15c8882b4bc01085a8a5b00168139dcb8f4b2bbe22929ce196d43532898d98a3b0ea4d63112ba25e724bb50711e3cf55954cf30b4503b73d785253104c2df8c19b5b63e92bd6b1ff2573751ec9c508085f3f206c719aa4643776bf425344348cbf63f1450389"sv,
                "d336f2002c558eb518c773608387bd500704156043b76104eca2309afa67d69ad9b00e6b83417e088d3f93435922d4e8242e9631f962cd9fc258f3505305d636"sv},
            hmac_test_vector{"50ec304fa342839457d7eb28791b671ba5c425f711c3a351cc76149d481f0547179540fff239f054ff2c078454bfdd92b72b199aa783d562a1e6fd319cf9f8e4d6948b3ed2bcfa80a1d270396209a060051eada0544347f3335c1872266d5e6c1553d9b54cb3e740c631eef0abe2faac1703a7b21deb422d0c3e2b09f0647d06"sv, "ef71b7b3ca0f904dc50447ae548096b2b3603b312a5e59d490851b270ee99aef259401bdf2c3efc3b1531ce78176401666aa30db94ec4a30eb281494bef5205dd87f3350c1c4a56f3d040b12167214391b30b121697a7915e9224b871a3c355f111a9493be7b7df870ff5c589bdedbc4dada062b3072ac2c93590829ab26a09dd74d6eaf714e3e07532c57e09921"sv, "39d94c4e1c8456bed8637e592e4231854df3a6ffce98463e4a85c477d9fd34d27035cdfccfcfd385d91e4e38e8c75d9ff941de80742e985baa9c94dbec5a6837"sv},
            hmac_test_vector{
                "082e7b4604dbd3608df7932475e4279bb288688ef998cceb8e16d9695a18e06f3ecce733a7b9e71f62473878b2824941a01b945d93afd1f5204c6a19233230aa0fd64c77822d78a61d266f569279a182fe9f2c287a2108abec16817724e7ebe32456915bbebfeebe659d20053d4f9926741d1837d576d7d79a7b06ca82c279e3"sv,
                "e5606f31ca4d0f5d62730f443f6db0edd8224f1881eaf27f9af3215d06e2f72ddfbd78b467082541422ece34e323a8bd45489fe6db8fedd4c9dfec4954ba286e971db9d078a7d0a8dbfe8f5f166f1e51a4d4fbd21dbb916e65c40d75244b6db87747d98de672371995abfacebe983a325e8f0ae22fb706d7d76a2be95fdeec91e60581f397b1831cd8fcb688c4e7"sv,
                "d675982ccc457324e24a8ac6db3710b38e5f18c5057730cb7ea2a37b4ba44c41dde0874e43836cc95e97ff0b3ac10410497f9664177b0e576be8c508ab1c7857"sv},
            hmac_test_vector{
                "035f55033df01f670015a828eff154a245e8ca7474b0b3330cabbe5fdd74e89560b8fa075347532aa46ae7ae907888b30ca4653a6419d0d9224944b43181a6a842c1cbc96fcc3b0f1e7b344c2956f2613c652eb27e44e5d773765a9521fb5e0c7125cf31d9a75f7f38ef96ea01b61b159cd52fc4095a7a94c7db0aeaf40a9929"sv, "8a0349d4d1ed8c4af533e9e83468b5859bb68237798038171346684499c9dc2b5970730533eb2ca04d1680630820f58d32ecf0bd7db7cab72ffc27651c94831cd1220e2113aeba6c889092abb3904d8a264b2332f2d9df0f63ac36d7eabb57c85be0c331587f5f330d69c7c91f00e606de9bc49ec22c9ea815203ca2ed867fb65d743a3beca6427f4669c9c432b7"sv, "3780ef695742f09a160c8dd7d35e2758b08284e8150934d222db31df2767d40d7c815c526ecee5f787030c8dc5f050c419ec6ea7563650dcce1480892d3088e6"sv},
            hmac_test_vector{
                "d63b50b54e1536e35d5f3c6e29f1e49a78ca43fa22b31232c71f0300bd56517e4cd29ba11ee9f206f1ad31ee8f118c87004d6c6dfe837b70a9a2fa987c8b5b6680720c5dbf8791c1fcd6d59fa16cc20df9bc0fb39f41598a376476e45b9f06add8e34af01b373a9ce6a3d189484cacb6cbe0d3d5ef34d709d72c1dee43dc79da"sv,
                "f78343071f61ee7d9f791bd53132e6d557928bcfe4b214bebf6f3592e46374c7ab148c3c4d6a1443a4675cf4321298c865b440631947b6b05f2c2a337d1cbb9b3661de974b4604eb41cc77c3659e85470e47e16f22a34619db935d59cbf5e1101ed401c020db069eff1035e9d1bff77bd8b3379e05ac0c20bc0e98aad7d7304dedd3bc5ed4136184649b5e0f7e5b"sv,
                "086f674d778db491e73b6fbc5126233c6b6e1f066963356d49ea386d9c0868ad25bf6edad0371cde87cea94a18c6dba47535dfce2e40d2246ab17980495d656c"sv},
        };

    for (auto &&vec : test_vectors) {
        INFO(vec.message);
        INFO(vec.key);
        INFO(vec.digest);

        std::vector<std::uint8_t> message_bytes;
        auto r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.message, std::back_inserter(message_bytes));
        REQUIRE(r);

        std::vector<std::uint8_t> key_bytes;
        r = ivy::bintext_decode<ivy::hexchars_lc>(
            vec.key, std::back_inserter(key_bytes));
        REQUIRE(r);

        std::array<std::byte, ivy::digest_length_v<ivy::hash_algorithm::sha512>>
            digest_bytes{};

        auto nbytes = ivy::hash_data_hmac(ivy::hash_algorithm::sha512,
                                          message_bytes,
                                          key_bytes,
                                          digest_bytes);
        if (!nbytes) {
            INFO(nbytes.error().message());
            REQUIRE(nbytes);
        }

        auto digest =
            ivy::bintext_encode_to_string<ivy::hexchars_lc>(digest_bytes);
        REQUIRE(digest == vec.digest);
    }
}
