/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/25 17:21
 * @version: 1.0.1
 * @description: 雷达极坐标系数据存储
 */
#ifndef SILLY_UTILS_SILLY_RADAR_POLAR_H
#define SILLY_UTILS_SILLY_RADAR_POLAR_H


#include <iostream>
#include <math/silly_matrix.h>

#define TZX_RADAR_POLAR_FILE_SUFFIX  ".rpolar"

class silly_radar_polar
{
public:

    silly_radar_polar() = default;

    /// <summary>
    ///
    /// </summary>
    /// <param name="buff">输出: 数据区域</param>
    /// <param name="len">输出: 数据区域长度</param>
    /// <returns></returns>
    bool serialize(char** buff, size_t& len);

    /// <summary>
    ///
    /// </summary>
    /// <param name="buff">输入: 数据区域</param>
    /// <param name="len">输入: 数据区域长度</param>
    /// <returns></returns>
    bool unserialize(const char* buff, const size_t& len);

    bool read(const std::string& path);
    bool save(const std::string& path);

    silly_radar_polar operator=(const silly_radar_polar& other)
    {
        this->total = other.total;
        this->r_size = other.r_size;
        this->az_size = other.az_size;
        this->meters = other.meters;
        this->az_start = other.az_start;
        this->center_x = other.center_x;
        this->center_y = other.center_y;
        // this->fill = other.fill;
        memcpy(this->name, other.name, sizeof(this->name));
        memcpy(this->units, other.units, sizeof(this->units));

        this->data = other.data;
        return *this;
    }
    silly_radar_polar copy(const silly_radar_polar& other)
    {
        this->total = other.total;
        this->r_size = other.r_size;
        this->az_size = other.az_size;
        this->meters = other.meters;
        this->az_start = other.az_start;
        this->center_x = other.center_x;
        this->center_y = other.center_y;
        // this->fill = other.fill;
        memcpy(this->name, other.name, sizeof(this->name));
        memcpy(this->units, other.units, sizeof(this->units));

        this->data = new float[this->r_size * this->az_size];
        memcpy(this->data, other.data, this->r_size * this->az_size * sizeof(float));
        return *this;
    }


public:

    size_t total{ 0 };
    // 长度
    size_t r_size{ 0 };

    // 方位角数量
    size_t az_size{ 0 };
    // range#meters_between_gates=75.00000298023224  长度上每个格子表示的长度,单位为米
    float meters;
    /*
 * azimuth 一般指方位角。 方位角，又称地平经度(Azimuth angle，缩写为Az)，是在平面上量度物体之间的角度差的方法之一。是从某点的指北方向线起，依顺时针方向到目标方向线之间的水平夹角。
 * */
    // 起始方位角
    float az_start{ 0. };
    // 中心点经纬度
    float center_x{ 0. };
    float center_y{ 0. };

    // float fill{ SHRT_MAX + 1 };

    char name[32]{ 0 };
    char units[32]{ 0 };

    float* data{nullptr};
};
#endif //SILLY_UTILS_SILLY_RADAR_POLAR_H
