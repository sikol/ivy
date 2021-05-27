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

#include <ivy/string_literal.hxx>

namespace ivy {

    class exception : public std::exception {
        std::string _error_text;
        mutable std::string _formatted;

    public:
        exception(std::string error_text)
            : _error_text(std::move(error_text))
        {
        }

        ~exception() override = default;

        [[nodiscard]] virtual auto facility() const noexcept
            -> char const * = 0;
        [[nodiscard]] virtual auto severity() const noexcept -> char = 0;
        [[nodiscard]] virtual auto code() const noexcept -> char const * = 0;

        auto what() const noexcept -> char const * override
        {
            if (_formatted.empty())
                _formatted = std::format("{}-{}-{}, {}",
                                         facility(),
                                         severity(),
                                         code(),
                                         _error_text);

            return _formatted.c_str();
        }
    };

    template <string_literal facility_text,
              char severity_text,
              string_literal code_text>
    class message : public exception {
    public:
        message(std::string error_text)
            : exception(std::move(error_text))
        {
        }

        [[nodiscard]] auto facility() const noexcept -> char const * override
        {
            return facility_text.value;
        }

        [[nodiscard]] auto severity() const noexcept -> char override
        {
            return severity_text;
        }

        [[nodiscard]] auto code() const noexcept -> char const * override
        {
            return code_text.value;
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
