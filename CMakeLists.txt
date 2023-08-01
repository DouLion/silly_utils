cmake_minimum_required(VERSION 3.15)

project(silly_utils)

include(CheckIncludeFileCXX)
if(SUB_PROJECT)


else()

# 设置默认程序根目录,程序中所有路径均参照此路径,方便调试和发布
if(IS_DEBUG)
    add_definitions("-DDEFAULT_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
else()
    # add_definitions("-DDEFAULT_ROOT_DIR=\"\"")
    add_definitions("-DDEFAULT_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
endif()

add_definitions("-DDEFAULT_DATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/data\"")
message("CC ${CMAKE_CURRENT_SOURCE_DIR}")
message("DD ${CMAKE_SOURCE_DIR}")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
include(env_check)
include(dm8_check)
include(grib_api_check)
include_directories(${CMAKE_SOURCE_DIR}/su)
endif()

##########################################
# Add sub dirs.
##########################################
add_subdirectory(su)
if(SUB_PROJECT)

else()
add_subdirectory(test)
endif()