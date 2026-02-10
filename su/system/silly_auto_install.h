/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2026-02-10
 * @file: silly_auto_install.h
 * @description: 自动安装为系统服务,包含linux windows, docker
 * @version: v1.0.1 2026-02-10 dou li yang
 *
 */
#ifndef SILLY_AUTO_INSTALL_H
#define SILLY_AUTO_INSTALL_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <thread>
#include <chrono>

// --- 平台差异化头文件 ---
#if defined(_WIN32)
#include <windows.h>
#include <winsvc.h>
#include <tchar.h>
#include <process.h>
#pragma comment(lib, "advapi32.lib")
#define PATH_SEPARATOR "\\"
#else
#include <unistd.h>
#include <limits.h>
#define PATH_SEPARATOR "/"
#endif

// 确保 C++17
#if __cplusplus < 201703L
#error "This code requires C++17 or later."
#endif

namespace fs17 = std::filesystem;

class suAutoInstall
{
  public:
    suAutoInstall()
    {
        // Windows 下需要初始化静态成员以便回调函数访问
        if (m_instance == nullptr)
            m_instance = this;
    }

    // --- 主入口处理函数 ---
    void Process(int argc, char** argv)
    {
        // 初始化基础信息
        if (!BasicInit(argv[0]))
            return;

        // 1. 如果没有参数，进入正常运行模式
        if (argc < 2)
        {
            RunAsServiceHook();  // Windows 特有逻辑
            return;
        }

        std::string cmd = argv[1];
        if (cmd != "install" && cmd != "uninstall")
        {
            RunAsServiceHook();  // 即使带了其他参数，也可能是业务参数，尝试挂钩服务
            return;
        }

        // 2. 解析安装/卸载参数
        ParseArgs(argc, argv);
        if (m_name.empty())
        {
            ShowUsage(argv[0]);
            exit(-1);
        }

        // 安全检查：分身不能执行安装
        if (m_exe.find(CLONE_SUFFIX) != std::string::npos)
        {
            std::cerr << "[错误] 服务运行副本无法执行安装/卸载操作。" << std::endl;
            std::cerr << "请运行原始可执行程序。" << std::endl;
            exit(-1);
        }

        // 3. 执行逻辑并退出
        bool result = false;
        if (cmd == "install")
            result = Install();
        else
            result = Uninstall();

        exit(result ? 0 : -1);
    }

  private:
    const std::string CLONE_SUFFIX = ".srv_run";
#if defined(_WIN32)
    const std::string EXE_EXT = ".exe";
#else
    const std::string EXE_EXT = "";
#endif

    std::string m_name;
    std::string m_exe;
    fs17::path m_current_dir;
    bool m_skip_confirm = false;
    std::unordered_map<std::string, std::string> m_EnvKV;

    // --- 静态单例指针 (Windows 回调需要) ---
    static suAutoInstall* m_instance;

    // --- 基础工具 ---
    void ShowUsage(const char* progName) const
    {
        std::cerr << "用法:" << std::endl;
        std::cerr << "  " << progName << " install   --NAME <服务名>" << std::endl;
        std::cerr << "  " << progName << " uninstall --NAME <服务名> [-y]" << std::endl;
    }

    static std::string Trim(const std::string& str)
    {
        const auto strBegin = str.find_first_not_of(" \t\r\n");
        if (strBegin == std::string::npos)
            return "";
        const auto strEnd = str.find_last_not_of(" \t\r\n");
        return str.substr(strBegin, strEnd - strBegin + 1);
    }

    fs17::path GetSelfExePath()
    {
#if defined(_WIN32)
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        return fs17::path(buffer);
#else
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        if (count != -1)
            return fs17::path(std::string(result, count));
        return "";
#endif
    }

    bool BasicInit(const char* argv0)
    {
        fs17::path selfPath = GetSelfExePath();
        if (selfPath.empty())
            selfPath = fs17::absolute(argv0);  // Fallback

        m_exe = selfPath.filename().string();
        m_current_dir = selfPath.parent_path();
        return true;
    }

    void ParseArgs(int argc, char** argv)
    {
        for (int i = 2; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg == "--NAME" && i + 1 < argc)
            {
                m_name = argv[i + 1];
                size_t extPos = m_name.find(".service");  // 兼容 Linux 写法
                if (extPos != std::string::npos)
                    m_name = m_name.substr(0, extPos);
            }
            else if (arg == "-y" || arg == "--yes")
            {
                m_skip_confirm = true;
            }
        }
    }

    bool CheckRoot()
    {
#if defined(_WIN32)
        BOOL fIsRunAsAdmin = FALSE;
        PSID pAdminSID = NULL;
        SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
        if (AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pAdminSID))
        {
            if (!CheckTokenMembership(NULL, pAdminSID, &fIsRunAsAdmin))
                fIsRunAsAdmin = FALSE;
            FreeSid(pAdminSID);
        }
        return fIsRunAsAdmin == TRUE;
