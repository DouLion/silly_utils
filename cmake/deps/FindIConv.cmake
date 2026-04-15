# FindIConv.cmake
# 查找 Iconv 编码转换库
# DISABLE_ICONV=ON 时跳过查找

message("\n[deps] 检查库 Iconv")

if(DISABLE_ICONV)
    message(STATUS "[deps] 禁用 Iconv")
    set(SU_THIRD_SUPPORT_ICONV 0)
else()
    # 使用 CMake 内置查找
    find_package(ICu CONFIG QUIET)  # 先检查 ICU
    find_package(Iconv QUIET)
    
    if(Iconv_FOUND)
        message(STATUS "[deps] Iconv found: TRUE")
        message(STATUS "[deps] Iconv version: ${Iconv_VERSION}")
        set(SU_THIRD_SUPPORT_ICONV 1)
    else()
        message(WARNING "[deps] 未发现 Iconv")
        set(SU_THIRD_SUPPORT_ICONV 0)
    endif()
endif()
