/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_EXCEPTION_HXX_INCLUDED
#define IVY_EXCEPTION_HXX_INCLUDED

#include <exception>
#include <format>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <ivy/string_literal.hxx>

namespace ivy {

    template <string_literal facility_text,
              char severity_text,
              string_literal code_text,
              string_literal format>
    class message : public std::exception {
        std::string _formatted;

    public:
        message(message const &) = default;
        message(message &&) noexcept = default;

        explicit message()
        try {
            _formatted = std::format("{}-{}-{}, {}",
                                     facility_text.value,
                                     severity_text,
                                     code_text.value,
                                     format.value);
        } catch (...) {
        }

        template <typename T1, typename... Rest>
        requires(!std::same_as<std::remove_cvref_t<T1>,
                               message>) explicit message(T1 &&t1,
                                                          Rest &&...rest)
        try {
            _formatted = std::format(
                "{}-{}-{}, {}",
                facility_text.value,
                severity_text,
                code_text.value,
                std::format(format.value, t1, std::forward<Rest>(rest)...));
        } catch (...) {
        }

        auto what() const noexcept -> char const * override
        {
            return _formatted.c_str();
        }
    };

    auto print_exception(std::ostream &strm,
                         std::exception const &e,
                         char prefix = '%') -> void
    {
        strm << prefix;

        strm << e.what();

        strm << ".\n";

        try {
            std::rethrow_if_nested(e);
        } catch (std::exception const &e) {
            print_exception(strm, e, '-');
        } catch (...) {
            strm << '-' << "Unknown error.\n";
        }
    }

    auto print_exception(std::ostream &strm,
                         std::exception_ptr const &eptr,
                         char prefix = '%') -> void
    {
        try {
            std::rethrow_exception(eptr);
        } catch (std::exception const &e) {
            print_exception(strm, e, prefix);
        } catch (...) {
            strm << prefix << "Unknown error.\n";
        }
    }

    auto print_current_exception(std::ostream &strm, char prefix = '%') -> void
    {
        print_exception(strm, std::current_exception(), prefix);
    }

    template <typename char_type,
              typename traits,
              string_literal F,
              char S,
              string_literal C,
              string_literal M>
    auto operator<<(std::basic_ostream<char_type, traits> &strm,
                    message<F, S, C, M> const &m)
        -> std::basic_ostream<char_type, traits> &
    {
        strm << '%' << m.what() << ".\n";
        return strm;
    }

} // namespace ivy

#endif // IVY_EXCEPTION_HXX_INCLUDED
