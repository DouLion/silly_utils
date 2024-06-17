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


#include <gdal_priv.h>
#include <gdal_alg.h>
#include <ogr_spatialref.h>
#include <cpl_conv.h>

enum class silly_proj_enum
{
    GCS_WGS_1984 = 4326,
    PCS_WGS_1984_WEB_MERCATOR = 3857,
    GCS_CHINA_2000 = 4490,
    GCS_XIAN_1980 = 4610,
    GCS_BEIJING_1954 = 4214,
    BaiDu09_LL = 910102,  // 百度经纬度 (bd09ll 910102)
    GCJ02_LL = 910101,    // 四维、高德经纬度 (GCJ02 910101)
    Baidu09_MC = 910112,  // 百度墨卡托 (bd09mc 910112)
    GCJ02_MC = 910111     // 四维、高德墨卡托 (GCJ02 墨卡托 910111)</

};

struct silly_proj_param
{
    silly_proj_enum from;
    silly_proj_enum to;
    double central{114.0};
};

class silly_projection_define
{
  public:
    /// <summary>
    /// 根据参数构建转换
    /// </summary>
    /// <param name="p"></param>
    /// <returns></returns>
    bool begin(const silly_proj_param &p);

    bool convert(const double &fromX, const double &fromY, double &toX, double &toY);

    /// <summary>
    /// 关闭转换,释放转换对象
    /// </summary>
    /// <returns></returns>
    bool close();

  private:
    static char *epsg3857Wkt;
    static char *espg4326Wkt;
    static char *espg4490Wkt;
    static char *espg4610Wkt;
    static char *espg4214Wkt;

    OGRCoordinateTransformation *m_poTransform;
};

#endif  // SILLY_UTILS_SILLY_PROJECTION_DEFINE_H
