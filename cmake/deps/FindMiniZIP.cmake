# FindMiniZIP.cmake
# 查找 MiniZIP 轻量 ZIP 库
# DISABLE_MINI_ZIP=ON 时跳过查找
# 注意：vcpkg 提供的是 unofficial-minizip，target 名称为 unofficial::minizip::minizip

message("\n[deps] 检查库 minizip")

if(DISABLE_MINI_ZIP)
    message(STATUS "[deps] 禁用 minizip")
    set(SU_THIRD_SUPPORT_MINI_ZIP 0)
else()
    find_package(unofficial-minizip CONFIG)
    if(unofficial-minizip_FOUND)
        message(STATUS "[deps] minizip found: ${unofficial-minizip_FOUND}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES unofficial::minizip::minizip)
        set(SU_THIRD_SUPPORT_MINI_ZIP 1)
    else()
        message(WARNING "[deps] 未发现 minizip")
        set(SU_THIRD_SUPPORT_MINI_ZIP 0)
    endif()
endif()
