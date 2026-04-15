# FindGZIP.cmake
# 查找 GZIP 压缩支持
# DISABLE_GZIP=ON 时跳过查找

message("\n[deps] 检查库 GZIP")

if(DISABLE_GZIP)
    message(STATUS "[deps] 禁用 GZIP")
    set(SU_THIRD_SUPPORT_GZIP 0)
else()
    # GZIP 通常由 ZLIB 提供，检查 ZLIB 是否可用
    if(SU_THIRD_SUPPORT_ZLIB)
        message(STATUS "[deps] GZIP 支持已包含在 ZLIB 中")
        set(SU_THIRD_SUPPORT_GZIP 1)
    else()
        message(WARNING "[deps] 未发现 GZIP 支持（需要 ZLIB）")
        set(SU_THIRD_SUPPORT_GZIP 0)
    endif()
endif()
