/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/7 13:01
 * @version: 1.0.1
 * @description:
 */
//
// Created by dell on 2024/6/7. 实现
//

#pragma once
#include "silly_proj.h"

void silly_proj::gauss_to_lonlat(const double& gx, const double& gy, double& lon, double& lat, const silly_guass_param& p)
{
    double centralMeridian = p.central;

    // 辅助项
    double n = (p.major_axis - p.major_axis * (1 - 1 / p.flatten)) / (p.major_axis + p.major_axis * (1 - 1 / p.flatten));
    double semiMinorAxis = p.major_axis * (1 - 1 / p.flatten);
    double firstEccentricitySquared = (p.major_axis * p.major_axis - semiMinorAxis * semiMinorAxis) / (p.major_axis * p.major_axis);
    double secondEccentricitySquared = (p.major_axis * p.major_axis - semiMinorAxis * semiMinorAxis) / (semiMinorAxis * semiMinorAxis);

    // 计算Y坐标对应的纬度
    double mu = (gy - 10000000.0) / (p.scale * semiMinorAxis);
    double phi = mu + (3 * n / 2 - 27 * n * n * n / 32) * sin(2 * mu) + (21 * n * n / 16 - 55 * n * n * n / 32) * sin(4 * mu) + (151 * n * n * n / 96) * sin(6 * mu) + (1097 * n * n * n * n / 512) * sin(8 * mu);

    // 计算X坐标对应的经度
    double lambda = gx - p.easting;
    double lambdaPrime = lambda / (p.major_axis * cos(phi));

    // 迭代计算纬度
    double phiPrime = phi;
    double phiNew = phi + (phiPrime - phi - lambdaPrime * cos(phi)) / (1 - firstEccentricitySquared * sin(phi) * sin(phi) - lambdaPrime * lambdaPrime * cos(phi) * cos(phi));

    while (fabs(phiNew - phiPrime) > 1e-10)
    {
        phiPrime = phiNew;
        phiNew = phi + (phiPrime - phi - lambdaPrime * cos(phiPrime)) / (1 - firstEccentricitySquared * sin(phiPrime) * sin(phiPrime) - lambdaPrime * lambdaPrime * cos(phiPrime) * cos(phiPrime));
    }

    phi = phiNew;

    // 转换为经纬度
    lat = phi * 180.0 / SU_PI;
    lon = (centralMeridian + lambdaPrime * 180.0 / (SU_PI * p.major_axis * cos(phi))) * 180.0 / SU_PI;
}

void silly_proj::gauss_to_lonlat(const double& central, const double& gx, const double& gy, double& lon, double& lat)
{
    double Y = gx, X = gy;

    Y -= 500000;
    double e1 = (1 - sqrt(1 - WGS84_E2)) / (1 + sqrt(1 - WGS84_E2));
    double M = X;
    double mu = M / (WGS84_SEMI_MAJOR_AXIS * (1 - WGS84_E2 / 4.0 - 3 * WGS84_E2 * WGS84_E2 / 64.0 - 5 * WGS84_E2 * WGS84_E2 * WGS84_E2 / 256.0));
    double phi1 = mu + (3 * e1 / 2 - 27 * e1 * e1 * e1 / 32) * sin(2 * mu) + (21 * e1 * e1 / 16 - 55 * e1 * e1 * e1 * e1 / 32) * sin(4 * mu) + (151 * e1 * e1 * e1 / 96) * sin(6 * mu) + (1097 * e1 * e1 * e1 * e1 / 512) * sin(8 * mu);

    double C1 = WGS84_E2 * cos(phi1) * cos(phi1) / (1 - WGS84_E2);
    double T1 = tan(phi1) * tan(phi1);
    double N1 = WGS84_SEMI_MAJOR_AXIS / sqrt(1 - WGS84_E2 * sin(phi1) * sin(phi1));
    double R1 = WGS84_SEMI_MAJOR_AXIS * (1 - WGS84_E2) / pow(1 - WGS84_E2 * sin(phi1) * sin(phi1), 1.5);
    double D = Y / N1;

    // 经纬度计算
    double phi = phi1 - (N1 * tan(phi1) / R1) * (D * D / 2 - (5 + 3 * T1 + 10 * C1 - 4 * C1 * C1 - 9 * e1) * D * D * D * D / 24 + (61 + 90 * T1 + 298 * C1 + 45 * T1 * T1 - 252 * e1 - 3 * C1 * C1) * D * D * D * D * D * D / 720);

    double lambda = central * SU_PI / 180 + (D - (1 + 2 * T1 + C1) * D * D * D / 6 + (5 - 2 * C1 + 28 * T1 - 3 * C1 * C1 + 8 * e1 + 24 * T1 * T1) * D * D * D * D * D / 120) / cos(phi1);

    // 转换为度
    lat = phi * 180 / SU_PI;
    lon = lambda * 180 / SU_PI;
}

