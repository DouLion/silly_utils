# FindLZ4.cmake
# 查找 LZ4 快速压缩库
# DISABLE_LZ4=ON 时跳过查找

message("\n[deps] 检查库 lz4")

if(DISABLE_LZ4)
    message(STATUS "[deps] 禁用 lz4")
    set(SU_THIRD_SUPPORT_LZ4 0)
else()
    find_package(lz4 CONFIG)
    if(lz4_FOUND)
        message(STATUS "[deps] lz4 found: ${lz4_FOUND}")
        message(STATUS "[deps] lz4 version: ${lz4_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES lz4::lz4)
        set(SU_THIRD_SUPPORT_LZ4 1)
    else()
        message(WARNING "[deps] 未发现 lz4")
        set(SU_THIRD_SUPPORT_LZ4 0)
    endif()
endif()
