# FindLibSpatialite.cmake
# 查找 SpatiaLite 空间数据库
# DISABLE_SPATIALITE=ON 时跳过查找

message("\n[deps] 检查库 SPATIALITE")

if(DISABLE_SPATIALITE)
    message(STATUS "[deps] 禁用 SPATIALITE")
    set(SU_THIRD_SUPPORT_SPATIALITE 0)
else()
    find_package(spatialite CONFIG)
    if(spatialite_FOUND)
        message(STATUS "[deps] SpatiaLite found: ${spatialite_FOUND}")
        message(STATUS "[deps] SpatiaLite version: ${spatialite_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES spatialite::spatialite)
        set(SU_THIRD_SUPPORT_SPATIALITE 1)
    else()
        message(WARNING "[deps] 未发现 SpatiaLite")
        set(SU_THIRD_SUPPORT_SPATIALITE 0)
    endif()
endif()