#else
        return geteuid() == 0;
#endif
    }

    fs17::path EnvFile() const
    {
        fs17::path p = m_current_dir;
        p.append(m_name + ".env");
        return p;
    }

    bool LoadEnv()
    {
        fs17::path file = EnvFile();
        m_EnvKV.clear();

        if (!fs17::exists(file))
        {
            // 生成默认配置
            std::ofstream f(file);
            f << "# 服务环境变量配置 (Service Environment)\n";
#if defined(_WIN32)
            f << "# PATH=C:\\Windows\\System32;C:\\MyApp\\bin\n";
#else
            f << "# LD_LIBRARY_PATH=/usr/local/lib\n";
#endif
            f << "# DB_HOST=127.0.0.1\n";
        }

        std::ifstream f(file);
        std::string line;
        while (std::getline(f, line))
        {
            line = Trim(line);
            if (line.empty() || line[0] == '#')
                continue;
            size_t pos = line.find('=');
            if (pos != std::string::npos)
            {
                std::string k = Trim(line.substr(0, pos));
                std::string v = Trim(line.substr(pos + 1));
                m_EnvKV[k] = v;
                // 立即生效到当前进程
#if defined(_WIN32)
                _putenv_s(k.c_str(), v.c_str());
#else
                setenv(k.c_str(), v.c_str(), 1);
#endif
            }
        }
        return true;
    }

    // ========================================================
    // ==================== WINDOWS IMPLEMENTATION ============
    // ========================================================
