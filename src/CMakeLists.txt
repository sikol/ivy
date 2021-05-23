# Copyright (c) 2019, 2020, 2021 SiKol Ltd.
# Distributed under the Boost Software License, Version 1.0.

add_library(ivy)
target_include_directories(ivy PUBLIC include)
target_compile_features(ivy PUBLIC cxx_std_20)
target_sources(ivy PRIVATE 
    config.cxx
    uri.cxx
    datum.cxx
    log.cxx
    charenc.cxx
    http.cxx
    hash.cxx
    stringchannel.cxx
    string.cxx)
    
if(WIN32)
    target_sources(ivy PRIVATE 
        win32/hash.cxx
        win32/error.cxx 
        win32/bcrypt.cxx
        win32/registry.cxx
        win32/httpsys.cxx)

    target_link_libraries(ivy PUBLIC bcrypt httpapi WIL)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(ivy PUBLIC IVY_ENABLE_TRACING)
endif()
