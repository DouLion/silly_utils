/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/20 18:28
 * @version: 1.0.1
 * @description: 时间处理工具
 */
#ifndef SILLY_UTILS_SILLY_TIME_HPP
#define SILLY_UTILS_SILLY_TIME_HPP

#include <datetime/simple_time.h>
using namespace simple_time;

/// <summary>
/// 在这个类里面
/// 2023年1月1日 描述为 
/// </summary>
class silly_time
{
	typedef unsigned int istint;
public:
	/// <summary>
	/// 一年中的的第几天
	/// </summary>
	/// <param name="year"></param>
	/// <param name="month"></param>
	/// <param name="day"></param>
	/// <returns></returns>
	static istint days_of_year(istint year, istint month, istint day)
	{
		static istint year1[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
		static istint year2[12] = { 31,29,31,30,31,30,31,31,30,31,30,31 };
		istint days = 0;
		if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
			for (istint i = 0; i < month - 1; i++) {
				days += year2[i];
			}
		}
		else {
			for (istint i = 0; i < month - 1; i++) {
				days += year1[i];
			}
		}
		days += day;
		return days;
	}

	static istint hours_of_year(istint year, istint month, istint day, istint hour)
	{
		istint days = days_of_year(year, month, day);
		return (days - 1) * 24 + hour;
	}

	static istint minutes_of_year(istint year, istint month, istint day, istint hour, istint minute)
	{
		istint hours = hours_of_year(year, month, day, hour);
		return hours * 60 + minute;
	}

	static istint seconds_of_year(istint year, istint month, istint day, istint hour, istint minute, istint second)
	{
		istint minutes = minutes_of_year(year, month, day, hour, minute);
		return minutes * 60 + second;
	}
};

#endif //SILLY_UTILS_SILLY_TIME_HPP
