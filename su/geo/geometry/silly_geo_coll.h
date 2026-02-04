/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/25 10:23
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_GEO_COLL_H
#define SILLY_GEO_COLL_H

#include <geo/silly_geo.h>
#include <geo/geometry/silly_geo_prop.h>
#include <variant>
#include <stdexcept>
#include <type_traits>
#include <string>

struct GeoFiledInfo
{
    std::string name;
    std::string u8name;
    eGeoFieldType type;
    uint16_t index;
};

// --- 1. 类型特征萃取 (Type Traits) ---
// 用于建立 C++ 类型 (suPoint) 到 枚举 (eGeometryType) 和 字符串名称 的映射
// 这样以后加类型只需要在这里加一行，不用改主类逻辑
template <typename T>
struct GeoTraits;

#define REGISTER_GEO_TYPE(CPP_TYPE, ENUM_TYPE, NAME_STR) \
    template <>                                          \
    struct GeoTraits<CPP_TYPE>                           \
    {                                                    \
        static constexpr eGeometryType type = ENUM_TYPE; \
        static constexpr const char* name = NAME_STR;    \
    };

REGISTER_GEO_TYPE(suPoint, eGeometryType::Point, "Point")

REGISTER_GEO_TYPE(suMultiPoint, eGeometryType::MultiPoint, "MultiPoint")

REGISTER_GEO_TYPE(suLine, eGeometryType::LineString, "LineString")

REGISTER_GEO_TYPE(suMultiLine, eGeometryType::MultiLineString, "MultiLineString")

REGISTER_GEO_TYPE(suPoly, eGeometryType::Polygon, "Polygon")

REGISTER_GEO_TYPE(suMultiPoly, eGeometryType::MultiPolygon, "MultiPolygon")

#undef REGISTER_GEO_TYPE

class suGeoColl
{
public:
    using suGeoCollType = std::variant<suPoint, suMultiPoint, suLine, suMultiLine, suPoly, suMultiPoly>;

    // 默认构造
    suGeoColl() = default;

    // --- 2. 泛型构造函数 ---
    // 只要传入的是 suPoint 等合法类型，自动匹配，无需写6个构造函数
    template <typename T, typename = std::enable_if_t<std::is_constructible_v<suGeoCollType, std::decay_t<T>>>>
    explicit suGeoColl(T&& geometry)
    {
        set(std::forward<T>(geometry));
    }

    // --- 3. 泛型 Set 函数 ---
    // 自动更新 variant 和 m_type，保证绝对一致
    template <typename T>
    void set(T&& geometry)
    {
        using DecayedT = std::decay_t<T>;
        // 编译期检查类型是否合法，报错信息更友好
        static_assert(std::is_constructible_v<suGeoCollType, DecayedT>, "Unsupported geometry type!");

        m_geometry = std::forward<T>(geometry);
        m_type = GeoTraits<DecayedT>::type;
    }

    ///  获取矢量信息
    suPoint& asPoint()
    {
        return Access<suPoint>();
    }

    const suPoint& asPoint() const
    {
        return Access<suPoint>();
    }

    suMultiPoint& asMultiPoint()
    {
        return Access<suMultiPoint>();
    }

    const suMultiPoint& asMultiPoint() const
    {
        return Access<suMultiPoint>();
    }

    suLine& asLine()
    {
        return Access<suLine>();
    }

    const suLine& asLine() const
    {
        return Access<suLine>();
    }

    suMultiLine& asMultiLine()
    {
        return Access<suMultiLine>();
    }

    const suMultiLine& asMultiLine() const
    {
        return Access<suMultiLine>();
    }

    suPoly& asPoly()
    {
        return Access<suPoly>();
    }

    const suPoly& asPoly() const
    {
        return Access<suPoly>();
    }

    suMultiPoly& asMultiPoly()
    {
        return Access<suMultiPoly>();
    }

    const suMultiPoly& asMultiPoly() const
    {
        return Access<suMultiPoly>();
    }

    eGeometryType type() const
    {
        return m_type;
    }

    ///  获取属性信息
    std::unordered_map<std::string, suGeoProp>& properties()
    {
        return m_properties;
    }

    const std::unordered_map<std::string, suGeoProp>& properties() const
    {
        return m_properties;
    }

    // 稍微优化了一下属性访问，统一错误处理
    std::string asString(const std::string& key) const
    {
        return getProp(key).asString();
    }

    int32_t asInt32(const std::string& key) const
    {
        return getProp(key).asInt32();
    }

    double asDouble(const std::string& key) const
    {
        return getProp(key).asDouble();
    }

    long long asInt64(const std::string& key) const
    {
        return getProp(key).asInt64();
    }

    std::vector<unsigned char> asBinary(const std::string& key) const
    {
        return getProp(key).asBinary();
    }

private:
    // --- 核心泛型访问器 ---
    template <typename T>
    T& Access()
    {
        // 尝试获取指针，比 try-catch 性能更好
        if (auto* val = std::get_if<T>(&m_geometry))
        {
            return *val;
        }
        throw std::runtime_error(std::string("不是 ") + GeoTraits<T>::name + " 类型!");
    }

    template <typename T>
    const T& Access() const
    {
        if (const auto* val = std::get_if<T>(&m_geometry))
        {
            return *val;
        }
        throw std::runtime_error(std::string("不是 ") + GeoTraits<T>::name + " 类型!");
    }

    const suGeoProp& getProp(const std::string& key) const
    {
        // 这里可以使用 at，也可以添加自定义的检查
        return m_properties.at(key);
    }

private:
    suGeoCollType m_geometry;
    eGeometryType m_type{eGeometryType::Invalid};

    std::unordered_map<std::string, suGeoProp> m_properties;
    std::map<uint16_t, std::string> m_prop_index; // 写入shp时,控制属性段的输出顺序
};

#endif  // SILLY_GEO_COLL_H
