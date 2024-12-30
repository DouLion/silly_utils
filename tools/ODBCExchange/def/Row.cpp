/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-27
 * @file: Row.cpp
 * @description: Row实现
 * @version: v1.0.1 2024-12-27 dou li yang
 */
#include "Row.h"
#include <encode/silly_crc.h>
X::Row::Row()
{
    clear();
}
X::Row::Row(const size_t& size)
{
    items.resize(size);
}
void X::Row::clear()
{
    items.clear();
}
void X::Row::resize(const size_t& size)
{
    items.resize(size);
}
bool X::Row::FromBin(const std::string& bin)
{
    uint32_t crc = 0;
    memcpy(&crc, bin.data(), sizeof(crc));
    size_t offset = sizeof(crc) + sizeof(uint32_t) * items.size();
    char* p = (char*)(bin.data() + offset);
    if (crc != silly::crc::crc32(p, bin.size() - offset))
    {
        return false;
    }
    offset = sizeof(crc);
    std::vector<uint32_t> vlen(items.size());
    for (size_t i = 0; i < items.size(); ++i)
    {
        memcpy(&vlen[i], p + offset, sizeof(vlen[i]));
        offset += sizeof(vlen[i]);
    }
    offset = sizeof(crc) + sizeof(uint32_t) * items.size();
    for (size_t i = 0; i < vlen.size(); ++i)
    {
        if (items[i].FromBin(std::string(p + offset, vlen[i])))
        {
            offset += vlen[i];
        }
        else
        {
            return false;
        }
    }

    return true;
}
std::string X::Row::AsBin() const
{
    // crc32 item1 长度 item2 长度 item3长度  ... 各列数据..
    std::vector<uint32_t> vlen(items.size());
    std::string bin;
    for (size_t i = 0; i < items.size(); ++i)
    {
        std::string tmp = items[i].AsBin();
        vlen[i] = tmp.size();
        bin.append(tmp);
    }
    uint32_t crc = silly::crc::crc32(bin);
    size_t offset = sizeof(crc) + sizeof(uint32_t) * items.size();
    std::string head(0, offset);

    memcpy(&head[0], &crc, sizeof(crc));
    offset = sizeof(crc);
    for (size_t i = 0; i < items.size(); ++i)
    {
        memcpy(&head[offset], &vlen[i], sizeof(vlen[i]));
        offset += sizeof(vlen[i]);
    }

    return head + bin;
}
