# FindGDAL.cmake
# 查找 GDAL 地理数据库
# DISABLE_GDAL=ON 时跳过查找

message("\n[deps] 检查库 GDAL")

if(DISABLE_GDAL)
    message(STATUS "[deps] 禁用 GDAL")
    set(SU_THIRD_SUPPORT_GDAL 0)
else()
    find_package(GDAL CONFIG)
    if(GDAL_FOUND)
        message(STATUS "[deps] GDAL found: ${GDAL_FOUND}")
        message(STATUS "[deps] GDAL version: ${GDAL_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES GDAL::GDAL)
        set(SU_THIRD_SUPPORT_GDAL 1)
    else()
        message(WARNING "[deps] 未发现 GDAL")
        set(SU_THIRD_SUPPORT_GDAL 0)
    endif()
endif()
