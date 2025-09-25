/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/25 10:23
 * @version: 1.0.1
 * @description: 矢量的属性段
 */
#ifndef SILLY_UTILS_SILLY_GEO_PROP_H
#define SILLY_UTILS_SILLY_GEO_PROP_H
#include <su_marco.h>
#include <tiff.h>

// 属性段类型,与org_core.h中定义的OGRFieldType一致
enum class eGeoFieldType : int8_t
{
    None = -1,      // 无效
    Int = 0,        // 整形
    Numeric = 2,    // 浮点数
    String = 4,     // 字符串
    Binary = 8,     // 二进制流
    Date = 9,       // 日期
    Time = 10,      // 时间
    DateTime = 11,  // 日期加时间
    Long = 12       // 长整形
};

struct GeoFiledInfo
{
    std::string name;
    std::string u8name;
    eGeoFieldType type;
    uint16_t index;
};
class silly_geo_prop
{
  public:
    silly_geo_prop() = default;
    silly_geo_prop(const std::string& s);
    silly_geo_prop(const int& i);
    silly_geo_prop(const double& d);
    silly_geo_prop(const long long& ll);
    silly_geo_prop(const std::vector<unsigned char>& bs);
    silly_geo_prop(const silly_geo_prop& other);
    ~silly_geo_prop();

  public:
    eGeoFieldType type() const;
    std::string as_string() const;
    int32_t as_int32() const;
    double as_double() const;
    std::vector<unsigned char> as_binary() const;
    long long as_int64() const;

  private:
    eGeoFieldType m_type{eGeoFieldType::None};
    std::any m_data;
};

#endif  // SILLY_UTILS_SILLY_GEO_PROP_H
