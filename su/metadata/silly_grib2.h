/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/11/17 14:03
 * @version: 1.0.1
 * @description: grib2格式的读取, 参照 https://github.com/weathersource/grib_api/tree/master
 */
#ifndef SILLY_UTILS_SILLY_GRIB2_H
#define SILLY_UTILS_SILLY_GRIB2_H
#include <geo/silly_geo.h>
/// <summary>
/// grib2的帧数据
/// </summary>
class silly_grib2_frame
{
  public:
    bool is_valid{false};

    std::vector<double> data;
};
namespace silly
{
namespace grib2
{
class message
{
  public:

};

class meta
{
   
  public:
    std::vector<message> m_messages;
    silly_rect m_rect;
    size_t m_rows = 0;
    size_t m_cols = 0;
    double m_xdelta = 0.0;
    double m_ydelta = 0.0;

  private:
    void* m_grb2_handel = nullptr;
};

class utils
{
  public:
  private:
};
}  // namespace grib2
}  // namespace silly
class silly_grib2_utils
{
  public:
    /// <summary>
    /// 读取grib2文件中的某一帧数据
    /// </summary>
    /// <param name="path"></param>
    /// <param name="grb"></param>
    /// <param name="fidx"></param>
    /// <returns></returns>
    static bool read(const std::string& path, silly_grib2_frame& grb, const size_t& fidx = 0);

    /// <summary>
    /// 读取grib2文件中的所有数据
    /// </summary>
    /// <param name="path"></param>
    /// <param name="grb"></param>
    /// <returns></returns>
    static bool read(const std::string& path, std::map<size_t, silly_grib2_frame>& msgf_grb);

  private:
    static bool open_grib2_handle(const std::string& path, void** file_h, void** grb2_c, void** grb2_h);

    static bool load_grib2_frame(const void* grb2_h, silly_grib2_frame& grb, const bool& skip = true);

    static bool close_grib2_handle(void* file_h, void* grb2_c, void* grb2_h);
};

#endif  // SILLY_UTILS_SILLY_GRIB2_H
