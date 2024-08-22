/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 24-8-9 下午6:58
 * @version: 1.0.1
 * @description: silly_netcdf 类声明
 */
#ifndef SILLY_UTILS_SILLY_NETCDF_H
#define SILLY_UTILS_SILLY_NETCDF_H
#include <netcdf>
#include <log/silly_log.h>
using namespace netCDF;
using namespace netCDF::exceptions;
// lon维度必须在倒数第一个
// lat维度必须在倒数第二个

/** 补充有效维度
 *  名称     类型      units
 *  time
 *                      day since 1970-01-01 00:00:00
 *                      year since 1970-01-01 00:00:00
 *                      minutes since 1970-01-01 00:00:00
 *                      days since 2022-01-01 18:00:00
 *  height              meters  kilometers ...
 *
 *  level
 *                      hPa 对应维度的单位
 *
 *
 */

class silly_netcdf_geo
{
  public:
    float xfirst{0.0f};
    float xlast{0.0f};
    size_t xlen{0};
    std::string xname{"lon"};
    // 默认经度 东为正方向
    std::string xunits{"degrees_east"};
    float xmax{180.0f};
    float xmin{-180.0f};
    float yfirst{0.0f};
    float ylast{0.0f};
    size_t ylen{0};
    std::string yname{"lat"};
    // 默认纬度 北为正方向
    std::string yunits{"degrees_north"};
    float ymax{90.0f};
    float ymin{-90.0f};
};

// 单波段的数据
class silly_netcdf_band_data
{
  public:
    std::string group{"acc"};
    float scale{1.};
    float fill{-9999.};
    float offset{0.};
    std::string units{"mm"};
    std::vector<float> grid; // 西北角为原点,逐行存储

};

class silly_netcdf_data
{
  public:
    // 坐标信息维度
    silly_netcdf_geo dgeo;
    // 其他维度, 必须具有实际意义
    std::vector<std::tuple<std::string, std::vector<float>, std::string>> dextra;
    // 波段数据
    std::map<std::string, std::vector<silly_netcdf_band_data>> grp_bands;

};

class silly_netcdf
{
  public:
    /// <summary>
    /// 打开netcdf文件
    /// </summary>
    /// <param name="path">文件路径</param>
    /// <returns></returns>
    bool open(const std::string& path);


    std::vector<std::string> members();

    /// <summary>
    /// 获取地理信息
    /// </summary>
    /// <param name="lon">经度</param>
    /// <param name="lat">纬度</param>
    /// <returns></returns>
    bool geometry(const std::string& lon, const std::string& lat);

    /// <summary>
    /// 读取netcdf完整内容
    /// </summary>
    /// <param name="group">组名称</param>
    /// <param name="lon">经度</param>
    /// <param name="lat">纬度</param>
    /// <returns></returns>
    bool read(const std::string& group, const std::string& lon, const std::string& lat);

    bool write(const std::string& path, const silly_netcdf_data& snd);

    /// <summary>
    /// 关闭netcdf文件
    /// </summary>
    /// <returns></returns>
    void close();

    std::string err();

    double left() const;

    double right() const;

    double bottom() const;

    double top() const;

    double scale() const;

    double fill() const;

    double offset() const;

    double xdelta() const;

    double ydelta() const;

    size_t width() const;

    size_t height() const;

    /// <summary>
    /// 获取数据, 均按照数据存储顺序返回
    /// </summary>
    std::vector<float> data(const std::string& band_name);
    std::vector<float> data(const size_t& index);
    std::vector<std::vector<float>> data();

    /// <summary>
    /// 获取波段名称
    /// </summary>
    /// <returns></returns>
    std::vector<std::string> band_names();

  private:
    /* 网格点顺序
     * 0 自西向东, 自南向北
     * 1 自东向西, 自北向南
     * 2 自南向北, 自西向东
     * */
    int m_sort{0};
    NcFile m_nc_file;
    NcGroup m_nc_all_grps;
    std::string m_err;
    size_t m_width{0};
    size_t m_height{0};
    double m_scale{1.};
    double m_fill{-9999.};
    double m_offset{0.};
    double m_left{0.};
    double m_right{0.};
    double m_bottom{0.};
    double m_top{0.};
    double m_xdelta{0.};
    double m_ydelta{0.};
    std::vector<std::pair<std::string, std::vector<float>>> m_nm_data;
};

#endif  // SILLY_UTILS_SILLY_NETCDF_H
