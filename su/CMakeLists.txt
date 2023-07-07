cmake_minimum_required(VERSION 3.15)

project(sutils CXX)

foreach(dirname ${CMAKE_CURRENT_SOURCE_DIR})
    file(GLOB_RECURSE SU_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
            "${dirname}/*.h"
            "${dirname}/*.hpp"
            "${dirname}/*.c"
            "${dirname}/*.cpp"
            )
   
endforeach()


find_path(OTL_INCLUDE_DIRS "otl/otlv4.h")
include_directories(${OTL_INCLUDE_DIRS})

add_library(${PROJECT_NAME} STATIC ${SU_SRC_FILES})

set(Boost_LIB_PREFIX "")
set(CMAKE_VERBOSE_MAKEFILE ON)
# set(Boost_DEBUG TRUE)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_NO_WARN_NEW_VERSIONS 1)

### Packages 
find_package(Boost REQUIRED COMPONENTS system thread date_time chrono regex filesystem unit_test_framework program_options iostreams locale serialization )
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::boost Boost::system Boost::thread Boost::date_time Boost::chrono Boost::regex Boost::filesystem Boost::unit_test_framework Boost::program_options Boost::iostreams Boost::locale Boost::serialization) 

find_package(cryptopp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC cryptopp::cryptopp)

find_package(Iconv)
target_link_libraries(${PROJECT_NAME} PUBLIC Iconv::Iconv)

find_package(polyclipping CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC polyclipping::polyclipping)

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC JsonCpp::JsonCpp)

# 解压缩模块
find_package(ZLIB REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC ZLIB::ZLIB)

find_package(libzip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC libzip::zip)

find_package(BZip2 REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC BZip2::BZip2)