/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/15 11:33
 * @version: 1.0.1
 * @description: 测站降雨记录
 */
#ifndef SILLY_UTILS_SILLY_PPTN_R_H
#define SILLY_UTILS_SILLY_PPTN_R_H
#include <iostream>

// 无效的降雨值, 用于替换数据库中记录的NULL
#define SILLY_PPTN_INVALID_DRP_R      9999

#define SIILY_STCD_LENGHT  10
#define SILLY_PPTN_R_LENGHT sizeof(char)+SIILY_STCD_LENGHT + sizeof(unsigned int) * 2 + sizeof(float)

#define SILLY_PPTN_R_BEG_CHAR   '*'

/// 用法:
/*
int example() {
	std::string time = "2023-01-01 12:00:00";
	silly_pptn sp;
	sp.drp = 12.3;
	sp.stcd = "611H3000";
	sp.tm = "2023-08-26 12:10:06";

	unsigned char* buff = nullptr;
	size_t len = 0;
	sp.serialize(&buff, len);

	silly_pptn sp2;
	sp2.unserialize(buff, len);
	SU_MEM_FREE(buff);
	return 0;
}
*/

class silly_pptn
{
public:
	// 主要内容
	std::string stcd;
	// 长度2个int 8字节
	std::string tm;
	// short 2个字节
	float drp{ 0 };
	// 这里往下都不要了
	// 这个一般都可以忽略, 不要了
	float intv{ 0 };
	// 次要内容

	/// <summary>
	/// 将对象序列化位二进制数据  stcd\0 tm drp ; 
	/// tm 用一个int表示年月日, 一个int表示时分秒
	/// 降雨数值 乘以10转为short, SILLY_PPTN_INVALID_DRP_R表示
	/// </summary>
	/// <param name="c_in">序列化后的二进制数据</param>
	/// <param name="len">序列化后的二进制数据长度</param>
	/// <returns></returns>
	bool serialize(unsigned char** c_in, size_t& len);

	/// <summary>
	/// 将二进制数据反序列化位该对象
	/// </summary>
	/// <param name="c_in"></param>
	/// <param name="len"></param>
	/// <returns></returns>
	bool unserialize(const unsigned char* c_in, const size_t& len);

};

#endif //SILLY_UTILS_SILLY_PPTN_R_H
