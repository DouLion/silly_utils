cmake_minimum_required(VERSION 3.15)
project(silly_utils)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

include(CheckEnv)
include(CheckOSEndian)
include(CheckDM8)
include(CheckEccodes)
include(CheckCUDA)

# 判断当前目录是否为 cmake的根目录,如果不是,则是被其他项目引用的,不需要编译测试,工具模块
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
  set(SU_AS_PROJECT TRUE)
  set(SU_AS_LIB FALSE)
else()
  set(SU_AS_PROJECT FALSE)
  set(SU_AS_LIB TRUE)
endif()

if(SU_AS_PROJECT)
  message(STATUS "作为单个工程编译")
  # 设置默认程序根目录,程序中所有路径均参照此路径,方便调试和发布
  add_definitions("-DDEFAULT_SU_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")

  add_definitions("-DDEFAULT_SU_DATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/data\"")
elseif()
  message(STATUS "作为嵌入库编译")
endif()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/su)

# ##############################################################################
# Add sub dirs.
# ##############################################################################

add_subdirectory(su)

if(SU_AS_PROJECT)
  add_subdirectory(test)
  add_subdirectory(tools)
  add_subdirectory(extend)
endif()