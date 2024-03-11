# silly_utils

通用工具模块,目的是能够再cmake中直接引用该项目代码



## 使用方式

+ 拷贝本项目目录下cmake/silly_utils.cmake文件,到目标项目根目录cmake/silly_utils.cmake,或者按照自己的需要修改:

  ```cmake
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
  execute_process(COMMAND ${CMAKE_COMMAND} -E  copy "${SILLY_UTILS_FETCH_ROOT}/silly_utils/cmake/env_check.cmake" "${CMAKE_SOURCE_DIR}/su_cmake/env_check.cmake")
  execute_process(COMMAND ${CMAKE_COMMAND} -E  copy "${SILLY_UTILS_FETCH_ROOT}/silly_utils/.clang-format" "${CMAKE_SOURCE_DIR}/.clang-format")
  # 下面这两行需要确认一下是否可以删除
  execute_process(COMMAND ${CMAKE_COMMAND} -E  copy "${SILLY_UTILS_FETCH_ROOT}/silly_utils/cmake/dm8_check.cmake" "${CMAKE_SOURCE_DIR}/su_cmake/dm8_check.cmake")
  execute_process(COMMAND ${CMAKE_COMMAND} -E  copy "${SILLY_UTILS_FETCH_ROOT}/silly_utils/cmake/grib_api_check.cmake" "${CMAKE_SOURCE_DIR}/su_cmake/grib_api_check.cmake")
  
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/su_cmake;${CMAKE_MODULE_PATH}")
  include(env_check)
  include(dm8_check)
  include(grib_api_check)
  
  include_directories("${SILLY_UTILS_FETCH_ROOT}/silly_utils/su")
  
  
  ```

+ 在项目根目录的CMakeList.txt添加如下内容

  ```cmake
  # 会完成与silly_utils一致的环境配置,添加silly_utils头文件目录等信息
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
  include(silly_utils)
  ```

+ 以[动态预警项目](http://123.56.193.136:16080/Warn/DynamicWarn)为例

  ```cmake
  cmake_minimum_required(VERSION 3.15)
  project(DynamicWarn)
  
  # 只需要添加如下两行
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
  include(silly_utils)
  
  
  if(IS_DEBUG)
      add_definitions("-DDEFAULT_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")
  else()
      # add_definitions("-DDEFAULT_ROOT_DIR=\"\"")
      add_definitions("-DDEFAULT_ROOT_DIR=\"./\"")
  endif()
  
  set(DEFAULT_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Config")
  add_definitions("-DDEFAULT_CONFIG_DIR=\"${DEFAULT_CONFIG_DIR}\"")
  
  include_directories("${CMAKE_SOURCE_DIR}/core")
  
  add_subdirectory(core)
  add_subdirectory(web)
  add_subdirectory(risk_web)
  add_subdirectory(app)
  add_subdirectory(tool)
  ```

+ 记得要链接这个库

  ```cmake
  target_link_libraries(${PROJECT_NAME} PUBLIC sutils)
  ```


## OPENMP
    https://blog.csdn.net/ab0902cd/article/details/108770396
    https://blog.csdn.net/jjjstephen/article/details/132201248
    https://blog.csdn.net/yongbaofeng1234/article/details/88651113
    https://zhuanlan.zhihu.com/p/61857547

## VCPKG依赖库:

+ windows 下执行:

  ```
  vcpkg install msinttypes boost gdal zlib bzip2 libzip jsoncpp polyclipping cryptopp libiconv otl curl netcdf-cxx4 rapidjson sqlite3 libspatialite libgeotiff protozero libpng libjpeg-turbo simpleini

  ```


+ linux下额外安装

  ```
  vcpkg install geos freexl curl librttopo proj libxml2 tiff libuuid
  ```
  
  

## 功能列表

+ 见FUNCTIONS.md文件

