project(Algo_319125134 VERSION 0.1.0 LANGUAGES C CXX)

add_library(${PROJECT_NAME} SHARED algorithm.cpp inner_map.cpp)


set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/exe/algorithms
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${COMMON_DIR})
