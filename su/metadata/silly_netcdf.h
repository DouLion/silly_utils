/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 24-8-9 下午6:58
 * @version: 1.0.1
 * @description: silly_netcdf 类声明
 */
#ifndef SILLY_NETCDF_H
#define SILLY_NETCDF_H

#include <log/silly_log.h>
#include <files/silly_file.h>
#include <metadata/silly_tzx_grid.h>

#if SU_THIRD_SUPPORT_NETCDF_CXX
#include <netcdf>
#endif
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

class suNetCDF
{
  public:
    class Geo
    {
      public:
        size_t xlen = 0;
        std::string xname =  "lon";
        float xmax= 180.0f;
        float xmin= -180.0f;
        float xstep= 0.0;
        size_t ylen = 0;
        std::string yname = "lat";
        float ymax = 90.0f;
        float ymin = -90.0f;
        float ystep = 0.0;
    };

    // 单波段的数据
    class Band
    {
      public:
        std::string group{"acc"};
        std::string name;
        float scale = 1.0;;
        float fill = -9999.;
        float offset = 0.0;
        std::string units = "mm";
        std::vector<float> grid;  // 西北角为原点,逐行存储
    };

    class Data
    {
      public:
        // 坐标信息维度
        Geo dgeo;
        // 其他维度, 必须具有实际意义
        std::vector<std::tuple<std::string, std::vector<float>, std::string>> dextra;
        // 波段数据
        std::map<std::string, std::vector<Band>> grp_bands;
    };

  public:
    /// <summary>
    /// 打开netcdf文件
    /// </summary>
    /// <param name="path">文件路径</param>
    /// <returns></returns>
    bool open(const suPath& file);

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
    /// <param name="lon">经度字段名称</param>
    /// <param name="lat">纬度字段名称</param>
    /// <returns></returns>
    bool read(const std::string& group, const std::string& lon = "lon", const std::string& lat = "lat");

    silly_tzx_grid convert(const size_t& b, const size_t& e) const;

    static bool write(const suPath& file, const Data& snd);

    bool write(const suPath& file) const;

    /// <summary>
    /// 关闭netcdf文件
    /// </summary>
    /// <returns></returns>
    void close();

    std::string err();

    Geo geo() const;

    float left() const;

    float right() const;

    float bottom() const;

    float top() const;

    float scale() const;

    float fill() const;

    float offset() const;

    float xdelta() const;

    float ydelta() const;

    size_t width() const;

    size_t height() const;

    /// <summary>
    /// 获取数据, 均按照数据存储顺序返回
    /// </summary>
    std::vector<float> data(const std::string& band_name) const;
    std::vector<float> data(const size_t& index) const;
    std::vector<std::vector<float>> data() const;

    std::vector<Band> bands(const size_t& b, const size_t& e) const;

    /// <summary>
    /// 获取波段名称
    /// </summary>
    /// <returns></returns>
    std::vector<std::string> band_names() const;

  private:
    /* 网格点顺序
     * 0 自东向西, 自南向北
     * 1 自东向西, 自北向南
     * */
    int m_sort{0};
#if SU_THIRD_SUPPORT_NETCDF_CXX
    netCDF::NcFile m_nc_file;
    netCDF::NcGroup m_nc_all_grps;
#endif
    std::string m_err;
    Geo m_geo;
    float m_fill = -9999.0;
    float m_offset = 0.0;
    float m_scale = 1.0;
    std::vector<std::string> m_dem_names;
    std::vector<std::string> m_attr_names;
    std::vector<Band> m_bands;
};

#endif  // SILLY_NETCDF_H
