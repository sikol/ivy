# Copyright (c) 2019, 2020, 2021 SiKol Ltd.
# Distributed under the Boost Software License, Version 1.0.

cmake_minimum_required (VERSION 3.19)

project("ivy")

option(IVY_DISABLE_TESTS "Don't build tests" OFF)
option(IVY_DISABLE_SAMPLES "Don't build samples" OFF)

add_subdirectory(thirdparty)

if(NOT IVY_DISABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(NOT IVY_DISABLE_SAMPLES)
    add_subdirectory(samples)
endif()

add_subdirectory(src)
