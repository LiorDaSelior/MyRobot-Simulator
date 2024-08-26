cmake_minimum_required(VERSION 3.5.0)
project(cpp_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pthread -Wall -Wextra -Werror -pedantic")

set(COMMON_DIR ${CMAKE_SOURCE_DIR}/common)

add_subdirectory(myrobot)
add_subdirectory(Algo_204666085)
add_subdirectory(Algo_319125134)