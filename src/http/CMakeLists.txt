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

target_sources(ivy-http PRIVATE
    include/ivy/http/header.hxx
    include/ivy/http/method.hxx
    include/ivy/http/request.hxx
    include/ivy/http/response.hxx
    include/ivy/http/service.hxx
    include/ivy/http/status.hxx
    include/ivy/http/version.hxx
)

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

    target_sources(ivy-http PRIVATE
        include/ivy/win32/httpsys.hxx
        include/ivy/win32/httpsys/header.hxx
        include/ivy/win32/httpsys/initialise_handle.hxx
        include/ivy/win32/httpsys/request_context.hxx
        include/ivy/win32/httpsys/request_controller.hxx
        include/ivy/win32/httpsys/request_queue.hxx
        include/ivy/win32/httpsys/response.hxx
        include/ivy/win32/httpsys/response_writer.hxx
        include/ivy/win32/httpsys/server_session.hxx
        include/ivy/win32/httpsys/service.hxx
        include/ivy/win32/httpsys/url_group.hxx
    )

    target_link_libraries(ivy-http PUBLIC httpapi WIL::WIL)
endif()
