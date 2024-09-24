/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-24
 * @file: silly_file_lock.cpp
 * @description: silly_file_lock实现
 * @version: v1.0.1 2024-09-24 dou li yang
 */
#include "silly_file_lock.h"

bool silly_file_lock::lock(const std::string& u8file)
{
#if WIN32
    auto wsfile = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(u8file).c_str();
    hFile = CreateFile(wsfile, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        m_err = GetLastError();
        return false;
    }
    return true;
#else

    return false;
#endif
}
void silly_file_lock::unlock()
{
#if WIN32
    if (hFile)
    {
        OVERLAPPED overlapped = {0};
        overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        if (!UnlockFileEx(hFile, 0, MAXDWORD, MAXDWORD, &overlapped))
        {
            m_err = GetLastError();
        }

        CloseHandle(overlapped.hEvent);
    }
#else

#endif
}
std::string silly_file_lock::err()
{
    return m_err;
}
