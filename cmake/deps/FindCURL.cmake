# FindCURL.cmake
# 查找 CURL 网络库
# DISABLE_CURL=ON 时跳过查找

message("\n[deps] 检查库 CURL")

if(DISABLE_CURL)
    message(STATUS "[deps] 禁用 CURL")
    set(SU_THIRD_SUPPORT_CURL 0)
else()
    find_package(CURL CONFIG)
    if(CURL_FOUND)
        message(STATUS "[deps] CURL found: ${CURL_FOUND}")
        message(STATUS "[deps] CURL version: ${CURL_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES CURL::libcurl)
        set(SU_THIRD_SUPPORT_CURL 1)
    else()
        message(WARNING "[deps] 未发现 CURL")
        set(SU_THIRD_SUPPORT_CURL 0)
    endif()
endif()
