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
    config.cxx
    uri.cxx
    datum.cxx
    log.cxx
    http.cxx
    hash.cxx
    stringchannel.cxx
    string.cxx
    error.cxx
    charenc.cxx
    db/odbc.cxx)
    
if(WIN32)
    target_sources(ivy PRIVATE 
        win32/hash.cxx
        win32/error.cxx 
        win32/bcrypt.cxx
        win32/registry.cxx
        win32/httpsys/initialise_handle.cxx
        win32/httpsys/request_queue.cxx
        win32/httpsys/server_session.cxx
        win32/httpsys/url_group.cxx
        win32/httpsys/request_controller.cxx
        win32/httpsys/service.cxx
        win32/httpsys/response_writer.cxx
        win32/httpsys/response.cxx
        win32/httpsys/header.cxx)

    target_link_libraries(ivy PUBLIC bcrypt httpapi WIL::WIL)
endif()

target_link_libraries(ivy PUBLIC nanodbc PkgConfig::icu-uc)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(ivy PUBLIC IVY_ENABLE_TRACING)
endif()
