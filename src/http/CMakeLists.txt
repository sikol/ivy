# Copyright (c) 2019, 2020, 2021 SiKol Ltd.
# Distributed under the Boost Software License, Version 1.0.

add_library(ivy-http)

target_include_directories(ivy-http PUBLIC include)
target_compile_features(ivy-http PUBLIC cxx_std_20)
set_target_properties(ivy-http PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(ivy-http PUBLIC ivy)

target_compile_options(ivy-http PRIVATE
     $<$<CXX_COMPILER_ID:MSVC>:
          ${IVY_MSVC_FLAGS}>
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          ${IVY_CLANG_FLAGS}>)

target_sources(ivy-http PRIVATE src/http.cxx)

if(WIN32)
    target_sources(ivy-http PRIVATE 
        src/win32/initialise_handle.cxx
        src/win32/request_queue.cxx
        src/win32/server_session.cxx
        src/win32/url_group.cxx
        src/win32/request_controller.cxx
        src/win32/service.cxx
        src/win32/response_writer.cxx
        src/win32/response.cxx
        src/win32/header.cxx)

    target_link_libraries(ivy-http PUBLIC httpapi WIL::WIL)
endif()
