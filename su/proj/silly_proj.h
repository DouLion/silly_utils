/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/7 13:01
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_PROJ_H
#define SILLY_UTILS_SILLY_PROJ_H
#include <su_marco.h>

#define SU_GAUSS3_NO(lon) static_cast<int>(lon / 3.0 + 1)
#define SU_GAUSS6_NO(lon) static_cast<int>(lon / 6.0 + 1)
#define SU_GAUSS3_L0(no) static_cast<double>(3 * no - 1.5)
#define SU_GAUSS6_L0(no) static_cast<double>(6 * no - 3.0)

// 地球椭球体参数
// 长半轴
#define WGS84_SEMI_MAJOR_AXIS 6378137.0f
// 扁率
#define WGS84_FLATTENING_R (1.0 / 298.257222101f)

#define WGS84_E2 (2 * WGS84_FLATTENING_R - WGS84_FLATTENING_R * WGS84_FLATTENING_R)

// 54年北京坐标系参数
#define ELLIPSOID_54_BJ_A 6378245.0f
#define ELLIPSOID_54_BJ_F 0.0033523298692591f

// 80年西安坐标系参数
#define ELLIPSOID_80_XA_A 6378140.0f
#define ELLIPSOID_80_XA_F 0.0033528131778969f

// WGS84坐标系参数
#define ELLIPSOID_84_WGS_A 6378137.0f
#define ELLIPSOID_84_WGS_F 0.0033528106647475f


struct silly_gauss_param
{
    // 中央经度线
    double central{117.0};
    double scale{0.9996};
    double easting{500000.0};  // 东偏移
    // 长半轴
    double major_axis{WGS84_SEMI_MAJOR_AXIS};
    // 扁率
    double flatten{WGS84_FLATTENING_R};
};

class silly_proj
{
  public:
    /// <summary>
    /// 高斯投影反算经纬度
    /// </summary>
    /// <param name="gx"></param>
    /// <param name="gy"></param>
    /// <param name="lon"></param>
    /// <param name="lat"></param>
    /// <param name="p"></param>
    static void gauss_to_lonlat(const double& gx, const double& gy, double& lon, double& lat, const silly_gauss_param& p);

    /// <summary>
    /// 高斯投影反算经纬度,使用静态值速度会更快
    /// </summary>
    /// <param name="gx"></param>
    /// <param name="gy"></param>
    /// <param name="lon"></param>
    /// <param name="lat"></param>
    static void gauss_to_lonlat(const double& central, const double& gx, const double& gy, double& lon, double& lat);

    /// <summary>
    /// 经纬度转高斯投影
    /// </summary>
    /// <param name="lon"></param>
    /// <param name="lat"></param>
    /// <param name="gx"></param>
    /// <param name="gy"></param>
    /// <param name="p"></param>
    static void lonlat_to_gauss(const double& lon, const double& lat, double& gx, double& gy, const silly_gauss_param& p);

    /// <summary>
    /// 经纬度转高斯投影,使用静态值速度会更快
    /// </summary>
    /// <param name="lon"></param>
    /// <param name="lat"></param>
    /// <param name="gx"></param>
    /// <param name="gy"></param>
    static void lonlat_to_gauss(const double& central, const double& lon, const double& lat, double& gx, double& gy);

    /// <summary>
    /// 墨卡托转经纬度
    /// </summary>
    /// <param name="mctx"></param>
    /// <param name="mcty"></param>
    /// <param name="lon"></param>
    /// <param name="lat"></param>
    static void mercator_to_lonlat(const double& mctx, const double& mcty, double& lon, double& lat);

    /// <summary>
    /// 经纬度转墨卡托
    /// </summary>
    /// <param name="lon"></param>
    /// <param name="lat"></param>
    /// <param name="mctx"></param>
    /// <param name="mcty"></param>
    static void lonlat_to_mercator(const double& lon, const double& lat, double& mctx, double& mcty);

    /// <summary>
    /// 墨卡托转高斯投影
    /// </summary>
    /// <param name="mctx"></param>
    /// <param name="mcty"></param>
    /// <param name="gx"></param>
    /// <param name="gy"></param>
    /// <param name="p"></param>
    static void mercator_to_gauss(const double& mctx, const double& mcty, double& gx, double& gy, const silly_gauss_param& p = silly_gauss_param());

    /// <summary>
    /// 高斯投影转墨卡托
    /// </summary>
    /// <param name="gx"></param>
    /// <param name="gy"></param>
    /// <param name="mctx"></param>
    /// <param name="mcty"></param>
    /// <param name="p"></param>
    static void gauss_to_mercator(const double& gx, const double& gy, double& mctx, double& mcty, const silly_gauss_param& p = silly_gauss_param());

    /// <summary>
    /// 墨卡托转高斯投影,使用静态值速度会更快
    /// </summary>
    /// <param name="central"></param>
    /// <param name="mctx"></param>
    /// <param name="mcty"></param>
    /// <param name="gx"></param>
    /// <param name="gy"></param>
    static void mercator_to_gauss(const double& central, const double& mctx, const double& mcty, double& gx, double& gy);

    /// <summary>
    /// 高斯投影转墨卡托,使用静态值速度会更快
    /// </summary>
    /// <param name="central"></param>
    /// <param name="gx"></param>
    /// <param name="gy"></param>
    /// <param name="mctx"></param>
    /// <param name="mcty"></param>
    static void gauss_to_mercator(const double& central, const double& gx, const double& gy, double& mctx, double& mcty);

  private:
};

#endif  // SILLY_UTILS_SILLY_PROJ_H
