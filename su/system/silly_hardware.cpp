/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-25
 * @file: silly_hardware
 * @description: silly_hardware实现
 * @version: v1.0.1 2025-12-25 dou li yang
 */
#include "silly_hardware.h"
#ifdef _WIN32
#include <iphlpapi.h>
#include <iomanip>
#include <comdef.h>
#include <Wbemidl.h>
#include <winioctl.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

#include <system/silly_system.h>
std::map<std::string, std::string> GetAllMacAddresses()
{
    std::map<std::string, std::string> macs;

    // 获取适配器信息
    ULONG size = sizeof(IP_ADAPTER_INFO);
    PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO*)malloc(size);

    if (GetAdaptersInfo(pAdapterInfo, &size) == ERROR_BUFFER_OVERFLOW)
    {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(size);
    }

    if (GetAdaptersInfo(pAdapterInfo, &size) != NO_ERROR)
    {
        free(pAdapterInfo);
        return macs;
    }

    PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
    while (pAdapter)
    {
        // 只考虑物理地址长度为6字节的以太网类型
        if (pAdapter->AddressLength == 6)
        {
            // 跳过回环和明显是虚拟的设备（可扩展）
            std::string desc(pAdapter->Description);
            std::transform(desc.begin(), desc.end(), desc.begin(), ::tolower);
            bool isVirtual = desc.find("virtual") != std::string::npos;
            isVirtual = isVirtual || desc.find("vmware") != std::string::npos;
            isVirtual = isVirtual || desc.find("vbox") != std::string::npos;
            isVirtual = isVirtual || desc.find("hyper-v") != std::string::npos;
            isVirtual = isVirtual || desc.find("tap") != std::string::npos;
            isVirtual = isVirtual || desc.find("docker") != std::string::npos;
            isVirtual = isVirtual || desc.find("sangfor") != std::string::npos;
            isVirtual = isVirtual || desc.find("tap-windows") != std::string::npos;
            isVirtual = isVirtual || desc.find("loopback") != std::string::npos;

            // 跳过未连接的适配器（可选）
            bool isConnected = (pAdapter->Type == MIB_IF_TYPE_ETHERNET || pAdapter->Type == IF_TYPE_IEEE80211) && pAdapter->IpAddressList.IpAddress.String[0] != '\0';

            if (!isVirtual && isConnected)
            {
                std::stringstream ss;
                for (DWORD i = 0; i < pAdapter->AddressLength; ++i)
                {
                    ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(pAdapter->Address[i]);
                    if (i != pAdapter->AddressLength - 1)
                        ss << ":";
                }
                macs[ss.str()] = pAdapter->Description;
            }
        }
        pAdapter = pAdapter->Next;
    }

    free(pAdapterInfo);

    return macs;
}
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ntdll.lib")

std::string GetPhysicalDriveSerial(int driveIndex) {
    std::string path = "\\\\.\\PhysicalDrive" + std::to_string(driveIndex);
    HANDLE hDevice = CreateFileA(
        path.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hDevice == INVALID_HANDLE_VALUE) {
        return "";
    }

    // 查询 STORAGE_DEVICE_DESCRIPTOR
    STORAGE_PROPERTY_QUERY query = {};
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;

    DWORD size = 0;
    // 先获取所需缓冲区大小
    if (!DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
                         &query, sizeof(query),
                         NULL, 0, &size, NULL) &&
        GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        CloseHandle(hDevice);
        return "";
        }

    std::vector<BYTE> buffer(size, 0);
    if (!DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
                         &query, sizeof(query),
                         buffer.data(), size,
                         &size, NULL)) {
        CloseHandle(hDevice);
        return "";
                         }

    PSTORAGE_DEVICE_DESCRIPTOR desc = (PSTORAGE_DEVICE_DESCRIPTOR)buffer.data();

    std::string serial;
    if (desc->SerialNumberOffset != 0) {
        char* raw = (char*)buffer.data() + desc->SerialNumberOffset;
        serial = std::string(raw);

        // 移除尾部空格和 null
        while (!serial.empty() && (serial.back() == ' ' || serial.back() == '\0')) {
            serial.pop_back();
        }
    }

    CloseHandle(hDevice);
    return serial;
}
#else
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
std::map<std::string, std::string> GetAllMacAddresses();
std::string GetMacAddress()
{
    DIR* dir = opendir("/sys/class/net/");
    if (!dir)
        return "";

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_type != DT_DIR || strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // 跳过虚拟接口（如 lo）
        if (std::string(entry->d_name).find("lo") == 0)
            continue;

        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0)
            continue;

        struct ifreq ifr;
        strncpy(ifr.ifr_name, entry->d_name, IFNAMSIZ - 1);

        if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0)
        {
            close(sock);
            unsigned char* mac = (unsigned char*)ifr.ifr_hwaddr.sa_data;
            std::stringstream ss;
            for (int i = 0; i < 6; ++i)
            {
                ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(mac[i]);
                if (i != 5)
                    ss << ":";
            }
            closedir(dir);
            return ss.str();
        }
        close(sock);
    }
    closedir(dir);
    return "";
}
#endif

std::map<std::string, std::string> suHardWare::MacInfo()
{
    return GetAllMacAddresses();
}
std::map<std::string, std::string> suHardWare::DiskSerial()
{
    std::map<std::string, std::string> ret;
#if _WIN32
    try
    {
        for (int i = 0; i < 256; ++i)
        {
            std::string serial = GetPhysicalDriveSerial(i);

            if (!serial.empty())
            {
                std::cout << "PhysicalDrive" << i << " (Storage API): " << serial << "\n";
            }

        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << "\n";
    }
#else
#endif
    return ret;
}