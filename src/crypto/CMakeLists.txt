# Copyright (c) 2019, 2020, 2021 SiKol Ltd.
# Distributed under the Boost Software License, Version 1.0.

add_library(ivy-crypto)

target_include_directories(ivy-crypto PUBLIC include)
target_compile_features(ivy-crypto PUBLIC cxx_std_20)
set_target_properties(ivy-crypto PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(ivy-crypto PUBLIC ivy)

target_compile_options(ivy-odbc PRIVATE
     $<$<CXX_COMPILER_ID:MSVC>:
          ${IVY_MSVC_FLAGS}>
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          ${IVY_CLANG_FLAGS}>)

if(WIN32)
    target_sources(ivy-crypto PRIVATE 
        src/win32/bcrypt.cxx
        src/win32/hash.cxx)
    target_link_libraries(ivy-crypto PUBLIC bcrypt WIL::WIL)
endif()
