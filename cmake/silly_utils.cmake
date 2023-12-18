# 添加第三方依赖包
INCLUDE(FetchContent)
# FetchContent_MakeAvailable was not added until CMake 3.14
IF (${CMAKE_VERSION} VERSION_LESS 3.14)
    INCLUDE(add_FetchContent_MakeAvailable.cmake)
ENDIF ()
# FILE(MAKE_DIRECTORY ${SU_DIRECTORY})
SET(SOURCE_DIR "${SU_DIRECTORY}")
SET(SILLY_UTIL_TAG master)  # 指定版本
# SET(SILLY_UTILS_GIT_URL  http://123.56.193.136:16080/douliyang/silly_utils.git)  # 指定git仓库地址,外网地址,速度可能比较慢
SET(SILLY_UTILS_GIT_URL http://192.168.0.60/douliyang/silly_utils.git)  # 指定git仓库地址, 内网地址,内网开发用这个

FetchContent_Declare(
        silly_utils
        GIT_REPOSITORY ${SILLY_UTILS_GIT_URL}
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/silly_utils
)

FetchContent_MakeAvailable(silly_utils)