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

class silly_geo_prop
{
  public:
    enum class enum_prop_type
    {
        eptNone = 0,      // 无效
        eptInt = 1,       // 整形
        eptNumeric = 2,   // 浮点数
        eptString = 3,    // 字符串
        eptBinary = 4,    // 二进制流
        eptTime = 5,      // 时间
        eptDate = 6,      // 日期
        eptDateTime = 7,  // 日期加时间
        eptLong = 8       // 长整形
    };

  public:
    silly_geo_prop() = default;
    silly_geo_prop(const unsigned char* d, const size_t& l, const enum_prop_type& t);
    silly_geo_prop(const std::string& s);
    silly_geo_prop(const int& i);
    silly_geo_prop(const double& d);
    silly_geo_prop(const char* c, const size_t& l);
    silly_geo_prop(const long long& ll);
    silly_geo_prop(const std::vector<unsigned char>& bs);

    silly_geo_prop(const silly_geo_prop& other);
    void destroy();
    ~silly_geo_prop();

  public:
    enum_prop_type value_type();
    std::string as_string();
    int as_int();
    double as_double();
    unsigned char* as_binary(size_t& l);
    long long as_longlong();

  private:
    enum_prop_type m_type{enum_prop_type::eptNone};
    std::any m_data;
    /*size_t len{0};
    unsigned char* data{nullptr};*/
};

#endif  // SILLY_UTILS_SILLY_GEO_PROP_H
