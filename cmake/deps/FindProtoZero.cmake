# FindProtoZero.cmake
# 查找 ProtoZero MVT 瓦片库
# DISABLE_PROTO_ZERO=ON 时跳过查找

message("\n[deps] 检查库 PROTO_ZERO")

if(DISABLE_PROTO_ZERO)
    message(STATUS "[deps] 禁用 PROTO_ZERO")
    set(SU_THIRD_SUPPORT_PROTO_ZERO 0)
else()
    find_package(protozero CONFIG)
    if(protozero_FOUND)
        message(STATUS "[deps] ProtoZero found: ${protozero_FOUND}")
        message(STATUS "[deps] ProtoZero version: ${protozero_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES protozero::protozero)
        set(SU_THIRD_SUPPORT_PROTO_ZERO 1)
    else()
        message(WARNING "[deps] 未发现 ProtoZero")
        set(SU_THIRD_SUPPORT_PROTO_ZERO 0)
    endif()
endif()
