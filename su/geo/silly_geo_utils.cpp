

#include "silly_geo_utils.h"
#ifndef NDEBUG
#include <log/silly_log.h>
#endif
#if SU_THIRD_SUPPORT_GDAL
#include <ogr_spatialref.h>
#include <ogrsf_frmts.h>
#include <gdal_alg.h>
#include <ogr_api.h>
#endif
#include <polyclipping/clipper.hpp>
#include <encode/silly_encode.h>
#include <files/silly_file.h>
#include <geo/proj/silly_proj.h>
using namespace silly::geo;
using namespace ClipperLib;

void suGeoUtils::init_proj_env()
{
    SET_ENV("PROJ_LIB", std::filesystem::current_path().append("share").append("proj").append("proj.db").string());
}
void suGeoUtils::init_gdal_env()
{
#if SU_THIRD_SUPPORT_GDAL
    init_proj_env();
    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
    OGRRegisterAll();
    CPLSetConfigOption("SHAPE_ENCODING", "");
#endif
}

void suGeoUtils::destroy_gdal_env()
{
#if SU_THIRD_SUPPORT_GDAL
    OGRCleanupAll();
#endif
}

void suGeoUtils::centroid(const suRing& ring, double& area, double& sumX, double& sumY)
{
    area = 0.0;
    sumX = 0.0;
    sumY = 0.0;

    const auto& pts = ring.points;
    if (pts.size() < 3)
        return;  // 至少需要3个点

    for (size_t i = 0; i < pts.size(); i++)
    {
        size_t j = (i + 1) % pts.size();
        double xi = pts[i].x, yi = pts[i].y;
        double xj = pts[j].x, yj = pts[j].y;

        double partial_area = (xi * yj) - (xj * yi);
        area += partial_area;
        sumX += (xi + xj) * partial_area;
        sumY += (yi + yj) * partial_area;
    }
    area *= 0.5;  // 实际面积
}

suPoint suGeoUtils::centroid(const suPoly& poly)
{
    double total_area = 0.0;
    double total_sum_x = 0.0;
    double total_sum_y = 0.0;

    // 处理外环
    double outer_area, outer_sum_x, outer_sum_y;
    centroid(poly.outer, outer_area, outer_sum_x, outer_sum_y);
    total_area += outer_area;
    total_sum_x += outer_sum_x;
    total_sum_y += outer_sum_y;

    // 处理内环（孔洞）
    for (const auto& hole : poly.holes)
    {
        double hole_area, hole_sum_x, hole_sum_y;
        centroid(hole, hole_area, hole_sum_x, hole_sum_y);
        total_area -= hole_area;  // 孔洞面积为负
        total_sum_x -= hole_sum_x;
        total_sum_y -= hole_sum_y;
    }

    // 计算质心
    suPoint ret;
    if (std::abs(total_area) > 1e-10)
    {  // 避免除以零
        ret.x = total_sum_x / (3.0 * total_area);
        ret.y = total_sum_y / (3.0 * total_area);
    }
    else
    {
        // 面积为0时（如线状多边形），返回第一个点
        ret = poly.outer.points.empty() ? suPoint{0, 0} : poly.outer.points[0];
    }

    return ret;
}
void suGeoUtils::centroid(const suPoly& poly, suPoint& polyCentroid, double& polyArea)
{
    double total_area = 0.0;
    double sum_x = 0.0, sum_y = 0.0;

    // 处理外环
    double outer_area = suGeoUtils::area(poly.outer.points);
    suPoint outer_centroid;
    suGeoUtils::centroid(poly.outer, outer_area, outer_centroid.x, outer_centroid.y);
    total_area += outer_area;
    sum_x += outer_area * outer_centroid.x;
    sum_y += outer_area * outer_centroid.y;

    // 处理内环（孔洞）
    for (const auto& hole : poly.holes)
    {
        double hole_area = suGeoUtils::area(hole.points);
        suPoint hole_centroid;
        suGeoUtils::centroid(hole, hole_area, hole_centroid.x, hole_centroid.y);
        total_area -= hole_area;  // 孔洞面积为负
        sum_x -= hole_area * hole_centroid.x;
        sum_y -= hole_area * hole_centroid.y;
    }

    // 返回结果
    polyArea = total_area;
    if (std::abs(total_area) > 1e-10)
    {
        polyCentroid.x = sum_x / total_area;
        polyCentroid.y = sum_y / total_area;
    }
    else
    {
        polyCentroid = poly.outer.points.empty() ? suPoint{0, 0} : poly.outer.points[0];
    }
}
suPoint suGeoUtils::centroid(const suMultiPoly& multiPoly)
{
    suPoint total_centroid = {0, 0};
    double total_area = 0.0;

    for (const auto& poly : multiPoly)
    {
        suPoint poly_centroid;
        double poly_area;
        suGeoUtils::centroid(poly, poly_centroid, poly_area);

        if (std::abs(poly_area) > 1e-10)
        {
            total_centroid.x += poly_area * poly_centroid.x;
            total_centroid.y += poly_area * poly_centroid.y;
            total_area += poly_area;
        }
    }

    if (std::abs(total_area) > 1e-10)
    {
        total_centroid.x /= total_area;
        total_centroid.y /= total_area;
    }
    else
    {
        total_centroid = {0, 0};  // 所有多边形面积为零
    }

    return total_centroid;
}

