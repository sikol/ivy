# Copyright (c) 2019, 2020, 2021 SiKol Ltd.
# Distributed under the Boost Software License, Version 1.0.

cmake_minimum_required (VERSION 3.19)

project("ivy")

if(WIN32)
    find_package(WIL REQUIRED)
endif()

set(PKG_CONFIG_EXECUTABLE ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/pkgconf/pkgconf.exe)
find_package(PkgConfig REQUIRED)
pkg_check_modules(icu-uc REQUIRED IMPORTED_TARGET icu-uc)

#find_package(ICU REQUIRED)

set(IVY_CLANG_FLAGS
    -W 
    -Wall 
    -Wextra
    -Werror)

set(IVY_MSVC_FLAGS
    /W4 /WX)

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
