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

    bool Create(const suDem::Info& _info);

    bool Read(const suPath& file, const bool& onlyhead = false);

    bool Write(const suPath& file, const int& ph = 15, const int& pd = 3) const;

    std::string StringifyLL(const int& precision = 3) const;

  private:
    bool ReadASC(const suPath& file, const bool& onlyhead = false);
    bool ReadBIN(const suPath& file);
    bool ReadPRJ(const suPath& file);

    bool WriteASC(const suPath& file, const int& ph, const int& pd) const;
    bool WriteBIN(const suPath& file) const;
    bool WritePRJ(const suPath& file) const;

  private:
    std::string m_root;
    std::string m_name;
    std::string m_type;
    std::string m_err;
};

#endif  // SILLY_UTILS_SILLY_ASCII_GRID_H
