//
// Created by dell on 2024/1/25.
//

#include "silly_geo_prop.h"
#include <charconv> // C++17用于高效数字转字符串，如果编译器支持的话

// 辅助函数：将 variant 的 index 映射回枚举
// 顺序必须与 GeoPropValue 定义的顺序一致: 
// monostate(0), int32(1), int64(2), double(3), string(4), binary(5)
eGeoFieldType suGeoProp::type() const
{
    switch (m_data.index())
    {
    case 0: return eGeoFieldType::None;
    case 1: return eGeoFieldType::Int;
    case 2: return eGeoFieldType::Long;
    case 3: return eGeoFieldType::Numeric;
    case 4: return eGeoFieldType::String;
    case 5: return eGeoFieldType::Binary;
    default: return eGeoFieldType::None;
    }
}

// --- String 转换 (尽可能转换一切) ---
std::string suGeoProp::asString() const
{
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            return "";
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        }
        else if constexpr (std::is_same_v<T, std::vector<unsigned char>>) {
            return "<BINARY>"; // 或者做 hex dump
        }
        else {
            // 数字类型
            return std::to_string(arg);
        }
    }, m_data);
}

// --- Double 转换 (实现原本逻辑中的“智能转换”) ---
double suGeoProp::asDouble() const
{
    return std::visit([](auto&& arg) -> double {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_arithmetic_v<T>) {
            return static_cast<double>(arg); // int, long -> double
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            try {
                return std::stod(arg); // 尝试解析字符串
            } catch (...) {
                return 0.0;
            }
        }
        return 0.0; // binary or none
    }, m_data);
}

// --- Int32 转换 (保持原本的严格逻辑，或稍微放宽) ---
int32_t suGeoProp::asInt32() const
{
    // 如果想要严格匹配：
    if (auto* val = std::get_if<int32_t>(&m_data)) return *val;
    
    // 如果想要兼容 Long (只要不溢出)：
    if (auto* val = std::get_if<int64_t>(&m_data)) return static_cast<int32_t>(*val);
    
    // 如果原逻辑是严格的，就返回0
    return 0;
}

// --- Int64 转换 ---
long long suGeoProp::asInt64() const
{
    if (auto* val = std::get_if<int64_t>(&m_data)) return *val;
    if (auto* val = std::get_if<int32_t>(&m_data)) return static_cast<long long>(*val);
    return 0;
}

// --- Binary 转换 ---
std::vector<unsigned char> suGeoProp::asBinary() const
{
    if (auto* val = std::get_if<std::vector<unsigned char>>(&m_data)) {
        return *val;
    }
    return {};
}