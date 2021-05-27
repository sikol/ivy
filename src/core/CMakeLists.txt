# Copyright (c) 2019, 2020, 2021 SiKol Ltd.
# Distributed under the Boost Software License, Version 1.0.

add_library(ivy)

target_include_directories(ivy PUBLIC include)
target_compile_features(ivy PUBLIC cxx_std_20)
set_target_properties(ivy PROPERTIES CXX_EXTENSIONS OFF)

target_compile_options(ivy PRIVATE
     $<$<CXX_COMPILER_ID:MSVC>:
          ${IVY_MSVC_FLAGS}>
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          ${IVY_CLANG_FLAGS}>)

target_sources(ivy PRIVATE 
    src/config.cxx
    src/uri.cxx
    src/datum.cxx
    src/log.cxx
    src/hash.cxx
    src/stringchannel.cxx
    src/string.cxx
    src/error.cxx
    src/charenc.cxx
    src/db.cxx)
    
if(WIN32)
    target_sources(ivy PRIVATE 
        src/win32/error.cxx 
        src/win32/registry.cxx)

    target_link_libraries(ivy PUBLIC WIL::WIL)
endif()

target_link_libraries(ivy PUBLIC PkgConfig::icu-uc)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(ivy PUBLIC IVY_ENABLE_TRACING)
endif()
