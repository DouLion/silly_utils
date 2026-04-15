# FindCGAL.cmake
# 查找 CGAL 计算几何库
# DISABLE_CGAL=ON 时跳过查找

message("\n[deps] 检查库 CGAL")

if(DISABLE_CGAL)
    message(STATUS "[deps] 禁用 CGAL")
    set(SU_THIRD_SUPPORT_CGAL 0)
else()
    find_package(CGAL CONFIG)
    if(CGAL_FOUND)
        message(STATUS "[deps] CGAL found: ${CGAL_FOUND}")
        message(STATUS "[deps] CGAL version: ${CGAL_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES CGAL::CGAL)
        set(SU_THIRD_SUPPORT_CGAL 1)
    else()
        message(WARNING "[deps] 未发现 CGAL")
        set(SU_THIRD_SUPPORT_CGAL 0)
    endif()
endif()
