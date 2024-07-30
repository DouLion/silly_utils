/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/5/18 16:21
 * @version: 1.0.1
 * @software: TzxIsoLine
 * @description:
 */
#pragma once

#ifndef ELLIPSOID_PROJ_H
#define ELLIPSOID_PROJ_H

#include <cmath>

#define ELLIPSOID_I_PI 0.0174532925199433f
// double ELLIPSOID_54_BJ_E2 = 2 * f - f * f;
// 54年北京坐标系参数
// 长轴
#define ELLIPSOID_54_BJ_A 6378245.0f
// 扁率   (a-b)/a
#define ELLIPSOID_54_BJ_F 0.0033523298692591f

#define ELLIPSOID_54_BJ_E2 0.006693421622965873f

// 80年西安坐标系参数
#define ELLIPSOID_80_XA_A 6378140.0f
// double f=1/298.257;
#define ELLIPSOID_80_XA_F 0.0033528131778969f

#define ELLIPSOID_80_XA_E2 (2 * ELLIPSOID_80_XA_F - ELLIPSOID_80_XA_F * ELLIPSOID_80_XA_F)

// WGS84坐标系参数
#define ELLIPSOID_84_WGS_A 6378137.0f
#define ELLIPSOID_84_WGS_F 0.0033528106647475f
#define ELLIPSOID_84_WGS_E2 (2 * ELLIPSOID_84_WGS_F - ELLIPSOID_80_XA_F * ELLIPSOID_84_WGS_F)
// double f = 1 / 298.257223563;

#define ELLIPSOID_ZONE_WIDE 6.0f

#define GUASS_TO_LNG_LAT(l0, gx, gy, lng, lat, pjnm)

class EllipsoidProj
{
  public:
    // 获取带号
    static int GetProjNo(const double& lng, const double& lat)
    {
        int ProjNo = (int)(lng / ELLIPSOID_ZONE_WIDE);
        return ProjNo;
    }
    static void CoordinatetoGaussXY_BJ_54(const int& ProjNo, const double& lng, const double& lat, double& gaussX, double& gaussY)
    {
        // int ProjNo = 19;// (int)(lng / ELLIPSOID_ZONE_WIDE);
        double longitude0 = (double)ProjNo * ELLIPSOID_ZONE_WIDE + ELLIPSOID_ZONE_WIDE / 2.0;
        longitude0 = longitude0 * ELLIPSOID_I_PI;

        // 经度转换为弧度
        double longitude = lng * ELLIPSOID_I_PI;
        // 纬度转换为弧度
        double latitude = lat * ELLIPSOID_I_PI;

        double ee = ELLIPSOID_54_BJ_E2 * (1.0 - ELLIPSOID_54_BJ_E2);
        double N = ELLIPSOID_54_BJ_A / sqrt(1.0 - ELLIPSOID_54_BJ_E2 * sin(latitude) * sin(latitude));  // 该点的卯酉圈曲率半径
        double T = tan(latitude) * tan(latitude);
        double C = ee * cos(latitude) * cos(latitude);
        double A = (longitude - longitude0) * cos(latitude);
        double M = ELLIPSOID_54_BJ_A * ((1.0 - ELLIPSOID_54_BJ_E2 / 4.0 - 3.0 * ELLIPSOID_54_BJ_E2 * ELLIPSOID_54_BJ_E2 / 64 - 5 * ELLIPSOID_54_BJ_E2 * ELLIPSOID_54_BJ_E2 * ELLIPSOID_54_BJ_E2 / 256.0) * latitude -
                                        (3.0 * ELLIPSOID_54_BJ_E2 / 8.0 + 3.0 * ELLIPSOID_54_BJ_E2 * ELLIPSOID_54_BJ_E2 / 32 + 45.0 * ELLIPSOID_54_BJ_E2 * ELLIPSOID_54_BJ_E2 * ELLIPSOID_54_BJ_E2 / 1024.0) * sin(2.0 * latitude) +
                                        (15.0 * ELLIPSOID_54_BJ_E2 * ELLIPSOID_54_BJ_E2 / 256.0 + 45.0 * ELLIPSOID_54_BJ_E2 * ELLIPSOID_54_BJ_E2 * ELLIPSOID_54_BJ_E2 / 1024.0) * sin(4.0 * latitude) -
                                        (35.0 * ELLIPSOID_54_BJ_E2 * ELLIPSOID_54_BJ_E2 * ELLIPSOID_54_BJ_E2 / 3072.0) * sin(6.0 * latitude));
        gaussX = N * (A + (1 - T + C) * A * A * A / 6 + (5 - 18 * T + T * T + 72 * C - 58 * ee) * A * A * A * A * A / 120);
        gaussY = M + N * tan(latitude) * (A * A / 2 + (5 - T + 9 * C + 4 * C * C) * A * A * A * A / 24 + (61 - 58 * T + T * T + 600 * C - 330 * ee) * A * A * A * A * A * A / 720);
        double X0 = 1000000.0 * (ProjNo + 1) + 500000.0;
        double Y0 = 0;
        gaussX = gaussX + X0;
        gaussY = gaussY + Y0;
    }

