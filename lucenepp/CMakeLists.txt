####################################
# init
####################################

cmake_minimum_required(VERSION 3.5)

project(LucenePP)

set(LucenePP_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(LucenePP_VERSION_MAJOR 3)
set(LucenePP_VERSION_MINOR 0)
set(LucenePP_VERSION_PATCH 9)
set(LucenePP_SOVERSION "0")

set(LucenePP_VERSION
  "${LucenePP_VERSION_MAJOR}.${LucenePP_VERSION_MINOR}.${LucenePP_VERSION_PATCH}")


# set default build type as release
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

####################################
# CMake Modules
####################################

# include specific modules
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${LucenePP_SOURCE_ROOT}/cmake")
# fetch dependencies
include(dependencies)

# Enable C++11
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

####################################
# platform specific options
####################################
if(WIN32 OR WIN64)
 add_compile_options(/utf-8)
endif()

if(MSVC)
# Disable automatic boost linking on Windows as libraries are added to the linker explicitly
# add_definitions(-DBOOST_ALL_NO_LIB)

# enable exceptions, see http://msdn.microsoft.com/en-us/library/1deeycx5.aspx
add_definitions(-EHsc)

# Disable including too many Windows headers
add_definitions(-DWIN32_LEAN_AND_MEAN)

# Disable the min/max macros that conflict with std::min/std::max
add_definitions(-DNOMINMAX)
endif()

if(NOT WIN32 AND NOT CMAKE_SYSTEM MATCHES "SunOS-5*.")
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(CYGWIN)
  add_definitions(-D__LARGE64_FILES)
endif()

if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
  set(CMAKE_SKIP_BUILD_RPATH FALSE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
  if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  endif()
endif()


####################################
# bootstrap
####################################

include(TestCXXAcceptsFlag)

add_subdirectory(include)
add_subdirectory(src)

message("\n\n** LucenePP Build Summary **")
message("  Version:          ${LucenePP_VERSION}")
message("  Prefix:           ${CMAKE_INSTALL_PREFIX}")
message("  Build Type:       ${CMAKE_BUILD_TYPE}")
message("  Architecture:     ${CMAKE_SYSTEM_PROCESSOR}")
message("  System:           ${CMAKE_SYSTEM_NAME}")
message("  Boost Include:    ${Boost_INCLUDE_DIRS}")
message("  Boost Libraries:  ${Boost_LIBRARY_DIRS}")
message("  Zlib Include:     ${ZLIB_INCLUDE_DIRS}")
message("  Zlib Library:     ${ZLIB_LIBRARY_RELEASE}")
