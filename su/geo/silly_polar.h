//
// Created by dell on 2023/11/17.
//

#ifndef SILLY_UTILS_SILLY_POLAR_H
#define SILLY_UTILS_SILLY_POLAR_H

/*
 * azimuth 一般指方位角。 方位角，又称地平经度(Azimuth angle，缩写为Az)，是在平面上量度物体之间的角度差的方法之一。是从某点的指北方向线起，依顺时针方向到目标方向线之间的水平夹角。
 * */

class silly_polar
{
  public:

    /// <summary>
    /// 将笛卡尔坐标转换为极坐标
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <param name="r"></param>
    /// <param name="azimuth"></param>
    static void normal_to_polar(const double& x, const double& y, double& r, double& azimuth);
    
    /// <summary>
    /// 将笛卡尔坐标转换为极坐标
    /// </summary>
    /// <param name="x0"></param>
    /// <param name="y0"></param>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <param name="r"></param>
    /// <param name="azimuth"></param>
    static void normal_to_polar(const double& x0, const double& y0, const double& x, const double& y, double& r, double& azimuth);
    
    /// <summary>
    /// 将极坐标转换为笛卡尔坐标
    /// </summary>
    /// <param name="r"></param>
    /// <param name="azimuth"></param>
    /// <param name="x"></param>
    /// <param name="y"></param>
    static void normal_to_coord(const double& r, const double& azimuth, double& x, double& y);
    
    /// <summary>
    /// 将极坐标转换为笛卡尔坐标
    /// </summary>
    /// <param name="r"></param>
    /// <param name="azimuth"></param>
    /// <param name="x0"></param>
    /// <param name="y0"></param>
    /// <param name="x"></param>
    /// <param name="y"></param>
    static void normal_to_coord(const double& r, const double& azimuth, const double& x0, const double& y0, double& x, double& y);
};

#endif  // SILLY_UTILS_SILLY_POLAR_H
