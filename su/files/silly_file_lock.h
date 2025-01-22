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
namespace silly
{
namespace file
{
class lock
{
  public:

    /// 构造函数 锁定文件
    lock(const std::string& u8file);

    void unlock();

  private:
    std::string m_err;
    silly_handle m_file_hdl = INVALID_HANDLE_VALUE;
#if WIN32

    // 获取文件大小
    LARGE_INTEGER fileSize;
    // 设置锁定参数
    OVERLAPPED overlapped = {0};
#endif
};
}
}


#endif  // SILLY_UTILS_SILLY_FILE_LOCK_H
