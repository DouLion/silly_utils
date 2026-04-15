# FindBZIP2.cmake
# 查找 BZip2 压缩库
# DISABLE_BZIP2=ON 时跳过查找

message("\n[deps] 检查库 BZip2")

if(DISABLE_BZIP2)
    message(STATUS "[deps] 禁用 BZip2")
    set(SU_THIRD_SUPPORT_BZIP2 0)
else()
    find_package(BZip2 CONFIG)
    if(BZip2_FOUND)
        message(STATUS "[deps] BZip2 found: TRUE")
        message(STATUS "[deps] BZip2 version: ${BZip2_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES BZip2::BZip2)
        set(SU_THIRD_SUPPORT_BZIP2 1)
    else()
        message(WARNING "[deps] 未发现 BZip2")
        set(SU_THIRD_SUPPORT_BZIP2 0)
    endif()
endif()
