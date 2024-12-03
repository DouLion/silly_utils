cmake_minimum_required(VERSION 3.15)

project(sutils CXX)

foreach(dirname ${CMAKE_CURRENT_SOURCE_DIR})
  file(
    GLOB_RECURSE SU_SRC_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "${dirname}/*.h" "${dirname}/*.hpp" "${dirname}/*.c" "${dirname}/*.cpp")

endforeach()

add_library(${PROJECT_NAME} STATIC ${SU_SRC_FILES})

target_precompile_headers(
        ${PROJECT_NAME} PUBLIC
        ${SU_FILE_ROOT}/su/su_global.h
        ${SU_FILE_ROOT}/su/su_marco.h
        ${SU_FILE_ROOT}/su/geo/silly_geo.h
        ${SU_FILE_ROOT}/su/log/silly_log.h
        ${SU_FILE_ROOT}/su/string/silly_format.h
        ${SU_FILE_ROOT}/su/database/otl/silly_otl.h
        ${SU_FILE_ROOT}/su/datetime/silly_posix_time.h
        ${SU_FILE_ROOT}/su/json/silly_jsonpp.h
        ${SU_FILE_ROOT}/su/image/png_utils.h
)

find_package(cryptopp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC cryptopp::cryptopp)

target_link_libraries(${PROJECT_NAME} PUBLIC eccodes)


find_package(Iconv)
target_link_libraries(${PROJECT_NAME} PUBLIC Iconv::Iconv)

find_package(polyclipping CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC polyclipping::polyclipping)

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC JsonCpp::JsonCpp)

find_package(ZLIB REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC ZLIB::ZLIB)

find_package(libzip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC libzip::zip)

find_package(BZip2 REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC BZip2::BZip2)

find_package(minizip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC minizip::minizip)

find_path(GZIP_HPP_INCLUDE_DIRS "gzip/compress.hpp")
target_include_directories(${PROJECT_NAME} PUBLIC ${GZIP_HPP_INCLUDE_DIRS})

find_package(7zip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC 7zip::7zip)

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

find_library(SPATIALITE_LIB NAMES "spatialite")
target_link_libraries(${PROJECT_NAME} PUBLIC ${SPATIALITE_LIB})

find_package(unofficial-sqlite3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC unofficial::sqlite3::sqlite3)

# redis
find_package(hiredis CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC hiredis::hiredis)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)

# 硬件信息 find_package(infoware CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME} PUBLIC infoware) Cairo
find_path(CAIRO_INCLUDE_DIR cairo/cairo.h)
set(CAIRO_NAMES ${JRTPLIB_NAMES} cairo)
message("# ${CAIRO_NAMES}")
find_library(CAIRO_LIBRARY NAMES ${CAIRO_NAMES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${CAIRO_LIBRARY})

find_package(Freetype REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Freetype::Freetype) # since CMake
                                                                 # 3.10

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog
                                             spdlog::spdlog_header_only)

find_package(lz4 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC lz4::lz4)

find_package(websocketpp CONFIG REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC websocketpp::websocketpp)
#[[find_package(Boost REQUIRED random)
target_link_libraries(${PROJECT_NAME} PUBLIC  Boost::random)]]

if(IS_LINUX)

  find_package(unofficial-libuuid CONFIG REQUIRED)
  target_link_libraries(${PROJECT_NAME} PUBLIC unofficial::UUID::uuid)

  find_package(GEOS CONFIG REQUIRED)
  target_link_libraries(${PROJECT_NAME} PUBLIC GEOS::geos_c)

  find_package(GEOS CONFIG REQUIRED)
  target_link_libraries(${PROJECT_NAME} PUBLIC GEOS::geos)

  find_library(FREEXL_LIB NAMES "freexl")
  target_link_libraries(${PROJECT_NAME} PUBLIC ${FREEXL_LIB})

  find_library(PROJ_LIB NAMES "proj")
  target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJ_LIB})

  find_package(CURL CONFIG REQUIRED)
  target_link_libraries(${PROJECT_NAME} PUBLIC CURL::libcurl)

  find_library(TIFF_LIB NAMES "tiff")
  target_link_libraries(${PROJECT_NAME} PUBLIC ${TIFF_LIB})

  find_package(LibXml2 REQUIRED)
  target_link_libraries(${PROJECT_NAME} PUBLIC LibXml2::LibXml2)

  find_library(RTTOPO_LIB NAMES "rttopo")
  target_link_libraries(${PROJECT_NAME} PUBLIC ${RTTOPO_LIB})

  find_package(unofficial-minizip CONFIG REQUIRED)
  target_link_libraries(${PROJECT_NAME} PUBLIC unofficial::minizip::minizip)

  # Cairo图像

  find_package(PkgConfig REQUIRED)
  pkg_check_modules(cairo REQUIRED IMPORTED_TARGET cairo)
  target_link_libraries(${PROJECT_NAME} PUBLIC PkgConfig::cairo)

  find_package(Fontconfig REQUIRED) # since CMake 3.14
  target_link_libraries(${PROJECT_NAME} PUBLIC Fontconfig::Fontconfig)

  find_package(PNG REQUIRED)
  target_link_libraries(${PROJECT_NAME} PUBLIC PNG::PNG)
  find_library(PIXMAN_LIBRARY NAMES pixman-1)
  target_link_libraries(${PROJECT_NAME} PUBLIC ${PIXMAN_LIBRARY})

endif()
