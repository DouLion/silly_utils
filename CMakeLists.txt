CMAKE_MINIMUM_REQUIRED(VERSION 3.15)
PROJECT(silly_utils)
INCLUDE(CheckIncludeFileCXX)

# 判断当前目录是否为 cmake的根目录,如果不是,则是被其他项目引用的,不需要编译测试,工具模块
IF ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    SET(SU_AS_PROJECT TRUE)
    SET(SU_AS_LIB FALSE)
ELSE ()
    SET(SU_AS_PROJECT FALSE)
    SET(SU_AS_LIB TRUE)
ENDIF ()

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE "Debug")
    MESSAGE("AUTO SET CMAKE_BUILD_TYPE : Debug")
ENDIF()

IF (SU_AS_PROJECT)

    # 设置默认程序根目录,程序中所有路径均参照此路径,方便调试和发布
    ADD_DEFINITIONS("-DDEFAULT_SU_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")

    ADD_DEFINITIONS("-DDEFAULT_SU_DATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/data\"")
    SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
    INCLUDE(env_check)
    INCLUDE(dm8_check)
    INCLUDE(grib_api_check)
    
ENDIF ()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/su)

##########################################
# Add sub dirs.
##########################################
ADD_SUBDIRECTORY(su)
IF (SU_AS_PROJECT)
    ADD_SUBDIRECTORY(test)
    ADD_SUBDIRECTORY(tools)
ENDIF ()
