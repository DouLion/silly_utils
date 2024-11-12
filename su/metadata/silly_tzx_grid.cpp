/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/24 15:43
 * @version: 1.0.1
 * @description:
 */
//
// Created by dell on 2024/6/24. 实现
//

#include "silly_tzx_grid.h"

#include <lz4.h>
/// <summary>
/// 数据段必须以这四个字符开头
/// </summary>
const static char silly_tzx_grid_0 = '_';
const static char silly_tzx_grid_1 = 'T';
const static char silly_tzx_grid_2 = 'Z';
const static char silly_tzx_grid_3 = 'X';

/// <summary>
/// 版本1 压缩grid内容
/// </summary>
const static char silly_tzx_grid_V1 = '1';

const int silly_tzx_grid_NAMEL = 32;
const int silly_tzx_grid_UNITL = 32;

#define SILLY_TZX_GRID_MEMCPY_TYPE(buff, val) \
    memcpy(buff, &val, sizeof(val));          \
    buff += sizeof(val);

silly_tzx_grid::silly_tzx_grid()
{
    m_prefix[0] = silly_tzx_grid_V1;
    m_prefix[1] = silly_tzx_grid_1;
    m_prefix[2] = silly_tzx_grid_2;
    m_prefix[3] = silly_tzx_grid_3;
}

bool silly_tzx_grid::serialize(char** buff, size_t& len)
{
    bool status = false;
    if (!grid.row() || !grid.col())  // 以网格点数据为主
    {
        return status;
    }
    total = sizeof(m_prefix) + sizeof(total) + sizeof(left) * 4 + sizeof(xdelta) * 2 + sizeof(name) + sizeof(units) + sizeof(row) + sizeof(col) + sizeof(float) * grid.row() * grid.col();
    len = total;

    if (*buff)
    {
        free(*buff);
    }

    *buff = (char*)SILLY_TZX_GRID_MALLOC(total);
    if (!*buff)
    {
        return status;
    }
    char* p = *buff;

    memcpy(p, m_prefix, sizeof(m_prefix));
    p += sizeof(m_prefix);
    memcpy(p, &total, sizeof(total));
    p += sizeof(total);

    memcpy(p, &left, sizeof(left));
    p += sizeof(left);
    memcpy(p, &top, sizeof(top));
    p += sizeof(top);
    memcpy(p, &right, sizeof(right));
    p += sizeof(right);
    memcpy(p, &bottom, sizeof(bottom));
    p += sizeof(bottom);

    memcpy(p, &xdelta, sizeof(xdelta));
    p += sizeof(xdelta);
    memcpy(p, &ydelta, sizeof(ydelta));
    p += sizeof(ydelta);

    memcpy(p, name, sizeof(name));
    p += sizeof(name);
    memcpy(p, units, sizeof(units));
    p += sizeof(units);

    memcpy(p, &row, sizeof(row));
    p += sizeof(row);
    memcpy(p, &col, sizeof(col));
    p += sizeof(col);

    memcpy(p, grid.seek_row(0), row * col * sizeof(float));
    status = true;

    return status;
}

