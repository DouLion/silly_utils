/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-02-14
 * @file: silly_radar_occluder.cpp
 * @description: silly_radar_occluder实现
 * @version: v1.0.1 2025-02-14 dou li yang
 */
#include "silly_radar_occluder.h"
#include <geo/proj/silly_proj.h>

// // 将极坐标转换为笛卡尔坐标
silly_point polar2cartesian(const silly_point& center, const double& r, const double& deg)
{
    return silly_point(center.x + r * std::cos(DEG2RAD(deg)), center.y + r * std::sin(DEG2RAD(deg)));
}

bool silly_radar_occluder::get_dem(const std::string& file)
{
    // silly::geo::utils::init_gdal_env();
    // 读取 tif 文件
    GDALDataset* poDataset;
    poDataset = (GDALDataset*)GDALOpen(file.c_str(), GA_ReadOnly);
    if (poDataset == nullptr)
    {
        std::cerr << "Failed to open DEM file" << std::endl;
        return false;
    }
    x_size = poDataset->GetRasterXSize();
    y_size = poDataset->GetRasterYSize();
    GDALRasterBand* poBand = poDataset->GetRasterBand(1);
    m_dem_data.resize(x_size * y_size);
    poBand->RasterIO(GF_Read, 0, 0, x_size, y_size, m_dem_data.data(), x_size, y_size, GDT_Float64, 0, 0);

    // 获取坐标范围（地理坐标）
    double geoTransform[6];
    poDataset->GetGeoTransform(geoTransform);
    double min_x = geoTransform[0];  //
    double max_y = geoTransform[3];  // 左上角Y坐标
    x_res = geoTransform[1];         // 经度方向的分辨率（度）
    y_res = geoTransform[5];         // 纬度方向的分辨率（度）
    double max_x = min_x + x_size * x_res;
    double min_y = max_y + y_size * y_res;

    left_top = silly_point(min_x, max_y);      // 左上角
    right_bottom = silly_point(max_x, min_y);  // 右下角
    left_bottom = silly_point(min_x, min_y);   // 左下
    right_top = silly_point(max_x, max_y);     // 右上

    GDALClose(poDataset);

    // silly::geo::utils::destroy_gdal_env();
    return true;
}

