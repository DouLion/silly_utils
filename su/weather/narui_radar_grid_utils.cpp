//
// Created by dell on 2024/3/18.
//

#include "narui_radar_grid_utils.h"
#include <fstream>

// 转化小端序的int值
#define CS_CONVERT_NOMAL_INT(src_ptr) ((int*)src_ptr)[0]
// 转化小端序的short值
#define CS_CONVERT_NOMAL_SHORT(src_ptr) ((short*)src_ptr)[0]

bool silly_nrd_grid_utils::read(const std::string& file)
{
    bool status = false;
    if (read_header(file))
        ;
    {
        status = read_grid(file);
    }
    return status;
}

bool silly_nrd_grid_utils::read_header(const std::string& file)
{
    bool status = false;
    std::ifstream ifs;
    ifs.open(file, std::ios::out | std::ios::binary);
    // ifs.open(path, std::ios::in);
    //判断文件是否打开成功
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

    memcpy(head.VolumeLabel, idx, sizeof(head.VolumeLabel));

    idx += sizeof(head.VolumeLabel);

    memcpy(head.VersionNo, idx, sizeof(head.VersionNo));
    idx += sizeof(head.VersionNo);

    head.FileLength = CS_CONVERT_NOMAL_INT(idx);
    idx += sizeof(head.FileLength);

    head.bottom = CS_CONVERT_NOMAL_INT(idx) / head.deg_scale;
    idx += sizeof(int);
    // std::cout << head.bottom << std::endl;

    head.left = CS_CONVERT_NOMAL_INT(idx) / head.deg_scale;
    idx += sizeof(int);
    // std::cout << head.left << std::endl;

    head.top = CS_CONVERT_NOMAL_INT(idx) / head.deg_scale;
    idx += sizeof(int);
    // std::cout << head.top << std::endl;

    head.right = CS_CONVERT_NOMAL_INT(idx) / head.deg_scale;
    idx += sizeof(int);
    // std::cout << head.right << std::endl;

    head.rows = CS_CONVERT_NOMAL_INT(idx);  // 5500
    idx += sizeof(head.rows);

    head.cols = CS_CONVERT_NOMAL_INT(idx);  // 5600
    idx += sizeof(head.cols);

    head.dlat = CS_CONVERT_NOMAL_INT(idx) / head.deg_scale;  // 0.003
    idx += sizeof(int);

    head.dlon = CS_CONVERT_NOMAL_INT(idx) / head.deg_scale;  // 0.003
    idx += sizeof(int);

    head.calt = CS_CONVERT_NOMAL_INT(idx);
    idx += sizeof(head.calt);

    memcpy(head.varCode, idx, sizeof(head.varCode));
    idx += sizeof(head.varCode);

    memcpy(head.varUnit, idx, sizeof(head.varUnit));
    idx += sizeof(head.varUnit);

    memcpy(head.varName, idx, sizeof(head.varName));
    idx += sizeof(head.varName);

    head.varID = CS_CONVERT_NOMAL_SHORT(idx);  // unsigned short varID;  // 产品编号
    idx += sizeof(head.varID);

    head.mode = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.mode);

    head.range = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.range);

    head.scale = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.scale);

    head.offset = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.offset);

    head.clear = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.clear);

    head.missing = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.missing);

    head.minCode = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.minCode);

    head.span = CS_CONVERT_NOMAL_INT(idx);
    idx += sizeof(head.span);

    head.syear = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.syear);

    head.smonth = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.smonth);

    head.sday = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.sday);

    head.shour = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.shour);

    head.sminute = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.sminute);

    head.ssecond = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.ssecond);

    head.eyear = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.eyear);

    head.emonth = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.emonth);

    head.eday = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.eday);

    head.ehour = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.ehour);

    head.eminute = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.eminute);

    head.esecond = CS_CONVERT_NOMAL_SHORT(idx);
    idx += sizeof(head.esecond);

    memcpy(head.rgnID, idx, sizeof(head.rgnID));
    idx += sizeof(head.rgnID);

    memcpy(head.rgnName, idx, sizeof(head.rgnName));
    idx += sizeof(head.rgnName);

    memcpy(head.country, idx, sizeof(head.country));
    idx += sizeof(head.country);

    memcpy(head.reserve, idx, sizeof(head.reserve));

    idx += sizeof(head.reserve);

    SUM_MEM_DEL_ARR(pidx)

    status = true;
    return status;
}

bool silly_nrd_grid_utils::read_grid(const std::string& file, const double& dst_scale)
{
    bool status = false;
    std::ifstream ifs;
    ifs.open(file, std::ios::out | std::ios::binary);
    // ifs.open(path, std::ios::in);
    //判断文件是否打开成功
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
    cpptr += sizeof(nrd_header);
    matrix_2d<float> tmp_grid;
    if (!tmp_grid.create(head.rows, head.cols))
    {
        SUM_MEM_DEL_ARR(idx)
        return false;
    }

    short* pidx = (short*)cpptr;
    if (!head.missing)  // missing == 0
    {
        for (int r = 0; r < head.rows; ++r)
        {
            for (int c = 0; c < head.cols; ++c)
            {
                tmp_grid[r][c] = static_cast<float>((pidx[0] - head.offset) / head.scale);
                pidx++;
            }
        }
    }
    else
    {
        for (int r = 0; r < head.rows; ++r)
        {
            for (int c = 0; c < head.cols; ++c)
            {
                if (pidx[0] == head.missing)
                {
                    grid[r][c] = 0;
                }
                else
                {
                    tmp_grid[r][c] = (pidx[0] - head.offset) / head.scale;
                }
                pidx++;
            }
        }
    }
    // grid = tmp_grid;
    if (dst_scale == head.scale)
    {
        grid = tmp_grid;
    }
    else
    {
        size_t drows = tmp_grid.row() * head.dlat / dst_scale;
        size_t dcols = tmp_grid.col() * head.dlon / dst_scale;
        matrix_tools::resize(tmp_grid, grid, drows, dcols);
        tmp_grid.destroy();
    }

    status = true;
    SUM_MEM_DEL_ARR(idx)
    return status;
}
