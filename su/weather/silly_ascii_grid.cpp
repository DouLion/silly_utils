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
bool silly_ascii_grid::read_asc(const std::string& path)
{
    bool status = false;
    std::ifstream inFile(path);
    if (!inFile)
    {
        std::cerr << "Unable to open file: " << path << std::endl;
        return false;
    }
    std::string line;
    std::stringstream linestream;
    std::string key;
    while (std::getline(inFile, line))
    {
        linestream.clear();
        linestream.str(line);
        linestream >> key;
        if (key == "ncols")
        {
            linestream >> ncols;
        }
        else if (key == "nrows")
        {
            linestream >> nrows;
        }
        else if (key == "xllcorner")
        {
            linestream >> xllcorner;
        }
        else if (key == "yllcorner")
        {
            linestream >> yllcorner;
        }
        else if (key == "cellsize")
        {
            linestream >> cellsize;
        }
        else if (key == "NODATA_value")
        {
            linestream >> NODATA;
        }
        if (key == "NODATA_value")
            break;  // header section ends after NODATA_value
    }

    m_data.create(nrows, ncols);

    double value;
    for (int r = 0; r < nrows; ++r)
    {
        for (int c = 0; c < ncols; ++c)
        {
            inFile >> value;
            //data[r * ncols + c];
            m_data[r][c] = value;
        }
    }

    inFile.close();
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



    if(static_cast<size_t>((nrows * ncols+2) * sizeof(double)) != total)
    {
        return status;
    }
    m_data.create(nrows, ncols);
    size_t i = 0;

    for(size_t r = 0; r < nrows; ++r)
    {
        for(size_t c = 0; c < ncols; ++c)
        {
           /* MAXV = SU_MAX(curr[i], MAXV);
            MINV = SU_MIN(curr[i], MINV);*/
            m_data[r][c] = curr[0];
            curr++;
        }
    }

    status = true;
    return status;
}
