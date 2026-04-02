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
#include <files/silly_file.h>

class suHGT
{
  public:
    struct HgtData
    {
        std::string filepath;             // HGT 文件的完整路径
        int southwest_lat = 0;            // 瓦片西南角的纬度（整数）
        int southwest_lon = 0;            // 瓦片西南角的经度（整数）
        int resolution_arc_seconds = 0;   // 分辨率：1 (SRTM1) 或 3 (SRTM3) 弧秒
        int grid_size = 0;                // 瓦片一边的采样点数量 (SRTM1 为 3601, SRTM3 为 1201)
        std::vector<int16_t> elevations;  // 存储海拔高度数据 (16位有符号整数，单位：米)

        void unload_elevations()
        {
            elevations.clear();
            elevations.shrink_to_fit();
        }

        void clear()
        {
            filepath.clear();
            southwest_lat = 0;
            southwest_lon = 0;
            resolution_arc_seconds = 0;
            grid_size = 0;
            unload_elevations();
        }
    };

    /// <summary>
    /// 扫描指定根目录下的所有 .hgt 文件，并加载其元数据。
    /// </summary>
    /// <param name="root_path">根目录路径</param>
    /// <returns>成功加载至少一个瓦片时返回 true，否则返回 false</returns>
    bool open(const std::string& root_path);

    /// <summary>
    /// 卸载所有已加载的海拔数据，释放内存，但保留瓦片元数据。
    /// </summary>
    void unload_all_elevations();

    /// <summary>
    /// 卸载指定瓦片的海拔数据。
    /// </summary>
    /// <param name="lat">瓦片西南角纬度</param>
    /// <param name="lon">瓦片西南角经度</param>
    /// <returns>成功卸载返回 true，否则返回 false</returns>
    bool unload_tile_elevations(int lat, int lon);

    /// <summary>
    /// 根据经纬度返回海拔高度（最近邻插值）。
    /// </summary>
    /// <param name="lat">纬度</param>
    /// <param name="lon">经度</param>
    /// <returns>海拔高度（米），如果数据不可用返回 -32768</returns>
    int16_t get_elevation(double lat, double lon);

    /// <summary>
    /// 根据经纬度返回海拔高度（双线性插值）。
    /// </summary>
    /// <param name="lat">纬度</param>
    /// <param name="lon">经度</param>
    /// <returns>插值后的海拔高度（米），如果数据不可用返回 -32768.0</returns>
    double get_interpolated_elevation(double lat, double lon);

  private:
    std::map<std::pair<int, int>, HgtData> hgt_tiles_;

    /// <summary>
    /// 从文件名中解析坐标信息，并填充 HgtData 结构体。
    /// 文件名格式：[N|S]yy[E|W]xxx.hgt，如 N40E120.hgt
    /// </summary>
    /// <param name="hgt_filepath">HGT 文件的完整路径</param>
    /// <param name="hgt_data">输出参数，保存解析结果</param>
    /// <returns>解析成功返回 true，否则返回 false</returns>
    bool parse_metadata(const std::string& hgt_filepath, HgtData& hgt_data);

    /// <summary>
    /// 获取指定坐标对应的 HgtData，如果数据未加载则按需加载。
    /// </summary>
    /// <param name="lat">纬度</param>
    /// <param name="lon">经度</param>
    /// <returns>指向 HgtData 的指针，如果瓦片不存在或加载失败返回 nullptr</returns>
    HgtData* get_hgt_data(double lat, double lon);

    /// <summary>
    /// 从文件中加载指定 HgtData 的海拔数据到内存。
    /// 自动根据文件大小判断分辨率（1 弧秒或 3 弧秒）。
    /// </summary>
    /// <param name="hgt_data">待加载的 HgtData 结构体</param>
    /// <returns>成功加载返回 true，否则返回 false</returns>
    bool load_elevations_for_tile(HgtData& hgt_data);

    /// <summary>
    /// 安全地获取 HGT 数据中指定行列索引处的海拔值。
    /// </summary>
    /// <param name="hgt_data">HGT 数据</param>
    /// <param name="row">行索引</param>
    /// <param name="col">列索引</param>
    /// <returns>海拔值，如果索引超出范围返回 -32768</returns>
    int16_t get_elevation_at_index(const HgtData& hgt_data, int row, int col) const;
};

#endif  // SILLY_HGT_H