double suGeoUtils::azimuth(const suPoint& from, const suPoint& to)
{
    double theta = atan2(to.x - from.x, to.y - from.y);
    theta = theta * 180.0 / SU_PI;
    return theta;
}

std::string suGeoUtils::angle_to_desc(const double& angle)
{
    std::string desc;
    if (angle >= -15.0 && angle <= 15.0)
    {
        desc = "北部";
    }
    else if (angle > 15.0 && angle < 75.0)
    {
        desc = "东北部";
    }
    else if (angle >= 75.0 && angle <= 105.0)
    {
        desc = "东部";
    }
    else if (angle > 105.0 && angle < 165.0)
    {
        desc = "东南部";
    }
    else if ((angle >= 165.0 && angle <= 180.0) || (angle >= -180.0 && angle <= -165.0))
    {
        desc = "南部";
    }
    else if (angle > -165.0 && angle < -105.0)
    {
        desc = "西南部";
    }
    else if (angle >= -105.0 && angle <= -75.0)
    {
        desc = "西部";
    }
    else if (angle > -75.0 && angle < -15.0)
    {
        desc = "西北部";
    }
    else
    {
        desc = "局部";
    }
    return desc;
}

std::vector<suGeoColl> suGeoUtils::read(const std::filesystem::path& file, const bool& ignore_prop)
{
    std::vector<suGeoColl> ret;
    read(file, ret, ignore_prop);
    return ret;
}

bool suGeoUtils::read(const std::filesystem::path& file, std::vector<suGeoColl>& collections, const bool& ignore_prop)
{
    bool status = false;
#if SU_THIRD_SUPPORT_GDAL
    std::map<uint16_t, GeoFiledInfo> properties;
    if (!suGDAL::CheckFieldInfo(file.string(), properties))
    {
        SLOG_ERROR("检查矢量[{}]信息失败\n", file.u8string());
        return status;
    }
    // 打开现有 shp 文件
    auto dataset = static_cast<GDALDataset*>(GDALOpenEx(file.string().c_str(), GDAL_OF_ALL | GDAL_OF_READONLY, nullptr, nullptr, nullptr));
    if (dataset == nullptr)
    {
        // 处理文件打开失败的情况
        SLOG_ERROR("打开文件失败: {}", file.u8string())
        return status;
    }
    // 获得数据集中图层数
    int layerCount = dataset->GetLayerCount();
    SLOG_DEBUG("图层数: {}", layerCount);
    for (int i = 0; i < layerCount; i++)
    {
        OGRLayer* layer = dataset->GetLayer(i);
        if (layer == nullptr)
        {
            // 处理图层获取失败的情况
            SLOG_ERROR("获取图层失败");
            GDALClose(dataset);
            return status;
        }
        SLOG_DEBUG("图层名: {}", layer->GetName())
        layer->ResetReading();
        OGRFeature* feature;
        int64_t iEntities = layer->GetFeatureCount();  // 获取属性的个数,即矢量数据的个数
        if (iEntities <= 0)
        {
            GDALClose(dataset);
            return status;
        }
        while ((feature = layer->GetNextFeature()) != nullptr)  // 遍历 矢量数据
        {
            suGeoColl tmp;
            OGRGeometry* ogrGeo = feature->GetGeometryRef();  // 获取矢量数据
            if (ogrGeo == nullptr)
            {
                OGRFeature::DestroyFeature(feature);
                continue;
            }
            if (!ignore_prop)
            {
                // 读取属性数据
                suGDAL::ReadProperties(feature, properties, tmp.properties());
            }
            if (suGDAL::ReadGeometry(ogrGeo, tmp))
            {
                collections.push_back(tmp);
            }
            OGRFeature::DestroyFeature(feature);
        }
        // 一个图层结束
    }
    GDALClose(dataset);
#endif
    return !collections.empty();
}

