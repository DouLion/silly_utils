/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-09-25
 * @file: silly_geometry.c
 * @description: silly_geometry实现
 * @version: v1.0.1 2025-09-25 dou li yang
 */
#include "silly_geometry.h"
using namespace su;
eGeometryType Geometry::Type() const
{
    if (mType == eGeometryType::Point && mPoint)
    {
        return eGeometryType::Point;
    }
    if (mType == eGeometryType::MultiPoint && mMultiPoint)
    {
        return eGeometryType::MultiPoint;
    }
    if (mType == eGeometryType::LineString && mLine)
    {
        return eGeometryType::LineString;
    }
    if (mType == eGeometryType::MultiLineString && mMultiLine)
    {
        return eGeometryType::MultiLineString;
    }
    if (mType == eGeometryType::Polygon && mPoly)
    {
        return eGeometryType::Polygon;
    }
    if (mType == eGeometryType::MultiPolygon && mMultiPoly)
    {
        return eGeometryType::MultiPolygon;
    }
    return eGeometryType::Invalid;
}
std::shared_ptr<silly_point> Geometry::Point() const
{
    if (mType == eGeometryType::Point && mPoint)
    {
        return mPoint;
    }
    return nullptr;
}
std::shared_ptr<silly_multi_point> Geometry::MultiPoint() const
{
    if (mType == eGeometryType::MultiPoint && mMultiPoint)
    {
        return mMultiPoint;
    }
    return nullptr;
}
std::shared_ptr<silly_line> Geometry::Line() const
{
    if (mType == eGeometryType::LineString && mLine)
    {
        return mLine;
    }
    return nullptr;
}
std::shared_ptr<silly_multi_line> Geometry::MultiLine() const
{
    if (mType == eGeometryType::MultiLineString && mMultiLine)
    {
        return mMultiLine;
    }
    return nullptr;
}
std::shared_ptr<silly_poly> Geometry::Poly() const
{
    if (mType == eGeometryType::Polygon && mPoly)
    {
        return mPoly;
    }
    return nullptr;
}
std::shared_ptr<silly_multi_poly> Geometry::MultiPoly() const
{
    if (mType == eGeometryType::MultiPolygon && mMultiPoly)
    {
        return mMultiPoly;
    }
    return nullptr;
}
eGeoFieldType Geometry::FiledType(const std::string& key) const
{
    return mFields.at(key).type();
}
int Geometry::GetInt(const std::string& key)
{
    return mFields.at(key).as_int32();
}
std::string Geometry::GetStr(const std::string& key) const
{
    return mFields.at(key).as_string();
}
double Geometry::GetDouble(const std::string& key) const
{
    return mFields.at(key).as_double();
}
std::vector<unsigned char> Geometry::GetBin(const std::string& key) const
{
    return mFields.at(key).as_binary();
}
long long Geometry::GetLL(const std::string& key) const
{
    return mFields.at(key).as_int64();
}