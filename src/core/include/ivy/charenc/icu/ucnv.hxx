/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CHARENC_ICU_UCNV_HXX_INCLUDED
#define IVY_CHARENC_ICU_UCNV_HXX_INCLUDED

#include <memory>
#include <span>
#include <system_error>
#include <vector>

#include <ivy/error.hxx>
#include <ivy/expected.hxx>

namespace ivy::icu {

    struct ucnv;

    struct ucnv_deleter {
        auto operator()(ucnv *) noexcept -> void;
    };

    using ucnv_handle = std::unique_ptr<ucnv, ucnv_deleter>;

    auto make_ucnv(char const *source, char const *target, bool accept_invalid)
        -> expected<ucnv_handle, error>;

    auto ucnv_convert_bytes(ucnv_handle &,
                            bool flush,
                            std::span<std::byte const> input,
                            std::vector<std::byte> &output)
        -> expected<void, error>;

} // namespace ivy::icu

#endif // IVY_CHARENC_ICU_UCNV_HXX_INCLUDED
