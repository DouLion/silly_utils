//
// Created by dell on 2024/1/25.
//

#include "silly_radar_grid.h"
#include "files/silly_file.h"
#include <su_marco.h>
#include <fstream>

/// <summary>
/// 数据段必须以这四个字符开头
/// </summary>
const static char SILLY_RADAR_GRID_0 = '_';
const static char SILLY_RADAR_GRID_1 = 'T';
const static char SILLY_RADAR_GRID_2 = 'Z';
const static char SILLY_RADAR_GRID_3 = 'X';

const int SILLY_RADAR_GRID_NAMEL = 32;
const int SILLY_RADAR_GRID_UNITL = 32;

#define SILLY_RADAR_GRID_MEMCPY_TYPE(buff, val) memcpy(buff, &val, sizeof(val)); buff += sizeof(val);


silly_radar_grid::silly_radar_grid()
{
    m_prefix[0] = SILLY_RADAR_GRID_0;
    m_prefix[1] = SILLY_RADAR_GRID_1;
    m_prefix[2] = SILLY_RADAR_GRID_2;
    m_prefix[3] = SILLY_RADAR_GRID_3;
}

bool silly_radar_grid::serialize(char** buff, size_t& len)
{
    bool status = false;
    if (!grid.row() || !grid.col())	// 以网格点数据为主
    {
        return status;
    }
    total = sizeof(m_prefix) + sizeof(total) + sizeof(left) * 4 + sizeof(xdelta) * 2 + sizeof(name) + sizeof(units) + sizeof(row) + sizeof(col) + sizeof(float) * grid.row() * grid.col();
    len = total;

    if (*buff)
    {
        free(*buff);
    }

    *buff = (char*)SILLY_RADAR_GRID_MALLOC(total);
    if (!*buff) { return status; }
    char* p = *buff;

    memcpy(p, m_prefix, sizeof(m_prefix)); p += sizeof(m_prefix);
    memcpy(p, &total, sizeof(total)); p += sizeof(total);

    memcpy(p, &left, sizeof(left)); p += sizeof(left);
    memcpy(p, &top, sizeof(top)); p += sizeof(top);
    memcpy(p, &right, sizeof(right)); p += sizeof(right);
    memcpy(p, &bottom, sizeof(bottom)); p += sizeof(bottom);

    memcpy(p, &xdelta, sizeof(xdelta)); p += sizeof(xdelta);
    memcpy(p, &ydelta, sizeof(ydelta)); p += sizeof(ydelta);

    memcpy(p, name, sizeof(name)); p += sizeof(name);
    memcpy(p, units, sizeof(units)); p += sizeof(units);

    memcpy(p, &row, sizeof(row)); p += sizeof(row);
    memcpy(p, &col, sizeof(col)); p += sizeof(col);

    memcpy(p, grid.seek_row(0), row * col * sizeof(float));
    status = true;

    return status;
}

bool silly_radar_grid::unserialize(const char* buff, const size_t& len)
{
    bool status = false;
    if (!(SILLY_RADAR_GRID_0 == buff[0] && SILLY_RADAR_GRID_1 == buff[1] && SILLY_RADAR_GRID_2 == buff[2] && SILLY_RADAR_GRID_3 == buff[3]))
    {
        return status;
    }
    char* p = (char*)buff + 4;
    total = ((size_t*)p)[0]; p += sizeof(size_t);
    if (total > len)
    {
        return false;
    }
    left = ((float*)p)[0]; p += sizeof(float);
    top = ((float*)p)[0]; p += sizeof(float);
    right = ((float*)p)[0]; p += sizeof(float);
    bottom = ((float*)p)[0]; p += sizeof(float);

    xdelta = ((float*)p)[0]; p += sizeof(float);
    ydelta = ((float*)p)[0]; p += sizeof(float);

    memcpy(name, p, sizeof(name)); p += sizeof(name);
    memcpy(units, p, sizeof(units)); p += sizeof(units);

    row = ((size_t*)p)[0]; p += sizeof(size_t);
    col = ((size_t*)p)[0]; p += sizeof(size_t);

    grid.create(row, col, true);
    memcpy(grid.seek_row(0), p, row * col * sizeof(float));

    status = true;
    return status;
}
void silly_radar_grid::puzzle(const std::vector<silly_radar_grid>& srg_list)
{
    // left = 109.47, top = 29.88, right = 112.82, bottom = 27.19;

    left = 109.47;
    top = 29.88;
    right = 112.82;
    bottom = 27.19;
    ydelta = 0.0025;
    xdelta = 0.0025;
    row = std::round((top - bottom) / ydelta);
    col = std::round((right - left) / ydelta);
    grid.create(row, col);
    for (int r = 0; r < row; r++)
    {
        for (int c = 0; c < col; c++)
        {
            float x = left + c * xdelta;
            float y = top - r * ydelta;
            // 使用两个值之间的最大值
            float value = -999999.0; // invalid data

            for (auto srg : srg_list)
            {
                int tx, ty;
                tx = std::round((x - srg.left) / srg.xdelta);
                ty = std::round((srg.top - y) / srg.ydelta);
                if (tx >= 0 && tx < srg.col && ty >= 0 && ty < srg.row)
                {
                    value = SU_MAX(value, srg.grid.at(ty, tx));
                }
            }
            grid.at(r, c) = value;
        }
    }
    return;
}

bool silly_radar_grid::read(const std::string& path)
{
    bool status = false;
    char* buff = nullptr;
#ifndef NDEBUG
    std::cout << "Read GRID : " << path << std::endl;
#endif
    std::ifstream inputFileStream(path, std::ios::binary | std::ios::ate);
    if (!inputFileStream.is_open())
    {
        std::cout << "Error opening file: " << path << std::endl;
        return false;
    }

    size_t fileSize = static_cast<size_t>(inputFileStream.tellg());
    inputFileStream.seekg(0, std::ios::beg);

    if(fileSize)
    {
        buff = new char[fileSize];
        inputFileStream.read(buff, fileSize);
        status = unserialize(buff, fileSize);
        delete [] buff;
    }
    inputFileStream.close();
    // SILLY_RADAR_GRID_FREE(buff)
    return status;
}
bool silly_radar_grid::save(const std::string& path)
{
    bool status = false;
    char* buff = nullptr;
    size_t len = 0;
    if (serialize(&buff, len))
    {
        FILE* pf = fopen(path.c_str(), "wb");
        if (pf)
        {
            fwrite(buff, 1, len, pf);
            fclose(pf);
            status = true;
        }

        //status = FileUtils::WriteAll(path.c_str(), buff, len);
    }
    SILLY_RADAR_GRID_FREE(buff)
    return status;
}
