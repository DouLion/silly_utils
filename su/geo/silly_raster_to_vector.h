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
    Ratio = 1,   // 计算比例
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
        double level = -1e12;
        int index = -1;
        // 忽略过小的面
        int ignore_count = 0;
    };

  public:
    MatchingSquares() = default;
    ~MatchingSquares();

    void SetLevels(const std::map<int, float>& levels);

    void SetMat(const suMatrix<float>& mat);

    bool TracePoly();

    bool TraceLine();

    void TraceRing();

    std::vector<suLine> TraceLine(const int& level);

    std::vector<suLine> GetLines(const ReadParam& param);
    std::vector<suPoly> GetPolys(const ReadParam& param);
    std::map<int, std::vector<suSegment>> m_l2seg;
  protected:
    /**
     * 构建各个层及对应的01矩阵
     * 当m_levels的值递增时 1, 表示大于阈值的的格点
     *  例如降雨  指定阈值时100.0, 会标记所有大于100的值表示强降雨
     * 当m_levels的值递减时 1, 表示小于阈值的的格点
     *  例如降雨  指定阈值时20.0, 会标记所有小于20.0的值表示严重干旱
     */
    void MakeWhiteBlack();
    void MakeWhiteBlack(const int& level);
    void TraceWhiteBlack(const int& level);
    [[nodiscard]] suPoint CALC_LINE_0_INTER(const int& r, const int& c, const float& t, const bool& useHalf);
    [[nodiscard]] suPoint CALC_LINE_1_INTER(const int& r, const int& c, const float& t, const bool& useHalf);
    [[nodiscard]] suPoint CALC_LINE_2_INTER(const int& r, const int& c, const float& t, const bool& useHalf);
    [[nodiscard]] suPoint CALC_LINE_3_INTER(const int& r, const int& c, const float& t, const bool& useHalf);


  private:
    std::map<int, float> m_levels;
    std::map<int, suMatrix<int8_t>> m_l2wb;  // 各个层及对应的01矩阵,
    suMatrix<float> m_mat0;
    int m_width = 0;
    int m_height = 0;
};

#endif  // SILLY_RASTER_TO_VECTOR_H
