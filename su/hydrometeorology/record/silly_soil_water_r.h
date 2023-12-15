/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/15 11:34
 * @version: 1.0.1
 * @description: 土壤含水量记录
 */
#ifndef SILLY_UTILS_SILLY_SOIL_WATER_R_H
#define SILLY_UTILS_SILLY_SOIL_WATER_R_H
#include <iostream>

class silly_soil_water
{
public:
    unsigned int pid{0};
	std::string tm;
    float sw{0};
    float percent{0};
    float drp{0};

    /// <summary>
    /// 将对象序列化位二进制数据  
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

#endif //SILLY_UTILS_SILLY_SOIL_WATER_R_H
