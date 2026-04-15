# FindZLIB.cmake
# 查找 ZLIB 压缩库
# DISABLE_ZLIB=ON 时跳过查找

message("\n[deps] 检查库 ZLIB")

if(DISABLE_ZLIB)
    message(STATUS "[deps] 禁用 ZLIB")
    set(SU_THIRD_SUPPORT_ZLIB 0)
else()
    find_package(ZLIB CONFIG)
    if(ZLIB_FOUND)
        message(STATUS "[deps] ZLIB found: TRUE")
        message(STATUS "[deps] ZLIB version: ${ZLIB_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES ZLIB::ZLIB)
        set(SU_THIRD_SUPPORT_ZLIB 1)
    else()
        message(WARNING "[deps] 未发现 ZLIB")
        set(SU_THIRD_SUPPORT_ZLIB 0)
    endif()
endif()
