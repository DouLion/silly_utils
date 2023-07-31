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
# 检查达梦数据库的一些信息

 message(STATUS  "DM8 libs: $ENV{DM_HOME}.")
 message(STATUS  "JAVA: $ENV{JAVA_HOME}.")
if(True)
    add_definitions("-DHAS_DM8")
    set(DM8_HOME  $ENV{DM_HOME})
    set(DM8_INCLUDE ${DM8_HOME}/include)
    set(DM8_BIN ${DM8_HOME}/bin)
    include_directories(DM8_INCLUDE)
    foreach(dirname ${DM8_BIN})
        file(GLOB_RECURSE DM8_LIBRARIES "${dirname}/*.lib")
    endforeach()
    message("DM8 libs: ${DM8_LIBRARIES}")
else()
    message("DM8 libs: not exist.")
endif()


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
find_package(Boost REQUIRED COMPONENTS system thread date_time chrono regex filesystem program_options iostreams locale serialization )
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::boost Boost::system Boost::thread Boost::date_time Boost::chrono Boost::regex Boost::filesystem Boost::program_options Boost::iostreams Boost::locale Boost::serialization) 

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

find_package(RapidJSON CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC rapidjson)

find_package( OpenCV CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC opencv_core opencv_imgcodecs opencv_imgproc)

find_package(netCDFCxx CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC netCDF::netcdf netCDF::netcdf-cxx4)

find_package(PNG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC PNG::PNG)

find_package(JPEG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC ${JPEG_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${JPEG_INCLUDE_DIR})