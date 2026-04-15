# FindCairo.cmake
# 查找 Cairo 绘图库
# DISABLE_CAIRO=ON 时跳过查找

message("\n[deps] 检查库 CAIRO")

if(DISABLE_CAIRO)
    message(STATUS "[deps] 禁用 CAIRO")
    set(SU_THIRD_SUPPORT_CAIRO 0)
else()
    find_package(Cairo CONFIG)
    if(Cairo_FOUND)
        message(STATUS "[deps] CAIRO found: ${Cairo_INCLUDE_DIR}")
        message(STATUS "[deps] CAIRO libs: ${Cairo_LIBRARIES}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES Cairo::cairo)
        set(SU_THIRD_SUPPORT_CAIRO 1)
    else()
        message(WARNING "[deps] 未发现 CAIRO")
        set(SU_THIRD_SUPPORT_CAIRO 0)
    endif()
endif()
