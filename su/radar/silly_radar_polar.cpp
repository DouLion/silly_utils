//
// Created by dell on 2024/1/25.
//

#include "silly_radar_polar.h"
#include <files/silly_file.h>

#define TZX_RADAR_POLAR_MEMCPY_TYPE(buff, val) memcpy(buff, &val, sizeof(val)); buff += sizeof(val);

bool silly_radar_polar::serialize(char** buff, size_t& len)
{
    bool status = false;
    total = sizeof(total) + sizeof(r_size) + sizeof(az_size) + sizeof(meters) + sizeof(az_start) + sizeof(center_x) + sizeof(center_y) + sizeof(name) + sizeof(units) + sizeof(float) * az_size * r_size;
    *buff = new char[total];
    char* p = *buff;
    TZX_RADAR_POLAR_MEMCPY_TYPE(p, total);
    TZX_RADAR_POLAR_MEMCPY_TYPE(p, r_size);
    TZX_RADAR_POLAR_MEMCPY_TYPE(p, az_size);
    TZX_RADAR_POLAR_MEMCPY_TYPE(p, meters);
    TZX_RADAR_POLAR_MEMCPY_TYPE(p, az_start);
    TZX_RADAR_POLAR_MEMCPY_TYPE(p, center_x);
    TZX_RADAR_POLAR_MEMCPY_TYPE(p, center_y);
    TZX_RADAR_POLAR_MEMCPY_TYPE(p, name);
    TZX_RADAR_POLAR_MEMCPY_TYPE(p, units);

    size_t i = 0;
    size_t all = az_size * r_size;
    while (i < all)
    {
        float val = data[i++];
        TZX_RADAR_POLAR_MEMCPY_TYPE(p, val);
    }
    len = total;
    status = true;
    return status;

}

bool silly_radar_polar::unserialize(const char* buff, const size_t& len)
{
    bool status = false;
    char* p = (char*)buff;
    total = ((size_t*)p)[0]; p += sizeof(total);

    if (total != len) // 文件大小检查需要一致
    {
        return status;
    }

    r_size = ((size_t*)p)[0]; p += sizeof(total);
    az_size = ((size_t*)p)[0]; p += sizeof(az_size);
    meters = ((float*)p)[0]; p += sizeof(meters);
    az_start = ((float*)p)[0]; p += sizeof(az_start);
    center_x = ((float*)p)[0]; p += sizeof(center_x);
    center_y = ((float*)p)[0]; p += sizeof(center_y);
    memcpy(name, p, sizeof(name)); p += sizeof(name);
    memcpy(units, p, sizeof(units)); p += sizeof(units);

    size_t i = 0;
    size_t all = az_size * r_size;

    size_t check = (total - (p - buff)) / sizeof(float);
    if (all != check)	// 网格点数量不一致
    {
        return status;
    }

    data = new float[r_size * az_size];
    float* sp = (float*)p;
    while (i < all)
    {
        data[i] = sp[i];
        i++;
    }

    status = true;

    return status;
}

bool silly_radar_polar::read(const std::string& path)
{
    bool status = false;
    char* buff = nullptr;
    int len = 0;
    /*if (FileUtils::ReadAll(path.c_str(), &buff, len))
    {
        status = unserialize(buff, len);
    }*/
    if (buff)
    {
        free(buff);
    }
    return status;
}

bool silly_radar_polar::save(const std::string& path)
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

        // status = FileUtils::WriteAll(path.c_str(), buff, len);
    }
    if (buff)
    {
        delete buff;
    }
    return status;
}
