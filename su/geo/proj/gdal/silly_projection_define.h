/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/7 13:02
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_PROJECTION_DEFINE_H
#define SILLY_UTILS_SILLY_PROJECTION_DEFINE_H
#include <su_marco.h>
#if SU_THIRD_SUPPORT_GDAL
#include <ogr_spatialref.h>
#endif

enum eCrsEsriCode
{
    // ESRI:102113

    WGS_84_WEB_MERCATOR = 102113  // Web地图墨卡托 ESRI: 102113
};

// 参考坐标系 的 EPSG授权编码
enum eCrsEpsgCode : int
{
    // EPSG:4326

    WGS_84_WORLD_MERCATOR = 3857,  //
    GCS_WGS_1984 = 4326,           //  经纬度
    CGCS_2000 = 4490,              // 经纬度
    CGCS2000_GAUSS_E75 = 4534,     // 高斯克吕格75度带
    CGCS2000_GAUSS_E78 = 4535,
    CGCS2000_GAUSS_E81 = 4536,
    CGCS2000_GAUSS_E84 = 4537,
    CGCS2000_GAUSS_E87 = 4538,
    CGCS2000_GAUSS_E90 = 4539,
    CGCS2000_GAUSS_E93 = 4540,
    CGCS2000_GAUSS_E96 = 4541,
    CGCS2000_GAUSS_E99 = 4542,
    CGCS2000_GAUSS_E102 = 4543,
    CGCS2000_GAUSS_E105 = 4544,
    CGCS2000_GAUSS_E108 = 4545,
    CGCS2000_GAUSS_E111 = 4546,
    CGCS2000_GAUSS_E114 = 4547,
    CGCS2000_GAUSS_E117 = 4548,
    CGCS2000_GAUSS_E120 = 4549,
    CGCS2000_GAUSS_E123 = 4550,
    CGCS2000_GAUSS_E126 = 4551,
    CGCS2000_GAUSS_E129 = 4552,
    CGCS2000_GAUSS_E132 = 4553,
    CGCS2000_GAUSS_E135 = 4554
};

#define ESPG_WKT silly::geo::proj::CRS::wkt
#define ESPG_PROJ4 silly::geo::proj::CRS::proj4
#define ESRI_WKT silly::geo::proj::CRS::esri_wkt
#define CENTRAL2CGCS200 silly::geo::proj::CRS::central2cgcs2000
#if SU_THIRD_SUPPORT_GDAL
#define ORG_SP_REF silly::geo::proj::CRS::reference
#endif

namespace silly::geo::proj
{
class CRS
{
  public:
    static std::string wkt(const eCrsEpsgCode &def);
    static std::string esri_wkt(const eCrsEpsgCode &def);

    static std::string proj4(const eCrsEpsgCode &def);

    /**
     *
     * @param central 中央经线获取CGCS2000的EPSG编码
     * @return
     */
    static eCrsEpsgCode central2cgcs2000(const int &central);
#if SU_THIRD_SUPPORT_GDAL
    static OGRSpatialReference reference(const eCrsEpsgCode &def);
#endif

  protected:
    static std::unordered_map<eCrsEpsgCode, std::string> mEPSG2WKT;
    static std::unordered_map<eCrsEpsgCode, std::string> mEPSG2PROJ4;
    static std::unordered_map<eCrsEsriCode, std::string> mESRI2WKT;
};
//{

//    GCS_WGS_1984 = 4326,               // WGS84 经纬度
//    PCS_WGS_1984_WEB_MERCATOR = 3857,  // Web地图墨卡托
//    GCS_CHINA_2000 = 4490,             // 大地2000
//    GCS_XIAN_1980 = 4610,              // 西安80
//    GCS_BEIJING_1954 = 4214,           // 北京54
//    BaiDu09_LL = 910102,               // 百度经纬度 (bd09ll 910102)
//    GCJ02_LL = 910101,                 // 四维、高德经纬度 (GCJ02 910101)
//    Baidu09_MC = 910112,               // 百度墨卡托 (bd09mc 910112)
//    GCJ02_MC = 910111,                 // 四维、高德墨卡托 (GCJ02 墨卡托 910111)
//    Gauss_Kruger_3 = 80103,            // 高斯投影3度带
//    Gauss_Kruger_6 = 80106,            // 高斯投影6度带

//};
}  // namespace silly::geo::proj

#endif  // SILLY_UTILS_SILLY_PROJECTION_DEFINE_H
