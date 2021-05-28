/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_EXCEPTION_HXX_INCLUDED
#define IVY_EXCEPTION_HXX_INCLUDED

#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <format>

#include <ivy/string_literal.hxx>

namespace ivy {

    template <string_literal facility_text,
              char severity_text,
              string_literal code_text,
              string_literal format>
    class message : public std::exception {
        std::string _formatted;

    public:
        template<typename... Args>
        message(Args&&... args)
        try {
            _formatted = std::format(
                "{}-{}-{}, {}",
                facility_text.value,
                severity_text,
                code_text.value,
                std::format(format.value, std::forward<Args>(args)...));
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

} // namespace ivy

#endif // IVY_EXCEPTION_HXX_INCLUDED
