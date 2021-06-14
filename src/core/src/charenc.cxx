/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#include <cstddef>
#include <format>
#include <iterator>
#include <ranges>
#include <span>

#include <ivy/charenc/error.hxx>
#include <ivy/charenc/icu/error.hxx>
#include <ivy/charenc/icu/ucnv.hxx>
#include <ivy/noncopyable.hxx>
#include <ivy/trace.hxx>

#include <unicode/uchar.h>
#include <unicode/ucnv.h>

namespace ivy::icu {

    constexpr std::size_t ucnv_buffer_size = 1024;

    namespace detail {

        auto errc_category::name() const noexcept -> char const *
        {
            return "ivy:icu";
        }

        auto errc_category::message(int c) const -> std::string
        {
            return u_errorName(static_cast<UErrorCode>(c));
        }

    } // namespace detail

    auto errc_category() -> detail::errc_category const &
    {
        static detail::errc_category c;
        return c;
    }

    auto make_error_code(errc e) -> std::error_code
    {
        return {static_cast<int>(e), errc_category()};
    }

    struct uconverter_deleter {
        auto operator()(UConverter *cnv) noexcept -> void
        {
            ucnv_close(cnv);
        }
    };

    using uconverter_handle = std::unique_ptr<UConverter, uconverter_deleter>;

    struct ucnv : nonmovable {
        uconverter_handle source;
        uconverter_handle target;

        UChar pivot_buffer[1024];
        UChar *pivot_source = &pivot_buffer[0];
        UChar *pivot_target = &pivot_buffer[0];

        ucnv(uconverter_handle &&source, uconverter_handle &&target);
    };

    ucnv::ucnv(uconverter_handle &&source_, uconverter_handle &&target_)
        : source(std::move(source_))
        , target(std::move(target_))
    {
    }

    auto ucnv_deleter::operator()(ucnv *cnv) noexcept -> void
    {
        delete cnv;
    }

    auto make_ucnv(char const *source, char const *target, bool accept_invalid)
        -> expected<ucnv_handle, error>
    {
        UErrorCode err{};

        UConverter *source_cnv = ucnv_open(source, &err);

        if (U_FAILURE(err))
            return make_unexpected(make_error<encoding_error>(std::format(
                "failed to open encoding '{}': {}", source, u_errorName(err))));

        auto source_h = uconverter_handle(source_cnv);

        UConverter *target_cnv = ucnv_open(target, &err);

        if (U_FAILURE(err)) {
            return make_unexpected(make_error<encoding_error>(std::format(
                "failed to open encoding '{}': {}", target, u_errorName(err))));
        }

        auto target_h = uconverter_handle(target_cnv);

        if (accept_invalid) {
            ucnv_setFromUCallBack(source_h.get(),
                                  UCNV_FROM_U_CALLBACK_SUBSTITUTE,
                                  nullptr,
                                  nullptr,
                                  nullptr,
                                  &err);
            ucnv_setFromUCallBack(target_h.get(),
                                  UCNV_FROM_U_CALLBACK_SUBSTITUTE,
                                  nullptr,
                                  nullptr,
                                  nullptr,
                                  &err);
            ucnv_setToUCallBack(source_h.get(),
                                UCNV_TO_U_CALLBACK_SUBSTITUTE,
                                nullptr,
                                nullptr,
                                nullptr,
                                &err);
            ucnv_setToUCallBack(target_h.get(),
                                UCNV_TO_U_CALLBACK_SUBSTITUTE,
                                nullptr,
                                nullptr,
                                nullptr,
                                &err);
        } else {
            ucnv_setFromUCallBack(source_h.get(),
                                  UCNV_FROM_U_CALLBACK_STOP,
                                  nullptr,
                                  nullptr,
                                  nullptr,
                                  &err);
            ucnv_setFromUCallBack(target_h.get(),
                                  UCNV_FROM_U_CALLBACK_STOP,
                                  nullptr,
                                  nullptr,
                                  nullptr,
                                  &err);
            ucnv_setToUCallBack(source_h.get(),
                                UCNV_TO_U_CALLBACK_STOP,
                                nullptr,
                                nullptr,
                                nullptr,
                                &err);
            ucnv_setToUCallBack(target_h.get(),
                                UCNV_TO_U_CALLBACK_STOP,
                                nullptr,
                                nullptr,
                                nullptr,
                                &err);
        }

        if (U_FAILURE(err)) {
            return make_unexpected(make_error<encoding_error>(std::format(
                "failed to configure encoding: {}", u_errorName(err))));
        }

        return ucnv_handle(new ucnv(std::move(source_h), std::move(target_h)));
    }

