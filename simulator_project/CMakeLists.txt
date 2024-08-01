project(simulator VERSION 0.1.0 LANGUAGES C CXX)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/exe)

add_library(house STATIC src/house.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include,
    ${PROJECT_NAME} PRIVATE ${COMMON_DIR}
)

add_executable(${PROJECT_NAME} 
    src/main.cpp
)

target_link_libraries( ${PROJECT_NAME} 
    PRIVATE 
    house
)
