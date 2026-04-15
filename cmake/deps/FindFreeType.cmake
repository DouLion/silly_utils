# FindFreeType.cmake
# 查找 FreeType 字体库
# DISABLE_FREE_TYPE=ON 时跳过查找

message("\n[deps] 检查库 Freetype")

if(DISABLE_FREE_TYPE)
    message(STATUS "[deps] 禁用 Freetype")
    set(SU_THIRD_SUPPORT_FREE_TYPE 0)
else()
    find_package(Freetype CONFIG)
    if(Freetype_FOUND)
        message(STATUS "[deps] Freetype found: ${Freetype_FOUND}")
        message(STATUS "[deps] Freetype version: ${Freetype_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES Freetype::Freetype)
        set(SU_THIRD_SUPPORT_FREE_TYPE 1)
    else()
        message(WARNING "[deps] 未发现 Freetype")
        set(SU_THIRD_SUPPORT_FREE_TYPE 0)
    endif()
endif()
