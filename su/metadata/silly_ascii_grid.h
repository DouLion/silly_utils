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
#include <files/silly_file.h>
#include <metadata/silly_dem.h>

class silly_ascii_grid : public sudem
{
  public:
    silly_ascii_grid() = default;
    ~silly_ascii_grid() = default;

    bool read(const std::filesystem::path& file);

    bool write(const std::filesystem::path& file);

    std::string stringify_ll(const int& precision = 3);

  private:
    bool read_asc(const std::filesystem::path& file);
    bool read_bin(const std::filesystem::path& file);

    bool write_asc(const std::filesystem::path& file);
    bool write_bin(const std::filesystem::path& file);

    bool read_prj(const std::filesystem::path& file);
    bool write_prj(const std::filesystem::path& file);

  public:
    double cellsize = 0;

  private:
    std::string m_root;
    std::string m_name;
    std::string m_type;
};

#endif  // SILLY_UTILS_SILLY_ASCII_GRID_H
