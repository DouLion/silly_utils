# FindPNG.cmake
# 查找 PNG 图像库
# DISABLE_PNG=ON 时跳过查找

message("\n[deps] 检查库 PNG")

if(DISABLE_PNG)
    message(STATUS "[deps] 禁用 PNG")
    set(SU_THIRD_SUPPORT_PNG 0)
else()
    find_package(PNG CONFIG)
    if(PNG_FOUND)
        message(STATUS "[deps] PNG found: ${PNG_FOUND}")
        message(STATUS "[deps] PNG version: ${PNG_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES PNG::PNG)
        set(SU_THIRD_SUPPORT_PNG 1)
    else()
        message(WARNING "[deps] 未发现 PNG")
        set(SU_THIRD_SUPPORT_PNG 0)
    endif()
endif()
