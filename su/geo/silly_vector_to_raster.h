/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/6 16:15
 * @version: 1.0.1
 * @description: https://www.cnblogs.com/cnblog-wuran/p/9700334.html
 *              矢量光栅化,核心是计算 y行 计算穿边次数, 对于闭合面,穿边次数一定是偶数次;
 *              记录下穿边的x1,x2,x3..x2n, 对其进行排序,每2个一组;
 *              x0, x1 之间是面内, x2, x3之间也是面内,一次类推
 *              特殊情况:
 *                  1. 面范围超过网格范围, 需要主动闭合超过范围的点,并且范围之外的不在考虑(已处理)
 *                  2. 当有多个面, 并且面有部分重合时,需要先合并重复部分,然后再进行排序,分组操作(已处理)
 */
#ifndef SILLY_VECTOR_TO_RASTER_H
#define SILLY_VECTOR_TO_RASTER_H

#include <geo/geometry/silly_geo_coll.h>
#include <math/silly_matrix.h>
#include <files/silly_path.h>

/// X扫描线算法
class XScanLine
{
public:
    struct Point
    {
        int x = 0;
        int y = 0;
        Point() = default;
        Point(const int& x, const int& y) : x(x), y(y) {};
    };

    struct RowPair
    {
        int beg = 0;
        int end = 0;
    };

    // 每一行 都有多个起终点序号对
    using ScanPairs =  std::map<int, std::vector<RowPair>>;

  public:
    void set(const suRect& rect, const double& cell_size);

    /// <summary>
    /// 光栅化单点
    /// </summary>
    /// <param name="point"></param>
    /// <returns></returns>
    void rasterize(const suPoint& point);

    /// <summary>
    /// 光栅化多点
    /// </summary>
    /// <param name="points">多点</param>
    /// <returns></returns>
    void rasterize(const suMultiPoint& points);

    /// <summary>
    /// 光栅化单线的算法, TODO: 有问题
    /// </summary>
    /// <param name="line"></param>
    /// <returns></returns>
    void rasterize(const suLine& line);

    /// <summary>
    /// 光栅化多条线
    /// </summary>
    /// <param name="lines">多线</param>
    /// <returns></returns>
    void rasterize(const suMultiLine& lines);

    /// <summary>
    /// 光栅化一个单点矢量
    /// </summary>
    /// <param name="poly"></param>
    /// <returns></returns>
    void rasterize(const suPoly& poly);

    /// <summary>
    /// 光栅化一个多面矢量
    /// </summary>
    /// <param name="m_polys"></param>
    /// <returns></returns>
    void rasterize(const suMultiPoly& m_polys);

    /// <summary>
    /// 光栅化任意一个矢量
    /// </summary>
    /// <param name="gc"></param>
    /// <returns></returns>
    void rasterize(const suGeoColl& gc);

    /// <summary>
    /// 将光栅化结果绘制到灰度图上
    /// </summary>
    /// <param name="file"></param>
    void image(const suPath& file);

    int width() const;
    int height() const;

    /// <summary>
    /// 填充pairs
    /// </summary>
    void fill();

    ScanPairs row_pairs() const;

    /// <summary>
    /// 将光栅化结果转为网格面输出
    /// </summary>
    /// <param name="path"></param>
    std::vector<suPoly> grids() const;

    /// <summary>
    /// 清空row_pairs
    /// </summary>
    void clear();

    int64_t num() const;

    /// 生成掩膜
    suMatrix<uint8_t> mask();

  private:
    /// <summary>
    /// 光栅化的基本算法, int运算效率高
    /// </summary>
    /// <param name="vertices_arr"></param>
    /// <returns></returns>
    void rasterize(const std::vector<std::vector<Point>>& vertices_arr);

    void check_line_point(const suPoint& , std::vector<Point>& vct, int& last_x, int& last_y) const;

    /// <summary>
    /// 添加新的行记录,会在
    /// </summary>
    /// <returns></returns>
    void add(const int& row, const std::vector<int>& edges);

  private:
    // 数据记录以经纬度左上角为原点, 向东为col的正方向,向下为row的正方向
    int m_width = 0;
    int m_height = 0;
    suRect m_rect;
    // 经纬度小数点后6位能精确到1米,更加精确意义不大
    double m_cell_size{0.000001};
    // 记录每一行在矢量内的多对起始列号
    ScanPairs m_row_pairs;
    std::vector<std::vector<uint8_t>> m_row_colors;
    int64_t m_num = 0;
};
#endif  // SILLY_VECTOR_TO_RASTER_H