#if SU_THIRD_SUPPORT_GDAL
// 根据eGeoFieldType 找gdal中属性的类型
OGRFieldType convertToOGRFieldType(const eGeoFieldType& type)
{
    OGRFieldType result = OFTString;
    switch (type)
    {
        case eGeoFieldType::None:
            result = OFTString;
            break;
        case eGeoFieldType::Int:
            result = OFTInteger;
            break;
        case eGeoFieldType::Numeric:
            result = OFTReal;
            break;
        case eGeoFieldType::String:
            result = OFTString;
            break;
        case eGeoFieldType::Binary:
            result = OFTBinary;
            break;
        case eGeoFieldType::Time:
            result = OFTTime;
            break;
        case eGeoFieldType::Date:
            result = OFTDate;
            break;
        case eGeoFieldType::DateTime:
            result = OFTDateTime;
            break;
        case eGeoFieldType::Long:
            result = OFTInteger64;
            break;
        default:
            result = OFTString;
            break;
    }
    return result;
}

#endif
bool suGeoUtils::write(const std::filesystem::path& file, const std::vector<suGeoColl>& collection, const eCrsEpsgCode& prj, const std::string& encode)
{
    bool status = false;
#if SU_THIRD_SUPPORT_GDAL
    if (collection.empty())
    {
        SLOG_ERROR("矢量为空");
        return status;
    }
    std::filesystem::path realPath = sufile::realpath(file);

    GDALDataset* outputData = static_cast<GDALDataset*>(suGDAL::GdalOpenDataset(realPath, false));
    if (outputData == nullptr)
    {
        SLOG_ERROR("创建输出文件失败");
        return false;
    }

    std::string LayerName = realPath.filename().stem().string();
    OGRSpatialReference ref = ORG_SP_REF(prj);
    OGRLayer* outputLayer = outputData->CreateLayer(LayerName.c_str(), &ref, wkbUnknown, nullptr);
    if (outputLayer == nullptr)
    {
        // 处理图层创建失败的情况
        SLOG_ERROR("创建图层失败")
        GDALClose(outputData);
        return false;
    }
    // TODO :

    for (const auto& [k, p] : collection.front().properties())  // 添加属性
    {
        OGRFieldType ogrType = convertToOGRFieldType(p.type());
        OGRFieldDefn fieldDef(k.c_str(), ogrType);
        if (outputLayer->CreateField(&fieldDef) != OGRERR_NONE)
        {
            SLOG_ERROR("创建属性段:{}失败", k);
            return false;
        }
    }
    for (const auto& coll : collection)
    {
        suGDAL::AddGeometry(outputLayer, coll);
    }
    // 手动创建.cpg文件
    std::filesystem::path cpgFile = realPath.parent_path();
    cpgFile.append(realPath.stem().string().append(".cpg"));
    sufile::write(cpgFile, encode);
    // 关闭数据集
    GDALClose(outputData);
    SLOG_DEBUG("写入矢量至{}成功", realPath.u8string());
#endif
    return status;
}

bool suGeoUtils::intersect(const suGeoColl& gc1, const suGeoColl& gc2)
{
    return false;
}

