# FindCryptoPP.cmake
# 查找 Crypto++ 加密库
# DISABLE_CRYPTOPP=ON 时跳过查找

message("\n[deps] 检查库 CryptoPP")

if(DISABLE_CRYPTOPP)
    message(STATUS "[deps] 禁用 CryptoPP")
    set(SU_THIRD_SUPPORT_CRYPTOPP 0)
else()
    find_package(cryptopp CONFIG)
    if(cryptopp_FOUND)
        message(STATUS "[deps] CryptoPP found: ${cryptopp_FOUND}")
        message(STATUS "[deps] CryptoPP version: ${cryptopp_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES cryptopp::cryptopp)
        set(SU_THIRD_SUPPORT_CRYPTOPP 1)
    else()
        message(WARNING "[deps] 未发现 CryptoPP")
        set(SU_THIRD_SUPPORT_CRYPTOPP 0)
    endif()
endif()
