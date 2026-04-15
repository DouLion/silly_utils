# FindFMT.cmake
# 查找 fmt 库
# DISABLE_FMT=ON 时跳过查找

message("\n[deps] 检查库 FMT")

if(DISABLE_FMT)
    message(STATUS "[deps] 禁用 FMT")
    set(SU_THIRD_SUPPORT_FMT 0)
else()
    find_package(fmt CONFIG)
    if(fmt_FOUND)
        message(STATUS "[deps] FMT found: ${fmt_FOUND}")
        message(STATUS "[deps] FMT version: ${fmt_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES fmt::fmt)
        set(SU_THIRD_SUPPORT_FMT 1)
    else()
        message(WARNING "[deps] 未发现 FMT")
        set(SU_THIRD_SUPPORT_FMT 0)
    endif()
endif()
