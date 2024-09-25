# 添加第三方依赖包
include(FetchContent)
# FetchContent_MakeAvailable was not added until CMake 3.14
if(${CMAKE_VERSION} VERSION_LESS 3.14)
    include(add_FetchContent_MakeAvailable.cmake)
endif()

# 设置silly_utils的下载目录
if("${SILLY_UTILS_FETCH_ROOT}" STREQUAL "")
    set(SILLY_UTILS_FETCH_ROOT ${CMAKE_SOURCE_DIR}/su_ext)
endif()

# 指定git仓库地址, 如果能连接192.168.0.60,使用该地址,速度会快一点,如果不能,使用123.56.193.136:16080,这个是.net的公网地址
if("${SILLY_UTILS_FETCH_IP_PORT}" STREQUAL "")
    set(SILLY_UTILS_FETCH_IP_PORT "192.168.0.60")
endif()

# 指定版本
set(SILLY_UTIL_TAG  master)  
set(SILLY_UTILS_GIT_URL  "http://${SILLY_UTILS_FETCH_IP_PORT}/douliyang/silly_utils.git")  


FetchContent_Declare(
  silly_utils
  GIT_REPOSITORY    ${SILLY_UTILS_GIT_URL}
  SOURCE_DIR        ${SILLY_UTILS_FETCH_ROOT}/silly_utils
)

FetchContent_MakeAvailable(silly_utils)

# 下载完成后 拷贝检测环境的.cmake文件
IF(NOT EXISTS "${CMAKE_SOURCE_DIR}/su_cmake")
    FILE(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/su_cmake")
ENDIF()
execute_process(COMMAND ${CMAKE_COMMAND} -E  copy "${SILLY_UTILS_FETCH_ROOT}/silly_utils/cmake/CheckEnv.cmake" "${CMAKE_SOURCE_DIR}/su_cmake/CheckEnv.cmake")
execute_process(COMMAND ${CMAKE_COMMAND} -E  copy "${SILLY_UTILS_FETCH_ROOT}/silly_utils/.clang-format" "${CMAKE_SOURCE_DIR}/.clang-format")
# 下面这两行需要确认一下是否可以删除
execute_process(COMMAND ${CMAKE_COMMAND} -E  copy "${SILLY_UTILS_FETCH_ROOT}/silly_utils/cmake/dm8_check.cmake" "${CMAKE_SOURCE_DIR}/su_cmake/dm8_check.cmake")
execute_process(COMMAND ${CMAKE_COMMAND} -E  copy "${SILLY_UTILS_FETCH_ROOT}/silly_utils/cmake/eccodes_check.cmake" "${CMAKE_SOURCE_DIR}/su_cmake/eccodes_check.cmake")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/su_cmake;${CMAKE_MODULE_PATH}")
include(CheckEnv)
include(dm8_check)
include(eccodes_check)

include_directories("${SILLY_UTILS_FETCH_ROOT}/silly_utils/su")