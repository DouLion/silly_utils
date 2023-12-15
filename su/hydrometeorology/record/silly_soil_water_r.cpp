//
// Created by dell on 2023/12/15.
//

#include "silly_soil_water_r.h"
#include "su_marco.h"
#include <cstring>
//unsigned int pid{ 0 };
//std::string tm;
//float sw{ 0 };
//float percent{ 0 };
//float drp{ 0 };
#define SILLY_SOIL_WATER_BEG_CHAR   '_'

#define SILLY_SOIL_WATER_LENGTH  sizeof(char*) + sizeof(unsigned int) * 3 + sizeof(short) * 3

bool silly_soil_water::serialize(unsigned char** c_in, size_t& len)
{
	unsigned char buff[SILLY_SOIL_WATER_LENGTH] = { 0 };
	buff[0] = SILLY_SOIL_WATER_BEG_CHAR;
	// GRID
	memcpy(buff + 1, &pid, sizeof(pid));
	// TM
	int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
	if (EOF == sscanf(tm.c_str(), SU_STD_TIME_FORMAT1, &year, &month, &day, &hour, &minute, &second))
	{
		return false;
	}
	unsigned int ymd = ((((0x00 | year) << 8) | month) << 8) | day;
	unsigned int hms = ((((0x00 | hour) << 8) | minute) << 8) | second;
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
