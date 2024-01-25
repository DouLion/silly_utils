/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/25 17:13
 * @version: 1.0.1
 * @description: 雷达坐标系
 */
#ifndef SILLY_UTILS_SILLY_RADAR_PROJ_H
#define SILLY_UTILS_SILLY_RADAR_PROJ_H

#include <su_marco.h>

class silly_radar_earth
{
public:
    double r_numda, r_phy, r_h;
    double EarthRadius, r_res;
    int rx, ry;

public:
    //根据经纬度值来计算方位和地心角
    void geo2polar(double phy, double delta_numda, double& beta, double& theta);
    void polar2geo(double beta, double theta, double& phy, double& delta_numda);

    void arcS_of_gps(double Az, double S, double& lon, double& lat);
    void gps_to_Azimuth(double lon, double lat, double& Az, double& beta);
    void gps_to_arcS(double lon, double lat, double& Az, double& S);
    void gps_to_arcS(double lon, double lat, int& sx, int& sy);

    // constructor
    silly_radar_earth();
    virtual ~silly_radar_earth() = default;
};

//-------------------- 等效地球代替地球半径近似定位计算 ---------------------------
class silly_radar_proj : public silly_radar_earth
{
private:
    double Km;

public:
    void Initialized(double dNdz, int x0, int y0, int res);

    //根据雷达回波坐标计算目标物的地理坐标
    void tilt_of_gps(double e, double dl, double& dh, double& dS);
    void tilt_of_gps(double e, double az, double dl, double& lon, double& lat, double& dh, double& dS);

    void tilt_of_gps(double e, int i, int j, double& lon, double& lat, double& dh, double& dS);

    //根据经纬度值和仰角来计算方位、距离
    void gps_to_tilt(double lon, double lat, double e, double& Az, double& L, double& H);
    void gps_to_tilt(double lon, double lat, double e, double& x, double& y);

    void DEM_to_tilt(double lon, double lat, double h, double& ele, double& az, double& L, double& S);

    void ES_of_H(double e, double dS, double& dh);
    void HS_of_E(double H, double dS, double& ele);

    //根据计算波束高度
    void RayPath_of_SH(double e, int Bins, short* S, short* H);

    // constructor
    silly_radar_proj(double lon, double lat, double h);
    virtual ~silly_radar_proj() = default;
};

#endif //SILLY_UTILS_SILLY_RADAR_PROJ_H
