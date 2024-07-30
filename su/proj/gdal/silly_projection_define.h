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



enum class silly_proj_def_enum
{
    
    GCS_WGS_1984 = 4326,   // WGS84 经纬度
    PCS_WGS_1984_WEB_MERCATOR = 3857,  // Web地图墨卡托
    GCS_CHINA_2000 = 4490,  // 大地2000
    GCS_XIAN_1980 = 4610,  // 西安80
    GCS_BEIJING_1954 = 4214,     // 北京54
    BaiDu09_LL = 910102,  // 百度经纬度 (bd09ll 910102)
    GCJ02_LL = 910101,    // 四维、高德经纬度 (GCJ02 910101)
    Baidu09_MC = 910112,  // 百度墨卡托 (bd09mc 910112)
    GCJ02_MC = 910111,     // 四维、高德墨卡托 (GCJ02 墨卡托 910111)
    Gauss_Kruger_3 = 80103,            // 高斯投影3度带
    Gauss_Kruger_6 = 80106,         // 高斯投影6度带

};


class silly_projection_define
{
  public:
    static const char* get(const silly_proj_def_enum& def) ;
    static const char* get(const int& def);
  protected:
    static char *epsg3857Wkt;
    static char *epsg4326Wkt;
    static char *epsg4490Wkt;
    static char *epsg4610Wkt;
    static char *epsg4214Wkt;
    static char *epsgGC03Wkt;


};

#endif  // SILLY_UTILS_SILLY_PROJECTION_DEFINE_H
