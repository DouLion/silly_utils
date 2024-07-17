cmake_minimum_required (VERSION 3.15)

project(core_lib CXX)
foreach(dirname ${CMAKE_CURRENT_SOURCE_DIR})
    file(GLOB_RECURSE SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
            "${dirname}/*.h"
            "${dirname}/*.hpp"
            "${dirname}/*.c"
            "${dirname}/*.cpp"
            )
endforeach()


add_library(${PROJECT_NAME} STATIC ${SRC}
        module_a/module_a_studio.cpp
        module_a/module_a_studio.h
        web/request_param.cpp
        web/request_param.h
        web/response_result.cpp
        web/response_result.h)

target_link_libraries(${PROJECT_NAME} PUBLIC sutils)

