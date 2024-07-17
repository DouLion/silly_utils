cmake_minimum_required(VERSION 3.15)

project(example_project)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
include(silly_utils)


# 设置默认程序根目录,程序中所有路径均参照此路径,方便调试和发布
if(IS_DEBUG)
    add_definitions("-DDEFAULT_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
else()
    add_definitions("-DDEFAULT_ROOT_DIR=\"./\"")
endif()

# 添加core目录下的头文件到项目
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core)

# 添加目录
add_subdirectory(core)  # core代码
add_subdirectory(app)  # svr项目
add_subdirectory(web)  # web项目











