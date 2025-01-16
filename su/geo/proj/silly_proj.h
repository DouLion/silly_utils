/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/7 13:01
 * @version: 1.0.1
 * @description:
 *  验证数据  
 *  121.5065,31.24396,10.0535
    地心坐标转化
    -2852335.3220309215,4653403.1465016846,3289053.8583770543
    121.5065000000,31.2439600000,10.0534999724
    墨卡托坐标转化
    13526041.7080728952,3664472.6874608034
    13526041.708072895 ,3664472.687460804
    121.5065000000,31.2439600000
    高斯坐标转化
    643517.2824050392,3459001.3302408773
    121.5064999999,31.2439600166
 */
#ifndef SILLY_UTILS_SILLY_PROJ_H
#define SILLY_UTILS_SILLY_PROJ_H
#include <su_marco.h>
#include <geo/silly_geo_const.h>

#define SU_GAUSS3_NO(lon) static_cast<int>(lon / 3.0 + 1)
#define SU_GAUSS6_NO(lon) static_cast<int>(lon / 6.0 + 1)
#define SU_GAUSS3_L0(no) static_cast<double>(3 * no - 1.5)
#define SU_GAUSS6_L0(no) static_cast<double>(6 * no - 3.0)

namespace silly
{
namespace geo
{

class proj
{
  public:
    struct param
    {
        // 中央经度线
        double central{117.0};
        double scale{0.9996};
        double easting{500000.0};  // 东偏移
        // 长半轴
        double major_axis{silly::geo::WGS84::A};
        // 扁率
        double flatten{silly::geo::WGS84::F};
    };

  public:
    /// <summary>
    /// 高斯投影反算经纬度
    /// </summary>
    /// <param name="gx"></param>
    /// <param name="gy"></param>
    /// <param name="lon"></param>
    /// <param name="lat"></param>
    /// <param name="p"></param>
    static void gauss_to_lonlat(const double& gx, const double& gy, double& lon, double& lat, const param& p);

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
    static void lonlat_to_gauss(const double& lon, const double& lat, double& gx, double& gy, const param& p);

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
    static void mercator_to_gauss(const double& mctx, const double& mcty, double& gx, double& gy, const param& p = param());

    /// <summary>
    /// 高斯投影转墨卡托
    /// </summary>
    /// <param name="gx"></param>
    /// <param name="gy"></param>
    /// <param name="mctx"></param>
    /// <param name="mcty"></param>
    /// <param name="p"></param>
    static void gauss_to_mercator(const double& gx, const double& gy, double& mctx, double& mcty, const param& p = param());

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

    /// <summary>
    /// 地心地固直角坐标系 转 经纬高坐标系
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <param name="z"></param>
    /// <param name="lon"></param>
    /// <param name="lat"></param>
    /// <param name="height"></param>
    static void ecef_to_lonlat(const double& x, const double& y, const double& z, double& lon, double& lat, double& height);

    /// <summary>
    /// 经纬高坐标系 转 地心地固直角坐标系
    /// </summary>
    /// <param name="lon"></param>
    /// <param name="lat"></param>
    /// <param name="height"></param>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <param name="z"></param>
    static void lonlat_to_ecef(const double& lon, const double& lat, const double& height, double& x, double& y, double& z);

  private:
};

}  // namespace geo
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_PROJ_H
