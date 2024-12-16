#!/usr/bin/env python
# -*- encoding: utf-8 -*-
"""
@copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights reserved.
            北京天智祥信息科技有限公司版权所有
@website: http://www.tianzhixiang.com.cn/
@author: dou li yang
@date: 2024-11-20
@file: auto_gen.py
@description: 自动生成带sutil的cmake项目
@version: v1.0.1 2024-11-20 dou li yang
"""

cmake_fitch_content = """
# 添加第三方依赖包
include(FetchContent)
# FetchContent_MakeAvailable was not added until CMake 3.14
if(${CMAKE_VERSION} VERSION_LESS 3.14)
    include(add_FetchContent_MakeAvailable.cmake)
endif()

# 设置silly_utils的下载目录
if("${SILLY_UTILS_FETCH_ROOT}" STREQUAL "")
    set(SILLY_UTILS_FETCH_ROOT ${CMAKE_SOURCE_DIR}/external)
endif()

# 指定git仓库地址, 如果能连接192.168.0.60,使用该地址,速度会快一点,如果不能,使用123.56.193.136:16080,这个是.net的公网地址
if("${SILLY_UTILS_FETCH_IP_PORT}" STREQUAL "")
    set(SILLY_UTILS_FETCH_IP_PORT "192.168.0.60")
endif()

# 指定版本
set(SILLY_UTIL_TAG  master)  
set(SILLY_UTILS_GIT_URL  "git@${SILLY_UTILS_FETCH_IP_PORT}:douliyang/silly_utils.git")


FetchContent_Declare(
  silly_utils
  GIT_REPOSITORY    ${SILLY_UTILS_GIT_URL}
  SOURCE_DIR        ${SILLY_UTILS_FETCH_ROOT}/silly_utils
)

FetchContent_MakeAvailable(silly_utils)

execute_process(COMMAND ${CMAKE_COMMAND} -E  copy "${SILLY_UTILS_FETCH_ROOT}/silly_utils/.clang-format" "${CMAKE_SOURCE_DIR}/.clang-format")
#
set(CMAKE_MODULE_PATH "${SILLY_UTILS_FETCH_ROOT}/silly_utils/cmake;${CMAKE_MODULE_PATH}")
message(STATUS "${CMAKE_MODULE_PATH}")
include(CheckEnv)
include(CheckOSEndian)
include(CheckDM8)
include(CheckEccodes)
include(CheckCUDA)

include_directories("${SILLY_UTILS_FETCH_ROOT}/silly_utils/su")
"""

cmake_root_content = """
project(DynamicWarn CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
include(FetchSU)
# 设置默认程序根目录,程序中所有路径均参照此路径,方便调试和发布
if(IS_DEBUG)
    add_definitions("-DDEFAULT_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
else()
    # add_definitions("-DDEFAULT_ROOT_DIR=\"\"")
    add_definitions("-DDEFAULT_ROOT_DIR=\"./\"")
endif()

##########################################
# global define.
##########################################
set(DEFAULT_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Config")
add_definitions("-DDEFAULT_CONFIG_DIR=\"${DEFAULT_CONFIG_DIR}\"")


##########################################.
# global include.
##########################################
include_directories("${CMAKE_SOURCE_DIR}/core")

##########################################
# Add sub dirs.
######################+####################
add_subdirectory(core)
add_subdirectory(web)
add_subdirectory(app)
add_subdirectory(tool)
"""


class ProjectOpt:
    def __init__(self):
        self.name
        self.use_cuda = False
        self.use_dm8 = False
        self.use_eccodes = False

        self.git_url = ""
        self.git_auto_add = False


if __name__ == '__main__':

    try:
        opt = ProjectOpt()
        opt.name = input("请输入工程名称:")
        print("工程名称:", opt.name)
        print("是否使用CUDA:", end="")
        opt.use_cuda = input().lower() == "y"
        print("是否使用DM8:", end="")
        opt.use_dm8 = input().lower() == "y"
        print("是否使用Eccodes:", end="")
        opt.use_eccodes = input().lower() == "y"
        opt.git_url = input("Git项目路径:")
        print("是否自动添加到git:", end="")
        opt.git_auto_add = input().lower() == "y"
        if opt.git_auto_add:
            print("请输入git用户名:")


    except Exception as e:
        print(e)

    exit(0)
