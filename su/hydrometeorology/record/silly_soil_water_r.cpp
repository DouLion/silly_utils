//
// Created by dell on 2023/12/15.
//

#include "silly_soil_water_r.h"
#include "hydrometeorology/silly_record_tools.h"
#include <cstring>

bool silly_soil_water::serialize(unsigned char** c_in, size_t& len)
{
    unsigned char buff[SILLY_SOIL_WATER_LENGTH] = {0};
    buff[0] = SILLY_SOIL_WATER_BEG_CHAR;
    // GRID
    memcpy(buff + 1, &pid, sizeof(pid));
    // TM
    unsigned int ymd = 0, hms = 0;
    if (!silly_record_tools::compress_time_format(tm, ymd, hms))
    {
        return false;
    }
    memcpy(buff + 1 + sizeof(unsigned int), &ymd, sizeof(ymd));
    memcpy(buff + 1 + sizeof(unsigned int) * 2, &hms, sizeof(hms));
    //
    short s_sw = sw * 10;
    short s_swp = percent * 10;
    short s_drp = drp * 10;
    memcpy(buff + 1 + sizeof(unsigned int) * 3, &s_sw, sizeof(s_sw));
    memcpy(buff + 1 + sizeof(unsigned int) * 3 + sizeof(short), &s_swp, sizeof(s_swp));
    memcpy(buff + 1 + sizeof(unsigned int) * 3 + sizeof(short) * 2, &s_drp, sizeof(s_drp));
    *c_in = (unsigned char*)malloc(SILLY_SOIL_WATER_LENGTH);
    if (!c_in)
    {
        return false;
    }
    memcpy(*c_in, buff, SILLY_SOIL_WATER_LENGTH);
    return true;
}

bool silly_soil_water::unserialize(const unsigned char* c_in, const size_t& len)
{
    if (len != SILLY_SOIL_WATER_LENGTH)
    {
        return false;
    }

    return false;
}
