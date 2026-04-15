# Find7ZIP.cmake
# 查找 7-Zip 支持
# DISABLE_7ZIP=ON 时跳过查找

message("\n[deps] 检查库 7zip")

if(DISABLE_7ZIP)
    message(STATUS "[deps] 禁用 7zip")
    set(SU_THIRD_SUPPORT_7ZIP 0)
else()
    find_package(7zip CONFIG QUIET)
    if(7zip_FOUND)
        message(STATUS "[deps] 7zip found: ${7zip_FOUND}")
        set(SU_THIRD_SUPPORT_7ZIP 1)
    else()
        message(WARNING "[deps] 未发现 7zip")
        set(SU_THIRD_SUPPORT_7ZIP 0)
    endif()
endif()
