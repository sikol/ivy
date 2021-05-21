# Copyright (c) 2019, 2020, 2021 SiKol Ltd.
# Distributed under the Boost Software License, Version 1.0.

cmake_minimum_required (VERSION 3.19)

add_executable(http_server http_server.cxx)
target_link_libraries(http_server PRIVATE ivy)
