/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_IO_STRINGCHANNEL_HXX_INCLUDED
#define IVY_IO_STRINGCHANNEL_HXX_INCLUDED

#include <vector>

#include <ivy/io/channel.hxx>
#include <ivy/string.hxx>

namespace ivy {

    template <character_encoding encoding,
              typename allocator = std::allocator<typename encoding::char_type>>
    class basic_stringchannel final {
    public:
        using value_type = typename encoding::char_type;
        using string_type = basic_string<encoding, allocator>;
        using encoding_type = encoding;

    private:
        using storage_type = std::vector<value_type, allocator>;
        storage_type _buffer;
        typename storage_type::size_type _readp{}, _writep{};

    public:
        basic_stringchannel() = default;
        basic_stringchannel(basic_stringchannel const &) = default;
        basic_stringchannel(basic_stringchannel &&) = default;
        basic_stringchannel(basic_string<encoding, allocator> const &s);

        auto str() const -> string_type;

        auto read(std::span<value_type>)
            -> expected<io_size_t, std::error_code>;

        auto write(std::span<value_type const>)
            -> expected<io_size_t, std::error_code>;
    };

    using u8stringchannel = basic_stringchannel<utf8_encoding>;
    using u16stringchannel = basic_stringchannel<utf16_encoding>;
    using u32stringchannel = basic_stringchannel<utf32_encoding>;
    using astringchannel = basic_stringchannel<ascii_encoding>;
    using wstringchannel = basic_stringchannel<system_wide_encoding>;

    static_assert(sequential_channel<u8stringchannel>);

    template <character_encoding encoding, typename allocator>
    basic_stringchannel<encoding, allocator>::basic_stringchannel(
        basic_string<encoding, allocator> const &s)
        : _buffer(s.begin(), s.end())
    {
    }

    template <character_encoding encoding, typename allocator>
    auto basic_stringchannel<encoding, allocator>::str() const -> string_type
    {
        return string_type(&_buffer[0], &_buffer[0] + _buffer.size());
    }

    template <character_encoding encoding, typename allocator>
    auto
    basic_stringchannel<encoding, allocator>::read(std::span<value_type> data)
        -> expected<io_size_t, std::error_code>
    {
        auto readable = std::span(_buffer.begin() + _readp, _buffer.end());
        if (readable.size() > data.size())
            readable = readable.subspan(0, data.size());

        std::ranges::copy(readable, data.data());
        _readp += readable.size();

        return readable.size();
    }

    template <character_encoding encoding, typename allocator>
    auto basic_stringchannel<encoding, allocator>::write(
        std::span<value_type const> data)
        -> expected<io_size_t, std::error_code>
    {
        try {
            auto ipos = _buffer.begin() + _writep;

            if (ipos < _buffer.end()) {
                auto erase = std::min(data.size(),
                                      static_cast<std::size_t>(
                                          std::distance(ipos, _buffer.end())));
                _buffer.erase(ipos, ipos + erase);
            }

            _buffer.insert(ipos, data.begin(), data.end());
            _writep += data.size();

            return data.size();
        } catch (std::bad_alloc const &) {
            return make_unexpected(
                make_error_code(std::errc::not_enough_memory));
        }

        return data.size();
    }

    extern template class basic_stringchannel<utf8_encoding>;
    extern template class basic_stringchannel<utf16_encoding>;
    extern template class basic_stringchannel<utf32_encoding>;
    extern template class basic_stringchannel<ascii_encoding>;
    extern template class basic_stringchannel<system_wide_encoding>;

} // namespace ivy

#endif // IVY_IO_STRINGCHANNEL_HXX_INCLUDED
