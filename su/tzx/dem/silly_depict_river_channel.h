/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-02-24
 * @file: silly_depict_river_channel.h
 * @description: 根据dem,河道横断面和纵断面,刻画完整河道的算法
 *                 统一使用高斯坐标处理横断面,纵断面
 * @version: v1.0.1 2025-02-24 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_DEPICT_RIVER_CHANNEL_H
#define SILLY_UTILS_SILLY_DEPICT_RIVER_CHANNEL_H
#include <geo/silly_geo.h>
namespace silly
{
namespace tzx
{
namespace river_channel
{
class depict
{
  public:
    struct options
    {
        silly_rect rect;       // 横断面和纵断面的矩形范围,高斯坐标
        double cell_size = 5;  // 单位米
        double l0; // 中央经线
    };
    struct point_z : public silly_point
    {
        point_z() = default;
        point_z(double xx, double yy)
        {
            x = xx;
            y = yy;

        }
        point_z(double xx, double yy, double zz)
        {
            x = xx;
        }
        double z = 0;
    };
    struct cross_section
    {
        std::vector<double> dists;  // 起点距
        std::vector<double> elevs;  // 高程
        point_z p0;                 // 起点的高斯坐标
    };

  public:
    /// <summary>
    /// 刻画河道
    /// </summary>
    /// <param name="cs0">开始横断面,高斯坐标</param>
    /// <param name="cs1">结束横断面,高斯坐标</param>
    /// <param name="vsection">两个横断面之间的纵断面,高斯坐标</param>
    /// <returns></returns>
    bool process(const cross_section& cs0, const cross_section& cs1, std::vector<point_z>& vsection, options& opt);

    /// <summary>
    /// 根据多个连续的横断面, 画出完整河道
    /// </summary>
    /// <param name="csections">高斯坐标</param>
    /// <param name="vsections">高斯坐标</param>
    /// <returns></returns>
    std::vector<std::vector<double>> process(const std::vector<cross_section> csections, std::vector<std::vector<point_z>>& vsections, options& opt);

    bool write(const std::string& filename, const std::vector<std::vector<double>>& channelData, const options& opt);
};
}  // namespace river_channel
}  // namespace tzx
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_DEPICT_RIVER_CHANNEL_H
