# Copyright (c) 2019, 2020, 2021 SiKol Ltd.
# Distributed under the Boost Software License, Version 1.0.

cmake_minimum_required (VERSION 3.19)

project("ivy")

option(IVY_DISABLE_TESTS "Don't build tests" OFF)

add_subdirectory(thirdparty)

if(NOT IVY_DISABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

add_library(ivy)
target_include_directories(ivy PUBLIC include)
target_compile_features(ivy PUBLIC cxx_std_20)
target_sources(ivy PRIVATE 
    src/config.cxx
    src/uri.cxx
    src/datum.cxx
    src/log.cxx)
    
if(WIN32)
    target_sources(ivy PRIVATE 
        src/win32/hash.cxx
        src/win32/error.cxx 
        src/win32/bcrypt.cxx)

    target_link_libraries(ivy PUBLIC bcrypt)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(ivy PUBLIC IVY_ENABLE_TRACING)
endif()