bool suGeoUtils::intersect(const suMultiPoly& multiPoly1, const suMultiPoly& multiPoly2)
{
    // TODO:
#if SU_THIRD_SUPPORT_GDAL
    // 创建 OGRPolygon 对象
    OGRMultiPolygon p1 = suGDAL::MultiPolyToOGR(multiPoly1);
    OGRMultiPolygon p2 = suGDAL::MultiPolyToOGR(multiPoly2);

    // 判断两个 OGRPolygon 是否相交
    if (p1.Intersects(&p2))
    {
        return true;
    }
#endif
    return false;
}
std::vector<suPoly> suGeoUtils::intersection(const suMultiPoly& multiPoly1, const suMultiPoly& multiPoly2)
{
    std::vector<suPoly> result;
#if SU_THIRD_SUPPORT_GDAL
    // 创建 OGRPolygon 对象
    OGRMultiPolygon org_ploy_1 = suGDAL::MultiPolyToOGR(multiPoly1);
    OGRMultiPolygon org_ploy_2 = suGDAL::MultiPolyToOGR(multiPoly2);

    /*// 判断两个 OGRPolygon 是否相交
    if (!org_ploy_1.Intersects(&org_ploy_2))
    {
        return result;
    }*/

    // 计算相交区域
    OGRGeometry* intersection = org_ploy_1.Intersection(&org_ploy_2);

    // 处理不同几何类型的情况
    OGRwkbGeometryType geometryType = intersection->getGeometryType();
    switch (geometryType)
    {
        // 单面
        case wkbPolygon:
        case wkbPolygon25D:
        {
            auto intersectingPolygon = (OGRPolygon*)(intersection);
            result.emplace_back(suGDAL::PolyFromOGR(intersectingPolygon));
            break;
        }
        // 多面
        case wkbMultiPolygon:
        case wkbMultiPolygon25D:
        {
            auto intersectingMultiPolygon = (OGRMultiPolygon*)(intersection);
            auto m_polys = suGDAL::MultiPolyFromOGR(intersectingMultiPolygon);
            for (const auto& poly : m_polys)
            {
                result.emplace_back(poly);
            }
            break;
        }
        default:
            break;
    }
#endif

    return result;
}
std::optional<suPoint> suGeoUtils::intersection(const suSegment& s1, const suSegment& s2)
{
    double x1 = s1.p0.x;
    double y1 = s1.p0.y;
    double x2 = s1.p1.x;
    double y2 = s1.p1.y;

    double x3 = s2.p0.x;
    double y3 = s2.p0.y;
    double x4 = s2.p1.x;
    double y4 = s2.p1.y;

    // 计算分母
    double denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    // 平行或重合
    if (std::fabs(denominator) < SU_TINY)
    {
        return std::nullopt;
    }

    // 计算交点的参数 t 和 u
    double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denominator;
    double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denominator;

    // 检查交点是否在线段内
    if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
    {
        double intersectX = x1 + t * (x2 - x1);
        double intersectY = y1 + t * (y2 - y1);
        return suPoint(intersectX, intersectY);
    }

    // 交点不在两条线段上
    return std::nullopt;
}

std::optional<suPointZ> suGeoUtils::intersection(const suSegmentZ& s1, const suSegmentZ& s2)
{
    double x1 = s1.p0.x;
    double y1 = s1.p0.y;
    double z1 = s1.p0.z;

    double x2 = s1.p1.x;
    double y2 = s1.p1.y;
    double z2 = s1.p1.z;

    double x3 = s2.p0.x;
    double y3 = s2.p0.y;
    double z3 = s2.p0.z;

    double x4 = s2.p1.x;
    double y4 = s2.p1.y;
    double z4 = s2.p1.z;

    // 计算分母
    double denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    // 平行或重合
    if (std::fabs(denominator) < SU_TINY)
    {
        return std::nullopt;
    }

    // 计算交点的参数 t 和 u
    double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denominator;
    double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denominator;

    // 检查交点是否在线段内
    if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
    {
        double intersectX = x1 + t * (x2 - x1);
        double intersectY = y1 + t * (y2 - y1);
        double intersectZ = ((z1 + t * (z2 - z1)) + (z3 + t * (z4 - z3))) / 2.0;
        return suPointZ(intersectX, intersectY, intersectZ);
    }

    // 交点不在两条线段上
    return std::nullopt;
}

