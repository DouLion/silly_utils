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
        status = read_asc();
    }
    else
    {
        status = read_bin();
    }
    return status;
}
bool silly_ascii_grid::read_asc()
{
    bool status = false;

    return status;
}
bool silly_ascii_grid::read_bin()
{
    bool status = false;
    size_t total = m_mmap.size_m();
    double* curr = ((double*)m_mmap.at_m(0));
    nrows = curr[0];
    ncols = curr[1];



    if(static_cast<size_t>((nrows * ncols+2) * sizeof(double)) != total)
    {
        return status;
    }
    m_data.create(nrows, ncols);
    size_t i = 0;
    curr+= (sizeof(double)*2);
    for(size_t r = 0; r < nrows; ++r)
    {
        for(size_t c = 0; c < ncols; ++c)
        {
           /* MAXV = SU_MAX(curr[i], MAXV);
            MINV = SU_MIN(curr[i], MINV);*/
            m_data[r][c] = curr[i++];
        }
    }

    status = true;
    return status;
}
