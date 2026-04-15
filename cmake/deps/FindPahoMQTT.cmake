# FindPahoMQTT.cmake
# 查找 Paho MQTT 客户端
# DISABLE_PAHO_MQTT=ON 时跳过查找

message("\n[deps] 检查库 PAHO_MQTT")

if(DISABLE_PAHO_MQTT)
    message(STATUS "[deps] 禁用 PahoMQTT")
    set(SU_THIRD_SUPPORT_PAHO_MQTT 0)
else()
    find_package(PahoMqttCpp CONFIG)
    if(PahoMqttCpp_FOUND)
        message(STATUS "[deps] PahoMqttCpp found: ${PahoMqttCpp_FOUND}")
        message(STATUS "[deps] PahoMqttCpp version: ${PahoMqttCpp_VERSION}")
        list(APPEND SU_ALL_REQUIRED_LIBRARIES PahoMqttCpp::paho-mqttpp3)
        set(SU_THIRD_SUPPORT_PAHO_MQTT 1)
    else()
        message(WARNING "[deps] 未发现 PahoMqttCpp")
        set(SU_THIRD_SUPPORT_PAHO_MQTT 0)
    endif()
endif()
