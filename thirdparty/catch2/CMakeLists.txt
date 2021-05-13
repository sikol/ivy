# Copyright (c) 2019, 2020, 2021 SiKol Ltd.
# Distributed under the Boost Software License, Version 1.0.

set(CATCH_INSTALL_DOCS OFF CACHE BOOL "Install documentation alongside library")
set(CATCH_INSTALL_EXTRAS OFF CACHE BOOL "Install extras alongside library")
set(CATCH_DEVELOPMENT_BUILD OFF CACHE BOOL "Build tests, enable warnings, enable Werror, etc")

add_subdirectory(src)
