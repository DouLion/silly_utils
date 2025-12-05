//
// Created by dell on 2025/9/23.
//

#ifndef SILLY_UTILS_SILLY_GDAL_H
#define SILLY_UTILS_SILLY_GDAL_H
#include <geo/silly_geo_coll.h>
#include <files/silly_file.h>
#if SU_THIRD_SUPPORT_GDAL
#include <gdal_priv.h>
#include <ogr_spatialref.h>
#include <ogrsf_frmts.h>
#include <gdal_alg.h>
#include <ogr_api.h>

#endif
class suGDAL
{
  public:
#if SU_THIRD_SUPPORT_GDAL

    /// <summary>
    /// 读取shp文件
    /// </summary>
    /// <param name="file"></param>
    /// <param name="collections"></param>
    /// <param name="ignore_prop"></param>
    /// <returns></returns>
    /// ================ gdal中矢量与silly utils中矢量互转 ================

    /// ================ 单点 ================

    /// <summary>
    /// 将 OGRPoint(单点) 转换为 suPoint(单点) 类型
    /// </summary>
    /// <param name="ogrPoint"></param>
    /// <returns></returns>
    static suPoint PointFromOGR(const OGRPoint* ogrPoint);

    /// <summary>
    /// 将 suPoint(单点) 转换为 OGRPoint(单点) 类型
    /// </summary>
    /// <param name="point"></param>
    /// <returns></returns>
    static OGRPoint PointToOGR(const suPoint& point);

    /// ================ 多点 ================

    /// <summary>
    /// 将 OGRMultiPoint(多点) 转换为 suMultiPoint(多点) 类型
    /// </summary>
    /// <param name="ogrMultiPoint"></param>
    /// <returns></returns>
    static suMultiPoint MultiPointFromOGR(const OGRMultiPoint* ogrMultiPoint);

    /// <summary>
    /// 将 suMultiPoint(多点) 转换为 OGRMultiPoint(多点) 类型
    /// </summary>
    /// <param name="multiPoint"></param>
    /// <returns></returns>
    static OGRMultiPoint MultiPointToOGR(const suMultiPoint& multiPoint);

    /// ================ 单线 ================

    /// <summary>
    /// OGRLineString(线)类型转为suLine(线)类型
    /// </summary>
    /// <param name="lineString"></param>
    /// <returns></returns>
    static suLine LineFromOGR(const OGRLineString* lineString);

    /// <summary>
    /// 将 suLine(线) 转换为 OGRLineString(线)类型
    /// </summary>
    /// <param name="line"></param>
    /// <returns></returns>
    static OGRLineString LineToOGR(const suLine& line);

    /// ================ 多线 ================

    /// <summary>
    /// OGRMultiLineString(多线)类型转为 silly_multiline(多线)类型 MultiLineFromOGR
    /// </summary>
    /// <param name="multiLineString"></param>
    /// <returns></returns>
    static suMultiLine MultiLineFromOGR(const OGRMultiLineString* multiLineString);

    /// <summary>
    /// 将 silly_multiline(多线) 转换为 OGRMultiLineString(多线)类型
    /// </summary>
    /// <param name="multiLine"></param>
    /// <returns></returns>
    static OGRMultiLineString MultiLineToOGR(const suMultiLine& multiLine);

    /// ================ 闭合环 ================

    /// <summary>
    /// 环OGRLinearRing对象，将其转换为suRing对象  (环) RingFromOGR
    /// </summary>
    /// <param name="ring"></param>
    /// <returns></returns>
    static suRing RingFromOGR(const OGRLinearRing* ring);

    /// <summary>
    /// 将 suRing 转换为 OGRPolygon
    /// </summary>
    /// <param name="ring"></param>
    /// <returns></returns>
    static OGRLinearRing RingToOGR(const suRing& ring);

    /// ================ 单面 ================

    /// <summary>
    /// OGRPolygon 对象转换为 suPoly (多环:外环+内环)对象  (单面) PolyFromOGR
    /// </summary>
    /// <param name="polygon"></param>
    /// <returns></returns>
    static suPoly PolyFromOGR(const OGRPolygon* polygon);

    /// <summary>
    /// 将 suPoly 转换为 OGRPolygon(单面)
    /// </summary>
    /// <param name="poly"></param>
    /// <returns></returns>
    static OGRPolygon PolyToOGR(const suPoly& poly);

    /// ================ 多面 ================

    /// <summary>
    /// 多面的OGRMultiPolygon对象转换为suMultiPoly(多面) MultiPolyFromOGR
    /// </summary>
    /// <param name="multiPolygon"></param>
    /// <returns></returns>
    static suMultiPoly MultiPolyFromOGR(const OGRMultiPolygon* multiPolygon);

    /// <summary>
    /// 将suMultiPoly对象转换为OGRMultiPolygon对象(多面)
    /// </summary>
    /// <param name="multiPoly"></param>
    /// <returns></returns>
    static OGRMultiPolygon MultiPolyToOGR(const suMultiPoly& multiPoly);

    /// ================ OGRGeometry ================

    /// <summary>
    /// 将 OGRGeometry 对象转换为silly_geo_coll对象,
    /// OGRGeometry是一个抽象类，无法实例化,只能以指针的方式使用,
    /// 注意: 返回的对象需要手动释放,释放方法:
    /// if (OGRGeometry* != nullptr)
    /// {
    ///    OGRGeometryFactory::destroyGeometry(OGRGeometry*);
    ///    OGRGeometry* = nullptr;
    /// }
    /// </summary>
    /// <param name="coll"></param>
    /// <returns></returns>
    static OGRGeometry* GeoCollToOGR(const suGeoColl& coll);

    /// <summary>
    /// 将silly_geo_coll对象转换为OGRGeometry对象
    /// </summary>
    /// <param name="geometry"></param>
    /// <returns></returns>
    static suGeoColl GeoCollFromOGR(const OGRGeometry* geometry);

    /// ----------------------------------- shp文件读写相关 -----------------------------------------------

    static std::string GradDriverName(const suPath& file);
    static void* GdalOpenDataset(const suPath& file, const bool& read = true);
    static bool CheckFieldInfo(const suPath& file, std::map<uint16_t, GeoFiledInfo>& idx2prop);

    static bool ReadProperties(const OGRFeature* feature, const std::map<uint16_t, GeoFiledInfo>& idx2prop, std::unordered_map<std::string, suGeoProp>& props);
    static bool ReadGeometry(const OGRGeometry* geometry, suGeoColl& geoColl);

    /// <summary>
    /// 是否为一个标准的shp文件
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    static bool IsValidShp(const suPath& file);

    /// <summary>
    /// 检查shp文件组中缺失的文件
    /// .shp: 存储几何矢量
    /// .dbf: 存储属性信息 (该文件不存在 gdal也能打开shp,成功读取矢量,但是无法读取属性信息)
    /// .shx: 存储几何矢量索引
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    static std::vector<std::string> ShpMissingFile(const suPath& file);

    // 写入所有类型的数据
    static bool AddGeometry(OGRLayer* layer, const suGeoColl& geometry, bool writeProp = true);
    static bool FillGeometry(const eGeometryType& type, OGRLayer* outputLayer, OGRFeature* feature, OGRGeometry* geometry, const suGeoColl& geo_coll);
    static bool FillField(OGRFeature* feature, const std::unordered_map<std::string, suGeoProp>& properties);

#endif
};

#endif  // SILLY_UTILS_SILLY_GDAL_H
