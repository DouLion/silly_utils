# FindSpdLog.cmake
# 查找 spdlog 库
# DISABLE_SPDLOG=ON 时跳过查找

message("\n[deps] 检查库 spdlog")

if(DISABLE_SPDLOG)
    message(STATUS "[deps] 禁用 spdlog")
    set(SU_THIRD_SUPPORT_SPDLOG 0)
else()
    find_package(spdlog CONFIG)
    if(spdlog_FOUND)
        message(STATUS "[deps] spdlog found: ${spdlog_FOUND}")
        message(STATUS "[deps] spdlog version: ${spdlog_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES spdlog::spdlog spdlog::spdlog_header_only)
        set(SU_THIRD_SUPPORT_SPDLOG 1)
    else()
        message(WARNING "[deps] 未发现 spdlog")
        set(SU_THIRD_SUPPORT_SPDLOG 0)
    endif()
endif()
