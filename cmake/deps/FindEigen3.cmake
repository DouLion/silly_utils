# FindEigen3.cmake
# 查找 Eigen3 线性代数库
# DISABLE_EIGEN3=ON 时跳过查找

message("\n[deps] 检查库 EIGEN3")

if(DISABLE_EIGEN3)
    message(STATUS "[deps] 禁用 EIGEN3")
    set(SU_THIRD_SUPPORT_EIGEN3 0)
else()
    find_package(Eigen3 CONFIG)
    if(Eigen3_FOUND)
        message(STATUS "[deps] Eigen3 found: ${Eigen3_FOUND}")
        message(STATUS "[deps] Eigen3 version: ${Eigen3_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES Eigen3::Eigen)
        set(SU_THIRD_SUPPORT_EIGEN3 1)
    else()
        message(WARNING "[deps] 未发现 Eigen3")
        set(SU_THIRD_SUPPORT_EIGEN3 0)
    endif()
endif()
