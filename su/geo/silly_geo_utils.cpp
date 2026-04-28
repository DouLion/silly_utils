

#include "silly_geo_utils.h"
#include <log/silly_log.h>
#if SU_THIRD_SUPPORT_GDAL
#include <ogr_spatialref.h>
#include <ogrsf_frmts.h>
#include <gdal_alg.h>
#include <ogr_api.h>
#endif
#if SU_THIRD_SUPPORT_POLY_CLIPPING
//#include <clipper2/clipper.h>
#endif
#include <encode/silly_encode.h>
#include <files/silly_file.h>
#include <geo/proj/silly_proj.h>
using namespace silly::geo;

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
    double outer_area = poly.outer.area();
    suPoint outer_centroid;
    suGeoUtils::centroid(poly.outer, outer_area, outer_centroid.x, outer_centroid.y);
    total_area += outer_area;
    sum_x += outer_area * outer_centroid.x;
    sum_y += outer_area * outer_centroid.y;

    // 处理内环（孔洞）
    for (const auto& hole : poly.holes)
    {
        double hole_area = hole.area();
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
    theta = theta * 180.0 / MATH::PI_;
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

std::vector<suGeoColl> suGeoUtils::read(const suPath& file, const bool& ignore_prop)
{
    std::vector<suGeoColl> ret;
    read(file, ret, ignore_prop);
    return ret;
}

bool suGeoUtils::read(const suPath& file, std::vector<suGeoColl>& collections, const bool& ignore_prop)
{
    bool status = false;
#if SU_THIRD_SUPPORT_GDAL
    std::map<uint16_t, GeoFiledInfo> properties;
    if (!suGDAL::CheckFieldInfo(file, properties))
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
        std::string layerName(layer->GetName());
        #if _WIN32
        if (!IS_UTF8(layerName))
        {
            layerName = GBKToUTF8(layerName);
        }
        #endif
        SLOG_DEBUG("图层名: {}", layerName)
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
bool suGeoUtils::write(const suPath& file, const std::vector<suGeoColl>& collection, const eCrsEpsgCode& prj, const std::string& encode)
{
    bool status = false;
#if SU_THIRD_SUPPORT_GDAL
    if (collection.empty())
    {
        SLOG_ERROR("矢量为空");
        return status;
    }

    GDALDataset* outputData = static_cast<GDALDataset*>(suGDAL::GdalOpenDataset(file, false));
    if (outputData == nullptr)
    {
        SLOG_ERROR("创建输出文件失败");
        return false;
    }

    std::string LayerName = file.stem_utf8();
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
        if (k == "ADCD")
        {
            fieldDef.SetWidth(10);  //
        }
        if (k == "ADNM")
        {
            fieldDef.SetWidth(50);
        }
        if (k == "Height")
        {
            fieldDef.SetPrecision(3);
        }

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
    suPath cpgFile = file.parent();
    cpgFile.append(file.stem().append(".cpg"));
    sufile::write(cpgFile, encode);
    // 关闭数据集
    GDALClose(outputData);
    SLOG_DEBUG("写入矢量至{}成功", file.u8string());
#endif
    return status;
}

bool suGeoUtils::intersect(const suMultiPoly& geo1, const suMultiPoly& geo2)
{
#if SU_THIRD_SUPPORT_GDAL
    // 创建 OGRPolygon 对象
    OGRMultiPolygon p1 = suGDAL::MultiPolyToOGR(geo1);
    OGRMultiPolygon p2 = suGDAL::MultiPolyToOGR(geo2);

    // 判断两个 OGRPolygon 是否相交
    if (p1.Intersects(&p2))
    {
        return true;
    }
#endif
    return false;
}

bool suGeoUtils::intersect(const suMultiPoly& geo1, const suPoly& geo2)
{
#if SU_THIRD_SUPPORT_GDAL
    // 创建 OGRPolygon 对象
    OGRMultiPolygon p1 = suGDAL::MultiPolyToOGR(geo1);
    OGRPolygon p2 = suGDAL::PolyToOGR(geo2);

    // 判断两个 OGRPolygon 是否相交
    if (p1.Intersects(&p2))
    {
        return true;
    }
#endif
    return false;
}

bool suGeoUtils::intersect(const suMultiPoly& geo1, const suLine& geo2)
{
    for (const auto& p : geo2)
    {
        if (geo1.contains(p))
        {
            return true;
        }
    }
    return false;
}

bool suGeoUtils::intersect(const suMultiPoly& geo1, const suPoint& geo2)
{
    return geo1.contains(geo2);
}

bool suGeoUtils::intersect(const suPoly& geo1, const suPoly& geo2)
{
#if SU_THIRD_SUPPORT_GDAL
    // 创建 OGRPolygon 对象
    OGRPolygon p1 = suGDAL::PolyToOGR(geo1);
    OGRPolygon p2 = suGDAL::PolyToOGR(geo2);

    // 判断两个 OGRPolygon 是否相交
    if (p1.Intersects(&p2))
    {
        return true;
    }
#endif
    return false;
}

bool suGeoUtils::intersect(const suPoly& geo1, const suLine& geo2)
{
    for (const auto& p : geo2)
    {
        if (geo1.contains(p))
        {
            return true;
        }
    }
    return false;
}

bool suGeoUtils::intersect(const suPoly& geo1, const suPoint& geo2)
{
    return geo1.contains(geo2);
}
#if SU_THIRD_SUPPORT_POLY_CLIPPING

#endif

suMultiPoly suGeoUtils::intersection(const suMultiPoly& p1, const suMultiPoly& p2)
{
    suMultiPoly result;
#if SU_THIRD_SUPPORT_POLY_CLIPPING

#endif

    return result;
}

suMultiPoly suGeoUtils::intersection(const suMultiPoly& p1, const suPoly& p2)
{
    suMultiPoly ret;
#if SU_THIRD_SUPPORT_GDAL
    OGRMultiPolygon mp = suGDAL::MultiPolyToOGR(p1);
    OGRPolygon poly = suGDAL::PolyToOGR(p2);
    OGRGeometry* xs = mp.Intersection(&poly);
    if (xs)
    {
        if (xs->getGeometryType() == wkbMultiPolygon)
        {
            ret = suGDAL::MultiPolyFromOGR((OGRMultiPolygon*)xs);
        }
        else if (xs->getGeometryType() == wkbPolygon)
        {
            ret.push_back(suGDAL::PolyFromOGR((OGRPolygon*)xs));
        }
            
    }
    
#endif
    return ret;
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

bool suGeoUtils::nearby(const suPoint& point, const suLine& line, const double& dist)
{
    if (line.size() < 2)
    {
        if (line.size() == 0)
        {
            return false;
        }
        return point.dist(line.front()) < dist;
    }
    for (size_t i = 1; i < line.size(); i++)
    {
        const auto& p0 = line[i-1];
        const auto& p1 = line[i];
        if (suSegment(p0, p1).distance(point) < dist)
        {
            return true;
        }
    }
    return false;
}
std::vector<suLine> suGeoUtils::intersection(const suMultiPoly& multiPoly, const suLine& line)
{
    // TODO:
    return std::vector<suLine>();
}

double suGeoUtils::area_sqkm(const suPoly& poly, const double& l0)
{
    suPoly gpoly = lonlat2gauss(poly, l0);
    return gpoly.area() / 1e6;
}

double suGeoUtils::area_sqkm(const suMultiPoly& multiPoly, const double& l0)
{
    double ret = 0;
    for (const auto& poly : multiPoly)
    {
        ret += area_sqkm(poly, l0);
    }
    return ret;
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

suPoint suGeoUtils::lonlat2gauss(const suPoint& p, const double& l0)
{
    suPoint ret;
    LONLAT2GAUSS(l0, p.x, p.y, ret.x, ret.y);
    return ret;
}
suMultiPoint suGeoUtils::lonlat2gauss(const suMultiPoint& mp, const double& l0)
{
    suMultiPoint ret;
    for (const auto& p : mp)
    {
        ret.push_back(suGeoUtils::lonlat2gauss(p, l0));
    }
    return ret;
}
suLine suGeoUtils::lonlat2gauss(const suLine& line, const double& l0)
{
    suLine ret;
    for (const auto& p : line)
    {
        ret.push_back(suGeoUtils::lonlat2gauss(p, l0));
    }
    return ret;
}
suMultiLine suGeoUtils::lonlat2gauss(const suMultiLine& mline, const double& l0)
{
    suMultiLine ret;
    for (const auto& line : mline)
    {
        ret.push_back(suGeoUtils::lonlat2gauss(line, l0));
    }
    return ret;
}
suRing suGeoUtils::lonlat2gauss(const suRing& ring, const double& l0)
{
    suRing ret;
    for (const auto& p : ring.points)
    {
        ret.points.push_back(suGeoUtils::lonlat2gauss(p, l0));
    }
    return ret;
}
suPoly suGeoUtils::lonlat2gauss(const suPoly& poly, const double& l0)
{
    suPoly ret;
    ret.outer = suGeoUtils::lonlat2gauss(poly.outer, l0);
    for (const auto& ring : poly.holes)
    {
        ret.holes.push_back(suGeoUtils::lonlat2gauss(ring, l0));
    }
    return ret;
}
suMultiPoly suGeoUtils::lonlat2gauss(const suMultiPoly& mpoly, const double& l0)
{
    suMultiPoly ret;
    for (const auto& poly : mpoly)
    {
        ret.push_back(suGeoUtils::lonlat2gauss(poly, l0));
    }

    return ret;
}

suPoint suGeoUtils::gauss2lonlat(const suPoint& p, const double& l0)
{
    suPoint ret;
    GAUSS2LONLAT(l0, p.x, p.y, ret.x, ret.y);
    return ret;
}
suMultiPoint suGeoUtils::gauss2lonlat(const suMultiPoint& mp, const double& l0)
{
    suMultiPoint ret;
    for (const auto& p : mp)
    {
        ret.push_back(suGeoUtils::gauss2lonlat(p, l0));
    }
    return ret;
}
suLine suGeoUtils::gauss2lonlat(const suLine& line, const double& l0)
{
    suLine ret;
    for (const auto& p : line)
    {
        ret.push_back(suGeoUtils::gauss2lonlat(p, l0));
    }
    return ret;
}
suMultiLine suGeoUtils::gauss2lonlat(const suMultiLine& mline, const double& l0)
{
    suMultiLine ret;
    for (const auto& line : mline)
    {
        ret.push_back(suGeoUtils::gauss2lonlat(line, l0));
    }
    return ret;
}
suRing suGeoUtils::gauss2lonlat(const suRing& ring, const double& l0)
{
    suRing ret;
    for (const auto& p : ring.points)
    {
        ret.points.push_back(suGeoUtils::gauss2lonlat(p, l0));
    }
    return ret;
}
suPoly suGeoUtils::gauss2lonlat(const suPoly& poly, const double& l0)
{
    suPoly ret;
    ret.outer = suGeoUtils::gauss2lonlat(poly.outer, l0);
    for (const auto& p : poly.holes)
    {
        ret.holes.push_back(suGeoUtils::gauss2lonlat(p, l0));
    }
    return ret;
}
suMultiPoly suGeoUtils::gauss2lonlat(const suMultiPoly& mpoly, const double& l0)
{
    suMultiPoly ret;
    for (const auto& poly : mpoly)
    {
        ret.push_back(suGeoUtils::gauss2lonlat(poly, l0));
    }
    return ret;
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
        totalDist += linez[i].first.dist(linez[i - 1].first);
    }
    double dist = 0;
    ret.push_back(std::make_pair(linez.front().first, bz));
    for (int i = 1; i < linez.size(); i++)
    {
        dist += linez[i].first.dist(linez[i - 1].first);
        double percent = dist / totalDist;
        double z = linez[i].second - ((1 - percent) * dzB + percent * dzE);
        ret.push_back(std::make_pair(linez[i].first, z));
    }

    return ret;
}
