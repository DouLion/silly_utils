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