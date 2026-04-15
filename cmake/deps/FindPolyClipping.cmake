# FindPolyClipping.cmake
# 查找 Clipper2 多边形切割库
# DISABLE_POLY_CLIPPING=ON 时跳过查找

message("\n[deps] 检查库 polyclipping")

if(DISABLE_POLY_CLIPPING)
    message(STATUS "[deps] 禁用 polyclipping")
    set(SU_THIRD_SUPPORT_POLY_CLIPPING 0)
else()
    find_package(Clipper2 CONFIG)
    if(Clipper2_FOUND)
        message(STATUS "[deps] polyclipping found: ${Clipper2_FOUND}")
        message(STATUS "[deps] polyclipping version: ${Clipper2_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES Clipper2::Clipper2)
        set(SU_THIRD_SUPPORT_POLY_CLIPPING 1)
    else()
        message(WARNING "[deps] 未发现 polyclipping")
        set(SU_THIRD_SUPPORT_POLY_CLIPPING 0)
    endif()
endif()
