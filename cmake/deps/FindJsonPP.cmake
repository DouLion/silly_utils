# FindJsonPP.cmake
# 查找 jsoncpp 库
# DISABLE_JSONPP=ON 时跳过查找

message("\n[deps] 检查库 jsoncpp")

if(DISABLE_JSONPP)
    message(STATUS "[deps] 禁用 jsoncpp")
    set(SU_THIRD_SUPPORT_JSONPP 0)
else()
    find_package(jsoncpp CONFIG)
    if(jsoncpp_FOUND)
        message(STATUS "[deps] jsoncpp found: ${jsoncpp_FOUND}")
        message(STATUS "[deps] jsoncpp version: ${jsoncpp_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES jsoncpp_lib)
        set(SU_THIRD_SUPPORT_JSONPP 1)
    else()
        message(WARNING "[deps] 未发现 jsoncpp")
        set(SU_THIRD_SUPPORT_JSONPP 0)
    endif()
endif()