#if defined(_WIN32)

    // 静态变量用于 Service 回调
    static SERVICE_STATUS g_ServiceStatus;
    static SERVICE_STATUS_HANDLE g_StatusHandle;

    bool Install()
    {
        if (!CheckRoot())
        {
            std::cerr << "[错误] 安装需要管理员权限 (Run as Administrator)。" << std::endl;
            return false;
        }

        std::cout << ">>> 正在安装 Windows 服务: " << m_name << " <<<" << std::endl;

        // 1. 停止旧服务
        StopService();

        // 2. 准备分身路径
        fs17::path originPath = m_current_dir;
        originPath.append(m_exe);
        // 注意：Windows 下 exe 是必须的，但 m_exe 可能已经包含了
        fs17::path clonePath = m_current_dir;

        // 构造 clone 文件名： app.exe -> app.srv_run.exe
        std::string cloneName = m_exe;
        size_t extPos = cloneName.rfind(".exe");
        if (extPos != std::string::npos)
            cloneName = cloneName.substr(0, extPos);
        cloneName += CLONE_SUFFIX + ".exe";

        clonePath.append(cloneName);

        // 3. 复制文件
        try
        {
            if (fs17::exists(clonePath))
                fs17::remove(clonePath);
            std::cout << "[步骤] 创建服务运行副本..." << std::endl;
            fs17::copy_file(originPath, clonePath, fs17::copy_options::overwrite_existing);
        }
        catch (std::exception& e)
        {
            std::cerr << "[错误] 复制文件失败: " << e.what() << std::endl;
            return false;
        }

        // 4. 读取环境配置 (Windows 下我们只在运行时加载 .env，不写入注册表，保持简单)
        LoadEnv();

        // 5. 创建服务
        SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (NULL == hSCManager)
        {
            std::cerr << "[错误] 无法打开服务管理器。错误码: " << GetLastError() << std::endl;
            return false;
        }

        // 构建命令行： "C:\Path\To\App.srv_run.exe"
        std::string binPath = "\"" + clonePath.string() + "\"";

        SC_HANDLE hService = CreateServiceA(hSCManager, m_name.c_str(), m_name.c_str(), SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, binPath.c_str(), NULL, NULL, NULL, NULL, NULL);

        if (hService == NULL)
        {
            DWORD err = GetLastError();
            if (err == ERROR_SERVICE_EXISTS)
            {
                std::cout << "[信息] 服务已存在，更新配置..." << std::endl;
                hService = OpenServiceA(hSCManager, m_name.c_str(), SERVICE_ALL_ACCESS);
                if (hService)
                {
                    ChangeServiceConfigA(hService, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, binPath.c_str(), NULL, NULL, NULL, NULL, NULL, NULL);
                }
            }
            else
            {
                std::cerr << "[错误] 创建服务失败。错误码: " << err << std::endl;
                CloseServiceHandle(hSCManager);
                return false;
            }
        }

        // 设置描述
        SERVICE_DESCRIPTIONA sd;
        std::string desc = m_name + " Service (Auto Install)";
        sd.lpDescription = (LPSTR)desc.c_str();
        ChangeServiceConfig2A(hService, SERVICE_CONFIG_DESCRIPTION, &sd);

        std::cout << "[步骤] 正在启动服务..." << std::endl;
        StartService(hService, 0, NULL);

        CloseServiceHandle(hService);
        CloseServiceHandle(hSCManager);

        std::cout << "[成功] 安装并启动完毕。" << std::endl;
        return true;
    }

    bool Uninstall()
    {
        if (!CheckRoot())
        {
            std::cerr << "[错误] 卸载需要管理员权限。" << std::endl;
            return false;
        }

        if (!m_skip_confirm)
        {
            std::cout << "警告: 即将卸载服务 [" << m_name << "]。是否继续? [y/N]: ";
            std::string input;
            std::getline(std::cin, input);
            if (input != "y" && input != "Y" && input != "yes")
                return false;
        }

        std::cout << ">>> 正在卸载服务... <<<" << std::endl;

        StopService();

        SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (hSCManager)
        {
            SC_HANDLE hService = OpenServiceA(hSCManager, m_name.c_str(), DELETE);
            if (hService)
            {
                if (DeleteService(hService))
                    std::cout << "[步骤] 服务注册表项已移除。" << std::endl;
                else
                    std::cerr << "[警告] 删除服务失败，错误码: " << GetLastError() << std::endl;
                CloseServiceHandle(hService);
            }
            CloseServiceHandle(hSCManager);
        }

        // 删除分身
        std::string cloneName = m_exe;
        size_t extPos = cloneName.rfind(".exe");
        if (extPos != std::string::npos)
            cloneName = cloneName.substr(0, extPos);
        cloneName += CLONE_SUFFIX + ".exe";

        fs17::path clonePath = m_current_dir;
        clonePath.append(cloneName);

        // 稍微等待一下句柄释放
        Sleep(1000);
        if (fs17::exists(clonePath))
        {
            try
            {
                fs17::remove(clonePath);
                std::cout << "[步骤] 运行副本文件已删除。" << std::endl;
            }
            catch (...)
            {
                std::cerr << "[提示] 无法立即删除副本文件(可能被占用)，将在下次重启清除。" << std::endl;
            }
        }

        std::cout << "[成功] 卸载完成。" << std::endl;
        return true;
    }

    void StopService()
    {
        SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (!hSCManager)
            return;
        SC_HANDLE hService = OpenServiceA(hSCManager, m_name.c_str(), SERVICE_STOP | SERVICE_QUERY_STATUS);
        if (hService)
        {
            SERVICE_STATUS_PROCESS ssp;
            DWORD dwBytesNeeded;
            if (QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
            {
                if (ssp.dwCurrentState != SERVICE_STOPPED)
                {
                    std::cout << "[步骤] 正在停止运行中的服务..." << std::endl;
                    ControlService(hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp);
                    // 简单的轮询等待停止
                    for (int i = 0; i < 10; i++)
                    {
                        QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded);
                        if (ssp.dwCurrentState == SERVICE_STOPPED)
                            break;
                        Sleep(500);
                    }
                }
            }
            CloseServiceHandle(hService);
        }
        CloseServiceHandle(hSCManager);
    }

    // --- Windows 服务运行时逻辑 ---
    // 为了不阻塞 main 函数，我们在独立线程中连接 SCM
    void RunAsServiceHook()
    {
        // 先加载环境变量
        LoadEnv();

        // 启动一个线程去尝试连接 SCM
        // 如果是控制台双击运行，StartServiceCtrlDispatcher 会立即失败返回，线程结束，不影响主逻辑
        // 如果是服务启动，它会阻塞住，直到服务停止
        std::thread scmThread([]() {
            SERVICE_TABLE_ENTRYA ServiceTable[] = {{(LPSTR) "", (LPSERVICE_MAIN_FUNCTIONA)ServiceMain}, {NULL, NULL}};
            // 连接 SCM。如果成功，当前线程会阻塞在这里处理控制消息
            StartServiceCtrlDispatcherA(ServiceTable);
        });
        scmThread.detach();  // 让它在后台跑
    }

    static void WINAPI ServiceMain(DWORD argc, LPSTR* argv)
    {
        g_StatusHandle = RegisterServiceCtrlHandlerA("", ServiceCtrlHandler);
        if (g_StatusHandle == NULL)
            return;

        // 告诉 SCM 我们正在运行
        g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
        g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
        g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
        g_ServiceStatus.dwWin32ExitCode = 0;
        g_ServiceStatus.dwServiceSpecificExitCode = 0;
        g_ServiceStatus.dwCheckPoint = 0;
        g_ServiceStatus.dwWaitHint = 0;

        SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

        // 注意：这里我们不需要做具体业务，业务在 main 的 while(1) 里
        // 这个函数只需要保持活着，直到收到 Stop 信号
    }

    static void WINAPI ServiceCtrlHandler(DWORD CtrlCode)
    {
        switch (CtrlCode)
        {
            case SERVICE_CONTROL_STOP:
            case SERVICE_CONTROL_SHUTDOWN:
                g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
                SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
                // 强制退出进程，因为主循环在 main 里，很难优雅通知
                exit(0);
                break;
            default:
                break;
        }
    }

