cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_CXX_STANDARD 20)

set(COMMON_DIR ${CMAKE_SOURCE_DIR}/common)

add_subdirectory(simulator_project)

include(CTest)
enable_testing()