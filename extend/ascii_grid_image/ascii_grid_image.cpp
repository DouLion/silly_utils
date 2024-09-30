//
// Created by dell on 2024/7/19.
//

#include "ascii_grid_image.h"
#include "image/png_utils.h"
#include "files/silly_file.h"
#include "proj/gdal/silly_proj_convert.h"
using namespace silly_image;

#include <cmath>  // 引入标准数学库

// 定义椭球参数
#define AXIS 6378136.6                                       // 地球长半轴
#define FLATTENING_R 1.0 / 298.257223563                     // 扁率
#define E2 (2 * FLATTENING_R - FLATTENING_R * FLATTENING_R)  // 第一偏心率的平方
// #define SU_PI 3.14159265358979323846

// 高斯投影坐标转经纬度坐标
void GaussToBL(double L0, double X, double Y, double* longitude, double* latitude)
{
    Y -= 500000;
    double e1 = (1 - sqrt(1 - E2)) / (1 + sqrt(1 - E2));
    double M = X;
    double mu = M / (AXIS * (1 - E2 / 4.0 - 3 * E2 * E2 / 64.0 - 5 * E2 * E2 * E2 / 256.0));
    double phi1 = mu + (3 * e1 / 2 - 27 * e1 * e1 * e1 / 32) * sin(2 * mu) + (21 * e1 * e1 / 16 - 55 * e1 * e1 * e1 * e1 / 32) * sin(4 * mu) + (151 * e1 * e1 * e1 / 96) * sin(6 * mu) + (1097 * e1 * e1 * e1 * e1 / 512) * sin(8 * mu);

    double C1 = E2 * cos(phi1) * cos(phi1) / (1 - E2);
    double T1 = tan(phi1) * tan(phi1);
    double N1 = AXIS / sqrt(1 - E2 * sin(phi1) * sin(phi1));
    double R1 = AXIS * (1 - E2) / pow(1 - E2 * sin(phi1) * sin(phi1), 1.5);
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
    *x = AXIS * lonRad;
    *y = AXIS * log(tan(SU_PI / 4 + latRad / 2));
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
    double N = R / sqrt(1 - E2 * sin(latRad) * sin(latRad));
    double T = tan(latRad) * tan(latRad);
    double C = E2 * cos(latRad) * cos(latRad) / (1 - E2);
    double A = (lonRad - centralMeridianRad) * cos(latRad);

    double M = R * ((1 - E2 / 4.0 - 3.0 * E2 * E2 / 64.0 - 5.0 * E2 * E2 * E2 / 256.0) * latRad - (3.0 * E2 / 8.0 + 3.0 * E2 * E2 / 32.0 + 45.0 * E2 * E2 * E2 / 1024.0) * sin(2.0 * latRad) +
                    (15.0 * E2 * E2 / 256.0 + 45.0 * E2 * E2 * E2 / 1024.0) * sin(4.0 * latRad) - (35.0 * E2 * E2 * E2 / 3072.0) * sin(6.0 * latRad));

    // 计算 X, Y 坐标
    *X = M + N * tan(latRad) * (A * A / 2.0 + (5.0 - T + 9.0 * C + 4.0 * C * C) * A * A * A * A / 24.0 + (61.0 - 58.0 * T + T * T + 600.0 * C - 330.0 * E2) * A * A * A * A * A * A / 720.0);
    *Y = N * (A + (1.0 - T + C) * A * A * A / 6.0 + (5.0 - 18.0 * T + T * T + 72.0 * C - 58.0 * E2) * A * A * A * A * A / 120.0) + 500000.0;  // 中央子午线偏移+500000.0
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
#define DEPTH_TINY 1e-2

void convert_image(double ncols, double nrows, double xllcorner, double yllcorner, double cellsize, double mid, double* data, double* qx, double* qy, char* img_path)
{
    double uMax = -99999.0, uMin = 99999.0, vMax = -99999.0, vMin = 99999.0;
    double hMax = -99999.0, hMin = 99999.0;
    if (data)
    {
        for (size_t r = 0; r < nrows; ++r)
        {
            for (size_t c = 0; c < ncols; ++c)
            {
                int pos = SU_MAX(0, SU_MIN(r * ncols + c, nrows * ncols));
                double h = data[pos];
                if (h < DEPTH_TINY)
                {
                    continue;
                }
                hMax = SU_MAX(hMax, h);
                hMin = SU_MIN(hMin, h);

                double u = qx[pos] / h;
                double v = qy[pos] / h;

                if (u > DEPTH_TINY)
                {
                    uMax = SU_MAX(uMax, u);
                    uMin = SU_MIN(uMin, u);
                }
                if (v > DEPTH_TINY)
                {
                    vMax = SU_MAX(vMax, v);
                    vMin = SU_MIN(vMin, v);
                }
            }
        }
    }

    /*hMax = SU_MAX(0., hMax);
    hMin = SU_MIN(0., hMin);
    uMax = SU_MAX(0., uMax);
    uMin = SU_MIN(0., uMin);
    vMax = SU_MAX(0., vMax);
    vMin = SU_MIN(0., vMin);*/

    double mct_letf = 0, mct_right = 0, mct_top = 0, mct_bottom = 0;
    const double gauss_left = xllcorner, gauss_right = xllcorner + cellsize * (ncols - 2), gauss_top = yllcorner + cellsize * (nrows - 2), gauss_bottom = yllcorner;
    // std::cout << mid << std::endl;
    {
        double left_top_x, left_top_y;
        GaussToBL(mid, gauss_top, gauss_left, &left_top_x, &left_top_y);
        double right_bottom_x, right_bottom_y;
        GaussToBL(mid, gauss_bottom, gauss_right, &right_bottom_x, &right_bottom_y);
        double left_bottom_x, left_bottom_y;
        GaussToBL(mid, gauss_bottom, gauss_left, &left_bottom_x, &left_bottom_y);
        double right_top_x, right_top_y;
        GaussToBL(mid, gauss_top, gauss_right, &right_top_x, &right_top_y);

        double geo_left = SU_MAX(left_top_x, left_bottom_x);

        // 112 .34067;                   // SU_MIN(left_top_x, left_bottom_x)
        double geo_right = SU_MIN(right_bottom_x, right_top_x);

        // 112 .5683;  // SU_MAX(right_top_x, right_bottom_x);
        double geo_top = SU_MIN(right_top_y, left_top_y);
        // 26 .98808;
        // SU_MAX(left_top_y, right_top_y);
        double geo_bottom = SU_MAX(left_bottom_y, right_bottom_y);
        // 26.86;
        // SU_MIN(left_bottom_y, right_bottom_y);

        latLonToMercator(geo_top, geo_left, &mct_letf, &mct_top);
        latLonToMercator(geo_bottom, geo_right, &mct_right, &mct_bottom);
        /* std::cout << geo_left << " " << geo_top << " " << geo_right << " " << geo_bottom << std::endl;
         std::cout << mct_letf << " " << mct_top << " " << mct_right << " " << mct_bottom << std::endl;*/
    }

    double mct_x_step = (mct_right - mct_letf) / (ncols - 2);
    double mct_y_step = (mct_top - mct_bottom) / (nrows - 2);
    png_data pd = silly_image::png_utils::create_empty((nrows - 2), (ncols - 2), PNG_COLOR_TYPE_RGB);
    png_data pd2 = silly_image::png_utils::create_empty((nrows - 2), (ncols - 2), PNG_COLOR_TYPE_RGB);
    double hstep = (hMax - hMin) / 255;
    double ustep = (uMax - uMin) / 255;
    double vstep = (vMax - vMin) / 255;

    for (size_t r = 0; r < nrows - 2; ++r)
    {
        for (size_t c = 0; c < ncols - 2; ++c)
        {
            double tmp_mct_x = mct_letf + c * mct_x_step;
            double tmp_mct_y = mct_top - r * mct_y_step;
            double tmp_gauss_x, tmp_gauss_y;
            mercator_gauss(tmp_mct_x, tmp_mct_y, tmp_gauss_x, tmp_gauss_y, mid);

            int gcol = std::round((tmp_gauss_x - gauss_left) / cellsize + 1);
            int grol = std::round((gauss_top - tmp_gauss_y) / cellsize);
            if (gcol >= 0 && grol > 0 && gcol < ncols && grol < nrows)
            {
                int pos = SU_MAX(0, SU_MIN(grol * ncols + gcol, nrows * ncols));
                double v = data[pos];
                silly_color tmp(0, 0, 0);
                if (v < DEPTH_TINY)
                {
                    tmp.red = (0 - uMin) / ustep;
                    tmp.green = (0 - vMin) / vstep;
                }
                else
                {
                    double qu = qx[pos] / v;
                    double qv = qy[pos] / v;

                    tmp.red = (qu - uMin) / ustep;
                    tmp.green = (qv - vMin) / vstep;
                }
                pd2.set_pixel(r, c, tmp);

                // 写入深度数据
                if (v > DEPTH_TINY)
                {
                    silly_color sc;
                    sc.red = static_cast<unsigned char>(std::min(255., (v - hMin) / hstep));
                    pd.set_pixel(r, c, sc);
                }
            }
            else
            {
                silly_color tmp(0, 0, 0);
                tmp.red = 255.0 * (0 - uMin) / ustep;
                tmp.green = 255.0 * (0 - vMin) / vstep;
                pd2.set_pixel(r, c, tmp);
            }
        }
    }
    std::string h_pd_data;
    {
        h_pd_data.resize(24);
        int ihMax = static_cast<int>(hMax / DEPTH_TINY);
        int ihMin = static_cast<int>(hMin / DEPTH_TINY);
        int iRow = static_cast<int>(nrows);
        int iCol = static_cast<int>(ncols);
        memcpy(&h_pd_data[0], &iRow, sizeof(iRow));
        memcpy(&h_pd_data[0] + sizeof(int) * 1, &iCol, sizeof(iCol));
        memcpy(&h_pd_data[0] + sizeof(int) * 2, &ihMax, sizeof(ihMax));
        memcpy(&h_pd_data[0] + sizeof(int) * 3, &ihMin, sizeof(ihMin));
    }
    std::string h_png_data;
    png_utils::memory_encode(pd, h_png_data);
    h_pd_data.append(h_png_data);
    silly_file::write(img_path, h_pd_data);
    pd.release();
    std::string filename = std::filesystem::path(img_path).filename().string();
    filename[0] = 'Q';
    std::string pd2_path = std::filesystem::path(img_path).parent_path().append(filename).string();
    std::string pd2_data;

    pd2_data.resize(24);
    int iUMax = static_cast<int>(uMax / DEPTH_TINY);
    int iVMax = static_cast<int>(vMax / DEPTH_TINY);
    int iUMin = static_cast<int>(uMin / DEPTH_TINY);
    int iVMin = static_cast<int>(vMin / DEPTH_TINY);
    int iRow = static_cast<int>(nrows);
    int iCol = static_cast<int>(ncols);
    memcpy(&pd2_data[0], &iRow, sizeof(iRow));
    memcpy(&pd2_data[0] + sizeof(int) * 1, &iCol, sizeof(iCol));
    memcpy(&pd2_data[0] + sizeof(int) * 2, &iUMax, sizeof(iUMax));
    memcpy(&pd2_data[0] + sizeof(int) * 3, &iUMin, sizeof(iUMin));
    memcpy(&pd2_data[0] + sizeof(int) * 4, &iVMax, sizeof(iVMax));
    memcpy(&pd2_data[0] + sizeof(int) * 5, &iVMin, sizeof(iVMin));
    std::string pd2_content;
    png_utils::memory_encode(pd2, pd2_content);
    pd2_data.append(pd2_content);
    silly_file::write(pd2_path, pd2_data);
    pd2.release();
}
void convert_image_test(double ncols, double nrows)
{
    double gx, gy;
    mercator_gauss(12702604.649057379, 3976012.012616653, gx, gy, 114.0);

    std::cout << gx << " " << gy << std::endl;

    return;
    std::cout << "Cols:\t" << ncols << std::endl;
    std::cout << "Rows:\t" << nrows << std::endl;
}