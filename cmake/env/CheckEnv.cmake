# CheckEnv.cmake
# 检查构建环境（平台、编译器、C++ 标准等）

message("\n===== 检查构建环境 =====")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# 确保 C++17 标准
if(CMAKE_CXX_STANDARD LESS 17)
    message(FATAL_ERROR "silly_utils 最低要求 C++17，当前版本：${CMAKE_CXX_STANDARD}")
endif()

# 设置默认构建类型
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
    message("AUTO SET CMAKE_BUILD_TYPE: Debug")
endif()

string(TOUPPER "${CMAKE_BUILD_TYPE}" UPPER_CMAKE_BUILD_TYPE)
if("DEBUG" STREQUAL "${UPPER_CXX_BUILD_TYPE}")
    set(DEBUG_MODE 1)
    message("构建模式：Debug")
else()
    set(DEBUG_MODE 0)
    message("构建模式：Release")
endif()

# 平台检测
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(IS_LINUX TRUE)
    set(PLATFORM_NAME "Linux")
    message("平台：Linux")
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(IS_WINDOWS TRUE)
    set(PLATFORM_NAME "Windows")
    message("平台：Windows")
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Android")
    message("平台：Android")
else()
    message(FATAL_ERROR "不支持的平台：${CMAKE_HOST_SYSTEM_NAME}")
endif()

# 架构检测
if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
    message("CPU: aarch64")
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
    message("CPU: x86_64")
else()
    message("CPU: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()

# 64 位检查
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_NAME "x64")
    message("架构：64-bit")
else()
    set(ARCH_NAME "x32")
    message(FATAL_ERROR "不支持 32 位平台")
endif()

# 编译器检测
if(MSVC)
    message("编译器：MSVC ${CMAKE_CXX_COMPILER_VERSION}")
    # MSVC 特定设置
    add_definitions(-DIS_WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DUNICODE)
    add_compile_options(/wd4819 /wd4005 /wd4834 /wd4996 /utf-8 /Zc:__cplusplus)
else()
    message("编译器：${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
endif()

# C++ 标准检查
foreach(feature ${CMAKE_CXX_COMPILE_FEATURES})
    if("${feature}" STREQUAL "cxx_std_17")
        add_definitions(-DSU_SUPPORT_CXX17=1)
        set(SU_SUPPORT_CXX17 TRUE)
    endif()
    if("${feature}" STREQUAL "cxx_std_20")
        # 可选支持 C++20
        add_definitions(-DSU_SUPPORT_CXX20=1)
        set(SU_SUPPORT_CXX20 TRUE)
    endif()
endforeach()

if(SU_SUPPORT_CXX17)
    message("C++ 标准：C++17")
endif()
if(SU_SUPPORT_CXX20)
    message("C++ 标准：C++20 (可用)")
endif()

# 通用编译选项
add_definitions(-DENABLE_SILLY_LOG)
add_definitions(-DNOMINMAX)
add_definitions(-DUNICODE)

if(DEBUG_MODE)
    add_definitions(-DPROJECT_ROOT="${CMAKE_SOURCE_DIR}")
else()
    add_definitions(-DPROJECT_ROOT="./")
endif()

message("===== 环境检查完成 =====\n")
