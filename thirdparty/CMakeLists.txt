# Copyright (c) 2019, 2020, 2021 SiKol Ltd.
# Distributed under the Boost Software License, Version 1.0.

#if(NOT IVY_DISABLE_TESTS AND NOT TARGET Catch2)
#    add_subdirectory(catch2)
#endif()

if(NOT TARGET nanodbc)
    add_subdirectory(nanodbc)
endif()

if(NOT TARGET srell)
    add_subdirectory(srell)
endif()
