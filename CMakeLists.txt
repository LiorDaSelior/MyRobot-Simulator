cmake_minimum_required(VERSION 3.5.0)
project(cpp_project)

set(CMAKE_CXX_STANDARD 20)

set(COMMON_DIR ${CMAKE_SOURCE_DIR}/common)

add_subdirectory(myrobot)
add_subdirectory(Algo_123456789)
add_subdirectory(Algo_319125134)
#add_subdirectory(Algo_319125134_1)

include(CTest)
enable_testing()