bool silly_radar_occluder::dem2polar(const silly_point& center, const double& radius)
{
    bool status = false;
    if (m_dem_data.empty())
    {
        SLOG_INFO("二维矩阵数据为空,请检查是否正确读取了dem文件");
        return status;
    }

    // 中央经线
    double central_lgtd = static_cast<int>((left_top.x + right_top.x) / 2 / 3) * 3;
    // 中心点
    silly_point center_point(center.x, center.y);
    // 中心点坐标转高斯坐标
    silly::geo::proj::convert::lonlat_to_gauss(central_lgtd, center.x, center.y, m_center.x, m_center.y);

    silly_point gs_left_top;      // 左上 高斯
    silly_point gs_right_bottom;  // 右下 高斯
    silly::geo::proj::convert::lonlat_to_gauss(central_lgtd, left_top.x, left_top.y, gs_left_top.x, gs_left_top.y);
    silly::geo::proj::convert::lonlat_to_gauss(central_lgtd, right_bottom.x, right_bottom.y, gs_right_bottom.x, gs_right_bottom.y);
    // 手动填入
    // silly_point gs_right_top(gs_right_bottom.x, gs_left_top.y);
    // silly_point gs_left_bottom(gs_left_top.x, gs_right_bottom.y);
    silly_point gs_right_top;    // 右上 高斯
    silly_point gs_left_bottom;  // 左下 高斯
    silly::geo::proj::convert::lonlat_to_gauss(central_lgtd, right_top.x, right_top.y, gs_right_top.x, gs_right_top.y);
    silly::geo::proj::convert::lonlat_to_gauss(central_lgtd, left_bottom.x, left_bottom.y, gs_left_bottom.x, gs_left_bottom.y);

    // 计算横向分辨率,米  / 计算纵向分辨率,米
    // double xResMeters = sqrt((gs_right_top.x - gs_right_bottom.x) * (gs_right_top.x - gs_right_bottom.x) + (gs_right_top.y - gs_right_bottom.y) * (gs_right_top.y - gs_right_bottom.y));
    // double yResMeters = sqrt((gs_right_bottom.x - gs_left_bottom.x) * (gs_right_bottom.x - gs_left_bottom.x) + (gs_right_bottom.y - gs_left_bottom.y) * (gs_right_bottom.y - gs_left_bottom.y));
    // xResMeters /= x_size;
    // yResMeters /= y_size;
    // double abx_x_res = std::abs(x_res);
    // double abx_y_res = std::abs(y_res);

    // 计算横向分辨率,米  / 计算纵向分辨率,米
    double xResMeters = gs_right_bottom.x - gs_left_top.x;
    double yResMeters = gs_left_top.y - gs_right_bottom.y;
    xResMeters /= x_size;
    yResMeters /= y_size;

    // 极坐标系参数
    double radiusMeters = radius * 1000;                                 // 最大半径（米）
    double lengthStep = 30;                                              // 长度步长（米）
    int polarRingsRadius = static_cast<int>(radiusMeters / lengthStep);  // 半径方向的分辨
    //// 动态计算角度分辨率
    // int polarRingsAngle = polarRingsRadius * 2 * 4 / 64;
    int polarRingsAngle = 360;  // 角度方向的分辨率

    // 初始化极坐标系
    m_polar_dem.resize(polarRingsAngle, std::vector<double>(polarRingsRadius, -1));
    // 填充极坐标系数据
    for (int thetaIndex = 0; thetaIndex < polarRingsAngle; ++thetaIndex)
    {
        double theta = 2 * M_PI * thetaIndex / polarRingsAngle;  // 角度（弧度）
        // double theta_deg = theta * 180 / M_PI;                   // 角度值
        for (int rIndex = 0; rIndex < polarRingsRadius; ++rIndex)
        {
            double r = rIndex * lengthStep;  // 半径（米）
            // 将极坐标转换为笛卡尔坐标 北方向为起始,顺时针为正方向
            double x = m_center.x + r * std::sin(theta);  // 笛卡尔X坐标（相对于中心）
            double y = m_center.y + r * std::cos(theta);  // 笛卡尔Y坐标（相对于中心）

            // 去顶这个位置在栅格中的位置
            int col = static_cast<int>((x - gs_left_top.x) / xResMeters);
            int row = static_cast<int>((gs_left_top.y - y) / yResMeters);

            //// 高斯转经纬度
            // double lon = 0.0, lat = 0.0;
            // silly_point gs_point(x, y);
            // silly::geo::proj::convert::gauss_to_lonlat(central_lgtd, x, y, lon, lat);
            //// 将经纬度转换为栅格索引
            // int col = static_cast<int>((lon - min_x) / abx_x_res);
            // int row = static_cast<int>((lat - max_y) / y_res);

            //  检查索引是否在有效范围内
            if (col >= 0 && col < x_size && row >= 0 && row < y_size)
            {
                m_polar_dem[thetaIndex][rIndex] = m_dem_data[row * x_size + col];
            }
        }
    }

    return true;
}

silly_poly silly_radar_occluder::occluder_poly(const double& height, const double& deg, const double& radius)
{
    silly_poly poly;
    int r_size = m_polar_dem.front().size();
    double n = radius * 1000.0 / r_size;
    double dp = 360.0 / m_polar_dem.size();  // 极坐标角度变化值
    double rad = DEG2RAD(deg);
    for (int i = 0; i < m_polar_dem.size(); ++i)
    {
        bool is_block = false;
        for (int j = 0; j < m_polar_dem[i].size(); ++j)
        {
            double h = n * j * std::atan(rad) + height;
            if (h < m_polar_dem[i][j])
            {
                poly.outer_ring.points.push_back(polar2cartesian(m_center, j * n, i * dp));
                is_block = true;
            }
        }
        if (!is_block)
        {
            poly.outer_ring.points.push_back(polar2cartesian(m_center, r_size * n, i * dp));
        }
    }

    return poly;
}
