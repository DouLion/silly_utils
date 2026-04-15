# FindEcCodes.cmake
# 查找 EcCodes 气象数据库（GRIB2）
# DISABLE_ECCODES=ON 时跳过查找

message("\n[deps] 检查库 ECCODES")

if(DISABLE_ECCODES)
    message(STATUS "[deps] 禁用 EcCodes (GRIB2)")
    set(SU_THIRD_SUPPORT_ECCODES 0)
else()
    find_package(eccodes CONFIG QUIET)
    if(eccodes_FOUND)
        message(STATUS "[deps] EcCodes found: ${eccodes_FOUND}")
        message(STATUS "[deps] EcCodes version: ${eccodes_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES eccodes::eccodes)
        set(SU_THIRD_SUPPORT_ECCODES 1)
    else()
        message(WARNING "[deps] 未发现 EcCodes")
        set(SU_THIRD_SUPPORT_ECCODES 0)
    endif()
endif()