#else
    // ========================================================
    // ==================== LINUX IMPLEMENTATION =============
    // ========================================================

    // ... (Linux 代码保持原样，仅做简单适配) ...
    bool Install()
    {
        fs17::path originPath = m_current_dir;
        originPath.append(m_exe);
        fs17::path clonePath = m_current_dir;
        clonePath.append(m_exe + CLONE_SUFFIX);

        if (!CheckRoot())
        {
            std::cerr << "[致命] 安装需要 ROOT 权限。" << std::endl;
            return false;
        }

        std::cout << ">>> 正在安装服务: " << m_name << " (root) <<<" << std::endl;
        StopService();  // Systemd Stop

        try
        {
            if (fs17::exists(clonePath))
                fs17::remove(clonePath);
            std::cout << "[步骤] 创建运行副本..." << std::endl;
            fs17::copy_file(originPath, clonePath, fs17::copy_options::overwrite_existing);
            fs17::permissions(clonePath, fs17::status(originPath).permissions());
        }
        catch (std::exception& e)
        {
            std::cerr << "[错误] " << e.what() << std::endl;
            return false;
        }

        LoadEnv();

        // 生成 Service 文件
        fs17::path svcFile = fs17::path("/etc/systemd/system/").append(m_name + ".service");
        std::ofstream f(svcFile);
        f << "[Unit]\nDescription=" << m_name << "\nAfter=network.target\n\n";
        f << "[Service]\nType=simple\nUser=root\nGroup=root\n";
        f << "ExecStart=" << clonePath.string() << "\n";
        f << "WorkingDirectory=" << m_current_dir.string() << "\n";
        if (!m_EnvKV.empty())
        {
            f << "# Environment\n";
            for (const auto& [k, v] : m_EnvKV)
                f << "Environment=\"" << k << "=" << v << "\"\n";
        }
        f << "\nRestart=always\nRestartSec=10\n\n[Install]\nWantedBy=multi-user.target\n";
        f.close();

        system("systemctl daemon-reload");
        std::string cmd = "systemctl enable ";
        cmd.append(m_name);
        system(cmd.c_str());

        std::cout << "[步骤] 启动服务..." << std::endl;
        StartServiceCmd();

        std::cout << "[成功] 安装完成。" << std::endl;
        return true;
    }

    bool Uninstall()
    {
        if (!CheckRoot())
        {
            std::cerr << "[致命] 需要 ROOT 权限。" << std::endl;
            return false;
        }
        if (!m_skip_confirm)
        {
            std::cout << "警告: 确认卸载 [" << m_name << "]? [y/N]: ";
            std::string input;
            std::getline(std::cin, input);
            if (input != "y" && input != "yes")
                return false;
        }

        std::cout << ">>> 卸载服务... <<<" << std::endl;
        StopService();
        std::string cmd = "systemctl disable ";
        cmd.append(m_name);
        system(cmd.c_str());

        fs17::path svcFile = fs17::path("/etc/systemd/system/").append(m_name + ".service");
        if (fs17::exists(svcFile))
            fs17::remove(svcFile);

        fs17::path clonePath = m_current_dir;
        clonePath.append(m_exe + CLONE_SUFFIX);
        if (fs17::exists(clonePath))
            fs17::remove(clonePath);

        system("systemctl daemon-reload");
        system("systemctl reset-failed");
        std::cout << "[成功] 已卸载。" << std::endl;
        return true;
    }

    void StopService()
    {
        system(("systemctl stop " + m_name).c_str());
    }
    void StartServiceCmd()
    {
        system(("systemctl start " + m_name).c_str());
    }
    void RunAsServiceHook()
    {
        LoadEnv(); /* Linux 自动运行，仅需加载环境 */
    }

#endif
};

// 初始化静态成员 (Windows only)
#if defined(_WIN32)
suAutoInstall* suAutoInstall::m_instance = nullptr;
SERVICE_STATUS suAutoInstall::g_ServiceStatus;
SERVICE_STATUS_HANDLE suAutoInstall::g_StatusHandle;
#endif
#endif  // SILLY_AUTO_INSTALL_H