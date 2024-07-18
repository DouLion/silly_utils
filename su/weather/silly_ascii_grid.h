/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/3 11:31
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_ASCII_GRID_H
#define SILLY_UTILS_SILLY_ASCII_GRID_H
#include "math/silly_matrix.h"
#include "files/silly_file.h"
#include "files/silly_mmap.h"
using namespace silly_math;

class silly_ascii_grid
{
  public:
    silly_ascii_grid() = default;
    ~silly_ascii_grid() = default;

    bool read(const std::string& path);

  private:
    bool read_asc(const std::string& path);
    bool read_bin();

  public:
    size_t ncols;
    size_t nrows;
    double xllcorner;
    double yllcorner;
    double cellsize;
    double MAXV{-99999999.0};
    double MINV{99999999.0};
    float NODATA;
    DMatrix m_data;

  private:
    silly_mmap m_mmap;
};

#endif  // SILLY_UTILS_SILLY_ASCII_GRID_H
