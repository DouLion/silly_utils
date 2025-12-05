//
// Created by dell on 2023/12/27.
//

#include "silly_micaps.h"
#include <files/silly_file.h>

#define TIME_FORMAT "%04d-%02d-%02d %02d:%02d:%02d"
#define MICAPS_DIAMOND_4_FORMAT "diamond 4"

#define MICAPS_SKIP_BLANK(s, off, idx)                           \
    off = idx;                                                   \
    while (s[off] == ' ' || s[off] == '\r' || s[off] == '\n')    \
    {                                                            \
        off++;                                                   \
    }                                                            \
    idx = off;                                                   \
    while (!(s[idx] == ' ' || s[idx] == '\r' || s[idx] == '\n')) \
    {                                                            \
        i++;                                                     \
    }

bool silly_micaps_diamond_4::check_head(const std::string &content, size_t &offset)
{
    bool status = false;
    offset = strlen(MICAPS_DIAMOND_4_FORMAT);
    format = content.substr(0, offset);
    if (MICAPS_DIAMOND_4_FORMAT != format)
    {
        return status;
    }
    size_t i = offset;
    MICAPS_SKIP_BLANK(content, offset, i);
    desc = content.substr(offset, i - offset + 1);
    MICAPS_SKIP_BLANK(content, offset, i);
    int year = std::stoi(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    int month = std::stoi(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    int day = std::stoi(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    int hour = std::stoi(content.substr(offset, i - offset + 1));
    char time_buff[32] = {0};
    sprintf(time_buff, TIME_FORMAT, year, month, day, hour, 0, 0);
    base_tm = time_buff;
    MICAPS_SKIP_BLANK(content, offset, i);
    aging = std::stoi(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    layer = std::stoi(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    lng_step = std::stof(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    lat_step = std::stof(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    left = std::stof(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    right = std::stof(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    bottom = std::stof(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    top = std::stof(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    lng_size = std::stof(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    lat_size = std::stof(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    iso_step = std::stof(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    iso_bv = std::stof(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    iso_ev = std::stof(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    smooth = std::stof(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    bold = std::stof(content.substr(offset, i - offset + 1));
    MICAPS_SKIP_BLANK(content, offset, i);
    status = true;
    return status;
}

bool silly_micaps_diamond_4::is_valid()
{
    bool status = false;
    //
    if (!(data.row() == lat_size && data.col() == lng_size))
    {
        return status;
    }
    // TODO: 补充其他 经纬度 经纬步长 等等验证

    status = true;
    return status;
}

bool silly_micaps_diamond_4::reset()
{
    // 其他数据清空
    lng_size = 0;
    lat_size = 0;
    data.release();
    return false;
}

bool silly_micaps_utils::read(const suPath &file, silly_micaps_diamond_4 &md4)
{
    bool status = false;
    std::string content;
    if (!sufile::read(file, content))
    {
        return status;
    }
    size_t total_size = content.size();
    size_t offset = 0;
    // 检查文件头
    if (!md4.check_head(content, offset))
    {
        return status;
    }

    char _dot = '.' - 1;
    std::vector<float> tmp;
    while (offset < total_size)
    {
        if (content[offset] > _dot)
        {
            size_t i = 0;
            char buff[32] = {0};
            while (content[offset] > _dot)
            {
                buff[i] = content[offset];
                i++;
                offset++;
            }
            tmp.push_back(std::stof(buff));
        }
        offset++;
    }
    md4.data.create(md4.lat_size, md4.lng_size);
    for (int r = 0; r < md4.lat_size; r++)
    {
        memcpy(md4.data.data() + (r * md4.lng_size), tmp.data() + ((md4.lat_size - r - 1) * md4.lng_size), md4.lng_size * sizeof(float));
    }

    status = md4.is_valid();
    if (!status)
    {
        md4.reset();
    }
    return status;
}

bool silly_micaps_utils::mmap_read(const suPath &file, silly_micaps_diamond_4 &md4)
{
    return false;
}

bool silly_micaps_utils::write(const suPath &file, const silly_micaps_diamond_4 &md4)
{
    return false;
}
