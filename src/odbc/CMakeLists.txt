# Copyright (c) 2019, 2020, 2021 SiKol Ltd.
# Distributed under the Boost Software License, Version 1.0.

add_library(ivy-odbc)

target_include_directories(ivy-odbc PUBLIC include)
target_compile_features(ivy-odbc PUBLIC cxx_std_20)
set_target_properties(ivy-odbc PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(ivy-odbc PUBLIC ivy nanodbc)

target_compile_options(ivy-odbc PRIVATE
     $<$<CXX_COMPILER_ID:MSVC>:
          ${IVY_MSVC_FLAGS}>
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          ${IVY_CLANG_FLAGS}>)

target_sources(ivy-odbc PRIVATE 
    src/odbc.cxx
)

target_sources(ivy-odbc PRIVATE
    include/ivy/db/odbc/connect.hxx
    include/ivy/db/odbc/connection.hxx
    include/ivy/db/odbc/query.hxx
    include/ivy/db/odbc/query_result.hxx
    include/ivy/db/odbc/result_set.hxx
    include/ivy/db/odbc/row.hxx
    include/ivy/db/odbc/value.hxx
)

if(WIN32)
    target_link_libraries(ivy-odbc PUBLIC odbc32 WIL::WIL)
endif()
