/*
 * Copyright (c) 2019, 2020, 2021 SiKol Ltd.
 * Distributed under the Boost Software License, Version 1.0.
 */

#ifndef IVY_CMDLINE_HXX_INCLUDED
#define IVY_CMDLINE_HXX_INCLUDED

#include <unordered_map>

#include <ivy/error.hxx>
#include <ivy/exception.hxx>
#include <ivy/expected.hxx>
#include <ivy/string.hxx>
#include <ivy/string/format.hxx>

namespace ivy {

    using cli_argencoding =
        message<"CLI", 'E', "ARGPARSE", "Invalid encoding in argument">;

    using cli_nooption =
        message<"CLI", 'E', "NOOPTION", "Unrecognised option '{}'">;

    using cli_noargument =
        message<"CLI", 'E', "NOARG", "Missing argument '{}'">;

    namespace detail {

        template <typename options_type>
        class option_base {
        public:
            virtual ~option_base() = default;

            virtual auto assign(options_type *, u8string const &) noexcept
                -> expected<void, error> = 0;
        };

        template <typename options_type, typename value_type>
        class option;

        template <typename options_type, typename encoding, typename allocator>
        class option<options_type, basic_string<encoding, allocator>>
            : public option_base<options_type> {
            using string_type = basic_string<encoding, allocator>;
            using member_type = string_type options_type::*;

            member_type _target;

        public:
            ~option() override = default;

            option(member_type target)
                : _target(target)
            {
            }

            auto assign(options_type *opts, u8string const &v) noexcept
                -> expected<void, error> override
            try {
                opts->*_target = v;
                return {};
            } catch (...) {
                return make_unexpected(make_error(std::current_exception()));
            }
        };

    } // namespace detail

    template <typename options_type>
    class cmdline {
        using base_type = detail::option_base<options_type>;

        std::unordered_map<u8string, std::unique_ptr<base_type>> _options;
        std::vector<std::pair<u8string, std::unique_ptr<base_type>>> _arguments;

    public:
        cmdline() = default;

        template <typename value_type>
        auto add_option(u8string const &name, value_type(options_type::*ptr))
            -> void
        {
            std::unique_ptr<base_type> option =
                std::make_unique<detail::option<options_type, value_type>>(ptr);
            _options.insert({name, std::move(option)});
        }

        template <typename value_type>
        auto add_argument(u8string const &name, value_type(options_type::*ptr))
            -> void
        {
            std::unique_ptr<base_type> option =
                std::make_unique<detail::option<options_type, value_type>>(ptr);
            _arguments.push_back({name, std::move(option)});
        }

        auto parse(int, char **argv) -> expected<options_type, error>
        try {
            options_type ret;

            std::vector<u8string> args;
            
            while (*argv) {
                std::string s(*argv);
                args.push_back(ivy::transcode<u8string>(s).or_throw_with_nested(
                    cli_argencoding()));
                ++argv;
            }

            auto it = args.begin();
            if (it != args.end())
                ++it; // skip argv[0]

            // Parse options.
            while (it < args.end()) {
                if (it->empty() || (*it)[0] != '/')
                    break;

                return make_unexpected(make_error<cli_nooption>(*it));
            }

            auto arg_it = _arguments.begin();
            while (arg_it < _arguments.end()) {
                if (it < args.end()) {
                    arg_it->second->assign(&ret, *it).or_throw();
                    ++arg_it;
                    ++it;
                    continue;
                }

                return make_unexpected(
                    make_error<cli_noargument>(arg_it->first));
            }

            return ret;
        } catch (...) {
            return make_unexpected(make_error(std::current_exception()));
        }
    };

} // namespace ivy

#endif // IVY_CMDLINE_HXX_INCLUDED
