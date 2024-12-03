cmake_minimum_required(VERSION 3.15)
project(silly_utils)

# 判断当前目录是否为 cmake的根目录,如果不是,则是被其他项目引用的,不需要编译测试,工具模块
message(STATUS "su 当前目录(CMAKE_CURRENT_SOURCE_DIR) ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "su 根目录 (CMAKE_SOURCE_DIR) ${CMAKE_SOURCE_DIR}")

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
	message(STATUS "作为单个工程编译")
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

	include(CheckEnv)
	include(CheckOSEndian)
	include(CheckDM8)
	include(CheckEccodes)
	include(CheckCUDA)

	include_directories(${CMAKE_SOURCE_DIR}/su)
	add_definitions("-DDEFAULT_SU_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
    add_definitions("-DDEFAULT_SU_DATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/data\"")
	add_subdirectory(su)
	add_subdirectory(test)
	add_subdirectory(tools)
	add_subdirectory(extend)
else()
	message(STATUS "作为嵌入库编译")
	add_subdirectory(su)
endif()