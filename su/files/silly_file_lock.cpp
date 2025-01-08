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

silly::file::lock::lock(const std::string& u8file)
{
#if WIN32
    hFile = CreateFile(std::filesystem::path(u8file).wstring().c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        m_err = GetLastError();
        throw std::runtime_error(m_err.c_str());
    }
    // 获取文件大小
    if (!GetFileSizeEx(hFile, &fileSize))
    {
        m_err = GetLastError();
        CloseHandle(hFile);
        throw std::runtime_error(m_err.c_str());
    }

    DWORD lockFlags = LOCKFILE_EXCLUSIVE_LOCK;  // 独占锁
    lockFlags |= LOCKFILE_FAIL_IMMEDIATELY;     // 不阻塞等待锁

    // 锁定整个文件
    BOOL result = LockFileEx(hFile, lockFlags, 0, fileSize.LowPart, fileSize.HighPart, &overlapped);
    if (!result)
    {
        m_err = GetLastError();
        CloseHandle(hFile);
        throw std::runtime_error(m_err.c_str());
        ;
    }
#else

    fd = open(u8file.c_str(), O_RDWR);
    if (fd == -1)
    {
        perror("打开文件失败");
        return 1;
    }

    // 定义锁结构体
    struct flock _flock;
    lock.l_type = F_WRLCK;  // 写锁 (F_RDLCK 为读锁)
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;  // 从文件开头开始
    lock.l_len = 0;    // 锁定整个文件 (0 表示直到文件末尾)

    // 尝试锁定文件
    int result = fcntl(fd, F_SETLK, &_flock);
    if (result == -1)
    {
        close(fd);
        if (errno == EACCES || errno == EAGAIN)
        {
            throw std::runtime_error("文件已被其他进程锁定");
        }
        else
        {
            throw std::runtime_error("锁定文件失败");
        }
    }
#endif
}
void silly::file::lock::unlock()
{
#if WIN32
    if (hFile != INVALID_HANDLE_VALUE)
    {
        if (!UnlockFileEx(hFile, 0, fileSize.LowPart, fileSize.HighPart, &overlapped))
        {
            m_err = GetLastError();
        }

        CloseHandle(hFile);
    }
#else
    // 解锁文件
    result = flock(fd, LOCK_UN);  // 解锁
    if (result == -1) {
        throw std::runtime_error("解锁文件失败");
    }

    // 关闭文件
    close(fd);
#endif
}
