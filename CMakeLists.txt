# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.19)

project("ivy")

add_subdirectory(thirdparty)

enable_testing()
add_subdirectory(tests)

add_library(ivy INTERFACE)
target_include_directories(ivy INTERFACE include)
target_compile_features(ivy INTERFACE cxx_std_20)
