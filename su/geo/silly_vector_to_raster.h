/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/6 16:15
 * @version: 1.0.1
 * @description: 矢量光栅化
 */
#ifndef SILLY_UTILS_SILLY_VECTOR_TO_RASTER_H
#define SILLY_UTILS_SILLY_VECTOR_TO_RASTER_H

#include <geo/silly_geo_coll.h>

struct raster_point
{
    raster_point() = default;
    raster_point(int _x, int _y)
    {
        x = _x;
        y = _y;
    }
    int x{0}, y{0};
};

struct cover_pair
{
    int beg;
    int end;

    // 重载<操作符,用于去重排序
    bool operator<(const cover_pair& other) const
    {
        return (beg < other.beg) || (beg == other.beg && end < other.end);
    }
};
/// X扫描线算法
/*
        TODO:	1. 对于经纬度或者墨卡托坐标系这一类来说, 如果x和y的cell_size不一致
                        2. 自定义左上右下
*/
class xscan_line_raster
{
  public:
    bool init();

    void reset();

    /// <summary>
    /// 光栅化单点的算法
    /// </summary>
    /// <param name="line"></param>
    /// <returns></returns>
    bool rasterization_point_algo(const silly_point& point);

    /// <summary>
    /// 光栅化单点
    /// </summary>
    /// <param name="points">单点</param>
    /// <returns></returns>
    bool rasterization(const silly_point& point);

    /// <summary>
    /// 光栅化多点
    /// </summary>
    /// <param name="points">多点</param>
    /// <returns></returns>
    bool rasterization(const silly_multi_point& points);

    /// <summary>
    /// 光栅化单线的算法
    /// </summary>
    /// <param name="line"></param>
    /// <returns></returns>
    bool rasterization_line_algo(const silly_line& line);

    /// <summary>
    /// 光栅化单条线
    /// </summary>
    /// <param name="line">单条线</param>
    /// <returns></returns>
    bool rasterization_line(const silly_line& line);

    /// <summary>
    /// 光栅化多条线
    /// </summary>
    /// <param name="lines">多线</param>
    /// <returns></returns>
    bool rasterization(const silly_multi_silly_line& lines);

    /// <summary>
    /// 光栅化一个单点矢量
    /// </summary>
    /// <param name="poly"></param>
    /// <returns></returns>
    bool rasterization(const silly_poly& poly);

    /// <summary>
    /// 光栅化一个多面矢量
    /// </summary>
    /// <param name="m_polys"></param>
    /// <returns></returns>
    bool rasterization(const silly_multi_poly& m_polys);

    /// <summary>
    /// TODO: 光栅化任意一个矢量(点? 线? 面)
    /// </summary>
    /// <param name="geo_coll"></param>
    /// <returns></returns>
    bool rasterization(const silly_geo_coll& geo_coll);

    /// <summary>
    /// 光栅化的基本算法, int运算效率高
    /// </summary>
    /// <param name="vertices_arr"></param>
    /// <returns></returns>
    bool rasterization(const std::vector<std::vector<raster_point>> vertices_arr);

    /// <summary>
    /// 去除重复的row_pairs
    /// </summary>
    /// <returns></returns>
    bool remove_repeat_row_pairs();

    /// <summary>
    /// 将光栅化结果绘制到灰度图上
    /// </summary>
    /// <param name="path"></param>
    void image(const std::string& path);

  public:
    /* 参照dem文件的表达方式
    ncols         9478
    nrows         7183
    xllcorner     218990.98442769
    yllcorner     3278474.0715599
    cellsize      9.8080330000000
    NODATA_value  -9999
    */
    // 数据记录以经纬度左下角为原点, 向东为col的正方向,向下为row的正方向
    int ncols{0};
    int nrows{0};
    float left{0.};
    float top{0.};
    float right{0.};
    float bottom{0.};
    // 经纬度小数点后6位能精确到1米,更加精确意义不大
    float cell_size{0.000001};
    // 记录每一行在矢量内的多对起始列号
    std::map<int, std::vector<cover_pair>> row_pairs;
};

#endif  // SILLY_UTILS_SILLY_VECTOR_TO_RASTER_H
