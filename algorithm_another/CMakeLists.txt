project(algorithm_another VERSION 0.1.0 LANGUAGES C CXX)

set (LIBRARY_OUTPUT_DIRECTORY $)

include_directories(include)

#add_library(simulator_library INTERFACE) #INTERFACE - only header files
add_library(another_algorithm_library SHARED algorithm.cpp inner_map.cpp)


set_target_properties(another_algorithm_library PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/exe/algorithms
)
set_target_properties(another_algorithm_library PROPERTIES OUTPUT_NAME "algorithm_another")

target_include_directories(another_algorithm_library PRIVATE ${PROJECT_SOURCE_DIR}/include ${COMMON_DIR})
