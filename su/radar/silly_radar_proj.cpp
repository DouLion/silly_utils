//
// Created by dell on 2024/1/25.
//

#include "silly_radar_proj.h"
#include <cmath>

//-------------------- 地球坐标系定位计算 ---------------------------
silly_radar_earth::silly_radar_earth()
{
    EarthRadius = 6371.1;
    rx = ry = 0;
    r_res = 1.000;
}

// 根据经纬度值来计算方位和地心角
void silly_radar_earth::geo2polar(double phy, double delta_numda, double& beta, double& theta)
{
    double cos_beta = sin(phy) * sin(r_phy) + cos(phy) * cos(r_phy) * cos(delta_numda);
    double sin_beta = sqrt(1 - cos_beta * cos_beta);
    if (sin_beta > 1)
        sin_beta = 1;
    else if (sin_beta < -1)
        sin_beta = -1;
    beta = asin(sin_beta);

    double sin_theta = cos(phy) * sin(delta_numda) / sin_beta;
    if (sin_theta > 1)
        sin_theta = 1;
    else if (sin_theta < -1)
        sin_theta = -1;

    theta = asin(sin_theta);
    double lat90 = asin(cos_beta * sin(r_phy));

    // 方位判断
    if (phy < lat90)
        theta = PI - theta;
    else if (delta_numda < 0)
        theta = 2 * PI + theta;

    return;
}

void silly_radar_earth::polar2geo(double beta, double theta, double& phy, double& delta_numda)
{
    double sin_phy = cos(beta) * sin(r_phy) + sin(beta) * cos(r_phy) * cos(theta);
    double cos_phy = sqrt(1 - sin_phy * sin_phy);
    phy = asin(sin_phy);

    double sin_numda = sin(theta) * sin(beta) / cos_phy;

    if (sin_numda > 1)
        sin_numda = 1;
    else if (sin_numda < -1)
        sin_numda = -1;

    delta_numda = asin(sin_numda);

    return;
}

void silly_radar_earth::arcS_of_gps(double az, double S, double& lon, double& lat)
{
    double beta = S / EarthRadius;
    double theta = az * PI / 180.0;
    double phy, delta_numda;

    polar2geo(beta, theta, phy, delta_numda);

    lat = phy * 180 / PI;
    lon = (delta_numda + r_numda) * 180 / PI;

    return;
}

void silly_radar_earth::gps_to_Azimuth(double lon, double lat, double& az, double& beta)
{
    double phy = lat * PI / 180.0;
    double delta_numda = lon * PI / 180.0 - r_numda;

    double theta;

    geo2polar(phy, delta_numda, beta, theta);

    az = theta * 180.0 / PI;

    return;
}

void silly_radar_earth::gps_to_arcS(double lon, double lat, double& az, double& S)
{
    gps_to_Azimuth(lon, lat, az, S);

    S *= EarthRadius;

    return;
}

void silly_radar_earth::gps_to_arcS(double lon, double lat, int& sx, int& sy)
{
    double phy = lat * PI / 180.0;
    double delta_numda = lon * PI / 180.0 - r_numda;

    double beta, theta;

    geo2polar(phy, delta_numda, beta, theta);

    sx = int(beta * EarthRadius * sin(theta) / r_res + rx + 0.5);
    sy = int(ry - beta * EarthRadius * cos(theta) / r_res + 0.5);

    return;
}

// constructor

//-------------------- 等效地球代替地球半径近似定位计算 ------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

silly_radar_proj::silly_radar_proj(double lon, double lat, double h)
{
    r_numda = lon * PI / 180.0;
    ;                          // unit:弧度
    r_phy = lat * PI / 180.0;  // unit:弧度
    r_h = h * 0.001;
}

void silly_radar_proj::Initialized(double dNdz,  // 线性梯度 N/Km
                                   int x0,
                                   int y0,  // 雷达站参考坐标
                                   int res  // 格点分辨率（米）
)
{
    r_res = res * 0.001;  // Unit:Kilometer
    rx = x0;
    ry = y0;  // radar position
    Km = 1 / (1 - EarthRadius * dNdz * 0.000001);
    Km = 4.0 / 3.0;
}

// 根据雷达回波坐标计算目标物的地理坐标

// 根据雷达回波坐标计算目标物的地理坐标
void silly_radar_proj::tilt_of_gps(double e,    // 仰角度数
                                   double dl,   // 雷达探测距离（千米）
                                   double& dh,  // 拔海高度返回值（千米）
                                   double& dS   // 水平距离
)
{
    if (dl < r_res)
    {
        dh = r_h;
        dS = dl;
        return;
    }

    double Rm = Km * EarthRadius;  // 等效地球半径
    double alpha = e * PI / 180.0;
    double beta = Km * atan(dl * cos(alpha) / (Rm + r_h + dl * sin(alpha)));

    dS = EarthRadius * beta;                                                               // 水平距离
    dh = sqrt((Rm + r_h) * (Rm + r_h) + dl * dl + 2 * (Rm + r_h) * dl * sin(alpha)) - Rm;  // 高度

    return;
}

