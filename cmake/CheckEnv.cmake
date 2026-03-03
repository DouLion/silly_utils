cmake_minimum_required(VERSION 3.15)

if(CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()
if(CMP0167)
  cmake_policy(SET CMP0167 NEW)
  set(NEW_BOOST_FIND ON)
endif()

include(CheckIncludeFileCXX)

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE "Debug")
  message("AUTO SET CMAKE_BUILD_TYPE : Debug")
endif()

string(TOUPPER "${CMAKE_BUILD_TYPE}" UPPER_CMAKE_BUILD_TYPE)
if("DEBUG" STREQUAL "${UPPER_CMAKE_BUILD_TYPE}")
set(DEBUG_MODE 1)
  message(STATUS "Debug 模式")
else()
  set(DEBUG_MODE 0)
  message(STATUS "Release 模式")
endif()

# 系统信息
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
  message(STATUS "Linux")
  set(IS_LINUX TRUE)
  set(PLATFORM_NAME "Linux")
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Android")
  message(STATUS "Android")
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
  message(STATUS "Windows")
  set(PLATFORM_NAME "Windows")
  set(IS_WINDOWS TRUE)
else()
  message( FATAL_ERROR  " ----- Unknown")
endif()

if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
  message(STATUS "this is aarch64 cpu")
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
  message(STATUS "this is x86_64 cpu")
endif()

# 检查系统位数
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCH_NAME "x64")
  message(STATUS "Target is 64 bits")
else()
  set(ARCH_NAME "x32")
  message(STATUS "Target is 32 bits")
  message(FATAL_ERROR "Not support on 32 bit platform")
endif()

# message("当前C++编译器支持的C++ 特性:")
foreach(feature ${CMAKE_CXX_COMPILE_FEATURES})
  if("${feature}" STREQUAL "cxx_std_11")
    add_definitions("-DSU_SUPPORT_CXX11=1")
    set(SU_SUPPORT_CXX11 TRUE)
  endif()
  if("${feature}" STREQUAL "cxx_std_17")
    add_definitions("-DSU_SUPPORT_CXX17=1")
    set(SU_SUPPORT_CXX17 TRUE)
  endif()
  if("${feature}" STREQUAL "cxx_std_20")
    # add_definitions("-DSU_SUPPORT_CXX20=1") set(SU_SUPPORT_CXX20 TRUE)
  endif()

endforeach()

# c++ 支持版本
if(SU_SUPPORT_CXX20)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  message("使用C++20标准")
elseif(SU_SUPPORT_CXX17)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  message("使用C++17标准")
elseif(SU_SUPPORT_CXX11)
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  message("使用C++11标准")
endif()

add_definitions("-DENABLE_SILLY_LOG")
# 命令行解析库里面的std::max
add_definitions("-DNOMINMAX")
# 指定项目编码类型 unicode 不指定默认utf8 ???
add_definitions("-DUNICODE")
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
  # ===== Linux 配置 =====
  set(CMAKE_THREAD_LIBS_INIT "-lpthread")
  set(CMAKE_HAVE_THREADS_LIBRARY 1)
  set(CMAKE_USE_WIN32_THREADS_INIT 0)
  set(CMAKE_USE_PTHREADS_INIT 1)
  set(THREADS_PREFER_PTHREAD_FLAG ON)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lpthread -fPIC -lc -Wall -fopenmp -Wno-unused-function -Wno-unused-variable -lodbc")
  set(STATIC_LIB_SUFFIX "a")
  set(DYNAMIC_LIB_SUFFIX "so")

elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
  # ===== Windows 配置 (通用) =====
  add_definitions("-DIS_WIN32")
  add_definitions("-DWIN32_LEAN_AND_MEAN")
  set(STATIC_LIB_SUFFIX "lib")
  set(DYNAMIC_LIB_SUFFIX "dll")

  # ===== 区分 MSVC 和 MinGW/GCC =====
  if(MSVC)
    # ===== MSVC 特定配置 =====
    add_compile_options(/wd4819 /wd4005 /wd4834 /wd4996 /utf-8 /openmp)

    # 去掉了手动的 /D_DEBUG 和 /D NDEBUG，CMake 会自动处理这些宏
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Ob0 /Od /RTC1 /EHsc /MDd")

    # MinSizeRel: 最小体积
    set(CMAKE_CXX_FLAGS_MINSIZEREL "/O1 /Zi /Ob2 /Oi /Os /EHsc /MD /D NDEBUG")

    # RelWithDebInfo: 发布带调试信息 (保持原样)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2 /Zi /Ob1 /EHsc /MD /D NDEBUG")

    # 1. 保留 /Zi：编译器必须生成包含调试信息的 .obj 文件
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Zi /Ob1 /EHsc /MD /D NDEBUG")

    # ---------------------------------------------------------
    # 2. 链接器标志 (Linker Flags)
    # ---------------------------------------------------------

    # 基础优化选项 (Release 模式标配)
    set(CMAKE_EXE_LINKER_FLAGS_BASE_RELEASE "/OPT:REF /OPT:ICF")
    set(CMAKE_SHARED_LINKER_FLAGS_BASE_RELEASE "/OPT:REF /OPT:ICF")

    # /DEBUG:FASTLINK 告诉链接器生成快速加载的 PDB
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_BASE_RELEASE} /DEBUG:FASTLINK")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_BASE_RELEASE} /DEBUG:FASTLINK")

    # ---------------------------------------------------------
    # 3. 确保 PDB 文件名与目标一致 (避免 vc140.pdb 冲突)
    # ---------------------------------------------------------
    # 虽然 /DEBUG:FASTLINK 通常会自动生成 <target>.pdb，
    # 但为了保险，我们可以强制 CMake 将 PDB 输出到运行时目录
    set(CMAKE_PDB_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

  else()
    # ===== MinGW (或任何非 MSVC 的 Windows 编译器，如 Clang) =====
    # 假设是 GCC/Clang 风格的编译器

    # 启用 OpenMP 支持
    add_compile_options(-fopenmp)
    # 如果需要链接 OpenMP 库 (CMake 有时需要)
    # target_link_libraries(your_target PRIVATE -lgomp) # 对于 MinGW-w64

    # 设置 C++ 标准和通用警告
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    # 或者直接添加到标志
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

    # 添加编译选项 (GCC/Clang 风格)
    add_compile_options(-Wall -Wno-unused-function -Wno-unused-variable)
    # MinGW 可能需要 -lodbc 来链接 ODBC
    # add_compile_options(-lodbc) # 通常更推荐在 target_link_libraries 中处理

    # 线程支持: MinGW 通常使用 pthreads
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    # 在链接目标时使用: target_link_libraries(your_target PRIVATE Threads::Threads)

    # fPIC 对于创建共享库很有用
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    # 设置 MinGW 特定的编译标志 (如果需要)
    # 例如，解决宽字符问题 (类似于 MSVC 的 /wd4819)
    # add_compile_options(-municode) # 如果需要 Unicode 支持
    add_compile_options(-fexec-charset=UTF-8 -finput-charset=UTF-8) # 确保源码是 UTF-8

    # 警告抑制 (GCC/Clang 风格)
    # add_compile_options(-Wno-multichar) # 示例
    # 注意: /wd4996 在 GCC 中对应的是 -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

    # MinGW 的静态库后缀是 .a，但为了与 MSVC 一致或通用性，可以保留 .lib
    # 但通常 MinGW 项目也使用 .a。这里根据你的项目习惯选择。
    # set(STATIC_LIB_SUFFIX "a") # 如果你想为 MinGW 使用 .a
    # 我们保留上面设置的 .lib，但要知道 MinGW 也能处理 .lib (作为静态库)
    include_directories(${SU_FILE_ROOT}/mingw)
  endif()

  # --- Windows 下通用的链接设置 ---
  # 将 ODBC 链接放在 Windows 通用部分，因为它可能在 MSVC 和 MinGW 中都需要
  # 但更推荐在具体 target_link_libraries 中处理
  # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lodbc")

endif()

# ##############################################################################
# 设置全局的可执行程序和链接库的生成路径.
# ##############################################################################
set(PROJECT_ROOT "${CMAKE_SOURCE_DIR}")
if(DEBUG_MODE)
    add_definitions(-DPROJECT_ROOT="${PROJECT_ROOT}")
else()
    add_definitions(-DPROJECT_ROOT="./")
endif()

set(EXECUTABLE_OUTPUT_PATH
    "${PROJECT_ROOT}/Bin/${PLATFORM_NAME}/${ARCH_NAME}/${CMAKE_BUILD_TYPE}")
set(LIBRARY_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}")


