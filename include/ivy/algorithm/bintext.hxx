/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_ALGORITHM_BINTEXT_HXX_INCLUDED
#define IVY_ALGORITHM_BINTEXT_HXX_INCLUDED

#include <algorithm>
#include <iterator>
#include <ranges>
#include <system_error>
#include <span>

#include <ivy/expected.hxx>

namespace ivy {

    struct base64 {
        static constexpr unsigned plain_bytes = 3;
        static constexpr unsigned encoded_bytes = 4;
        static constexpr unsigned bits_per_char = 6;

        static constexpr char table[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        static constexpr char pad = '=';

        static auto char_to_value(std::uint32_t c) -> int
        {
            if (c >= 'A' && c <= 'Z')
                return static_cast<int>(c) - 'A';
            if (c >= 'a' && c <= 'z')
                return (static_cast<int>(c) - 'a') + 26;
            if (c >= '0' && c <= '9')
                return (static_cast<int>(c) - '0') + 52;
            if (c == '+')
                return 62;
            if (c == '/')
                return 63;
            return -1;
        }
    };

    struct base32 {
        static constexpr unsigned plain_bytes = 5;
        static constexpr unsigned encoded_bytes = 8;
        static constexpr unsigned bits_per_char = 5;

        static constexpr char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
        static constexpr char pad = '=';

        static auto char_to_value(std::uint32_t c) -> int
        {
            if (c >= 'A' && c <= 'Z')
                return static_cast<int>(c) - 'A';

            if (c >= '2' && c <= '7')
                return (static_cast<int>(c) - '2') + 26;

            return -1;
        }
    };

    struct hexchars_uc {
        static constexpr unsigned plain_bytes = 1;
        static constexpr unsigned encoded_bytes = 2;
        static constexpr unsigned bits_per_char = 4;

        static constexpr char table[] = "0123456789ABCDEF";
        static constexpr char pad = '=';

        static auto char_to_value(std::uint32_t c) -> int
        {
            if (c >= '0' && c <= '9')
                return (static_cast<int>(c) - '0');

            if (c >= 'A' && c <= 'F')
                return (static_cast<int>(c) - 'A') + 10;

            return -1;
        }
    };

    struct hexchars_lc {
        static constexpr unsigned plain_bytes = 1;
        static constexpr unsigned encoded_bytes = 2;
        static constexpr unsigned bits_per_char = 4;

        static constexpr char table[] = "0123456789abcdef";
        static constexpr char pad = '=';

        static auto char_to_value(std::uint32_t c) -> int
        {
            if (c >= '0' && c <= '9')
                return (c - '0');
            if (c >= 'a' && c <= 'f')
                return (c - 'a') + 10;
            return -1;
        }
    };

    namespace detail {

        template <std::ranges::contiguous_range Range>
        auto bit_slice(Range const &r, unsigned start, unsigned len)
        {
            static_assert(sizeof(std::ranges::range_value_t<Range>) == 1);

            auto const *data = std::ranges::data(r);
            auto byte = start / 8;

            std::uint16_t value = data[byte] << 8;
            if ((start % 8) + len > 8)
                value |= data[byte + 1];

            auto mask = (1u << len) - 1;
            value >>= (16 - len) - (start % 8);
            auto ret = value & mask;

            return ret;
        }

        template <typename OutputIterator>
        class bit_writer {
            OutputIterator out;
            std::uint8_t cur = 0;
            unsigned left = 8;
            std::size_t written = 0;

            auto push()
            {
                *out++ = cur;
                cur = 0;
                left = 8;
            }

        public:
            bit_writer(OutputIterator out_) : out(out_) {}

            auto write(std::uint8_t v, unsigned len) -> std::size_t
            {
                std::size_t nwritten = 0;

                while (len) {
                    auto take = std::min(len, left);
                    std::uint8_t n = (v >> (len - take)) & ((1u << take) - 1);
                    std::uint8_t m = n << (left - take);
                    cur |= m;
                    len -= take;
                    left -= take;

                    if (left == 0) {
                        push();
                        nwritten++;
                    }
                }

                return nwritten;
            }
        };

    } // namespace detail

    template <typename Encoding,
              std::ranges::contiguous_range Input,
              typename OutputIterator>
    auto bintext_encode_block(Input &&input, OutputIterator output) -> void
    {
        static_assert(sizeof(std::ranges::range_value_t<Input>) == 1);

        auto in_size = std::ranges::size(input);
        std::size_t bits = in_size * 8;
        unsigned written = 0;

        for (unsigned bit = 0; bit < bits; bit += Encoding::bits_per_char) {
            auto v = detail::bit_slice(input, bit, Encoding::bits_per_char);
            *output++ = Encoding::table[v];
            ++written;
        }

        while (written++ < Encoding::encoded_bytes) {
            *output++ = Encoding::pad;
        }
    }

