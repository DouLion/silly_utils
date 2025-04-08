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
#include <pool/silly_thread_pool.h>

using namespace silly::geo::proj;
void silly_radar_occluder::set(const site& info)
{
    m_polar_dem.clear();
    m_center = info.position;
    m_elevation = info.elevation;
    m_radius = info.radius;
}

void silly_radar_occluder::set(const dem& info)
{
    m_rSize = std::ceil(m_radius / m_rStep);
    m_polar_dem.resize(m_dSize, std::vector<double>(m_rSize, 9999));
    m_l0 = std::floor(m_center.x / 3.0) * 3.0;
    if (info.l0 != -9999.0)
    {
        m_l0 = info.l0;
    }
    silly_point G0;
    convert::lonlat_to_gauss(m_l0, m_center.x, m_center.y, G0.x, G0.y);
    for (size_t d = 0; d < m_dSize; ++d)
    {
        for (size_t r = 0; r < m_rSize; ++r)
        {
            double rad = DEG2RAD(d * m_dStep);
            double radius = r * m_rStep;
            size_t row = 0, col = 0;
            if (info.l0 == -9999.0)
            {
                // 经纬度
                silly_point gp;
                convert::polar_to_cartesian(radius, rad, gp.x, gp.y, G0.x, G0.y);
                silly_point llp;
                convert::gauss_to_lonlat(m_l0, gp.x, gp.y, llp.x, llp.y);

                row = std::round((info.rect.max.y - llp.y) / (info.rect.max.y - info.rect.min.y) * info.rows);
                col = std::round((llp.x - info.rect.min.x) / (info.rect.max.x - info.rect.min.x) * info.cols);
            }
            else
            {
                // 高斯
                silly_point gp;
                convert::polar_to_cartesian(radius, rad, gp.x, gp.y, G0.x, G0.y);
                row = std::round((info.rect.max.y - gp.y) / (info.rect.max.y - info.rect.min.y) * info.rows);
                col = std::round((gp.x - info.rect.min.x) / (info.rect.max.x - info.rect.min.x) * info.cols);
            }

            m_polar_dem[d][r] = info.data[row * info.cols + col];
        }
    }
}

// bool silly_radar_occluder::get_dem(const std::string& file)
//{
//     // silly::geo::utils::init_gdal_env();
//     // 读取 tif 文件
//     GDALDataset* poDataset;
//     poDataset = (GDALDataset*)GDALOpen(file.c_str(), GA_ReadOnly);
//     if (poDataset == nullptr)
//     {
//         std::cerr << "Failed to open DEM file" << std::endl;
//         return false;
//     }
//     x_size = poDataset->GetRasterXSize();
//     y_size = poDataset->GetRasterYSize();
//     GDALRasterBand* poBand = poDataset->GetRasterBand(1);
//     m_dem_data.resize(x_size * y_size);
//     poBand->RasterIO(GF_Read, 0, 0, x_size, y_size, m_dem_data.data(), x_size, y_size, GDT_Float64, 0, 0);
//
//     // 获取坐标范围（地理坐标）
//     double geoTransform[6];
//     poDataset->GetGeoTransform(geoTransform);
//     double min_x = geoTransform[0];  //
//     double max_y = geoTransform[3];  // 左上角Y坐标
//     x_res = geoTransform[1];         // 经度方向的分辨率（度）
//     y_res = geoTransform[5];         // 纬度方向的分辨率（度）
//     double max_x = min_x + x_size * x_res;
//     double min_y = max_y + y_size * y_res;
//
//     left_top = silly_point(min_x, max_y);      // 左上角
//     right_bottom = silly_point(max_x, min_y);  // 右下角
//     left_bottom = silly_point(min_x, min_y);   // 左下
//     right_top = silly_point(max_x, max_y);     // 右上
//
//     GDALClose(poDataset);
//
//     // silly::geo::utils::destroy_gdal_env();
//     return true;
// }

void silly_radar_occluder::check_beam(size_t di, double pitch, double elev, double radius, double* stopLen)
{
    size_t i = 0;
    std::vector<double>& rhs = m_polar_dem[di];
    double rpitch = DEG2RAD(pitch);
    for (; i < m_rSize; ++i)
    {
        // double h = n * j * std::atan(rad) + uElev;
        double rr = (i + 0.5) * m_rStep;
        if (rr > radius)
        {
            break;
        }
        double viewh = std::atan(rpitch) * rr + elev;
        double demh = rhs[i];
        if (demh >= viewh)
        {
            break;
        }
    }
    *stopLen = i * m_rStep;
}

silly_poly silly_radar_occluder::occluder_poly(const double& pitch, const double& elev, const double& radius)
{
    double uElev = elev == 0 ? m_elevation : elev;
    double uR = radius == 0 ? m_radius : radius;
    silly_poly poly;
    std::vector<double> maxRadius(m_dSize, 0);
    silly_point G0;
    convert::lonlat_to_gauss(m_l0, m_center.x, m_center.y, G0.x, G0.y);
    {
        silly_thread_pool pool(std::thread::hardware_concurrency() * 2);
        for (int i = 0; i < m_polar_dem.size(); ++i)
        {
            // 这一段可以使用多线程加速
            // check_beam(i, pitch, elev, radius, &maxRadius[i]);
            pool.enqueue(&silly_radar_occluder::check_beam, this, i, pitch, elev, radius, &maxRadius[i]);
        }
    }

    for (int i = 0; i < m_dSize; ++i)
    {
        silly_point gp, llp;
        convert::polar_to_cartesian(maxRadius[i], DEG2RAD(m_dStep * i), gp.x, gp.y, G0.x, G0.y);
        convert::gauss_to_lonlat(m_l0, gp.x, gp.y, llp.x, llp.y);
        poly.outer_ring.points.push_back(llp);
    }
    return poly;
}
