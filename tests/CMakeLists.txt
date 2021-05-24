# Copyright (c) 2019, 2020, 2021 SiKol Ltd.
# Distributed under the Boost Software License, Version 1.0.

cmake_minimum_required(VERSION 3.19)

add_executable(test_ivy 
    main.cxx 
    test_string.cxx
    test_string_algo.cxx
    test_format.cxx 
    test_config.cxx 
    test_expected.cxx 
    test_error.cxx
    test_uri.cxx
    test_lazy.cxx
    test_datum.cxx
    test_log.cxx
    test_bintext.cxx
    test_hash.cxx
    test_pbkdf2.cxx
    test_charenc.cxx
    test_siphash.cxx
    test_http.cxx
    test_textchannel.cxx
    test_stringchannel.cxx
    test_csv.cxx)

target_link_libraries(test_ivy PRIVATE ivy Catch2::Catch2)
target_compile_definitions(test_ivy PRIVATE CATCH_CONFIG_NO_WINDOWS_SEH)

add_test(NAME test_ivy COMMAND $<TARGET_FILE:test_ivy>)
