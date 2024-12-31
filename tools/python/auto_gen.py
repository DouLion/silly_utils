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

import os

FETCH_SU_CONTENT = """
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

THIRD_SUPPORT_CONTENT = """
# 第三方库支持
set(ENABLE_ECCODES OFF) # GRIB2文件支持
set(ENABLE_CUDA OFF)
set(ENABLE_DM8 OFF)
set(ENABLE_GDAL OFF) # gdal支持
set(ENABLE_WEBSOCKET_PP OFF) # websocket支持
set(ENABLE_PAHO_MQTT OFF) # mqtt支持
"""

ROOT_CMAKE_CONTENT = """
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
include(ThirdSupport)
include(FetchSU)

##########################################
# global define.
##########################################
if(DEBUG_MODE)
    add_definitions(-DPROJ_ROOT="${CMAKE_SOURCE_DIR}")
else()
    add_definitions(-DPROJ_ROOT="./")
endif()

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
add_subdirectory(test)
add_subdirectory(tools)
"""

CORE_CMAKE_CONTENT = """
cmake_minimum_required(VERSION 3.15)
project(core CXX)

file(GLOB_RECURSE SOURCE_FILES RELATIVE
        ${CMAKE_CURRENT_SOURCE_DIR}
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
# 如果解开可以加速
#[[
target_precompile_headers(
        ${PROJECT_NAME} PUBLIC
        xxx.h
)]]

### Packages

target_link_libraries(${PROJECT_NAME} PUBLIC sutils)

"""

CORE_CONFIG_H = """/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 
 * @file: config.h
 * @description: config 类声明
 * @version: v1.0.1  dou li yang
 */
#ifndef CONFIG_H
#define CONFIG_H
#include <singleton/silly_singleton.h>
class config : public silly_singleton<config>
{
    friend class silly_singleton<config>;

  public:
    bool init(const std::string& file);

  protected:
    config() = default;
    ~config() = default;


  private:
};

#endif  // CONFIG_H

"""

CORE_CONFIG_CPP = """/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 
 * @file: config.cpp
 * @description: send_config实现
 * @version: v1.0.1 dou li yang
 */
#include "config.h"
bool config::init(const std::string& file)
{
    bool status = false;
  
    return status;
}

"""

IGNORE_CONTENT = """
.idea
.vs
.vscode
/cmake-build-*
/out
/Bin
"""

WEB_CMAKE_CONTENT = """
add_executable(${PROJECT_NAME} main.cpp)
# ##############################################################################

find_package(Drogon CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Drogon::Drogon)
target_link_libraries(${PROJECT_NAME} PUBLIC core)
# ##############################################################################
aux_source_directory(controllers CTL_SRC)
target_sources(${PROJECT_NAME} PRIVATE ${CTL_SRC})
"""
WEB_EXAMPLE_CPP = """
#pragma once
#include <drogon/drogon.h>
#include <log/silly_log.h>
#include <config/config.h>
int main(int argc, char** argv)
{
    if(silly_log::instance().init(argc, argv))
    {
        std::cerr << "日志加载失败" << std::endl;
        return -1;
    }
    // 初始化配置
    if (!config::instance().init(""))
    {
        SLOG_ERROR("配置加载失败");
        return -1;
    }
    int port = 999999;
    SLOG_INFO("监听端口:{}", port)
    drogon::app().addListener("0.0.0.0",port);
    drogon::app().setThreadNum(16);
    drogon::app().run();
    return 0;
}
"""

WEB_CTL_H = """
#pragma once
#include <drogon/HttpController.h>
#include <drogon/drogon.h>

using namespace drogon;

namespace project_name // 服务或者项目名称
{
namespace api  // 指明为API接口
{
class helper : public drogon::HttpController<helper>
{
public:
    METHOD_LIST_BEGIN
    // url路径尽量使用RUSTFUL风格   /资源名称/动作/限制副词   /宾语/谓语/形容副词
    // 完整url  ip:port/project_name/api/data/fetch/latest
    METHOD_ADD(helper::rotate_world, "/world/rotate",  Get, Post, Options);

    METHOD_LIST_END

