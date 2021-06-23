/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CSV_HXX_INCLUDED
#define IVY_CSV_HXX_INCLUDED

#include <format>
#include <stdexcept>
#include <string>
#include <vector>

#include <ivy/error.hxx>
#include <ivy/io/channel.hxx>
#include <ivy/string.hxx>

namespace ivy {

    class csv_error : public std::runtime_error {
    public:
        csv_error(std::string const &message)
            : std::runtime_error(message)
        {
        }
    };

    template <character_encoding encoding>
    struct basic_csv_config {
        encoding_char_type<encoding> separator = encoding::widen(',');
        encoding_char_type<encoding> quote_character = encoding::widen('"');
        encoding_char_type<encoding> escape_character = encoding::widen('\\');
        bool has_header = false;
        bool has_quotes = true;
        bool always_quote = false;
    };

    using wcsv_config = basic_csv_config<system_wide_encoding>;
    using u8csv_config = basic_csv_config<utf8_encoding>;
    using u16csv_config = basic_csv_config<utf16_encoding>;
    using u32csv_config = basic_csv_config<utf32_encoding>;

    template <character_encoding encoding,
              typename allocator = std::allocator<encoding_char_type<encoding>>>
    struct basic_csv_row {
        std::vector<basic_string<encoding, allocator>> fields;
    };

    using wcsv_row = basic_csv_row<system_wide_encoding>;
    using u8csv_row = basic_csv_row<utf8_encoding>;
    using u16csv_row = basic_csv_row<utf16_encoding>;
    using u32csv_row = basic_csv_row<utf32_encoding>;

    template <sequential_input_channel channel_type,
              typename allocator = std::allocator<
                  encoding_char_type<typename channel_type::encoding_type>>>
    class csv_reader {
    public:
        using encoding_type = typename channel_type::encoding_type;
        using config_type = basic_csv_config<encoding_type>;
        using row_type = basic_csv_row<encoding_type, allocator>;

    private:
        using char_type =
            encoding_char_type<typename channel_type::encoding_type>;
        using string_type = basic_string<encoding_type, allocator>;
        config_type _config;
        channel_type *_channel;

    public:
        csv_reader(channel_type &channel, config_type config = {});

        auto read() -> expected<row_type, error>;
    };

    template <sequential_input_channel channel_type, typename allocator>
    csv_reader<channel_type, allocator>::csv_reader(channel_type &chan,
                                                    config_type config)
        : _config(config)
        , _channel(&chan)
    {
    }

    template <sequential_input_channel channel_type, typename allocator>
    auto csv_reader<channel_type, allocator>::read()
        -> expected<row_type, error>
    {
        std::vector<char_type, allocator> word;
        row_type row;

        for (;;) {
            char_type c;

            auto r = _channel->read(std::span(&c, 1));
            if (!r)
                return make_unexpected(make_error<csv_error>(
                    std::format("file read failed: {}", r.error().what())));

            if (c == '\n') {
                row.fields.push_back(string_type(word));
                word.clear();
                return row;
            }

            if (c == _config.separator) {
                row.fields.push_back(word);
                word.clear();
                continue;
            }

            word.push_back(c);
        }
    }

} // namespace ivy

#endif // IVY_CSV_HXX_INCLUDED