    template <typename Encoding,
              typename InputIterator,
              typename OutputIterator>
    auto bintext_encode(InputIterator begin,
                        InputIterator end,
                        OutputIterator out) -> void
    {
        while (begin < end) {
            std::array<std::uint8_t, Encoding::plain_bytes> input{};
            std::array<std::uint8_t, Encoding::encoded_bytes> output{};

            std::size_t n = 0;
            while (n < Encoding::plain_bytes && begin < end) {
                input[n++] = static_cast<std::uint8_t>(*begin++);
            }

            bintext_encode_block<Encoding>(std::span<std::uint8_t>(input).subspan(0, n),
                                           &output[0]);
            std::ranges::copy(output, out);
        }
    }

    template <typename Encoding,
              std::ranges::contiguous_range Range,
              typename OutputIterator>
    auto bintext_encode(Range const &r, OutputIterator it) -> void
    {
        auto bytes = as_bytes(std::span(r));
        bintext_encode<Encoding>(bytes.begin(), bytes.end(), it);
    }

    template <typename Encoding, std::ranges::contiguous_range Range>
    auto bintext_encode_to_string(Range const &r) -> std::string
    {
        std::string ret;

        auto bytes = as_bytes(std::span(r));
        bintext_encode<Encoding>(
            bytes.begin(), bytes.end(), std::back_inserter(ret));
        return ret;
    }

    template <typename Encoding,
              std::ranges::contiguous_range Input,
              typename OutputIterator>
    auto bintext_decode_block(Input const &input, OutputIterator output) -> int
    {
        static_assert(sizeof(std::ranges::range_value_t<Input>) == 1);
        std::size_t nwritten = 0;
        detail::bit_writer<OutputIterator> writer(output);

        for (unsigned i = 0; i < Encoding::encoded_bytes; ++i) {
            if (input[i] == Encoding::pad) {
                while (++i < Encoding::encoded_bytes)
                    if (input[i] != Encoding::pad)
                        return -1;

                break;
            }

            auto this_v = Encoding::char_to_value(input[i]);
            if (this_v == -1)
                return -1;

            nwritten += writer.write(static_cast<uint8_t>(this_v), Encoding::bits_per_char);
        }

        return static_cast<int>(nwritten);
    }

    template <typename Encoding,
              typename InputIterator,
              typename OutputIterator>
    auto
    bintext_decode(InputIterator begin, InputIterator end, OutputIterator out)
        -> expected<std::size_t, std::error_code>
    {
        std::size_t nbytes = 0;

        while (begin != end) {
            std::array<unsigned char, Encoding::encoded_bytes> inbuf{};
            std::array<unsigned char, Encoding::plain_bytes> outbuf{};

            for (std::size_t i = 0; i < Encoding::encoded_bytes; ++i) {
                if (begin == end)
                    return make_unexpected(
                        std::make_error_code(std::errc::invalid_argument));
                inbuf[i] = *begin++;
            }

            auto n = bintext_decode_block<Encoding>(inbuf, &outbuf[0]);
            if (n == -1)
                return make_unexpected(
                    std::make_error_code(std::errc::invalid_argument));

            out = std::copy(&outbuf[0], &outbuf[0] + n, out);
            nbytes += n;
            if (static_cast<unsigned>(n) < Encoding::plain_bytes)
                break;
        }

        if (begin != end)
            return make_unexpected(
                std::make_error_code(std::errc::invalid_argument));

        return nbytes;
    }

    template <typename Encoding,
              std::ranges::contiguous_range Range,
              typename OutputIterator>
    auto bintext_decode(Range const &r, OutputIterator it)
        -> expected<std::size_t, std::error_code>
    {
        return bintext_decode<Encoding>(std::ranges::begin(r), std::ranges::end(r), it);
    }

    template <typename Encoding, std::ranges::contiguous_range Range>
    auto bintext_decode_to_string(Range const &r)
        -> expected<std::string, std::error_code>
    {
        std::string ret;

        auto res = bintext_decode<Encoding>(
            std::ranges::begin(r), std::ranges::end(r), std::back_inserter(ret));
        if (res)
            return ret;
        return make_unexpected(res.error());
    }

} // namespace ivy

#endif // IVY_ALGORITHM_BINTEXT_HXX_INCLUDED