    static void CoordinatetoGaussXY_WGS_84(const int& ProjNo, const double& lng, const double& lat, double& gaussX, double& gaussY)
    {
        // int ProjNo = 19;// (int)(lng / ELLIPSOID_ZONE_WIDE);
        double longitude0 = (double)ProjNo * ELLIPSOID_ZONE_WIDE + ELLIPSOID_ZONE_WIDE / 2.0;
        longitude0 = longitude0 * ELLIPSOID_I_PI;

        // 经度转换为弧度
        double longitude = lng * ELLIPSOID_I_PI;
        // 纬度转换为弧度
        double latitude = lat * ELLIPSOID_I_PI;

        double ee = ELLIPSOID_84_WGS_E2 * (1.0 - ELLIPSOID_84_WGS_E2);
        double N = ELLIPSOID_84_WGS_A / sqrt(1.0 - ELLIPSOID_84_WGS_E2 * sin(latitude) * sin(latitude));  // 该点的卯酉圈曲率半径
        double T = tan(latitude) * tan(latitude);
        double C = ee * cos(latitude) * cos(latitude);
        double A = (longitude - longitude0) * cos(latitude);
        double M = ELLIPSOID_84_WGS_A * ((1.0 - ELLIPSOID_84_WGS_E2 / 4.0 - 3.0 * ELLIPSOID_84_WGS_E2 * ELLIPSOID_84_WGS_E2 / 64 - 5 * ELLIPSOID_84_WGS_E2 * ELLIPSOID_84_WGS_E2 * ELLIPSOID_84_WGS_E2 / 256.0) * latitude -
                                         (3.0 * ELLIPSOID_84_WGS_E2 / 8.0 + 3.0 * ELLIPSOID_84_WGS_E2 * ELLIPSOID_84_WGS_E2 / 32 + 45.0 * ELLIPSOID_84_WGS_E2 * ELLIPSOID_84_WGS_E2 * ELLIPSOID_84_WGS_E2 / 1024.0) * sin(2.0 * latitude) +
                                         (15.0 * ELLIPSOID_84_WGS_E2 * ELLIPSOID_84_WGS_E2 / 256.0 + 45.0 * ELLIPSOID_84_WGS_E2 * ELLIPSOID_84_WGS_E2 * ELLIPSOID_84_WGS_E2 / 1024.0) * sin(4.0 * latitude) -
                                         (35.0 * ELLIPSOID_84_WGS_E2 * ELLIPSOID_84_WGS_E2 * ELLIPSOID_84_WGS_E2 / 3072.0) * sin(6.0 * latitude));
        gaussX = N * (A + (1 - T + C) * A * A * A / 6 + (5 - 18 * T + T * T + 72 * C - 58 * ee) * A * A * A * A * A / 120);
        gaussY = M + N * tan(latitude) * (A * A / 2 + (5 - T + 9 * C + 4 * C * C) * A * A * A * A / 24 + (61 - 58 * T + T * T + 600 * C - 330 * ee) * A * A * A * A * A * A / 720);
        double X0 = 1000000.0 * (ProjNo + 1) + 500000.0;
        double Y0 = 0;
        gaussX = gaussX + X0;
        gaussY = gaussY + Y0;
    }

    static void CoordinatetoGaussXY_WGS_84(const int& ProjNo, const int& len, const double* lng, const double* lat, double* gaussX, double* gaussY)
    {
        if (!lng || !lat || !gaussX || !gaussY)
        {
            return;
        }
        for (int i = 0; i < len; ++i)
        {
            EllipsoidProj::CoordinatetoGaussXY_WGS_84(ProjNo, lng[i], lat[i], gaussX[i], gaussY[i]);
        }
    }

    static void CoordinatetoGaussXY_BJ_54(const int& ProjNo, const int& len, const double* lng, const double* lat, double* gaussX, double* gaussY)
    {
        if (!lng || !lat || !gaussX || !gaussY)
        {
            return;
        }
        for (int i = 0; i < len; ++i)
        {
            EllipsoidProj::CoordinatetoGaussXY_BJ_54(ProjNo, lng[i], lat[i], gaussX[i], gaussY[i]);
        }
    }
};

#endif  // ELLIPSOID_PROJ_H
