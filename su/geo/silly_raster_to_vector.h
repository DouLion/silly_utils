/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/6 16:14
 * @version: 1.0.1
 * @description: Marching squares 算法 扩展到3维 Marching Cubes
 * 参照 https://blog.csdn.net/whuawell/article/details/74998280
 */
#ifndef SILLY_RASTER_TO_VECTOR_H
#define SILLY_RASTER_TO_VECTOR_H

#include <geo/silly_geo.h>
#include <math/silly_matrix.h>

enum class eMathingMode
{
    Interp = 1,  // 计算比例
    Middle = 2,  // 取格点中间
};
class MatchingSquares
{
  public:
    struct ReadParam
    {
        suRect bound;
        double dx = 0;
        double dy = 0;
        // level相关,这两个其实是用于验证
        int level = -1;
        // 忽略过小的面
        float ignore = 0;
    };

  public:
    MatchingSquares() = default;
    MatchingSquares(const eMathingMode& mode) : m_mode(mode)
    {
    }
    ~MatchingSquares();

    void SetLevels(const std::map<int, float>& levels);

    void SetMat(const suMatrix<float>& mat);

    void UseThreadPool();

    void Calc();

    std::vector<suLine> GetLines(const ReadParam& param) const;
    std::vector<suPoly> GetPolys(const ReadParam& param) const;

  protected:
    void TraceContinuousPoints(const int& level);

    /**
     * 构建各个层及对应的01矩阵
     * 当m_levels的值递增时 1, 表示大于阈值的的格点
     *  例如降雨  指定阈值时100.0, 会标记所有大于100的值表示强降雨
     * 当m_levels的值递减时 1, 表示小于阈值的的格点
     *  例如降雨  指定阈值时20.0, 会标记所有小于20.0的值表示严重干旱
     */
    void MakeWhiteBlack(const int& level);

    /**
     * 追踪各层的黑白矩阵,构建每个格点内的相交线段
     * @param level
     */
    void TraceWhiteBlackSegment(const int& level);

    /**
     *  计算在边上的交点,如下图,四个边
     *      * * 0 * *
     *      *       *
     *      3       1
     *      *       *
     *      * * 2 * *
     *
     * @param r 行
     * @param c 列
     * @param t 阈值
     * @param interp false 使用中心点, true 使用插值
     * @return
     */
    [[nodiscard]] suPoint X_POINT_ON_EDGE_0(const int& r, const int& c, const float& t, const bool& interp) const;
    [[nodiscard]] suPoint X_POINT_ON_EDGE_1(const int& r, const int& c, const float& t, const bool& interp) const;
    [[nodiscard]] suPoint X_POINT_ON_EDGE_2(const int& r, const int& c, const float& t, const bool& interp) const;
    [[nodiscard]] suPoint X_POINT_ON_EDGE_3(const int& r, const int& c, const float& t, const bool& interp) const;

  private:
    // 这里是计算过程中产生的缓存数据
    std::map<int, float> m_l2th;             // 各层级对应的阈值
    std::map<int, suMatrix<int8_t>> m_l2wb;  // 各个层及对应的01矩阵,
    std::map<int, std::vector<std::list<suPoint>>> m_l2points;
    std::map<int, std::vector<suSegment>> m_l2seg;

  protected:
    // 此为外部提供的信息
    suMatrix<float> m_mat0;
    int m_width = 0;
    int m_height = 0;
    eMathingMode m_mode = eMathingMode::Interp;
    bool m_useThreadPool = false;
};

#endif  // SILLY_RASTER_TO_VECTOR_H
