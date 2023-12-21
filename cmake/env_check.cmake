cmake_minimum_required(VERSION 3.10.0)
# 系统信息
message(${CMAKE_HOST_SYSTEM_NAME})
message(${CMAKE_HOST_SYSTEM_PROCESSOR})
 
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "This is Linux")
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Android")
	message(STATUS "This is Android")
ELSEIF (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "This is Windows")
endif()

if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
    message(STATUS "this is aarch64 cpu")
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
	message(STATUS "this is x86_64 cpu")
endif()

# 检查系统位数
IF (CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(ARCH_NAME "x64")
    MESSAGE(STATUS "Target is 64 bits")
ELSE ()
    SET(ARCH_NAME "x32")
    MESSAGE(STATUS "Target is 32 bits")
ENDIF ()



# c++ 17 已经包含一些类型定义了,可能会导致重复定义
ADD_DEFINITIONS("-D_HAS_STD_BYTE=0")
# 命令行解析库里面的std::max
ADD_DEFINITIONS("-DNOMINMAX")
# 指定项目编码类型 unicode 不指定默认utf8 ???
ADD_DEFINITIONS("-DUNICODE")
IF (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    SET(IS_LINUX TRUE)
    SET(PLATFORM_NAME "Linux")
    SET(CMAKE_THREAD_LIBS_INIT "-lpthread")
    SET(CMAKE_HAVE_THREADS_LIBRARY 1)
    SET(CMAKE_USE_WIN32_THREADS_INIT 0)
    SET(CMAKE_USE_PTHREADS_INIT 1)
    SET(THREADS_PREFER_PTHREAD_FLAG ON)

    SET(dm_include_path /opt/dmdbms/include)
    SET(dm_lib_path /opt/dmdbms/bin)
    ADD_DEFINITIONS("-DDM8_ENABLED")
    LINK_DIRECTORIES("/opt/dmdbms/bin")
    INCLUDE_DIRECTORIES(${dm_include_path})
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17  -lodbc -lpthread -fPIC -Wall")
    SET(STATIC_LIB_SUFFIX "a")
    SET(DYNAMIC_LIB_SUFFIX "so")
ELSEIF (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    SET(PLATFORM_NAME "Windows")
    SET(IS_WINDOWS TRUE)
    ADD_DEFINITIONS("-DTFF_INLINE_SPECIFIER=inline")
    ADD_DEFINITIONS("-DIS_WIN32")
    ADD_DEFINITIONS("-DWIN32_LEAN_AND_MEAN")
    add_compile_options(/wd4819 /wd4005 /wd4834 /utf-8)
    SET(STATIC_LIB_SUFFIX "lib")
    SET(DYNAMIC_LIB_SUFFIX "dll")

    IF (MSVC)
        # SET cmake cxx flags.
        SET(CMAKE_CXX_FLAGS_DEBUG "")
        SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_DEBUG /MDd /Zi /Ob0  /Od /RTC1 /Gy /EHsc")

        SET(CMAKE_CXX_FLAGS_MINSIZEREL "")
        SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MD /Zi /O1 /Ob2 /Oi /Os /D NDEBUG /GS- ")

        SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
        SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MD /Zi /O2 /Ob1 /D NDEBUG ")

        SET(CMAKE_CXX_FLAGS_RELEASE "")
        SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /Zi /O2 /Ob1 /D NDEBUG ")
    ENDIF (MSVC)

ENDIF ()


##########################################
# 设置全局的可执行程序和链接库的生成路径.
##########################################
SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/Bin/${PLATFORM_NAME}/${ARCH_NAME}/${CMAKE_BUILD_TYPE}")
SET(LIBRARY_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}")