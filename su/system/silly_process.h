/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-03
 * @file: silly_process.h
 * @description: silly_process 类声明
 * @version: v1.0.1 2024-12-03 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_PROCESS_H
#define SILLY_UTILS_SILLY_PROCESS_H
#include <su_marco.h>
class silly_process
{
  public:
    /// <summary>
    /// 定时清理windows的内存页,控制其占用内存
    /// </summary>
    /// <param name="seconds">每隔 xx秒 清理一次</param>
    /// <returns></returns>
    static void clear_working_set(const int& pid = 0, const int& seconds = 3600);

    /// <summary>
    /// 获取指定进程的内存使用情况, 如果pid为0,则获取自身进程的内存使用情况
    /// </summary>
    /// <param name="seconds">每隔 xx秒 清理一次</param>
    /// <returns></returns>
    static double memory_usage_kbyte(const int& pid = 0);
};

#endif  // SILLY_UTILS_SILLY_PROCESS_H
