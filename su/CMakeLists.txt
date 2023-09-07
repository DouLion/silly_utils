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

add_library(${PROJECT_NAME} STATIC ${SU_SRC_FILES} "geo/silly_geo_operation.cpp" "geo/silly_geo_operation.h")

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

# ½âÑ¹ËõÄ£¿é
find_package(ZLIB REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC ZLIB::ZLIB)

find_package(libzip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC libzip::zip)

find_package(BZip2 REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC BZip2::BZip2)

find_package(minizip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC minizip::minizip)

find_package(RapidJSON CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC rapidjson)

find_package(netCDFCxx CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC netCDF::netcdf netCDF::netcdf-cxx4)

find_package(PNG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC PNG::PNG)

find_package(JPEG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC ${JPEG_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${JPEG_INCLUDE_DIR})

find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC OpenSSL::SSL OpenSSL::Crypto)

find_path(PROTOZERO_INCLUDE_DIRS "protozero/basic_pbf_builder.hpp")
target_include_directories(${PROJECT_NAME} PUBLIC ${PROTOZERO_INCLUDE_DIRS})

find_package(GeoTIFF CONFIG REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${GEOTIFF_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${GEOTIFF_LIBRARIES})

find_package(GDAL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC GDAL::GDAL)


if(IS_LINUX)

    find_package(unofficial-libuuid CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC unofficial::UUID::uuid)

endif()