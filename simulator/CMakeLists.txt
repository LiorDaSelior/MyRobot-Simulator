project(simulator VERSION 0.1.0 LANGUAGES C CXX)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/exe)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

include_directories(include)

#add_library(simulator_library INTERFACE) #INTERFACE - only header files
add_library(simulator_library house.cpp simulator.cpp simulator_data.cpp my_battery_meter.cpp my_wall_sensor.cpp my_dirt_sensor.cpp AlgorithmRegistrar.cpp)
target_include_directories(simulator_library PRIVATE ${PROJECT_SOURCE_DIR}/include ${COMMON_DIR})

add_executable(${PROJECT_NAME} 
main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${COMMON_DIR})


target_link_libraries( ${PROJECT_NAME} 
    PRIVATE 
    simulator_library
)