void silly_radar_proj::tilt_of_gps(double e,     // 仰角度数
                                   double az,    // 雷达方位度数
                                   double dl,    // 雷达探测距离（千米）
                                   double& lon,  // 经度返回值
                                   double& lat,  // 纬度返回值
                                   double& dh,   // 拔海高度返回值（千米）
                                   double& dS    // 水平距离
)
{
    tilt_of_gps(e, dl, dh, dS);

    if (dl < r_res)
    {
        lon = r_numda * 180.0 / PI;
        lat = r_phy * 180.0 / PI;
        return;
    }

    arcS_of_gps(az, dS, lon, lat);

    return;
}

// 根据回波格点坐标计算目标物的地理坐标
void silly_radar_proj::tilt_of_gps(double e,     // 仰角度数
                                   int i,        // 东西向格点坐标
                                   int j,        // 南北向格点坐标
                                   double& lon,  // 经度返回值
                                   double& lat,  // 纬度返回值
                                   double& dh,   // 拔海高度返回值
                                   double& dS    // 经度返回值
)

{
    double dl = sqrt((i - rx) * (i - rx) + (ry - j) * (ry - j)) * r_res;
    double az = asin((i - rx) * r_res / dl);

    if (i >= rx && j <= ry)
        az = az * 180.0 / PI;
    else if (i < rx && j <= ry)
        az = 360.0 + az * 180.0 / PI;
    else
        az = 180.0 - az * 180.0 / PI;

    tilt_of_gps(e, az, dl, lon, lat, dh, dS);
}

// 根据地面距离计算波束高度
void silly_radar_proj::ES_of_H(double e,   // elevation angle
                               double dS,  // 经度返回值
                               double& dh  // return value og height
)
{
    if (dS <= r_res)
        dh = r_h;
    else
    {
        double alpha = e * PI / 180.0;
        double Rm = EarthRadius * Km;
        double betam = dS / Rm;
        dh = (Rm + r_h) * cos(alpha) / cos(alpha + betam) - Rm;  // 高度
    }
}

// 根据水平距离和高度计算仰角
void silly_radar_proj::HS_of_E(double H, double dS, double& ele)
{
    double Rm = EarthRadius * Km;
    double beta = dS / Rm;

    double cos_beta = cos(beta);
    if (cos_beta > 1.0)
        cos_beta = 1.0;
    double sin_beta = sin(beta);

    double tan_alpha = (cos_beta * (Rm + H) - (Rm + r_h)) / (sin_beta * (Rm + H));

    ele = atan(tan_alpha) * 180.0 / PI;

    return;
}

// 根据经纬度值和仰角来计算方位、距离
void silly_radar_proj::gps_to_tilt(double lon,  // 经度值
                                   double lat,  // 纬度值
                                   double e,    // 仰角度数
                                   double& Az,  // 雷达方位度数返回值
                                   double& L,   // 雷达探测距离返回值(千米)
                                   double& H    // 高度返回值(千米)
)
{
    double beta0;
    gps_to_Azimuth(lon, lat, Az, beta0);
    beta0 = beta0 / Km;

    double alpha = e * PI / 180.0;
    L = (Km * EarthRadius + r_h) * sin(beta0) / cos(alpha + beta0);

    double Rm = EarthRadius * Km;
    H = sqrt((Rm + r_h) * (Rm + r_h) + L * L + 2 * (Rm + r_h) * L * sin(alpha)) - Rm;  // 高度
    // H=r_h+L*sin(alpha)+L*L*cos(alpha)*cos(alpha)/(2*Rm) ;
}

void silly_radar_proj::gps_to_tilt(double lon,  // 经度值
                                   double lat,  // 纬度值
                                   double e,    // 仰角度数
                                   double& x,
                                   double& y  // 返回的格点坐标
)
{
    double Az, L, H;
    gps_to_tilt(lon, lat, e, Az, L, H);

    double sin_az = sin(Az * PI / 180);
    double cos_az = cos(Az * PI / 180);

    x = L * sin_az / r_res;
    y = L * cos_az / r_res;
}

// 根据经纬度和拔海高度计算仰角、方位和距离；
void silly_radar_proj::DEM_to_tilt(double lon,  // 经度值
                                   double lat,  // 纬度值
                                   double H,    // 高度值(千米)
                                   double& e,   // 仰角返回度数
                                   double& Az,  // 雷达方位度数返回值
                                   double& L,   // 雷达探测距离返回值(千米)
                                   double& S    // 水平距离返回值(千米)
)
{
    double beta;
    gps_to_Azimuth(lon, lat, Az, beta);
    S = beta * EarthRadius;
    if (H <= r_h)  // 没有遮挡
    {
        e = 0;
        L = S;
        return;
    }

    beta = beta / Km;
    double Rm = Km * EarthRadius;
    double cos_beta = cos(beta);
    if (cos_beta > 1.0)
        cos_beta = 1.0;
    double sin_beta = sin(beta);

    e = atan(((Rm + H) * cos_beta - (Rm + r_h)) / ((Rm + H) * sin_beta));

    L = (Rm + H) * sin_beta / cos(e);

    e = e * 180.0 / PI;
}

// 根据计算波束高度
void silly_radar_proj::RayPath_of_SH(double e,  // elevation angle
                                     int Bins,  // total numbers of range gates
                                     short* S,
                                     short* H)  // return value og height
{
    double dh, dS;
    double dl = 0;  // 雷达探测距离（千米）
    for (int i = 0; i < Bins; ++i)
    {
        tilt_of_gps(e, dl, dh, dS);

        S[i] = short(dS / r_res + 0.5);
        H[i] = short(dh * 1000 + 0.5);

        dl += r_res;
    }

    return;
}