   void rotate_world(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
};
} // namespace controllers
} // namespace web

"""

WEB_CTL_CPP = """
#include "helper.h"
#include <network/http/silly_http_server.h>

using namespace project_name::api;

void helper::rotate_world(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
    SU_CREATE_JSON_DEFAULT_RESPONSE;

    Json::Value data;
    data["msg"] = "旋转世界";
    respJson[SU_HTTP_JSON_RESPONSE_STATUS] = 1;
    respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = "Ok";
    respJson[SU_HTTP_JSON_RESPONSE_DATA] = data;

    SU_REQUEST_CALLBACK(silly_jsonpp::to_string(respJson));
}

"""

APP_CMAKE_CONTENT = """
cmake_minimum_required (VERSION 3.15)

# 添加任务应用
add_executable(00Example  "Example.cpp" )
target_link_libraries(00Example PUBLIC core)
"""

APP_EXAMPLE_CPP = """
#pragma once
#include <log/silly_log.h>
#include <config/config.h>
int main(int argc, char **argv)
{
    if(!silly_log::instance().init(argc, argv)) {
        std::cerr << "初始化日志失败" << std::endl;
        return -1;
    }
    SLOG_INFO("这是一个示例")
    if (!config::instance().init(""))
    {
        SLOG_ERROR("配置加载失败");
        return -1;
    }

return 0;
}
"""

TOOLS_CMAKE_CONTENT = """
cmake_minimum_required (VERSION 3.15)

# 添加工具xxx
add_executable(00Tool  "Tool00.cpp" )
target_link_libraries(00Tool PUBLIC core)
"""

TOOLS_EXAMPLE_CPP = APP_EXAMPLE_CPP

TEST_CMAKE_CONTENT = """
cmake_minimum_required (VERSION 3.15)

# 添加测试xxx
add_executable(00Test  "Test00.cpp" )
target_link_libraries(00Test PUBLIC core)
"""

TEST_EXAMPLE_CPP = APP_EXAMPLE_CPP


class ProjectOpt:
    def __init__(self):
        self.name = ""
        self.short_name = ""
        self.use_cuda = False
        self.use_dm8 = False
        self.use_eccodes = False
        self.git_url = ""
        self.git_auto_add = False
        self.port = 0


_opt = ProjectOpt()


def create_files():
    # 创建cmake 目录
    os.makedirs("cmake", exist_ok=True)
    os.makedirs("core/config", exist_ok=True)
    os.makedirs("config", exist_ok=True)
    os.makedirs("app", exist_ok=True)
    os.makedirs("tools", exist_ok=True)
    os.makedirs("test", exist_ok=True)

    with open("./cmake/FetchSU.cmake", "w", encoding="utf-8") as f:
        f.write(FETCH_SU_CONTENT)

    with open("./cmake/ThirdSupport.cmake", "w", encoding="utf-8") as f:
        f.write(THIRD_SUPPORT_CONTENT)
    hh = f'cmake_minimum_required(VERSION 3.15)\nproject({_opt.name})\n'
    with open("./CMakeLists.txt", "w", encoding="utf-8") as f:
        f.write(hh + ROOT_CMAKE_CONTENT)

    with open("./.gitignore", "w", encoding="utf-8") as f:
        f.write(IGNORE_CONTENT)

    with open('./core/' + _opt.short_name + '_marco.h', "w", encoding="utf-8") as f:
        f.write(f"""
#ifndef {_opt.short_name.upper()}_MARCO_H
#define {_opt.short_name.upper()}_MARCO_H
#include <singleton/silly_singleton.h>
#include <log/silly_log.h>
#include <json/silly_jsonpp.h>
#include <network/http/silly_http_server.h>
#include <string/silly_format.h>
#include <files/silly_file.h>

#include <datetime/silly_posix_time.h>
#include <datetime/silly_timer.h>
#include <string/silly_algorithm.h>

#endif //{_opt.short_name.upper()}_MARCO_H
        """)
    with open('./core/config/config.h', "w", encoding="utf-8") as f:
        f.write(CORE_CONFIG_H)
    with open('./core/config/config.cpp', "w", encoding="utf-8") as f:
        f.write(CORE_CONFIG_CPP)

    with open("./core/CMakeLists.txt", "w", encoding="utf-8") as f:
        f.write(CORE_CMAKE_CONTENT)

    # Web
    if _opt.port > 0:
        os.makedirs("web/controllers", exist_ok=True)

        with open("./web/CMakeLists.txt", "w", encoding="utf-8") as f:
            _CONTENT = f"cmake_minimum_required(VERSION 3.5)\nproject({_opt.name}_{_opt.port} CXX)\n"
            f.write(_CONTENT + WEB_CMAKE_CONTENT)

        with open("./web/main.cpp", "w", encoding="utf-8") as f:
            f.write(WEB_EXAMPLE_CPP)

        with open("./web/controllers/helper.h", "w", encoding="utf-8") as f:
            f.write(WEB_CTL_H)

        with open("./web/controllers/helper.cpp", "w", encoding="utf-8") as f:
            f.write(WEB_CTL_CPP)

    # app
    with open("./app/CMakeLists.txt", "w", encoding="utf-8") as f:
        f.write(APP_CMAKE_CONTENT)

    with open("./app/Example.cpp", "w", encoding="utf-8") as f:
        f.write(APP_EXAMPLE_CPP)

    # test
    with open("./test/CMakeLists.txt", "w", encoding="utf-8") as f:
        f.write(TEST_CMAKE_CONTENT)

    with open("./test/Test00.cpp", "w", encoding="utf-8") as f:
        f.write(TEST_EXAMPLE_CPP)

    # tools
    with open("./tools/CMakeLists.txt", "w", encoding="utf-8") as f:
        f.write(TOOLS_CMAKE_CONTENT)

    with open("./tools/Tool00.cpp", "w", encoding="utf-8") as f:
        f.write(TOOLS_EXAMPLE_CPP)


if __name__ == '__main__':

    try:
        _opt.name = input("工程全名:")
        _opt.name.strip()
        if len(_opt.name) == 0:
            raise Exception("工程名不能为空")
        print("工程全名:", _opt.name)
        _opt.short_name = input("工程名缩写:")
        _opt.short_name.strip()
        if len(_opt.short_name) == 0:
            raise Exception("工程名缩写不能为空")
        print("工程名缩写:", _opt.short_name)
        _p_port = input("占用端口:")
        _p_port.strip()
        if len(_p_port) == 0:
            _opt.port = 0
            print("不包含web模块")
        else:
            _opt.port = int(_p_port)
            print("占用端口:", _opt.port)
        create_files()
        '''
        _opt.git_url = input("Git项目路径:")
        print("是否自动添加到git:", end="")
        _opt.git_auto_add = input().lower() == "y"
        if _opt.git_auto_add:
            print("请输入git用户名:")
        '''

    except Exception as e:
        print(e)

    exit(0)
