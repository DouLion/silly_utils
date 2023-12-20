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
	/// ��ʱ������ѹ���ķ�ʽ, ������ ѹ��Ϊһ��unsigned int��ʾ, ʱ����ѹ��Ϊ1��unsigned int��ʾ
	/// ����һ��ushort��ʾȡֵ��Χ 0 ~ 65535��65535��, ���ն���һ��unsigned char ��ʾ, ƴ��Ϊһ��unsigned int, �Դ�����ʱ����
	/// </summary>
	/// <param name="time">ʱ������, ��ʽΪ ��-��-�� ʱ:��:��</param>
	/// <param name="ymd"></param>
	/// <param name="hms"></param>
	/// <returns></returns>
	static bool compress_time_format(const std::string& time, unsigned int& ymd, unsigned int& hms);

	static bool compress_time_format(const std::string& time, unsigned int& ymdhm);
	

	/// <summary>
	/// ��ѹʱ������ѹ����ʽ, �� compress_time_format ����
	/// </summary>
	/// <param name="ymd"></param>
	/// <param name="hms"></param>
	/// <param name="time"></param>
	/// <returns></returns>
	static bool decompress_time_format(unsigned int& ymd, unsigned int& hms, std::string& time);

	/// <summary>
	/// ������ˮ�����ݶ���һ�����ʵ�ȡֵ��Χ
	/// 1111 1111 1111 1 [111]��3λ��ʱС��λ ����2λС��, ����ֵ������4096
	/// </summary>
	/// <param name="src"></param>
	/// <param name="dst"></param>
	/// <returns></returns>
	static bool compress_float_format(const float& src, short& dst);

	/// <summary>
	/// ��compress_float_format ����
	/// </summary>
	/// <param name="src"></param>
	/// <param name="dst"></param>
	/// <returns></returns>
	static bool decompress_float_format(const short& src, float& dst);

	

};


#endif //SILLY_UTILS_SILLY_RECORD_TOOLS_H
