# FindOpenSSL.cmake
# 查找 OpenSSL 加密库
# DISABLE_OPENSSL=ON 时跳过查找

message("\n[deps] 检查库 OpenSSL")

if(DISABLE_OPENSSL)
    message(STATUS "[deps] 禁用 OpenSSL")
    set(SU_THIRD_SUPPORT_OPENSSL 0)
else()
    find_package(OpenSSL CONFIG)
    if(OpenSSL_FOUND)
        message(STATUS "[deps] OpenSSL found: ${OpenSSL_FOUND}")
        message(STATUS "[deps] OpenSSL version: ${OpenSSL_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES OpenSSL::SSL OpenSSL::Crypto)
        set(SU_THIRD_SUPPORT_OPENSSL 1)
    else()
        message(WARNING "[deps] 未发现 OpenSSL")
        set(SU_THIRD_SUPPORT_OPENSSL 0)
    endif()
endif()