bool suGeoUtils::intersect(const suPoly& multiPoly, const suPoint& point)
{
    suPoint ray_end(point.x + 1000, point.y);  // 向右引一条射线 1000单位

    // 外环
    bool is_in_outer = intersect(point, multiPoly.outer.points);
    if (is_in_outer)
    {
        // 内环
        for (const auto& inner : multiPoly.holes)
        {
            if (intersect(point, inner.points))  // 在内环内
            {
                return false;  // 如果这个点在一个内环内就属于在面外
            }
        }
        return true;  // 点在外环内,且不在任何一个内环内
    }
    return false;
}

bool suGeoUtils::intersect(const suMultiPoly& multiPoly, const suPoint& point)
{
    bool is_in = false;
    for (const auto& poly : multiPoly)
    {
        if (intersect(poly, point))
        {
            is_in = true;  // 如果点在任何一个多边形内,则认为在面内,即相交
            break;
        }
    }
    return is_in;
}
bool suGeoUtils::intersect(const suMultiPoly& multiPoly, const suLine& line)
{
    // TODO:
    return false;
}
bool suGeoUtils::nearby(const suPoint& point, const suLine& line, const double& dist)
{
    // TODO:
    return false;
}
std::vector<suLine> suGeoUtils::intersection(const suMultiPoly& multiPoly, const suLine& line)
{
    // TODO:
    return std::vector<suLine>();
}
double suGeoUtils::area(const std::vector<suPoint>& points)
{
    double result = 0.0;
    size_t pnum = points.size();
    // 确保至少有3个点才能构成一个多边形
    if (pnum < 3)
    {
        return 0.0;
    }

    for (size_t i = 0; i < pnum; ++i)
    {
        size_t j = (i + 1) % pnum;
        result += points[i].x * points[j].y;
        result -= points[j].x * points[i].y;
    }
    return std::abs(result) / 2.0;
}
double suGeoUtils::area(const suPoly& poly)
{
    double total_area = area(poly.outer.points);
    if (total_area < 1.E-15)
    {
        return total_area;
    }

    for (const auto& inner_ring : poly.holes)
    {
        total_area -= area(inner_ring.points);
    }

    return total_area;
}
double suGeoUtils::area_sqkm(const suPoly& poly, const double& l0)
{
    double total_area = area_sqkm(poly.outer.points, l0);
    if (total_area < 1.E-15)
    {
        return total_area;
    }
    for (const auto& inner_ring : poly.holes)
    {
        total_area -= area_sqkm(inner_ring.points, l0);
    }
    return total_area;
}
double suGeoUtils::area(const suMultiPoly& multiPoly)
{
    double total_area = 0;
    for (const auto& poly : multiPoly)
    {
        total_area += area(poly);
    }
    return total_area;
}
double suGeoUtils::area_sqkm(const suMultiPoly& multiPoly, const double& l0)
{
    double total_area = 0;
    for (const auto& poly : multiPoly)
    {
        total_area += area_sqkm(poly, l0);
    }
    return total_area;
}
std::vector<suPoly> suGeoUtils::trans_intersection(const suMultiPoly& multiPoly1, const suMultiPoly& multiPoly2)
{
    std::vector<suPoly> result;
    // TODO:
    return result;
}
std::vector<suLine> suGeoUtils::trans_intersection(const suMultiPoly& multiPoly1, const suLine& line)
{
    std::vector<suLine> result;
    // TODO:
    return result;
}
double suGeoUtils::area_sqkm(const std::vector<suPoint>& points, const double& l0)
{
    double maxx = -1e10, minx = 1e10;
    for (auto p : points)
    {
        maxx = std::max(maxx, p.x);
        minx = std::min(minx, p.x);
    }
    std::vector<suPoint> gpoints;
    for (auto p : points)
    {
        suPoint tmp;
        suGeoProj::lonlat_to_gauss(l0, p.x, p.y, tmp.y, tmp.x);
        gpoints.push_back(tmp);
    }
    return area(gpoints) / 1e6;
}
std::vector<suPoint> suGeoUtils::smooth_line(const std::vector<suPoint>& line, const int& mod, const int& interp)
{
    return std::vector<suPoint>();
}
std::vector<suPoint> suGeoUtils::smooth_ring(const std::vector<suPoint>& ring, const int& mod, const int& interp)
{
    return std::vector<suPoint>();
}
std::vector<suPoint> suGeoUtils::simplify_line(const std::vector<suPoint>& line, const double& dist)
{
    return std::vector<suPoint>();
}
std::vector<suPoint> suGeoUtils::simplify_ring(const std::vector<suPoint>& ring, const double& dist)
{
    return std::vector<suPoint>();
}