bool silly_tzx_grid::serializev1(char** buff, size_t& len)
{
    bool status = false;
    if (!grid.row() || !grid.col())  // 以网格点数据为主
    {
        return status;
    }
    SU_MEM_FREE(*buff)
    // 先计算压缩数据大小
    char* cps_data = nullptr;
    size_t cps_len = 0;
    if (!lz4_cps_data((char*)grid.seek_row(0), row * col * sizeof(float), &cps_data, cps_len) || !cps_len)
    {
        SU_MEM_FREE(cps_data)
        return false;
    }

    total = sizeof(m_prefix) + sizeof(total) + sizeof(left) * 4 + sizeof(xdelta) * 2 + sizeof(name) + sizeof(units) + sizeof(row) + sizeof(col) + sizeof(cps_len) + cps_len;
    *buff = (char*)SILLY_TZX_GRID_MALLOC(total);
    if (!*buff)
    {
        return status;
    }
    char* p = *buff;

    memcpy(p, m_prefix, sizeof(m_prefix));
    p += sizeof(m_prefix);

    SILLY_TZX_GRID_MEMCPY_TYPE(p, total)

    SILLY_TZX_GRID_MEMCPY_TYPE(p, left)

    SILLY_TZX_GRID_MEMCPY_TYPE(p, top)

    SILLY_TZX_GRID_MEMCPY_TYPE(p, right)

    SILLY_TZX_GRID_MEMCPY_TYPE(p, bottom)

    SILLY_TZX_GRID_MEMCPY_TYPE(p, xdelta)

    SILLY_TZX_GRID_MEMCPY_TYPE(p, ydelta)

    memcpy(p, name, sizeof(name));
    p += sizeof(name);
    memcpy(p, units, sizeof(units));
    p += sizeof(units);

    SILLY_TZX_GRID_MEMCPY_TYPE(p, row)

    SILLY_TZX_GRID_MEMCPY_TYPE(p, col)

    SILLY_TZX_GRID_MEMCPY_TYPE(p, cps_len)

    memcpy(p, cps_data, cps_len);
    SU_MEM_FREE(cps_data)
    status = true;
    len = total;
    return status;
}

bool silly_tzx_grid::unserialize(const char* buff, const size_t& len)
{
    bool status = false;
    if (!(silly_tzx_grid_1 == buff[1] && silly_tzx_grid_2 == buff[2] && silly_tzx_grid_3 == buff[3]))
    {
        return status;
    }
    if (silly_tzx_grid_V1 == buff[0])
    {
        return unserializev1(buff, len);
    }
    else if (silly_tzx_grid_0 != buff[0])
    {
        return false;
    }

    char* p = (char*)buff + 4;
    total = ((size_t*)p)[0];
    p += sizeof(size_t);
    if (total != len)
    {
        return false;
    }
    left = ((float*)p)[0];
    p += sizeof(float);
    top = ((float*)p)[0];
    p += sizeof(float);
    right = ((float*)p)[0];
    p += sizeof(float);
    bottom = ((float*)p)[0];
    p += sizeof(float);

    xdelta = ((float*)p)[0];
    p += sizeof(float);
    ydelta = ((float*)p)[0];
    p += sizeof(float);

    memcpy(name, p, sizeof(name));
    p += sizeof(name);
    memcpy(units, p, sizeof(units));
    p += sizeof(units);

    row = ((size_t*)p)[0];
    p += sizeof(size_t);
    col = ((size_t*)p)[0];
    p += sizeof(size_t);

    this->grid.create(row, col, true);
    memcpy(grid.seek_row(0), p, row * col * sizeof(float));

    status = true;
    return status;
}

bool silly_tzx_grid::unserializev1(const char* buff, const size_t& len)
{
    char* p = (char*)buff + 4;

    total = ((size_t*)p)[0];
    p += sizeof(size_t);
    if (total != len)
    {
        return false;
    }
    left = ((float*)p)[0];
    p += sizeof(float);
    top = ((float*)p)[0];
    p += sizeof(float);

    right = ((float*)p)[0];
    p += sizeof(float);

    bottom = ((float*)p)[0];
    p += sizeof(float);

    xdelta = ((float*)p)[0];
    p += sizeof(float);
    ydelta = ((float*)p)[0];
    p += sizeof(float);

    memcpy(name, p, sizeof(name));
    p += sizeof(name);
    memcpy(units, p, sizeof(units));
    p += sizeof(units);

    row = ((size_t*)p)[0];
    p += sizeof(size_t);
    col = ((size_t*)p)[0];
    p += sizeof(size_t);
    cpsl = ((size_t*)p)[0];
    p += sizeof(size_t);

    char* dcps_data = nullptr;
    size_t dcps_len = row * col * sizeof(float);
    bool dcps_status = lz4_dcps_data(p, cpsl, &dcps_data, dcps_len);
    if (dcps_status && dcps_data && dcps_len == row * col * sizeof(float))
    {
        grid.create(row, col, true);
        memcpy(grid.seek_row(0), dcps_data, row * col * sizeof(float));
        SU_MEM_FREE(dcps_data)
        return true;
    }

    return false;
}

