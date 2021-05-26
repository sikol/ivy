# Copyright (c) 2019, 2020, 2021 SiKol Ltd.
# Distributed under the Boost Software License, Version 1.0.

add_library(nanodbc)
target_sources(nanodbc PRIVATE nanodbc.cpp)
target_compile_definitions(nanodbc PUBLIC 
    NANODBC_THROW_NO_SOURCE_LOCATION=1
    NANODBC_ENABLE_UNICODE=1)

target_include_directories(nanodbc PUBLIC include)
