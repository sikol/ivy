# Copyright (c) 2019, 2020, 2021 SiKol Ltd.
# Distributed under the Boost Software License, Version 1.0.

cmake_minimum_required(VERSION 3.19)

find_package(Catch2 REQUIRED)

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
    test_charenc.cxx
    test_siphash.cxx
    test_textchannel.cxx
    test_stringchannel.cxx
    test_csv.cxx)

target_link_libraries(test_ivy PRIVATE ivy Catch2::Catch2)
target_compile_definitions(test_ivy PRIVATE CATCH_CONFIG_NO_WINDOWS_SEH)
set_target_properties(test_ivy PROPERTIES CXX_EXTENSIONS OFF)

target_compile_options(test_ivy PRIVATE
     $<$<CXX_COMPILER_ID:MSVC>:
          ${IVY_MSVC_FLAGS}>
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          ${IVY_CLANG_FLAGS}>)

add_test(NAME test_ivy COMMAND $<TARGET_FILE:test_ivy>)

# HTTP
add_executable(test_ivy_http main.cxx test_http.cxx)

target_link_libraries(test_ivy_http PRIVATE ivy ivy-http Catch2::Catch2)
target_compile_definitions(test_ivy_http PRIVATE CATCH_CONFIG_NO_WINDOWS_SEH)
set_target_properties(test_ivy_http PROPERTIES CXX_EXTENSIONS OFF)

target_compile_options(test_ivy_http PRIVATE
     $<$<CXX_COMPILER_ID:MSVC>:
          ${IVY_MSVC_FLAGS}>
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          ${IVY_CLANG_FLAGS}>)

add_test(NAME test_ivy_http COMMAND $<TARGET_FILE:test_ivy_http>)

# Crypto
add_executable(test_ivy_crypto main.cxx test_pbkdf2.cxx test_hash.cxx)

target_link_libraries(test_ivy_crypto PRIVATE ivy ivy-crypto Catch2::Catch2)
target_compile_definitions(test_ivy_crypto PRIVATE CATCH_CONFIG_NO_WINDOWS_SEH)
set_target_properties(test_ivy_crypto PROPERTIES CXX_EXTENSIONS OFF)

target_compile_options(test_ivy_crypto PRIVATE
     $<$<CXX_COMPILER_ID:MSVC>:
          ${IVY_MSVC_FLAGS}>
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          ${IVY_CLANG_FLAGS}>)

add_test(NAME test_ivy_crypto COMMAND $<TARGET_FILE:test_ivy_crypto>)
