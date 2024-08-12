cmake_minimum_required(VERSION 3.5.0)
project(cpp_project)

set(CMAKE_CXX_STANDARD 20)

set(COMMON_DIR ${CMAKE_SOURCE_DIR}/common)

add_subdirectory(simulator)
add_subdirectory(algorithm_bfs)
add_subdirectory(algorithm_another)

include(CTest)
enable_testing()