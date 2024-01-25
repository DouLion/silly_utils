/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/25 17:21
 * @version: 1.0.1
 * @description: 雷达网格数据存储
 */
#ifndef SILLY_UTILS_SILLY_RADAR_GRID_H
#define SILLY_UTILS_SILLY_RADAR_GRID_H
#include <su_marco.h>
#include <iostream>
#include <math/silly_matrix.h>
#include <vector>
using namespace silly_math;
#define SILLY_RADAR_GRID_FILE_SUFFIX  ".rgrid"

#define SILLY_RADAR_GRID_MALLOC(l)  malloc(l);

#define SILLY_RADAR_GRID_FREE(p) {if(p){free(p); p = nullptr;}}
class silly_radar_grid {

public:
    silly_radar_grid();

    /// <summary>
    ///
    /// </summary>
    /// <param name="buff">输出: 数据区域</param>
    /// <param name="len">输出: 数据区域长度</param>
    /// <returns></returns>
    bool serialize(char** buff, size_t& len);

    /// <summary>
    ///
    /// </summary>
    /// <param name="buff">输入: 数据区域</param>
    /// <param name="len">输入: 数据区域长度</param>
    /// <returns></returns>
    bool unserialize(const char* buff, const size_t& len);

    /// <summary>
    /// 将多个网格数据拼接为一个,重叠部分采用最大值
    /// </summary>
    /// <param name="srg_list"></param>
    void puzzle(const std::vector<silly_radar_grid>& srg_list);

    bool read(const std::string& path);
    bool save(const std::string& path);

    silly_radar_grid operator=(silly_radar_grid other)
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
        this->grid.create(other.grid.row(), other.grid.col());
        for (size_t r = 0; r < this->grid.row(); ++r)
        {
            for (size_t c = 0; c < this->grid.col(); ++c)
            {
                this->grid.at(r, c) = SU_MAX(other.grid.at(r, c), 0);
            }
        }

        return *this;
    }


public:
    /*
    _TZX
    总大小
    高
    宽
    上下左右
    步长
    缩放
    偏移
    名称(UTF8)
    单位(UTF8)

    short的一维网格点,
    中国境内以x=0度,y=N90度为原点
    x轴以 0-E180为正  y轴以 N90-S90为正方向
    顺序,从左到右,从上到下
    */
    size_t total{ 0 };
    float left{ 0. };
    float right{ 0. };
    float top{ 0. };
    float bottom{ 0. };

    float xdelta{ 0. };
    float ydelta{ 0. };

    char name[32]{ 0 };
    char units[32]{ 0 };


    size_t row;
    size_t col;
    FMatrix grid;
private:
    char m_prefix[4];

};

#endif //SILLY_UTILS_SILLY_RADAR_GRID_H
