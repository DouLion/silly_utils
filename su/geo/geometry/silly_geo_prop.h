/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/25 10:23
 * @version: 1.0.1
 * @description: 矢量的属性段
 */
#ifndef SILLY_GEO_PROP_H
#define SILLY_GEO_PROP_H

#include <string>
#include <vector>
#include <variant>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

// 保持原有的枚举定义，用于对外兼容
enum class eGeoFieldType : int8_t
{
    None = -1,
    Int = 0,        // int32_t
    Numeric = 2,    // double
    String = 4,     // std::string
    Binary = 8,     // std::vector<unsigned char>
    Date = 9,       // (暂未实现逻辑，通常存为 string 或 int64)
    Time = 10,
    DateTime = 11,
    Long = 12       // int64_t
};

// 属性值的具体存储类型
// std::monostate 对应 None
using GeoPropValue = std::variant<std::monostate, int32_t, int64_t, double, std::string, std::vector<unsigned char>>;

class suGeoProp
{
public:
    // rule of zero: 不需要手动写析构、拷贝构造、赋值，variant 会自动处理

    suGeoProp() = default;

    // 泛型构造：自动匹配类型
    // 使用 std::decay_t 去除引用和 const，确保匹配 variant 内部类型
    template <typename T, 
              typename = std::enable_if_t<std::is_constructible_v<GeoPropValue, std::decay_t<T>>>>
    suGeoProp(T&& val) : m_data(std::forward<T>(val)) {}

    // 特化处理字符串字面量 (const char*) 转 std::string，防止匹配成 bool 或其他
    suGeoProp(const char* str) : m_data(std::string(str)) {}

public:
    eGeoFieldType type() const;
    
    // 核心转换函数
    std::string asString() const;
    int32_t asInt32() const;
    double asDouble() const;
    long long asInt64() const;
    std::vector<unsigned char> asBinary() const;

private:
    GeoPropValue m_data;
};

#endif // SILLY_GEO_PROP_H
