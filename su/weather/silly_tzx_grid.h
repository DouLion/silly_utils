/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/24 15:43
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_TZX_GRID_H
#define SILLY_UTILS_SILLY_TZX_GRID_H

#include <su_marco.h>
#include <math/silly_matrix.h>
using namespace  silly_math;

#define SILLY_TZX_GRID_FILE_SUFFIX ".rgrid"

#define SILLY_TZX_GRID_MALLOC(l) malloc(l);

#define SILLY_TZX_GRID_FREE(p) \
    {                          \
        if (p)                 \
        {                      \
            free(p);           \
            p = nullptr;       \
        }                      \
    }

struct silly_tzx_grid_rect
{
    double left;
    double top;
    double right;
    double bottom;
    double xdelta;
    double ydelta;
};

class silly_tzx_grid
{
  public:
    silly_tzx_grid();

    /// <summary>
    ///
    /// </summary>
    /// <param name="buff">输出: 数据区域</param>
    /// <param name="len">输出: 数据区域长度</param>
    /// <returns></returns>
    bool serialize(char** buff, size_t& len);

    bool serializev1(char** buff, size_t& len);

    /// <summary>
    ///
    /// </summary>
    /// <param name="buff">输入: 数据区域</param>
    /// <param name="len">输入: 数据区域长度</param>
    /// <returns></returns>
    bool unserialize(const char* buff, const size_t& len);
    bool unserializev1(const char* buff, const size_t& len);

    /// <summary>
    /// 将多个网格数据拼接为一个,重叠部分采用最大值
    /// </summary>
    /// <param name="srg_list"></param>
    void puzzle(const std::vector<silly_tzx_grid>& srg_list, const silly_tzx_grid_rect& rect);

    /// <summary>
    /// 获取最大网格点所在的 行列号 和值
    /// </summary>
    /// <param name="r"></param>
    /// <param name="c"></param>
    /// <param name="v"></param>
    void maxv(int& tr, int& tc, float& tv);

    bool read(const std::string& path);
    bool save(const std::string& path);

    silly_tzx_grid operator=(silly_tzx_grid other)
    {
        this->total = other.total;
        this->left = other.left;
        this->right = other.right;
        this->top = other.top;
        this->bottom = other.bottom;
        this->xdelta = other.xdelta;
        this->ydelta = other.ydelta;

        this->row = other.row;
        this->col = other.col;
        this->grid = other.grid;
        return *this;
    }

  private:
    /// <summary>
    /// 用lz4压缩数据
    /// </summary>
    /// <param name="srcd"></param>
    /// <param name="srcl"></param>
    /// <param name="dstd"></param>
    /// <param name="dstl"></param>
    /// <returns></returns>
    bool lz4_cps_data(const char* srcd, const size_t& srcl, char** dstd, size_t& dstl);

    /// <summary>
    /// 用lz4解压数据块
    /// </summary>
    /// <param name="srcd"></param>
    /// <param name="srcl"></param>
    /// <param name="dstd"></param>
    /// <param name="dstl"></param>
    /// <returns></returns>
    bool lz4_dcps_data(const char* srcd, const size_t& srcl, char** dstd, size_t& dstl);

  public:
    size_t total{0};
    float left{0.};
    float right{0.};
    float top{0.};
    float bottom{0.};

    float xdelta{0.};
    float ydelta{0.};

    char name[32]{0};
    char units[32]{0};

    size_t row{0};
    size_t col{0};
    size_t cpsl{0};  // 网格点数据的压缩后占用大小
    size_t srcl{0};  // 网格点数据的压缩前占用大小
    FMatrix grid;

  private:
    char m_prefix[4];
};

#endif  // SILLY_UTILS_SILLY_TZX_GRID_H
