cmake_minimum_required(VERSION 3.5.0)
project(cpp_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pthread")

set(COMMON_DIR ${CMAKE_SOURCE_DIR}/common)

add_subdirectory(myrobot)
add_subdirectory(Algo_123456789)
add_subdirectory(Algo_319125134)
#add_subdirectory(Algo_319125134_1)
#./myrobot -algo_path="/home/liorubantu/Code/homework/cpp/cpp_project_3/exe/algorithms" -house_path="/home/liorubantu/Code/homework/cpp/cpp_project_3/exe/houses"
#include(CTest)
#enable_testing()
