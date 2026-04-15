# FindJPEG.cmake
# 查找 JPEG 图像库
# DISABLE_JPEG=ON 时跳过查找

message("\n[deps] 检查库 JPEG")

if(DISABLE_JPEG)
    message(STATUS "[deps] 禁用 JPEG")
    set(SU_THIRD_SUPPORT_JPEG 0)
else()
    find_package(JPEG CONFIG)
    if(JPEG_FOUND)
        message(STATUS "[deps] JPEG found: ${JPEG_FOUND}")
        message(STATUS "[deps] JPEG version: ${JPEG_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES JPEG::JPEG)
        set(SU_THIRD_SUPPORT_JPEG 1)
    else()
        message(WARNING "[deps] 未发现 JPEG")
        set(SU_THIRD_SUPPORT_JPEG 0)
    endif()
endif()
