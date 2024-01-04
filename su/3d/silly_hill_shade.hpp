 /*
  * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
  * reserved.
  * @website: http://www.tianzhixiang.com.cn/
  * @author: dou li yang
  * @date: 2023/8/15 13:08
  * @version: 1.0.1
  * @software: silly_utils
  * @description:
  */
#pragma once

#ifndef SILLY_UTILS_SILLY_HILL_SHADE_HPP
#define SILLY_UTILS_SILLY_HILL_SHADE_HPP
#include <su_marco.h>
#include <math/silly_matrix.h>
using namespace silly_math;

class silly_hill_shade
{
public:
	/// <summary>
	/// 计算DEM中每个高程点的阴影值
	/// </summary>
	/// <typeparam name="T1"></typeparam>
	/// <typeparam name="T2"></typeparam>
	/// <param name="dem">dem数据</param>
	/// <param name="shade">输出数据, dem对应的阴影值(灰度值)</param>
	/// <param name="delta_lng">每个高程点代表的横向经度长度</param>
	/// <param name="delta"></param>
	/// <param name="azimuth_deg">光源的方位角，以度为单位</param>
	/// <param name="heightDeg">光源的高度角，以度为单位</param>
	/// <returns></returns>
	template <typename T1, typename T2>
	static bool hill_shading(const matrix_2d<T1>& dem, matrix_2d<T2>& shade, const double& delta_lng, const double& delta_lat, const double& azimuth_deg = 315.0, const double& height_deg = 45.0, double z_factor = 1.0)
	{
		matrix_2d<T1> tmp_dem = dem;
		size_t height = tmp_dem.row();
		size_t width = tmp_dem.col();
		if (!height || !width)
		{
			return false;
		}
		shade.create(height, width, true);

		const double zenith_deg = 90 - height_deg;
		const double zenithRad = zenith_deg * M_PI / 180.0; // 弧度转换
		double azimuth_rad = 360.0 - azimuth_deg + 90.0;
		if (azimuth_rad >= 360.0)
		{
			azimuth_rad = azimuth_rad - 360.0;
		}
		azimuth_rad = azimuth_rad * M_PI / 180.0;

		double zenithRad_cos = cos(zenithRad);
		double zenithRad_sin = sin(zenithRad);
		for (int i = 1; i < height - 1; i++)
		{
			for (int j = 1; j < width - 1; j++)
			{
				double a = tmp_dem.at(i - 1, j - 1); // e = 中心像素, other letters = neighbours
				double b = tmp_dem.at(i, j - 1);
				double c = tmp_dem.at(i + 1, j - 1);
				double d = tmp_dem.at(i - 1, j);
				double e = tmp_dem.at(i, j); // It was too ugly not to put a letter e, even if it is unused
				double f = tmp_dem.at(i + 1, j);
				double g = tmp_dem.at(i - 1, j + 1);
				double h = tmp_dem.at(i, j + 1);
				double k = tmp_dem.at(i + 1, j + 1); // Not i because of the for (int i = ...)

				double dz_dx = ((c + k + (2 * f)) - (a + g + (2 * d))) / (8 * delta_lat); // Derivatives
				double dz_dy = ((g + k + (2 * h)) - (a + c + (2 * b))) / (8 * delta_lng);
				double slope_rad = atan(z_factor * sqrt((dz_dx * dz_dx) + (dz_dy * dz_dy)));
				double aspect_rad = atan2(dz_dy, -dz_dx);
				if (aspect_rad < 0)
				{
					aspect_rad += 2 * M_PI;
				}

				double tmp_shade = 255.0 * ((cos(zenith_deg * M_PI / 180.0) * cos(slope_rad)) + (sin(zenith_deg * M_PI / 180.0) * sin(slope_rad) * cos(azimuth_rad - aspect_rad)));
				tmp_shade = std::min(255., std::max(0., tmp_shade));

				shade.at(i, j) = static_cast<T2>(tmp_shade);
			}
		}

		return true;
	}
};

#endif // SILLY_UTILS_SILLY_HILL_SHADE_HPP
