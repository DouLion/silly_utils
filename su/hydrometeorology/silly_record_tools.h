/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/18 14:25
 * @version: 1.0.1
 * @description:
 */
#include <su_marco.h>

#ifndef SILLY_UTILS_SILLY_RECORD_TOOLS_H
#define SILLY_UTILS_SILLY_RECORD_TOOLS_H

class silly_record_tools
{
public:
	/// <summary>
	/// 将时间内容压缩的方式, 年月日 压缩为一个unsigned int表示, 时分秒压缩为1个unsigned int表示
	/// 年用一个ushort表示取值范围 0 ~ 65535到65535年, 月日都用一个unsigned char 表示, 拼接为一个unsigned int, 以此类推时分秒
	/// </summary>
	/// <param name="time">时间内容, 格式为 年-月-日 时:分:秒</param>
	/// <param name="ymd"></param>
	/// <param name="hms"></param>
	/// <returns></returns>
	static bool compress_time_format(const std::string& time, unsigned int& ymd, unsigned int& hms);

	static bool compress_time_format(const std::string& time, unsigned int& ymdhm);
	

	/// <summary>
	/// 解压时间内容压缩方式, 与 compress_time_format 互逆
	/// </summary>
	/// <param name="ymd"></param>
	/// <param name="hms"></param>
	/// <param name="time"></param>
	/// <returns></returns>
	static bool decompress_time_format(unsigned int& ymd, unsigned int& hms, std::string& time);

	/// <summary>
	/// 常见的水文数据都有一个合适的取值范围
	/// 1111 1111 1111 1 [111]留3位表时小数位 保存2位小数, 整数值不超过4096
	/// </summary>
	/// <param name="src"></param>
	/// <param name="dst"></param>
	/// <returns></returns>
	static bool compress_float_format(const float& src, short& dst);

	/// <summary>
	/// 与compress_float_format 互逆
	/// </summary>
	/// <param name="src"></param>
	/// <param name="dst"></param>
	/// <returns></returns>
	static bool decompress_float_format(const short& src, float& dst);

	

};


#endif //SILLY_UTILS_SILLY_RECORD_TOOLS_H
