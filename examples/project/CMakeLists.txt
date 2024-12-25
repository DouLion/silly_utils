cmake_minimum_required(VERSION 3.15)

project(example_project)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
include(FetchSU)

include_directories(${CMAKE_SOURCE_DIR}/core)

# 设置默认程序根目录,程序中所有路径均参照此路径,方便调试和发布
if(DEBUG_MODE)
    set(DEFAULT_CONFIG_DIR "${CMAKE_SOURCE_DIR}/config")
    add_definitions("-DDEFAULT_CONFIG_DIR=\"${DEFAULT_CONFIG_DIR}\"")
    add_definitions("-DDEFAULT_ROOT_DIR=\"${CMAKE_SOURCE_DIR}\"")
else()
    add_definitions("-DDEFAULT_ROOT_DIR=\"./\"")
    add_definitions("-DDEFAULT_CONFIG_DIR=\"./config\"")
endif()

add_subdirectory(core)
add_subdirectory(app)
add_subdirectory(web)





