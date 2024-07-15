cmake_minimum_required(VERSION 3.15)

project(silly_utils_example CXX)

foreach(dirname ${CMAKE_CURRENT_SOURCE_DIR})
    file(GLOB_RECURSE SU_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${dirname}/*.cc" "${dirname}/*.c" "${dirname}/*.cpp")
    foreach(filename ${SU_SRC_FILES})
    get_filename_component(pure_filename ${filename} NAME_WE ABSOLUTE)
        set(temp_proj_name "SUExample-${pure_filename}")
        add_executable(${temp_proj_name} ${filename})
        target_link_libraries(${temp_proj_name} PUBLIC suitls)
    endforeach()

endforeach()

