/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/22 13:49
 * @version: 1.0.1
 * @software: silly_utils
 * @description: 从 https://github.com/mapbox/tippecanoe 项目中摘录过来
 */
#ifndef SILLY_MAPBOX_VECTOR_TILE_H
#define SILLY_MAPBOX_VECTOR_TILE_H

#include <string>
#include <map>
#include <set>
#include <vector>

struct MVTValue;
struct MVTLayer;

enum class eMVTOperation: int
{
    Invalid = 0,
    MoveTo = 1,
    LineTo = 2,
    ClosePath = 7
};

inline bool operator==(const int& lhs, const eMVTOperation& rhs)
{
    return static_cast<int>(lhs) == static_cast<int>(rhs);
}
inline bool operator==(const eMVTOperation& lhs, const int& rhs)
{
    return static_cast<int>(lhs) == static_cast<int>(rhs);
}

struct MVTGeometry
{
    long long x = 0;
    long long y = 0;
    eMVTOperation /* mvt_operation */ op = eMVTOperation::Invalid;
    MVTGeometry(int op, long long x, long long y);
    MVTGeometry(eMVTOperation op, long long x, long long y);

    bool operator<(MVTGeometry const &s) const
    {
        if (y < s.y || (y == s.y && x < s.x))
        {
            return true;
        }
        return false;
    }

    bool operator==(MVTGeometry const &s) const
    {
        return y == s.y && x == s.x;
    }
};

enum class eMVTGeometryType: int
{
    Point = 1,
    LineString = 2,
    Polygon = 3
};

struct MVTFeature
{
    std::vector<unsigned> tags{};
    std::vector<MVTGeometry> geometry{};
    int /* eMVTGeometryType */ type = 0;
    unsigned long long id = 0;
    bool has_id = false;
    bool dropped = false;

    MVTFeature()
    {
        has_id = false;
        id = 0;
    }
};

enum class eMVTValueType : int
{
    mvt_string,
    mvt_float,
    mvt_double,
    mvt_int,
    mvt_uint,
    mvt_sint,
    mvt_bool,
    mvt_null,
};

inline bool operator==(const int& lhs, const eMVTValueType& rhs)
{
    return static_cast<int>(lhs) == static_cast<int>(rhs);
}
inline bool operator==(const eMVTValueType& lhs, const int& rhs)
{
    return static_cast<int>(lhs) == static_cast<int>(rhs);
}

struct MVTValue
{
    eMVTValueType type;
    std::string string_value;
    union
    {
        float float_value;
        double double_value;
        long long int_value;
        unsigned long long uint_value;
        long long sint_value;
        bool bool_value;
        int null_value;
    } numeric_value;

    bool operator<(const MVTValue &o) const;
    std::string toString();

    MVTValue()
    {
        this->type = eMVTValueType::mvt_double;
        this->string_value = "";
        this->numeric_value.double_value = 0;
    }
};

struct MVTLayer
{
    int version = 0;
    std::string name = "";
    std::vector<MVTFeature> features{};
    std::vector<std::string> keys{};
    std::vector<MVTValue> values{};
    long long extent = 0;

    // Add a key-value pair to a feature, using this layer's constant pool
    void tag(MVTFeature &feature, std::string key, MVTValue value);

    // For tracking the key-value constants already used in this layer
    std::map<std::string, size_t> key_map{};
    std::map<MVTValue, size_t> value_map{};
};
struct MVTTile
{
    std::vector<MVTLayer> layers{};

    std::string encode(const bool &compressed = true);
    bool decode(const std::string &message, bool &was_compressed);
};

#endif  // SILLY_MAPBOX_VECTOR_TILE_H
