/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-11
 * @file: silly_schedule_index.cpp
 * @description: silly_schedule_index 头文件
 * @version: v1.0.1 2025-12-11 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_SCHEDULE_INDEX_H
#define SILLY_UTILS_SILLY_SCHEDULE_INDEX_H
#include <files/silly_file.h>
#include <log/silly_log.h>

class suScheduleFile;
class suScheduleIndex
{
    friend class suScheduleFile;
    /*
    #pragma pack(push, 1)
        struct INFO
        {
            char code[32] = {};
            size_t sort = 0;
        };
    #pragma pack(pop)
    */
protected:
    suScheduleIndex() = default;
    bool open(const suPath& file);
    bool write(const suPath& file) const;
    void add(const std::string& code);
    bool contains(const std::string& code) const;
    size_t sort(const std::string& code) const;
    size_t size() const;

    // private:
    std::unordered_map<std::string, size_t> m_code2sort;
    bool m_added = false;
    suPath m_file;
};
#endif  // SILLY_UTILS_SILLY_SCHEDULE_INDEX_H
