# Copyright (c) 2019, 2020, 2021 SiKol Ltd.
# Distributed under the Boost Software License, Version 1.0.

cmake_minimum_required(VERSION 3.19)

add_executable(test_ivy main.cxx test_string.cxx test_format.cxx test_config.cxx test_expected.cxx)
target_link_libraries(test_ivy PRIVATE ivy Catch2::Catch2)

add_test(NAME test_ivy COMMAND $<TARGET_FILE:test_ivy>)
