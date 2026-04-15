# FindGeoTIFF.cmake
# 查找 GeoTIFF 库
# DISABLE_GEOTIFF=ON 时跳过查找

message("\n[deps] 检查库 GeoTIFF")

if(DISABLE_GEOTIFF)
    message(STATUS "[deps] 禁用 GeoTIFF")
    set(SU_THIRD_SUPPORT_GEOTIFF 0)
else()
    find_package(GeoTIFF CONFIG)
    if(GeoTIFF_FOUND)
        message(STATUS "[deps] GeoTIFF found: ${GeoTIFF_FOUND}")
        message(STATUS "[deps] GeoTIFF version: ${GeoTIFF_VERSION}")
        # vcpkg 导出的 target 名称为 geotiff_library
        list(APPEND SU_ALL_REQUIRED_LIBRARIES geotiff_library)
        set(SU_THIRD_SUPPORT_GEOTIFF 1)
    else()
        message(WARNING "[deps] 未发现 GeoTIFF")
        set(SU_THIRD_SUPPORT_GEOTIFF 0)
    endif()
endif()
