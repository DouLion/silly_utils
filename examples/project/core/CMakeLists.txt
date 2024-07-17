cmake_minimum_required (VERSION 3.15)

project(core_lib CXX)
foreach(dirname ${CMAKE_CURRENT_SOURCE_DIR})
    file(GLOB_RECURSE CORE_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
            "${dirname}/*.h"
            "${dirname}/*.hpp"
            "${dirname}/*.c"
            "${dirname}/*.cpp"
            )
endforeach()


add_library(${PROJECT_NAME} STATIC ${CORE_SRC})

target_link_libraries(${PROJECT_NAME} PUBLIC sutils)

