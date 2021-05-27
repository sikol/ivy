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
    
target_sources(ivy PRIVATE
    include/ivy/byte.hxx
    include/ivy/charenc.hxx
    include/ivy/check.hxx
    include/ivy/csv.hxx
    include/ivy/datum.hxx
    include/ivy/db.hxx
    include/ivy/error.hxx
    include/ivy/exception.hxx
    include/ivy/expected.hxx
    include/ivy/flagset.hxx
    include/ivy/format.hxx
    include/ivy/hash.hxx
    include/ivy/lazy.hxx
    include/ivy/log.hxx
    include/ivy/noncopyable.hxx
    include/ivy/overload.hxx
    include/ivy/scope_guard.hxx
    include/ivy/siphash.hxx
    include/ivy/static_vector.hxx
    include/ivy/string.hxx
    include/ivy/string_literal.hxx
    include/ivy/trace.hxx
    include/ivy/unexpected.hxx

    include/ivy/algorithm/bintext.hxx

    include/ivy/buffer/buffer.hxx
    include/ivy/buffer/circular_buffer.hxx
    include/ivy/buffer/dynamic_buffer.hxx
    include/ivy/buffer/fixed_buffer.hxx
    include/ivy/buffer/pmr_buffer.hxx
    include/ivy/buffer/range_buffer.hxx

    include/ivy/charenc/icu/bytereader.hxx
    include/ivy/charenc/icu/encoding_traits.hxx
    include/ivy/charenc/icu/error.hxx
    include/ivy/charenc/icu/ucnv.hxx
    include/ivy/charenc/icu/ucnv_charconv.hxx
    include/ivy/charenc/icu/verifier.hxx

    include/ivy/charenc/ascii.hxx
    include/ivy/charenc/charconv.hxx
    include/ivy/charenc/error.hxx
    include/ivy/charenc/system.hxx
    include/ivy/charenc/system_wide.hxx
    include/ivy/charenc/utf16.hxx
    include/ivy/charenc/utf32.hxx
    include/ivy/charenc/utf8.hxx

    include/ivy/config/lex.hxx
    include/ivy/config/parse.hxx

    include/ivy/datum/boolean.hxx
    include/ivy/datum/integer.hxx
    include/ivy/datum/null.hxx
    include/ivy/datum/string.hxx

    include/ivy/db/connection.hxx
    include/ivy/db/error.hxx
    include/ivy/db/query.hxx
    include/ivy/db/query_result.hxx
    include/ivy/db/result_set.hxx
    include/ivy/db/row.hxx
    include/ivy/db/value.hxx

    include/ivy/io/channel.hxx
    include/ivy/io/pmrchannel.hxx
    include/ivy/io/stringchannel.hxx
    include/ivy/io/textchannel.hxx
    include/ivy/io/transcodechannel.hxx

    include/ivy/iterator/byte_decode.hxx
    include/ivy/iterator/null.hxx
    include/ivy/iterator/static_cast.hxx

    include/ivy/log/ostream_sink.hxx

    include/ivy/net/uri.hxx

    include/ivy/string/convert.hxx
    include/ivy/string/iostream.hxx
    include/ivy/string/join.hxx
    include/ivy/string/match.hxx
    include/ivy/string/split.hxx
    include/ivy/string/transcode.hxx
    include/ivy/string/trim.hxx
 )

if(WIN32)
    target_sources(ivy PRIVATE 
        src/win32/error.cxx 
        src/win32/registry.cxx
    )

    target_sources(ivy PRIVATE
        include/ivy/win32/error.hxx
        include/ivy/win32/heap.hxx
        include/ivy/win32/registry.hxx
        include/ivy/win32/windows.hxx
    )

    target_link_libraries(ivy PUBLIC WIL::WIL)
endif()

target_link_libraries(ivy PUBLIC PkgConfig::icu-uc)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(ivy PUBLIC IVY_ENABLE_TRACING)
endif()
