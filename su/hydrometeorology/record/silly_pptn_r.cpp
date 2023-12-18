//
// Created by dell on 2023/12/15.
//

#include "silly_pptn_r.h"
#include <cstring>
#include <hydrometeorology/silly_record_tools.h>


bool silly_pptn::serialize(unsigned char** c_in, size_t& len)
{
	char buff[SILLY_PPTN_R_LENGHT] = { 0 };
	buff[0] = SILLY_PPTN_R_BEG_CHAR;
	char stcd_buff[SIILY_STCD_LENGHT] = { 0 };
	sprintf(stcd_buff, "%s\0", stcd.c_str());
	memcpy(buff + 1, stcd_buff, SIILY_STCD_LENGHT);
	unsigned int ymd = 0, hms = 0;
	if (!silly_record_tools::compress_time_format(tm, ymd, hms))
	{
		return false;
	}

	memcpy(buff + 1 + SIILY_STCD_LENGHT, &ymd, sizeof(ymd));
	memcpy(buff + 1 + SIILY_STCD_LENGHT + sizeof(ymd), &hms, sizeof(hms));
	memcpy(buff + 1 + SIILY_STCD_LENGHT + sizeof(ymd) + sizeof(hms), &drp, sizeof(drp));

	*c_in = (unsigned char*)malloc(SILLY_PPTN_R_LENGHT);
	if (!c_in)
	{
		return false;
	}
	memcpy(*c_in, buff, SILLY_PPTN_R_LENGHT);
	len = SILLY_PPTN_R_LENGHT;
	return true;
}

bool silly_pptn::unserialize(const unsigned char* c_in, const size_t& len)
{
	if (!c_in || len != SILLY_PPTN_R_LENGHT || c_in[0] != SILLY_PPTN_R_BEG_CHAR)
	{
		return false;
	}
	char* p = (char*)c_in;
	stcd.resize(SIILY_STCD_LENGHT);
	memcpy(&(stcd[0]), p + 1, SIILY_STCD_LENGHT);
	unsigned int ymd = 0, hms = 0;

	memcpy(&ymd, p + 1 + SIILY_STCD_LENGHT, sizeof(ymd));
	memcpy(&hms, p + 1 + SIILY_STCD_LENGHT + sizeof(ymd), sizeof(hms));
	memcpy(&drp, p + 1 + SIILY_STCD_LENGHT + sizeof(ymd) + sizeof(hms), sizeof(drp));
	silly_record_tools::decompress_time_format(ymd, hms, tm);

	return true;
}
