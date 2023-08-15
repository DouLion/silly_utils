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
#include "marco.h"
#include "math/silly_matrix.h"
using namespace  silly_math;

class silly_hill_shade
{
	/// <summary>
	/// ����DEM��ÿ���̵߳����Ӱֵ
	/// </summary>
	/// <typeparam name="T1"></typeparam>
	/// <typeparam name="T2"></typeparam>
	/// <param name="dem">dem����</param>
	/// <param name="shade">�������, dem��Ӧ����Ӱֵ(�Ҷ�ֵ)</param>
	/// <param name="delta_lng">ÿ���̵߳����ĺ��򾭶ȳ���</param>
	/// <param name="delta"></param>
	/// <param name="azimuth_deg">��Դ�ķ�λ�ǣ��Զ�Ϊ��λ</param>
	/// <param name="heightDeg">��Դ�ĸ߶Ƚǣ��Զ�Ϊ��λ </param>
	/// <returns></returns>
	template <typename T1, typename T2>
	static bool hill_shading(const matrix_2d<T1>& dem, matrix_2d<T2>& shade, const double& delta_lng, const double& delta_lat, const double& azimuth_deg= 315.0, const double& height_deg = 45.0 )
	{

        size_t height = dem.rows();
        size_t width = dem.cols();
        if (!height || !width)
        {
            return false;
        }
        shade.create(height, width, true);

        const double zenithDeg = 90 - height_deg;
        const double zenithRad = zenithDeg * M_PI / 180.0;    // ����ת��
        double azimuthRad = 360.0 - azimuth_deg + 90.0;
        if (azimuthRad >= 360.0)
        {
            azimuthRad = azimuthRad - 360.0;
        }
        azimuthRad = azimuthRad * M_PI / 180.0;

        double zenithRad_cos = cos(zenithRad);
        double zenithRad_sin = sin(zenithRad);

        for (int i = 1; i < height - 1; i++) {
            for (int j = 1; j < width - 1; j++) {
                double a = dem.get_data()[i - 1][j - 1];            // e = ��������, other letters = neighbours
                double b = dem.get_data()[i][j - 1];
                double c = dem.get_data()[i + 1][j - 1];
                double d = dem.get_data()[i - 1][j];
                double e = dem.get_data()[i][j];               // It was too ugly not to put a letter e, even if it is unused
                double f = dem.get_data()[i + 1][j];
                double g = dem.get_data()[i - 1][j + 1];
                double h = dem.get_data()[i][j + 1];
                double k = dem.get_data()[i + 1][j + 1];        // Not i because of the for (int i = ...)

                double dz_dx = ((c + k + (2 * f)) - (a + g + (2 * d))) / (8 * delta_lat);         // Derivatives
                double dz_dy = ((g + k + (2 * h)) - (a + c + (2 * b))) / (8 * delta_lng);
                double slopeRad = atan(sqrt((dz_dx * dz_dx) + (dz_dy * dz_dy)));
                double aspectRad;

                if (dz_dx != 0)             // Indexing conditions (from the "how does Hillshading work" website)
                {
                    aspectRad = atan2(dz_dy, -dz_dx);
                    if (aspectRad < 0)                                        // ģ 2pi �Ա��⸺ֵ
                    {
                        aspectRad = 2 * M_PI + aspectRad;
                    }
                       
                }
                else
                {
                    if (dz_dy > 0)
                    {
                        aspectRad = M_PI / 2;
                    }    
                    else
                    {
                        if (dz_dy < 0)
                        {
                            aspectRad = 2 * M_PI - (M_PI / 2);
                        }
                        else
                        {
                            aspectRad = 0;
                        }
                    }
                }
                double tmp_shade = 255.0 * ((zenithRad_cos * cos(slopeRad)) + (zenithRad_sin * sin(slopeRad) * cos(azimuthRad - aspectRad)));
                tmp_shade = SU_MIN(255., SU_MAX(0., tmp_shade));
              
                shade.get_data()[i][j] = (T2)tmp_shade;

            }
        }

        return true;
	}
};

#endif //SILLY_UTILS_SILLY_HILL_SHADE_HPP
