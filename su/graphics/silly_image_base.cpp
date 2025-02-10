/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-01-02
 * @file: silly_image_base.cpp
 * @description: silly_image_base实现
 * @version: v1.0.1 2025-01-02 dou li yang
 */
#include "silly_image_base.h"

bool silly::image::base::valid(const std::string& bin)
{
    return valid(bin.c_str(), bin.size());
}

bool silly::image::base::valid(const char* data, const size_t len)
{
    if (len < HEADER.size())
    {
        return false;
    }
    for (size_t i = 0; i < HEADER.size(); ++i)
    {
        if (static_cast<uint8_t>(data[i]) != HEADER[i])
        {
            return false;
        }
    }
    return true;
}

bool silly::image::base::empty()
{
    return !m_bytes || !m_height || !m_width;
}

void silly::image::base::release()
{
    SU_MEM_DEL(m_bytes)
 /*   for(int i = 0; i < m_height; ++i)
    {
        SU_MEM_DEL(m_nbytes[i])
    }*/
    SU_MEM_DEL(m_nbytes)
}
