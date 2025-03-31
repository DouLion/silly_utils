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
#include "files/silly_file.h"

class silly_ascii_grid
{
  public:
    silly_ascii_grid() = default;
    ~silly_ascii_grid() = default;

    bool read(const std::string& path);

    bool write(const std::string& path);

  private:
    bool read_asc(const std::string& path);
    bool read_bin(const std::string& path);

    bool write_asc(const std::string& path);
    bool write_bin(const std::string& path);

    bool read_prj(const std::string path);
    bool write_prj(const std::string path);

  public:
    int precision = 1;  // 保留小数位
    double l0 = -9999.0;  // 高斯基带中心线经度
    size_t ncols = 0;
    size_t nrows = 0;
    double xllcorner = 0;
    double yllcorner = 0;
    double cellsize = 0;
    double MAXV{-99999999.0};
    double MINV{99999999.0};
    double NODATA=-9999.0;
    std::vector<double> m_data;  //

  private:
    std::string m_root;
    std::string m_name;
    std::string m_type;
};

#endif  // SILLY_UTILS_SILLY_ASCII_GRID_H
