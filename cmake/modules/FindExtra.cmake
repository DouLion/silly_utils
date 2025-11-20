
if(IS_LINUX)
    find_package(unofficial-libuuid CONFIG REQUIRED)
    list(APPEND SU_ALL_REQUIRED_LIBRARIES  unofficial::UUID::uuid)

    find_package(GEOS CONFIG REQUIRED)
    list(APPEND SU_ALL_REQUIRED_LIBRARIES  GEOS::geos_c)

    find_package(GEOS CONFIG REQUIRED)
    list(APPEND SU_ALL_REQUIRED_LIBRARIES  GEOS::geos)

    find_library(FREEXL_LIB NAMES "freexl")
    list(APPEND SU_ALL_REQUIRED_LIBRARIES  ${FREEXL_LIB})

    find_library(PROJ_LIB NAMES "proj")
    list(APPEND SU_ALL_REQUIRED_LIBRARIES  ${PROJ_LIB})

    find_package(CURL CONFIG REQUIRED)
    list(APPEND SU_ALL_REQUIRED_LIBRARIES  CURL::libcurl)

    find_library(TIFF_LIB NAMES "tiff")
    list(APPEND SU_ALL_REQUIRED_LIBRARIES  ${TIFF_LIB})

    find_package(LibXml2 REQUIRED)
    list(APPEND SU_ALL_REQUIRED_LIBRARIES  LibXml2::LibXml2)

    find_library(RTTOPO_LIB NAMES "rttopo")
    list(APPEND SU_ALL_REQUIRED_LIBRARIES  ${RTTOPO_LIB})

    find_package(unofficial-minizip CONFIG REQUIRED)
    list(APPEND SU_ALL_REQUIRED_LIBRARIES  unofficial::minizip::minizip)

    # Cairo图像

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(cairo REQUIRED IMPORTED_TARGET cairo)
    list(APPEND SU_ALL_REQUIRED_LIBRARIES  PkgConfig::cairo)

    find_package(Fontconfig REQUIRED) # since CMake 3.14
    list(APPEND SU_ALL_REQUIRED_LIBRARIES  Fontconfig::Fontconfig)

    find_package(PNG REQUIRED)
    list(APPEND SU_ALL_REQUIRED_LIBRARIES  PNG::PNG)
    find_library(PIXMAN_LIBRARY NAMES pixman-1)
    list(APPEND SU_ALL_REQUIRED_LIBRARIES  ${PIXMAN_LIBRARY})

endif()