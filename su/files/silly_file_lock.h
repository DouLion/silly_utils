/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-24
 * @file: silly_file_lock.h
 * @description: silly_file_lock 类声明
 * @version: v1.0.1 2024-09-24 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_FILE_LOCK_H
#define SILLY_UTILS_SILLY_FILE_LOCK_H
#include <su_marco.h>

class silly_file_lock
{
  public:
    bool lock(const std::string& u8file);

    void unlock();

    std::string err();

  private:
    std::string m_err;
#if WIN32
    HANDLE hFile = nullptr;
#endif
};

#endif  // SILLY_UTILS_SILLY_FILE_LOCK_H
