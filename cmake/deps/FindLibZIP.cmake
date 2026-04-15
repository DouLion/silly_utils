# FindLibZIP.cmake
# 查找 libzip ZIP 文件库
# DISABLE_LIBZIP=ON 时跳过查找

message("\n[deps] 检查库 libzip")

if(DISABLE_LIBZIP)
    message(STATUS "[deps] 禁用 libzip")
    set(SU_THIRD_SUPPORT_LIBZIP 0)
else()
    find_package(libzip CONFIG)
    if(libzip_FOUND)
        message(STATUS "[deps] libzip found: ${libzip_FOUND}")
        message(STATUS "[deps] libzip version: ${libzip_VERSION}")
        # vcpkg 导出的 target 名称为 libzip::zip
        list(APPEND SU_ALL_REQUIRED_LIBRARIES libzip::zip)
        set(SU_THIRD_SUPPORT_LIBZIP 1)
    else()
        message(WARNING "[deps] 未发现 libzip")
        set(SU_THIRD_SUPPORT_LIBZIP 0)
    endif()
endif()
