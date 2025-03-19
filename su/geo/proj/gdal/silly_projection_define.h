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
#if ENABLE_GDAL
#include <gdal_priv.h>
#include <gdal_alg.h>
#include <ogr_spatialref.h>
#include <cpl_conv.h>
#endif
namespace silly
{
namespace geo
{

namespace proj
{
class CRS
{
  public:
    enum type : int
    {
        // EPSG
        WGS_84_WEB_MERCATOR = 102113,  // Web地图墨卡托 ESRI: 102113
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

  public:
    static const std::string wkt(const type &def);
    static const std::string proj4(const type &def);
    static const std::string wellKnownGS(const type &def);
#if ENABLE_GDAL
    static const OGRSpatialReference reference(const type &def);
#endif

  protected:
    static std::string EPSG3857WKT;
    static std::string EPSG3857PROJ4;

    static std::string EPSG4326WKT;
    static std::string EPSG4326PROJ4;

    static std::string EPSG4490WKT;
    static std::string EPSG4490PROJ4;

    static std::string EPSG4534WKT;
    static std::string EPSG4534PROJ4;

    static std::string EPSG4535WKT;
    static std::string EPSG4535PROJ4;

    static std::string EPSG4536WKT;
    static std::string EPSG4536PROJ4;

    static std::string EPSG4537WKT;
    static std::string EPSG4537PROJ4;

    static std::string EPSG4538WKT;
    static std::string EPSG4538PROJ4;

    static std::string EPSG4539WKT;
    static std::string EPSG4539PROJ4;

    static std::string EPSG4540WKT;
    static std::string EPSG4540PROJ4;

    static std::string EPSG4541WKT;
    static std::string EPSG4541PROJ4;

    static std::string EPSG4542WKT;
    static std::string EPSG4542PROJ4;

    static std::string EPSG4543WKT;
    static std::string EPSG4543PROJ4;

    static std::string EPSG4544WKT;
    static std::string EPSG4544PROJ4;

    static std::string EPSG4545WKT;
    static std::string EPSG4545PROJ4;

    static std::string EPSG4546WKT;
    static std::string EPSG4546PROJ4;

    static std::string EPSG4547WKT;
    static std::string EPSG4547PROJ4;

    static std::string EPSG4548WKT;
    static std::string EPSG4548PROJ4;

    static std::string EPSG4549WKT;
    static std::string EPSG4549PROJ4;

    static std::string EPSG4550WKT;
    static std::string EPSG4550PROJ4;

    static std::string EPSG4551WKT;
    static std::string EPSG4551PROJ4;

    static std::string EPSG4552WKT;
    static std::string EPSG4552PROJ4;

    static std::string EPSG4553WKT;
    static std::string EPSG4553PROJ4;

    static std::string EPSG4554WKT;
    static std::string EPSG4554PROJ4;
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
}  // namespace proj
}  // namespace geo
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_PROJECTION_DEFINE_H
