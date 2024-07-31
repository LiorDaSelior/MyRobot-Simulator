project(simulator VERSION 0.1.0 LANGUAGES C CXX)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/exe)

add_library(helloworld_library STATIC src/helloworld.cpp)

target_include_directories(helloworld_library
    PUBLIC 
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(${PROJECT_NAME} 
    src/main.cpp
)

target_link_libraries( ${PROJECT_NAME} 
    PRIVATE 
    helloworld_library
)
