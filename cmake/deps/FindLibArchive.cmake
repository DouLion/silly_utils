# FindLibArchive.cmake
# 查找 libarchive 通用归档库
# DISABLE_LIBARCHIVE=ON 时跳过查找

message("\n[deps] 检查库 LibArchive")

if(DISABLE_LIBARCHIVE)
    message(STATUS "[deps] 禁用 LIBARCHIVE")
    set(SU_THIRD_SUPPORT_LIBARCHIVE 0)
else()
    find_package(LibArchive CONFIG)
    if(LibArchive_FOUND)
        message(STATUS "[deps] LibArchive found: ${LibArchive_FOUND}")
        message(STATUS "[deps] LibArchive version: ${LibArchive_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES LibArchive::LibArchive)
        set(SU_THIRD_SUPPORT_LIBARCHIVE 1)
    else()
        message(WARNING "[deps] 未发现 LIBARCHIVE")
        set(SU_THIRD_SUPPORT_LIBARCHIVE 0)
    endif()
endif()