void silly_proj::lonlat_to_gauss(const double& lon, const double& lat, double& gx, double& gy, const silly_guass_param& p)
{
    // 中央经线
    double L0 = p.central;

    // 将经纬度转换为弧度
    double lon_rad = lon * SU_PI / 180.0;
    double lat_rad = lat * SU_PI / 180.0;

    // 计算经差
    double lambda = (lon_rad - L0 * SU_PI / 180.0);

    // 辅助项
    double n = (p.major_axis - p.major_axis * (1 - 1 / p.flatten)) / (p.major_axis + p.major_axis * (1 - 1 / p.flatten));
    double semiMinorAxis = p.major_axis * (1 - 1 / p.flatten);
    double firstEccentricitySquared = (p.major_axis * p.major_axis - semiMinorAxis * semiMinorAxis) / (p.major_axis * p.major_axis);
    double secondEccentricitySquared = (p.major_axis * p.major_axis - semiMinorAxis * semiMinorAxis) / (semiMinorAxis * semiMinorAxis);

    // 计算N和T
    double radiusOfCurvature = p.major_axis / sqrt(1 - firstEccentricitySquared * sin(lat_rad) * sin(lat_rad));
    double tangentSquared = tan(lat_rad) * tan(lat_rad);
    double meridianCurvature = secondEccentricitySquared * cos(lat_rad) * cos(lat_rad);

    // 计算A和B
    double A = lambda * cos(lat_rad);
    double B = lambda * lambda * cos(lat_rad) * cos(lat_rad) / 2;

    // X坐标
    gx = radiusOfCurvature * (lambda + (1 - tangentSquared + meridianCurvature) * pow(A, 3) / 6 +
                              (5 - 18 * tangentSquared + tangentSquared * tangentSquared + 72 * meridianCurvature - 58 * (1 - firstEccentricitySquared) / (1 + firstEccentricitySquared)) * pow(A, 5) / 120) +
         p.easting;

    // Y坐标
    gy = p.scale * (semiMinorAxis * (lat_rad - (1 - n + (5 * n * n) / 4 - (5 * n * n * n) / 4) * sin(2 * lat_rad) / 2 + (1 - (3 * n) / 2 + (21 * n * n) / 16 - (55 * n * n * n) / 32) * sin(4 * lat_rad) / 24 -
                                     (1 - (11 * n) / 4 + (413 * n * n) / 96 - (1231 * n * n * n) / 256) * sin(6 * lat_rad) / 720 + (1 - (15 * n) / 4 + (517 * n * n) / 64 - (5147 * n * n * n) / 256) * sin(8 * lat_rad) / 40320)) +
         10000000.0;
}

void silly_proj::lonlat_to_gauss(const double& central, const double& lon, const double& lat, double& gx, double& gy)
{
    double latRad = lat * SU_PI / 180.0;
    double lonRad = lon * SU_PI / 180.0;

    // 将中央子午线转换为弧度
    double centralMeridianRad = central * SU_PI / 180.0;

    // 计算高斯-克吕格投影公式中的参数
    double N = WGS84_SEMI_MAJOR_AXIS / sqrt(1 - WGS84_E2 * sin(latRad) * sin(latRad));
    double T = tan(latRad) * tan(latRad);
    double C = WGS84_E2 * cos(latRad) * cos(latRad) / (1 - WGS84_E2);
    double A = (lonRad - centralMeridianRad) * cos(latRad);

    double M = WGS84_SEMI_MAJOR_AXIS * ((1 - WGS84_E2 / 4.0 - 3.0 * WGS84_E2 * WGS84_E2 / 64.0 - 5.0 * WGS84_E2 * WGS84_E2 * WGS84_E2 / 256.0) * latRad -
                                        (3.0 * WGS84_E2 / 8.0 + 3.0 * WGS84_E2 * WGS84_E2 / 32.0 + 45.0 * WGS84_E2 * WGS84_E2 * WGS84_E2 / 1024.0) * sin(2.0 * latRad) +
             (15.0 * WGS84_E2 * WGS84_E2 / 256.0 + 45.0 * WGS84_E2 * WGS84_E2 * WGS84_E2 / 1024.0) * sin(4.0 * latRad) - (35.0 * WGS84_E2 * WGS84_E2 * WGS84_E2 / 3072.0) * sin(6.0 * latRad));

    // 计算 X, Y 坐标
    gy = M + N * tan(latRad) * (A * A / 2.0 + (5.0 - T + 9.0 * C + 4.0 * C * C) * A * A * A * A / 24.0 + (61.0 - 58.0 * T + T * T + 600.0 * C - 330.0 * WGS84_E2) * A * A * A * A * A * A / 720.0);
    gx = N * (A + (1.0 - T + C) * A * A * A / 6.0 + (5.0 - 18.0 * T + T * T + 72.0 * C - 58.0 * WGS84_E2) * A * A * A * A * A / 120.0) + 500000.0;  // 中央子午线偏移+500000.0
}

void silly_proj::mercator_to_lonlat(const double& mctx, const double& mcty, double& lon, double& lat)
{
    lon = mctx / WGS84_SEMI_MAJOR_AXIS * SU_PI * 180;
    lat = mcty / WGS84_SEMI_MAJOR_AXIS * SU_PI * 180;
    lat = 180 / SU_PI * (2 * std::atan(std::exp(lat * SU_PI / 180)) - SU_PI / 2);
}

void silly_proj::lonlat_to_mercator(const double& lon, const double& lat, double& mctx, double& mcty)
{
    mctx = lon * WGS84_SEMI_MAJOR_AXIS * SU_PI / 180;
    mcty = std::log(std::tan((90 + lat) * SU_PI / 360)) / (SU_PI / 180);
    mcty = mcty * 20037508.34 / 180;
}

void silly_proj::mercator_to_gauss(const double& mctx, const double& mcty, double& gx, double& gy, const silly_guass_param& p)
{
}

void silly_proj::gauss_to_mercator(const double& gx, const double& gy, double& mctx, double& mcty, const silly_guass_param& p)
{
}

void silly_proj::mercator_to_gauss(const double& central, const double& mctx, const double& mcty, double& gx, double& gy)
{
    double lon, lat;
    mercator_to_lonlat(mctx, mcty, lon, lat);
    lonlat_to_gauss(central, lon, lat, gx, gy);
}

void silly_proj::gauss_to_mercator(const double& central, const double& gx, const double& gy, double& mctx, double& mcty)
{
    double lon, lat;
    gauss_to_lonlat(central, gx, gy, lon, lat);
    lonlat_to_mercator(lon, lat, mctx, mcty);
}
