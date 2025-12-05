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

class suAsciiGrid : public suDem
{
  public:
    suAsciiGrid() = default;
    ~suAsciiGrid() = default;

    bool create(const suDem::Info& _info);

    bool read(const suPath& file, const bool& onlyhead = false);

    bool write(const suPath& file) const;

    std::string stringify_ll(const int& precision = 3) const;

  private:
    bool read_asc(const suPath& file, const bool& onlyhead = false);
    bool read_bin(const suPath& file);
    bool read_prj(const suPath& file);

    bool write_asc(const suPath& file) const;
    bool write_bin(const suPath& file) const;
    bool write_prj(const suPath& file) const;

  private:
    std::string m_root;
    std::string m_name;
    std::string m_type;
    std::string m_err;
};

#endif  // SILLY_UTILS_SILLY_ASCII_GRID_H
