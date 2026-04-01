/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2026-04-01
 * @file: silly_hgt.h
 * @description: raw 高程栅格数据
 * @version: v1.0.1 2026-04-01 dou li yang
 */
#ifndef SILLY_HGT_H
#define SILLY_HGT_H
#include <su_macro.h>
#include <log/silly_log.h>

class suHGT
{
    // 字节序转换
    short swap_bytes(short val);

  public:
    struct HgtData
    {
        int southwest_lat = 0;            // 瓦片西南角的纬度
        int southwest_lon = 0;            // 瓦片西南角的经度
        int resolution_arc_seconds = 0;   // 分辨率：1 (SRTM1) 或 3 (SRTM3) 弧秒
        int grid_size = 0;                // 瓦片一边的采样点数量 (SRTM1 为 3601, SRTM3 为 1201)
        std::vector<int16_t> elevations;  // 存储海拔高度数据 (16位有符号整数，单位：米)

        // 检查数据是否已加载
        bool is_loaded() const
        {
            return !elevations.empty();
        }
        // 清除所有数据，重置为初始状态
        void clear()
        {
            southwest_lat = 0;
            southwest_lon = 0;
            resolution_arc_seconds = 0;
            grid_size = 0;
            elevations.clear();
        }
    };

    /// <summary>
    /// 加载HGT文件数据
    /// </summary>
    /// <param name="hgt_filepath">文件路径</param>
    /// <param name="hgt_data">hgt结构体</param>
    /// <returns></returns>
    bool read(const std::string& hgt_filepath, HgtData& hgt_data);

    /// <summary>
    /// 根据给定的 HgtData、经纬度返回海拔高度(单位：米)
    /// </summary>
    /// <param name="hgt_data">hgt结构体</param>
    /// <param name="lat">经度</param>
    /// <param name="lon">纬度</param>
    /// <returns></returns>
    int16_t get_elevation(const HgtData& hgt_data, double lat, double lon) const;
    int16_t get_elevation(const HgtData& hgt_data, int row, int col) const;

    // 根据给定的 HgtData、经纬度返回海拔高度（单位：米）。
    // 采用双线性插值，提供更精确和平滑的结果。
    // 返回 -32768 (标准缺失数据值) 如果坐标超出瓦片范围或数据缺失。
    // 如果传入的 HgtData 未加载，则抛出 std::runtime_error 异常。
    double get_interpolated_elevation(const HgtData& hgt_data, double latitude, double longitude) const;

    // 检查给定的经纬度是否在 HGT 瓦片覆盖的地理范围内。
    // 如果传入的 HgtData 未加载，则抛出 std::runtime_error 异常。
    bool is_coordinate_within_tile(const HgtData& hgt_data, double latitude, double longitude) const;

    // 获取 HGT 瓦片所覆盖的地理边界。
    // 如果传入的 HgtData 未加载，则抛出 std::runtime_error 异常。
    void get_tile_geographic_bounds(const HgtData& hgt_data, double& min_lat, double& max_lat, double& min_lon, double& max_lon) const;

    // 获取 HGT 瓦片中存储的最小和最大海拔值。
    // 如果传入的 HgtData 未加载，则抛出 std::runtime_error 异常。
    void get_min_max_elevation(const HgtData& hgt_data, short& min_elev, short& max_elev) const;

  private:
    /// <summary>
    /// 解析文件名并确定分辨率
    /// </summary>
    /// <param name="hgt_filepath">文件路径</param>
    /// <param name="filename">文件名</param>
    /// <param name="hgt_data">hgt结构体</param>
    /// <returns></returns>
    bool parse_filename(const std::string& hgt_filepath, const std::string& filename, HgtData& hgt_data);

    // 辅助函数：将经纬度转换为 HGT 网格的浮点型行/列索引。
    // 如果坐标超出瓦片范围，返回 false。
    bool get_coordinates(const HgtData& hgt_data, double latitude, double longitude, double& float_row, double& float_col) const;
};

#endif  // SILLY_HGT_H