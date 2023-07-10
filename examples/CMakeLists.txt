cmake_minimum_required(VERSION 3.15)

project(silly_utils_example CXX)

foreach(dirname ${CMAKE_CURRENT_SOURCE_DIR})
    file(GLOB_RECURSE SU_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
            "${dirname}/*.h"
            "${dirname}/*.hpp"
            "${dirname}/*.c"
            "${dirname}/*.cpp"
            )

endforeach()


add_executable(${PROJECT_NAME} ${SU_SRC_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC suitls)