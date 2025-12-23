/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-23
 * @file: RadarDataNaRui.c
 * @description:
 * @version: v1.0.1 2025-12-23 dou li yang
 */
#include "RadarDataNaRui.h"
#include <files/silly_file.h>
// 转化小端序的int值
#define CS_CONVERT_NORMAL_INT(src_ptr) ((int*)src_ptr)[0]
// 转化小端序的short值
#define CS_CONVERT_NORMAL_SHORT(src_ptr) ((short*)src_ptr)[0]

bool NaRuiRadar::read(const suPath& file, const bool& only_head)
{
    bool status = false;
    status = read_header(file);
    if (!status || only_head)
    {
        return status;
    }
    status = read_grid(file);
    return status;
}
bool NaRuiRadar::read_header(const suPath& file)
{
    bool status = false;
    std::ifstream ifs;
    ifs.open(file, std::ios::out | std::ios::binary);
    // ifs.open(path, std::ios::in);
    // 判断文件是否打开成功
    if (!ifs.is_open())
    {
        std::cerr << "file open failed" << std::endl;
        return status;
    }
    ifs.seekg(0, std::ios::end);
    std::streampos fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    char* idx = nullptr;
    try
    {
        idx = new char[fileSize];
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return status;
    }

    ifs.read(idx, fileSize);

    // 读取完成后关闭文件
    ifs.close();
    if (!idx)
    {
        return status;
    }

    char* pidx = idx;

    memcpy(m_header.VolumeLabel, idx, sizeof(m_header.VolumeLabel));

    idx += sizeof(m_header.VolumeLabel);

    memcpy(m_header.VersionNo, idx, sizeof(m_header.VersionNo));
    idx += sizeof(m_header.VersionNo);

    m_header.FileLength = CS_CONVERT_NORMAL_INT(idx);
    idx += sizeof(m_header.FileLength);

    m_header.bottom = CS_CONVERT_NORMAL_INT(idx) / m_header.deg_scale;
    idx += sizeof(int);
    // std::cout << m_header.bottom << std::endl;

    m_header.left = CS_CONVERT_NORMAL_INT(idx) / m_header.deg_scale;
    idx += sizeof(int);
    // std::cout << m_header.left << std::endl;

    m_header.top = CS_CONVERT_NORMAL_INT(idx) / m_header.deg_scale;
    idx += sizeof(int);
    // std::cout << m_header.top << std::endl;

    m_header.right = CS_CONVERT_NORMAL_INT(idx) / m_header.deg_scale;
    idx += sizeof(int);
    // std::cout << m_header.right << std::endl;

    m_header.rows = CS_CONVERT_NORMAL_INT(idx);  // 5500
    idx += sizeof(m_header.rows);

    m_header.cols = CS_CONVERT_NORMAL_INT(idx);  // 5600
    idx += sizeof(m_header.cols);

    m_header.dlat = CS_CONVERT_NORMAL_INT(idx) / m_header.deg_scale;  // 0.003
    idx += sizeof(int);

    m_header.dlon = CS_CONVERT_NORMAL_INT(idx) / m_header.deg_scale;  // 0.003
    idx += sizeof(int);

    m_header.calt = CS_CONVERT_NORMAL_INT(idx);
    idx += sizeof(m_header.calt);

    memcpy(m_header.varCode, idx, sizeof(m_header.varCode));
    idx += sizeof(m_header.varCode);

    memcpy(m_header.varUnit, idx, sizeof(m_header.varUnit));
    idx += sizeof(m_header.varUnit);

    memcpy(m_header.varName, idx, sizeof(m_header.varName));
    idx += sizeof(m_header.varName);

    m_header.varID = CS_CONVERT_NORMAL_SHORT(idx);  // unsigned short varID;  // 产品编号
    idx += sizeof(m_header.varID);

    m_header.mode = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.mode);

    m_header.range = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.range);

    m_header.scale = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.scale);

    m_header.offset = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.offset);

    m_header.clear = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.clear);

    m_header.missing = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.missing);

    m_header.minCode = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.minCode);

    m_header.span = CS_CONVERT_NORMAL_INT(idx);
    idx += sizeof(m_header.span);

    m_header.syear = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.syear);

    m_header.smonth = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.smonth);

    m_header.sday = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.sday);

    m_header.shour = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.shour);

    m_header.sminute = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.sminute);

    m_header.ssecond = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.ssecond);

    m_header.eyear = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.eyear);

    m_header.emonth = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.emonth);

    m_header.eday = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.eday);

    m_header.ehour = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.ehour);

    m_header.eminute = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.eminute);

    m_header.esecond = CS_CONVERT_NORMAL_SHORT(idx);
    idx += sizeof(m_header.esecond);

    memcpy(m_header.rgnID, idx, sizeof(m_header.rgnID));
    idx += sizeof(m_header.rgnID);

    memcpy(m_header.rgnName, idx, sizeof(m_header.rgnName));
    idx += sizeof(m_header.rgnName);

    memcpy(m_header.country, idx, sizeof(m_header.country));
    idx += sizeof(m_header.country);

    memcpy(m_header.reserve, idx, sizeof(m_header.reserve));

    idx += sizeof(m_header.reserve);

    SUM_MEM_DEL_ARR(pidx)

    status = true;
    return status;
}
bool NaRuiRadar::read_grid(const suPath& file, const double& dst_scale)
{
    bool status = false;
    std::ifstream ifs;
    ifs.open(file, std::ios::out | std::ios::binary);
    // ifs.open(path, std::ios::in);
    // 判断文件是否打开成功
    if (!ifs.is_open())
    {
        std::cerr << "file open failed" << std::endl;
        return status;
    }
    ifs.seekg(0, std::ios::end);
    std::streampos fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    char* idx = nullptr;
    try
    {
        idx = new char[fileSize];
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return status;
    }

    ifs.read(idx, fileSize);

    // 读取完成后关闭文件
    ifs.close();
    if (!idx)
    {
        return status;
    }
    char* cpptr = idx;
    cpptr += sizeof(m_header);
    suMatrix<float> tmp_grid;
    if (!tmp_grid.create(m_header.rows, m_header.cols))
    {
        SUM_MEM_DEL_ARR(idx)
        return false;
    }

    short* pidx = (short*)cpptr;
    if (!m_header.missing)  // missing == 0
    {
        for (int r = 0; r < m_header.rows; ++r)
        {
            for (int c = 0; c < m_header.cols; ++c)
            {
                tmp_grid[r][c] = static_cast<float>((pidx[0] - m_header.offset) / m_header.scale);
                pidx++;
            }
        }
    }
    else
    {
        for (int r = 0; r < m_header.rows; ++r)
        {
            for (int c = 0; c < m_header.cols; ++c)
            {
                if (pidx[0] == m_header.missing)
                {
                    m_matrix[r][c] = 0;
                }
                else
                {
                    tmp_grid[r][c] = (pidx[0] - m_header.offset) / m_header.scale;
                }
                pidx++;
            }
        }
    }
    // grid = tmp_grid;
    if (dst_scale == m_header.scale)
    {
        m_matrix = tmp_grid;
    }
    else
    {
        size_t drows = tmp_grid.row() * m_header.dlat / dst_scale;
        size_t dcols = tmp_grid.col() * m_header.dlon / dst_scale;
        // TODO : DLY
        //  matrix_tools::resize(tmp_grid, grid, drows, dcols);
        tmp_grid.release();
    }

    status = true;
    SUM_MEM_DEL_ARR(idx)
    return status;
}