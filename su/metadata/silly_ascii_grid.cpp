/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/3 11:31
 * @version: 1.0.1
 * @description:
 */
//
// Created by dell on 2024/6/3. 实现
//

#include "silly_ascii_grid.h"

char SAG_ASC_BEG[6] = "ncols";
bool silly_ascii_grid::read(const std::string& path)
{
    bool status = false;

    if (!m_mmap.open_m(path))
    {
        return status;
    }
    if (m_mmap.at_m(0)[0] == SAG_ASC_BEG[0] && m_mmap.at_m(1)[0] == SAG_ASC_BEG[1] && m_mmap.at_m(2)[0] == SAG_ASC_BEG[2] && m_mmap.at_m(3)[0] == SAG_ASC_BEG[3] && m_mmap.at_m(4)[0] == SAG_ASC_BEG[4])
    {
        status = read_asc(path);
    }
    else
    {
        status = read_bin();
    }
    return status;
}

std::string to_lower(const std::string& str)
{
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
    return lower_str;
}
bool silly_ascii_grid::read_asc(const std::string& path)
{
    bool status = false;
    std::string content;
    if (silly_file::read(path, content))
    {  // 假设 silly_file::read 返回 true 表示成功
        status = true;
    }
    else
    {
        std::cerr << "Failed to read file: " << path << std::endl;
        return status;
    }

    std::stringstream linestream(content);
    std::string key;
    while (!linestream.eof())
    {
        linestream >> key;
        if (to_lower(key) == "ncols")
        {
            linestream >> ncols;
        }
        else if (to_lower(key) == "nrows")
        {
            linestream >> nrows;
        }
        else if (to_lower(key) == "xllcorner")
        {
            linestream >> xllcorner;
        }
        else if (to_lower(key) == "yllcorner")
        {
            linestream >> yllcorner;
        }
        else if (to_lower(key) == "cellsize")
        {
            linestream >> cellsize;
        }
        else if (to_lower(key) == "nodata_value")
        {
            linestream >> NODATA;
        }
        else
        {
            std::cerr << "Invalid key: " << key << std::endl;
            return false;
        }

        if (to_lower(key) == "nodata_value")
            break;  // header section ends after NODATA_value
    }

    m_data.create(nrows, ncols);

    double value;
    for (int r = 0; r < nrows && linestream.good(); ++r)
    {
        for (int c = 0; c < ncols && linestream.good(); ++c)
        {
            linestream >> value;
            MAXV = SU_MAX(value, MAXV);
            MINV = SU_MIN(value, MINV);
            m_data[r][c] = value;
        }
    }

    return true;
}
bool silly_ascii_grid::read_bin()
{
    bool status = false;
    size_t total = m_mmap.size_m();
    double* curr = ((double*)m_mmap.at_m(0));
    nrows = curr[0];
    curr++;
    ncols = curr[0];
    curr++;

    if (static_cast<size_t>((nrows * ncols + 2) * sizeof(double)) != total)
    {
        return status;
    }
    m_data.create(nrows, ncols);
    size_t i = 0;

    for (size_t r = 0; r < nrows; ++r)
    {
        for (size_t c = 0; c < ncols; ++c)
        {
            MAXV = SU_MAX(curr[i], MAXV);
            MINV = SU_MIN(curr[i], MINV);
            m_data[r][c] = curr[0];
            curr++;
        }
    }

    status = true;
    return status;
}
