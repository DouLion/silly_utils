# FindWebSocketPP.cmake
# 查找 WebSocket++ 库（已废弃，保留兼容）
# DISABLE_WEBSOCKET_PP=ON 时跳过查找

message("\n[deps] 检查库 WEBSOCKETPP (已废弃)")

if(DISABLE_WEBSOCKET_PP)
    message(STATUS "[deps] 禁用 WEBSOCKETPP")
    set(SU_THIRD_SUPPORT_WEBSOCKET_PP 0)
else()
    find_package(WebSocketPP CONFIG QUIET)
    if(WebSocketPP_FOUND)
        message(STATUS "[deps] WebSocket++ found: ${WebSocketPP_FOUND}")
        set(SU_THIRD_SUPPORT_WEBSOCKET_PP 1)
    else()
        message(WARNING "[deps] 未发现 WebSocket++ (已废弃)")
        set(SU_THIRD_SUPPORT_WEBSOCKET_PP 0)
    endif()
endif()
