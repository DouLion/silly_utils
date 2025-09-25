/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-09-25
 * @file: silly_geometry.cpp
 * @description: silly_geometry 头文件
 * @version: v1.0.1 2025-09-25 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_GEOMETRY_H
#define SILLY_UTILS_SILLY_GEOMETRY_H
#include <geo/silly_geo.h>
#include <geo/silly_geo_filed.h>
#include <geo/silly_geo_prop.h>

namespace su
{
class Geometry
{
  public:
    eGeometryType Type() const;
    std::shared_ptr<silly_point> Point() const;
    std::shared_ptr<silly_multi_point> MultiPoint() const;
    std::shared_ptr<silly_line> Line() const;
    std::shared_ptr<silly_multi_line> MultiLine() const;
    std::shared_ptr<silly_poly> Poly() const;
    std::shared_ptr<silly_multi_poly> MultiPoly() const;

    eGeoFieldType FiledType(const std::string& key) const;
    int GetInt(const std::string& key);
    std::string GetStr(const std::string& key) const;
    double GetDouble(const std::string& key) const;
    std::vector<unsigned char> GetBin(const std::string& key) const;
    long long GetLL(const std::string& key) const;

  protected:
    eGeometryType mType{eGeometryType::Invalid};
    // 内容
    std::shared_ptr<silly_point> mPoint = nullptr;             // 单点
    std::shared_ptr<silly_multi_point> mMultiPoint = nullptr;  // 多点
    std::shared_ptr<silly_line> mLine = nullptr;               // 单线
    std::shared_ptr<silly_multi_line> mMultiLine = nullptr;    // 多线
    std::shared_ptr<silly_poly> mPoly = nullptr;               // 单面(内环+外环)
    std::shared_ptr<silly_multi_poly> mMultiPoly = nullptr;    // 多面(多个 单面)
    // 属性列表
    std::unordered_map<std::string, silly_geo_prop> mFields;
};
}  // namespace su

#endif  // SILLY_UTILS_SILLY_GEOMETRY_H