void silly_tzx_grid::puzzle(const std::vector<silly_tzx_grid>& srg_list, const silly_tzx_grid_rect& rect)
{
    row = std::round((top - bottom) / rect.ydelta);
    col = std::round((right - left) / rect.ydelta);
    grid.create(row, col);
    for (int r = 0; r < row; r++)
    {
        for (int c = 0; c < col; c++)
        {
            float x = left + c * rect.xdelta;
            float y = top - r * rect.ydelta;
            // 使用两个值之间的最大值
            float value = -999999.0;  // invalid data

            for (auto srg : srg_list)
            {
                int tx, ty;
                tx = std::round((x - srg.left) / srg.xdelta);
                ty = std::round((srg.top - y) / srg.ydelta);
                if (tx >= 0 && tx < srg.col && ty >= 0 && ty < srg.row)
                {
                    value = SU_MAX(value, srg.grid[ty][tx]);
                }
            }
            grid.at(r, c) = value;
        }
    }
    return;
}

void silly_tzx_grid::maxv(int& tr, int& tc, float& tv)
{
    tv = -1e10;
    for (int r = 0; r < row; r++)
    {
        for (int c = 0; c < col; c++)
        {
            if (grid[r][c] > tv)
            {
                tv = grid[r][c];
                tr = r;
                tc = c;
            }
        }
    }
}

bool silly_tzx_grid::read(const std::string& path)
{
    bool status = false;
    char* buff = nullptr;
#ifndef NDEBUG
    //  std::cout << "Read GRID : " << path << std::endl;
#endif
    std::ifstream inputFileStream(path, std::ios::binary | std::ios::ate);
    if (!inputFileStream.is_open())
    {
        std::cout << "Error opening file: " << path << std::endl;
        return false;
    }

    size_t fileSize = static_cast<size_t>(inputFileStream.tellg());
    inputFileStream.seekg(0, std::ios::beg);

    if (fileSize)
    {
        buff = new char[fileSize];
        inputFileStream.read(buff, fileSize);
        status = unserialize(buff, fileSize);
        delete[] buff;
    }
    inputFileStream.close();
    // SILLY_TZX_GRID_FREE(buff)
    return status;
}
bool silly_tzx_grid::save(const std::string& path)
{
    bool status = false;
    char* buff = nullptr;
    size_t len = 0;
    if (serializev1(&buff, len))
    {
        FILE* pf = fopen(path.c_str(), "wb");
        if (pf)
        {
            fwrite(buff, 1, len, pf);
            fclose(pf);
            status = true;
        }

        // status = FileUtils::WriteAll(path.c_str(), buff, len);
    }
    SILLY_TZX_GRID_FREE(buff)
    return status;
}

bool silly_tzx_grid::lz4_cps_data(const char* srcd, const size_t& srcl, char** dstd, size_t& dstl)
{
    if (srcd && srcl)
    {
        *dstd = (char*)malloc(srcl * 2);
        int len = LZ4_compress_default(srcd, *dstd, srcl, srcl * 2);
        if (len > 0)
        {
            dstl = static_cast<size_t>(len);
            return true;
        }
        SU_MEM_FREE(*dstd);
    }
    return false;
}

bool silly_tzx_grid::lz4_dcps_data(const char* srcd, const size_t& srcl, char** dstd, size_t& dstl)
{
    if (srcd && srcl)
    {
        *dstd = (char*)malloc(dstl * 2);
        int len = LZ4_decompress_safe(srcd, *dstd, srcl, dstl * 2);
        if (len > 0)
        {
            dstl = static_cast<size_t>(len);
            return true;
        }
        SU_MEM_FREE(*dstd);
    }
    return false;
}
void silly_tzx_grid::release()
{
    grid.release();
}
silly_tzx_grid silly_tzx_grid::operator=(silly_tzx_grid other)
{
    this->total = other.total;
    this->left = other.left;
    this->right = other.right;
    this->top = other.top;
    this->bottom = other.bottom;
    this->xdelta = other.xdelta;
    this->ydelta = other.ydelta;

    this->row = other.row;
    this->col = other.col;
    this->grid = other.grid;
    return *this;
}
