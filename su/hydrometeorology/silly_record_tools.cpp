#include "silly_record_tools.h"
#include "su_marco.h"
bool silly_record_tools::compress_time_format(const std::string& time, unsigned int& ymd, unsigned int& hms)
{
	int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
	if (EOF == sscanf(time.c_str(), SU_STD_TIME_FORMAT1, &year, &month, &day, &hour, &minute, &second))
	{
		SU_ERROR_PRINT("INVALID TIME FORMAT: %s", time.c_str());
		return false;
	}
	ymd = ((((0x00 | year) << 8) | month) << 8) | day;
	hms = ((((0x00 | hour) << 8) | minute) << 8) | second;
	return true;
}

bool silly_record_tools::decompress_time_format(unsigned int& ymd, unsigned int& hms, std::string& time)
{
	int year = (ymd) >> 16;
	int month = (ymd & 0xFF00) >> 8;
	int day = (ymd & 0xFF);
	int hour = (hms) >> 16;
	int minute = (hms & 0xFF00) >> 8;
	int second = (hms & 0xFF);
	char buff[128] = { 0 };
	sprintf(buff, SU_STD_TIME_FORMAT1, year, month, day, hour, minute, second);
	time = buff;
	return true;
}

bool silly_record_tools::compress_float_format(const float& src, short& dst)
{
	int max = 1 <<  12;
	dst = 0x0000;
	int d = std::round((src - (int)src) * 100);
	int a = (int)src;
	if (a > max)
	{
		SU_ERROR_PRINT("LARGE THAN 4096: %.2f", src);
		return false;
	}
	dst = (a << 3);
	dst = (dst | d);
	if (src > 0)
	{
		dst = (dst | 0x8000);
	}

	return true;
}

bool silly_record_tools::decompress_float_format(const short& src, float& dst)
{
	int a = src;
	a = ((a&0x7F) >> 3);
	int s = ((a & 0x8000));
	int d = src;
	d = (d << 15) >> 15;
	dst = a + d * 100.;
	if (!s)
	{
		dst = -1.;
	}
	return true;
}
