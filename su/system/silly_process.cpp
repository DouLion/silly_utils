/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-03
 * @file: silly_process.cpp
 * @description: silly_process实现
 * @version: v1.0.1 2024-12-03 dou li yang
 */
#include "silly_process.h"

double silly_process::memory_usage_kbyte(const int& pid)
{
    double result = 0.0;
#if WIN32
    bool close = false;
    HANDLE hProcess = nullptr;
    if (pid > 0)
    {
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        close = true;
    }
    else
    {
        hProcess = GetCurrentProcess();
    }
    if (hProcess != NULL)
    {
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
        {
            result = pmc.WorkingSetSize / 1024.0 / 1024.0;
        }
    }
    if (close)
    {
        CloseHandle(hProcess);
        hProcess = nullptr;
    }
#else

#endif
    return result;
}
void silly_process::clear_working_set(const int& pid, const int& seconds)
{
#if WIN32
    auto clear_func = [pid, seconds]() {
        while (1)
        {
            bool close = false;
            HANDLE hProcess = nullptr;
            if (pid > 0)
            {
                hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
                close = true;
            }
            else
            {
                hProcess = GetCurrentProcess();
            }
            if (hProcess != NULL)
            {
                // 创建一个与工作集大小相同的内存页
                SIZE_T minWorkingSetSize = 0;  // 下限设为0表示没有限制
                SIZE_T maxWorkingSetSize = 0;  // 上限设为0表示没有限制
                // 尝试设置工作集大小
                SetProcessWorkingSetSize(hProcess, minWorkingSetSize, maxWorkingSetSize);
                // 尝试清理工作集
                EmptyWorkingSet(hProcess);

#ifndef NDEBUG
                std::cout << "清空工作集" << std::endl;
                std::cout << "进程" << pid << "内存占用: " << memory_usage_kbyte(pid) << " kb" << std::endl;
#endif
            }
            if (close)
            {
                CloseHandle(hProcess);
                hProcess = nullptr;
            }

            std::this_thread::sleep_for(std::chrono::seconds(seconds));
        }
    };

    std::thread(clear_func).detach();

#endif
}
