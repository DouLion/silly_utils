cmake_minimum_required(VERSION 3.15)

project(silly_utils)

include(CheckIncludeFileCXX)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


# 指定 c++ 最高版本
check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# c++ 17 已经包含一些类型定义了,可能会导致重复定义
add_definitions("-D_HAS_STD_BYTE=0")
# 命令行解析库里面的std::max
add_definitions("-DNOMINMAX")
# 指定项目编码类型 unicode 不指定默认utf8 ???
add_definitions("-DUNICODE")
if (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(IS_LINUX TRUE)
    set(PLATFORM_NAME "Linux")
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17  -lodbc -lpthread -fPIC -L. /usr/local/ssl/lib64/libssl.a /usr/local/ssl/lib64/libcrypto.a")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17  -lodbc -lpthread -fPIC -Wl,-Bstatic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17  -lodbc -lpthread -fPIC ")
elseif (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(PLATFORM_NAME "Windows")
    set(IS_WINDOWS TRUE)
    add_definitions("-DTFF_INLINE_SPECIFIER=inline")
    add_definitions("-DIS_WIN32")
    add_definitions("-DWIN32_LEAN_AND_MEAN")
    add_compile_options(/wd4819 /wd4005 /wd4834)

    if(MSVC)
        # Set cmake cxx flags.
        set(CMAKE_CXX_FLAGS_DEBUG "")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_DEBUG /MDd /Zi /Ob0  /Od /RTC1 /Gy /EHsc")

        set(CMAKE_CXX_FLAGS_MINSIZEREL "")
        set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MD /Zi /O1 /Ob2 /Oi /Os /D NDEBUG /GS- ")

        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MD /Zi /O2 /Ob1 /D NDEBUG ")

        set(CMAKE_CXX_FLAGS_RELEASE "")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /Zi /O2 /Ob1 /D NDEBUG ")
    endif(MSVC)

endif ()

include_directories(${CMAKE_SOURCE_DIR}/su)

##########################################
# Add sub dirs.
##########################################
add_subdirectory(su)
if(NO_BUILD_SU_TEST)
else()
add_subdirectory(test)
endif()