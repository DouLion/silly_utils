//
// Created by dell on 2024/7/19.
//

#ifndef ASCII_GRID_IMAGE_H
#define ASCII_GRID_IMAGE_H


#if IS_WIN32 || IS_WIN64
#define TZX_DLL_EXPORT __declspec(dllexport)
#else
#include <tr1/memory>
// linux导出dll不需要声明
#define TZX_DLL_EXPORT
#endif
#include <su_marco.h>

#include <cmath>  // 引入标准数学库

// 定义椭球参数
#define a 6378136.6            // 地球长半轴
#define f 1.0 / 298.257223563  // 扁率
#define e2 (2 * f - f * f)     // 第一偏心率的平方
// #define SU_PI 3.14159265358979323846

// 高斯投影坐标转经纬度坐标
void GaussToBL(double L0, double X, double Y, double* longitude, double* latitude)
{
    Y -= 500000;
    double e1 = (1 - sqrt(1 - e2)) / (1 + sqrt(1 - e2));
    double M = X;
    double mu = M / (a * (1 - e2 / 4.0 - 3 * e2 * e2 / 64.0 - 5 * e2 * e2 * e2 / 256.0));
    double phi1 = mu + (3 * e1 / 2 - 27 * e1 * e1 * e1 / 32) * sin(2 * mu) + (21 * e1 * e1 / 16 - 55 * e1 * e1 * e1 * e1 / 32) * sin(4 * mu) + (151 * e1 * e1 * e1 / 96) * sin(6 * mu) + (1097 * e1 * e1 * e1 * e1 / 512) * sin(8 * mu);

    double C1 = e2 * cos(phi1) * cos(phi1) / (1 - e2);
    double T1 = tan(phi1) * tan(phi1);
    double N1 = a / sqrt(1 - e2 * sin(phi1) * sin(phi1));
    double R1 = a * (1 - e2) / pow(1 - e2 * sin(phi1) * sin(phi1), 1.5);
    double D = Y / N1;

    // 经纬度计算
    double phi = phi1 - (N1 * tan(phi1) / R1) * (D * D / 2 - (5 + 3 * T1 + 10 * C1 - 4 * C1 * C1 - 9 * e1) * D * D * D * D / 24 + (61 + 90 * T1 + 298 * C1 + 45 * T1 * T1 - 252 * e1 - 3 * C1 * C1) * D * D * D * D * D * D / 720);

    double lambda = L0 * SU_PI / 180 + (D - (1 + 2 * T1 + C1) * D * D * D / 6 + (5 - 2 * C1 + 28 * T1 - 3 * C1 * C1 + 8 * e1 + 24 * T1 * T1) * D * D * D * D * D / 120) / cos(phi1);

    // 转换为度
    *latitude = phi * 180 / SU_PI;
    *longitude = lambda * 180 / SU_PI;
}

// 函数：将度数转换为弧度
double degToRad(double deg)
{
    return deg * SU_PI / 180.0;
}
// 弧度转度函数
double radToDeg(double rad)
{
    return rad * 180.0 / SU_PI;
}
// 函数：将经纬度转换为墨卡托投影坐标
void latLonToMercator(double lat, double lon, double* x, double* y)
{
    // 将经纬度从度数转换为弧度
    double latRad = degToRad(lat);
    double lonRad = degToRad(lon);

    // 墨卡托投影公式
    *x = a * lonRad;
    *y = a * log(tan(SU_PI / 4 + latRad / 2));
}
// 经纬度转高斯-克吕格投影坐标（3度带）
void latLonToGauss(double L0, double lat, double lon, double* X, double* Y)
{
    // 经纬度转换为弧度
    const double R = 6378137.0;
    double latRad = degToRad(lat);
    double lonRad = degToRad(lon);

    double centralMeridian = L0;

    // 将中央子午线转换为弧度
    double centralMeridianRad = degToRad(centralMeridian);

    // 计算高斯-克吕格投影公式中的参数
    double N = R / sqrt(1 - e2 * sin(latRad) * sin(latRad));
    double T = tan(latRad) * tan(latRad);
    double C = e2 * cos(latRad) * cos(latRad) / (1 - e2);
    double A = (lonRad - centralMeridianRad) * cos(latRad);

    double M = R * ((1 - e2 / 4.0 - 3.0 * e2 * e2 / 64.0 - 5.0 * e2 * e2 * e2 / 256.0) * latRad - (3.0 * e2 / 8.0 + 3.0 * e2 * e2 / 32.0 + 45.0 * e2 * e2 * e2 / 1024.0) * sin(2.0 * latRad) +
                    (15.0 * e2 * e2 / 256.0 + 45.0 * e2 * e2 * e2 / 1024.0) * sin(4.0 * latRad) - (35.0 * e2 * e2 * e2 / 3072.0) * sin(6.0 * latRad));

    // 计算 X, Y 坐标
    *X = M + N * tan(latRad) * (A * A / 2.0 + (5.0 - T + 9.0 * C + 4.0 * C * C) * A * A * A * A / 24.0 + (61.0 - 58.0 * T + T * T + 600.0 * C - 330.0 * e2) * A * A * A * A * A * A / 720.0);
    *Y = N * (A + (1.0 - T + C) * A * A * A / 6.0 + (5.0 - 18.0 * T + T * T + 72.0 * C - 58.0 * e2) * A * A * A * A * A / 120.0) + 500000.0;  // 中央子午线偏移+500000.0
}
// 墨卡托投影坐标转经纬度
void mercatorToLatLon(double x, double y, double* lat, double* lon)
{
    const double R = 6378137.0;
    *lon = radToDeg(x / R);
    *lat = radToDeg(2 * atan(exp(y / R)) - SU_PI / 2);
}

void gauss_mercator(double gaussx, double gaussy, double& mx, double& my, double mid)
{
    double longitude, latitude;

    GaussToBL(mid, gaussy, gaussx, &longitude, &latitude);
    double x, y;
    latLonToMercator(latitude, longitude, &x, &y);
    mx = x;
    my = y;
}

void mercator_gauss(double mx, double my, double& gaussx, double& gaussy, double mid)
{
    double lat, lon;
    mercatorToLatLon(mx, my, &lat, &lon);

    latLonToGauss(mid, lat, lon, &gaussy, &gaussx);
}

#ifdef __cplusplus
extern "C" {
#endif
    TZX_DLL_EXPORT void convert_image(double ncols, double nrows, double xllcorner, double yllcorner, double cellsize, double mid, double* data, double* qx, double* qy, char* img_path);

TZX_DLL_EXPORT void convert_image_test(double ncols, double nrows);

#ifdef __cplusplus
}
#endif


#endif //ASCII_GRID_IMAGE_H
