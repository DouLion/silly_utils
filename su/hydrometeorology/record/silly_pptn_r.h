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
#define SILLY_PPTN_STCD_SPLITER       '\0'
#define SILLY_PPTN_RECORD_BEG         'P'
#define SILLY_PPTN_RECORD_BEG         'N'
/*

    unsigned int ymd = ((((0x00 | year) << 8) | month) << 8) | day;
    unsigned int hmc = ((((0x00 | hour) << 8) | minute) << 8) | second;
*/
class silly_pptn
{
public:
    // 主要内容
    std::string stcd;
    // 长度2个int 8字节
    std::string tm;
    // short 2个字节
    float drp;
    // 这个一般都可以忽略, 不要了
    float intv;
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