    auto ucnv_convert_bytes(ucnv_handle &ucnv,
                            bool flush,
                            std::span<std::byte const> input,
                            std::vector<std::byte> &output)
        -> expected<void, error>
    {
        UErrorCode err = U_ZERO_ERROR;

        char const *input_begin = reinterpret_cast<char const *>(input.data());
        char const *input_end = input_begin + input.size();

        do {
            std::byte buf[ucnv_buffer_size];

            IVY_TRACE("ucnv: converting {} bytes", (input_end - input_begin));

            char *output_chars = reinterpret_cast<char *>(&buf[0]);
            char *output_begin = output_chars;
            char *output_end = output_begin + sizeof(buf);

            IVY_TRACE("ucnv: output buffer is {} bytes",
                      (output_end - output_begin));

            err = U_ZERO_ERROR;
            ucnv_convertEx(ucnv->target.get(),
                           ucnv->source.get(),
                           &output_begin,
                           output_end,
                           &input_begin,
                           input_end,
                           &ucnv->pivot_buffer[0],
                           &ucnv->pivot_source,
                           &ucnv->pivot_target,
                           &ucnv->pivot_buffer[0] +
                               std::ranges::size(ucnv->pivot_buffer),
                           false,
                           flush,
                           &err);

            auto output_len = (output_begin - output_chars);
            IVY_TRACE("ucnv: result is {} bytes", output_len);

            std::ranges::copy(
                &buf[0], &buf[0] + output_len, std::back_inserter(output));

        } while (err == U_BUFFER_OVERFLOW_ERROR);

        if (U_FAILURE(err))
            return make_unexpected(make_error<encoding_error>(std::format(
                "ucnv_convertEx failed: {}",
                make_error_code(static_cast<errc>(err)).message())));

        // ICU unhelpfully tacks a zero byte onto the end of the output.
        // if (err != U_STRING_NOT_TERMINATED_WARNING && !output.empty())
        //    output.pop_back();

        return {};
    }

} // namespace ivy::icu

namespace ivy {

    auto isdigit(char32_t c) -> bool
    {
        return u_isdigit(c);
    }

    auto isupper(char32_t c) -> bool
    {
        return u_isupper(c);
    }

    auto islower(char32_t c) -> bool
    {
        return u_islower(c);
    }

    auto istitle(char32_t c) -> bool
    {
        return u_istitle(c);
    }

    auto isalpha(char32_t c) -> bool
    {
        return u_isalpha(c);
    }

    auto isalnum(char32_t c) -> bool
    {
        return u_isalnum(c);
    }

    auto isxdigit(char32_t c) -> bool
    {
        return u_isxdigit(c);
    }

    auto ispunct(char32_t c) -> bool
    {
        return u_ispunct(c);
    }

    auto isgraph(char32_t c) -> bool
    {
        return u_isgraph(c);
    }

    auto isblank(char32_t c) -> bool
    {
        return u_isblank(c);
    }

    auto isdefined(char32_t c) -> bool
    {
        return u_isdefined(c);
    }

    auto isspace(char32_t c) -> bool
    {
        return u_isspace(c);
    }

    auto iscntrl(char32_t c) -> bool
    {
        return u_iscntrl(c);
    }

    auto isprint(char32_t c) -> bool
    {
        return u_isprint(c);
    }

    auto isbase(char32_t c) -> bool
    {
        return u_isbase(c);
    }

    auto toupper(char32_t c) -> char32_t
    {
        return u_toupper(c);
    }

    auto tolower(char32_t c) -> char32_t
    {
        return u_tolower(c);
    }

} // namespace ivy