bool suGeoUtils::intersect(const suPoint& point, const std::vector<suPoint>& points)
{
    bool is_inside = false;
    const size_t count = points.size();
    if (count < 2)
        return false;  // 至少需要两个点构成线段

    for (size_t i = 0, j = count - 1; i < count; j = i++)
    {
        const auto& v1 = points[j];
        const auto& v2 = points[i];

        // 检查点是否在线段上（避免浮点精度问题）
        const double dx = v2.x - v1.x;
        const double dy = v2.y - v1.y;
        if (std::abs(dy) > std::numeric_limits<double>::epsilon())
        {
            const double intersection_x = (dx * (point.y - v1.y) / dy) + v1.x;
            if (std::abs(point.x - intersection_x) < std::numeric_limits<double>::epsilon())
            {
                return false;  // 点在边界上
            }
        }

        // 射线法判断内外
        if (((v1.y <= point.y && point.y < v2.y) || (v2.y <= point.y && point.y < v1.y)) && (point.x < (dx * (point.y - v1.y) / dy + v1.x)))
        {
            is_inside = !is_inside;
        }
    }
    return is_inside;
}
double suGeoUtils::distance(const suPoint& p1, const suPoint& p2)
{
    return std::sqrt(distance_sq(p1, p2));
}
double suGeoUtils::distance_km(const suPoint& p1, const suPoint& p2)
{
    /// https://github.com/atychang/geo-distance/blob/master/vincenty/cpp/CalcDistance.cc
    const double a = 6378137.0;            // WGS-84 Earth semi-major axis (m)
    const double f = 1.0 / 298.257223563;  // WGS-84 flattening factor of the Earth
    const double b = a * (1 - f);          // WGS-84 Earth semi-minor axis (m)

    double lat1 = p1.y * SU_PI / 180;
    double lon1 = p1.x * SU_PI / 180;
    double lat2 = p2.y * SU_PI / 180;
    double lon2 = p2.x * SU_PI / 180;
    double U1 = atan((1 - f) * tan(lat1));
    double U2 = atan((1 - f) * tan(lat2));
    double sinU1 = sin(U1), cosU1 = cos(U1);
    double sinU2 = sin(U2), cosU2 = cos(U2);
    double L = (lon2 - lon1) * SU_PI / 180;

    double sinLambda;
    double cosLambda;
    double sinSigma;
    double cosSigma;
    double sigma;
    double cosSqAlpha;
    double cos2SigmaM;

    double lambda = L, prevLambda;
    int iterationLimit = 100;

    do
    {
        sinLambda = sin(lambda);
        cosLambda = cos(lambda);
        sinSigma = sqrt(pow(cosU2 * sinLambda, 2) + pow(cosU1 * sinU2 - sinU1 * cosU2 * cosLambda, 2));

        if (sinSigma == 0)  // coincident points
            return 0;

        cosSigma = sinU1 * sinU2 + cosU1 * cosU2 * cosLambda;
        sigma = atan(sinSigma / cosSigma);
        double sinAlpha = cosU1 * cosU2 * sinLambda / sinSigma;
        cosSqAlpha = 1 - pow(sinAlpha, 2);

        if (cosSqAlpha == 0)  // equatorial line: cosSqAlpha = 0
            cos2SigmaM = 0;
        else
            cos2SigmaM = cosSigma - 2 * sinU1 * sinU2 / cosSqAlpha;

        double C = f / 16 * cosSqAlpha * (4 + f * (4 - 3 * cosSqAlpha));
        prevLambda = lambda;
        lambda = L + (1 - C) * f * sinAlpha * (sigma + C * sinSigma * (cos2SigmaM + C * cosSigma * (-1 + 2 * pow(cos2SigmaM, 2))));
    } while (std::abs(lambda - prevLambda) > 1e-12 && --iterationLimit > 0);

    if (iterationLimit == 0)  // formula failed to converge
        return 0;

    double uSq = cosSqAlpha * (pow(a, 2) - pow(b, 2)) / pow(b, 2);
    double A = 1 + uSq / 16384 * (4096 + uSq * (-768 + uSq * (320 - 175 * uSq)));
    double B = uSq / 1024 * (256 + uSq * (-128 + uSq * (74 - 47 * uSq)));
    double deltaSigma = B * sinSigma * (cos2SigmaM + B / 4 * (cosSigma * (-1 + 2 * pow(cos2SigmaM, 2)) - B / 6 * cos2SigmaM * (-3 + 4 * pow(sinSigma, 2)) * (-3 + 4 * pow(cos2SigmaM, 2))));

    double s = b * A * (sigma - deltaSigma);  // in the same units as a and b

    // bearing (direction) in radius
    // degree = radius * 180 / pi
    double revAz = atan2(cosU1 * sinLambda, -sinU1 * cosU2 + cosU1 * sinU2 * cosLambda);

    return s / 1000.0;
}
double suGeoUtils::distance_sq(const suPoint& p1, const suPoint& p2)
{
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

suGeoColl suGeoUtils::buffer(const suGeoColl& coll, const double& distance)
{
    suGeoColl ret;
    // TODO: 这个下面实现有问题, bufferedGeom可能会有内存泄露
    // 返回结果一定是个面或者多面, 使用多面作为返回值
#if SU_THIRD_SUPPORT_GDAL
    OGRGeometry* resOGRGeom = suGDAL::GeoCollToOGR(coll);
    if (resOGRGeom == nullptr)
    {
        SLOG_ERROR("Failed to convert suGeoColl to OGRGeometry");
        return ret;
    }
    OGRGeometry* bufferedGeom = resOGRGeom->Buffer(distance);  // 创建缓冲区
    if (bufferedGeom == nullptr)
    {
        OGRGeometryFactory::destroyGeometry(resOGRGeom);
        resOGRGeom = nullptr;
        SLOG_ERROR("Failed to buffer OGRGeometry");
        return ret;
    }
    ret = suGDAL::GeoCollFromOGR(bufferedGeom);
    if (resOGRGeom != nullptr)
    {
        OGRGeometryFactory::destroyGeometry(resOGRGeom);
        resOGRGeom = nullptr;
    }
    if (bufferedGeom != nullptr)
    {
        OGRGeometryFactory::destroyGeometry(bufferedGeom);
        bufferedGeom = nullptr;
    }
#endif
    return ret;
}
std::vector<std::pair<suPoint, double>> suGeoUtils::adjust(const std::vector<std::pair<suPoint, double>>& linez, const double& bz, const double& ez)
{
    std::vector<std::pair<suPoint, double>> ret;
    double dzB = (bz - linez.front().second);
    double dzE = (ez - linez.back().second);
    double totalDist = 0;
    for (int i = 1; i < linez.size(); i++)
    {
        totalDist += distance(linez[i].first, linez[i - 1].first);
    }
    double dist = 0;
    ret.push_back(std::make_pair(linez.front().first, bz));
    for (int i = 1; i < linez.size(); i++)
    {
        dist += distance(linez[i].first, linez[i - 1].first);
        double percent = dist / totalDist;
        double z = linez[i].second - ((1 - percent) * dzB + percent * dzE);
        ret.push_back(std::make_pair(linez[i].first, z));
    }

    return ret;
}
