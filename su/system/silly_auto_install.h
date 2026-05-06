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
#pragma once

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
#include <cstdlib>
#include <csignal>
#include <atomic>

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
#include <sys/wait.h>
#include <limits.h>
#define PATH_SEPARATOR "/"
#endif

// 确保 C++17
#if !defined(__cplusplus) || (__cplusplus < 201703L && !defined(_MSVC_LANG))
#error "This header requires C++17 or later. Please enable C++17 support."
#elif defined(_MSVC_LANG) && _MSVC_LANG < 201703L
#error "This header requires C++17 or later. Please enable C++17 support in MSVC."
#endif

namespace fs17 = std::filesystem;

class suAutoInstall
{
  public:
    suAutoInstall()
    {
        if (m_instance == nullptr)
            m_instance = this;
    }

    // --- 主入口处理函数 ---
    void Process(int argc, char** argv)
    {
        if (!BasicInit(argv[0]))
            return;

        // 1. 检查是否是 Watcher 启动的 Worker 进程
        if (IsWorkerProcess(argc, argv))
        {
            LoadEnv(false);
            // 恢复默认信号处理，因为父进程可能忽略了某些信号
            ResetSignalHandlers();
            return;
        }

        // 2. Docker / 容器环境特殊处理 (看门狗模式)
        if (IsContainer())
        {
            EnterContainerWatchdogMode(argc, argv);
            exit(0);
        }

        // --- 物理机/虚拟机 常规逻辑 ---
        if (argc < 2)
        {
            RunAsServiceHook();
            return;
        }

        std::string cmd = argv[1];
        if (cmd != "install" && cmd != "uninstall")
        {
            RunAsServiceHook();
            return;
        }

        ParseArgs(argc, argv);
        if (m_name.empty())
        {
            ShowUsage(argv[0]);
            exit(-1);
        }

        if (m_exe.find(CLONE_SUFFIX) != std::string::npos)
        {
            std::cerr << "[错误] 服务运行副本无法执行安装/卸载操作。" << std::endl;
            exit(-1);
        }

        bool result = (cmd == "install") ? Install() : Uninstall();
        exit(result ? 0 : -1);
    }

  private:
    const std::string CLONE_SUFFIX = ".srv_run";
    const std::string WORKER_FLAG = "--worker-process-run";

    std::string m_name;
    std::string m_exe;
    fs17::path m_current_dir;
    bool m_skip_confirm = false;
    std::unordered_map<std::string, std::string> m_EnvKV;

    static suAutoInstall* m_instance;
    // 用于 Docker 信号处理
    static std::atomic<bool> g_stop_requested;
    static std::atomic<int> g_child_pid;

    // --- 容器化 & 看门狗逻辑 ---
    bool IsContainer()
    {
#if !defined(_WIN32)
        if (fs17::exists("/.dockerenv"))
            return true;
#endif
        const char* c_env = std::getenv("container");
        if (c_env && std::string(c_env) == "docker")
            return true;
        if (std::getenv("KUBERNETES_SERVICE_HOST"))
            return true;
        if (std::getenv("FORCE_DOCKER_MODE"))
            return true;
        return false;
    }

    bool IsWorkerProcess(int argc, char** argv)
    {
        for (int i = 1; i < argc; ++i)
            if (std::string(argv[i]) == WORKER_FLAG)
                return true;
        return false;
    }

    // 信号处理：收到停止信号时，通知主循环退出
    static void SignalHandler(int signum)
    {
        if (signum == SIGTERM || signum == SIGINT)
        {
            g_stop_requested = true;
#if !defined(_WIN32)
            // 转发信号给子进程
            int child = g_child_pid.load();
            if (child > 0)
                kill(child, signum);
#endif
        }
    }

    void ResetSignalHandlers()
    {
#if !defined(_WIN32)
        signal(SIGTERM, SIG_DFL);
        signal(SIGINT, SIG_DFL);
#endif
    }

    // 看门狗主循环
    void EnterContainerWatchdogMode(int argc, char** argv)
    {
        std::cout << "[守护进程] 启动看门狗模式 (PID " << getpid() << ")..." << std::endl;
        LoadEnv(false);

        // 注册信号处理 (响应 docker stop)
        signal(SIGTERM, SignalHandler);
        signal(SIGINT, SignalHandler);

        std::vector<std::string> args;
        args.push_back(GetSelfExePath().string());
        for (int i = 1; i < argc; ++i)
            args.push_back(argv[i]);
        args.push_back(WORKER_FLAG);

        int crash_count = 0;

        // 全局异常捕获，确保守护进程本身不死
        try
        {
            while (!g_stop_requested)
            {
                std::cout << "[守护进程] 启动工作进程..." << std::endl;

                auto start_time = std::chrono::steady_clock::now();
                int exit_code = SpawnWorker(args);  // 这里会阻塞直到子进程结束
                auto end_time = std::chrono::steady_clock::now();

                // 检查是否是由于父进程收到了停止信号而导致的 wait 返回
                if (g_stop_requested)
                {
                    std::cout << "[守护进程] 接收到停止指令，正在退出..." << std::endl;
                    break;
                }

                if (exit_code == 0)
                {
                    std::cout << "[守护进程] 工作进程正常退出(0)。容器停止。" << std::endl;
                    break;
                }

                std::cerr << "[警告] 工作进程异常退出 (Code: " << exit_code << ")" << std::endl;

                auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
                if (duration < 2)
                {
                    crash_count++;
                    std::cerr << "[警告] 启动即崩溃，休眠 5 秒..." << std::endl;
                    // 使用分段休眠，以便能响应停止信号
                    for (int k = 0; k < 50 && !g_stop_requested; ++k)
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                else
                {
                    crash_count = 0;
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "[致命错误] 守护进程发生未捕获异常: " << e.what() << std::endl;
            // 守护进程崩了，容器也就结束了，但这极不可能发生
        }
        catch (...)
        {
            std::cerr << "[致命错误] 守护进程发生未知异常。" << std::endl;
        }
    }

    int SpawnWorker(const std::vector<std::string>& args)
    {
#if defined(_WIN32)
        std::string cmdLine;
        for (const auto& a : args)
            cmdLine += "\"" + a + "\" ";
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        if (!CreateProcessA(NULL, (LPSTR)cmdLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
            return -1;
        g_child_pid = (int)pi.dwProcessId;  // 记录子进程ID
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD exitCode = 0;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        g_child_pid = 0;
        return (int)exitCode;
#else
        pid_t pid = fork();
        if (pid == -1)
            return -1;
        if (pid == 0)
        {
            // Child
            // 恢复信号处理，否则子进程可能无法被 ctrl+c 杀掉
            signal(SIGTERM, SIG_DFL);
            signal(SIGINT, SIG_DFL);
            std::vector<char*> c_args;
            for (const auto& a : args)
                c_args.push_back(const_cast<char*>(a.c_str()));
            c_args.push_back(nullptr);
            execv(args[0].c_str(), c_args.data());
            exit(127);
        }
        else
        {
            // Parent
            g_child_pid = pid;
            int status;
            waitpid(pid, &status, 0);
            g_child_pid = 0;
            if (WIFEXITED(status))
                return WEXITSTATUS(status);
            if (WIFSIGNALED(status))
                return 128 + WTERMSIG(status);
            return -1;
        }
#endif
    }

    // --- 基础工具 (保持不变) ---
    void ShowUsage(const char* progName) const
    {
        std::cerr << "用法:" << std::endl;
        std::cerr << "  " << progName << " install   --NAME <服务名>" << std::endl;
        std::cerr << "  " << progName << " uninstall --NAME <服务名> [-y]" << std::endl;
    }
    static std::string Trim(const std::string& str)
    {
        auto begin = str.find_first_not_of(" \t\r\n");
        if (begin == std::string::npos)
            return "";
        auto end = str.find_last_not_of(" \t\r\n");
        return str.substr(begin, end - begin + 1);
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
            selfPath = fs17::absolute(argv0);
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
                size_t extPos = m_name.find(".service");
                if (extPos != std::string::npos)
                    m_name = m_name.substr(0, extPos);
            }
            else if (arg == "-y" || arg == "--yes")
                m_skip_confirm = true;
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
        p.append(m_name.empty() ? "service" : m_name);
        p.replace_extension(".env");
        return p;
    }
    bool LoadEnv(bool overwrite = true)
    {
        fs17::path file = EnvFile();
        if (m_name.empty())
        {
            fs17::path defaultEnv = m_current_dir;
            defaultEnv.append(m_exe).replace_extension(".env");
            if (fs17::exists(defaultEnv))
                file = defaultEnv;
            else
            {
                fs17::path simpleEnv = m_current_dir;
                simpleEnv.append(".env");
                if (fs17::exists(simpleEnv))
                    file = simpleEnv;
            }
        }
        m_EnvKV.clear();
        if (!fs17::exists(file))
            return false;
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
#if defined(_WIN32)
                if (overwrite || getenv(k.c_str()) == NULL)
                    _putenv_s(k.c_str(), v.c_str());
#else
                setenv(k.c_str(), v.c_str(), overwrite ? 1 : 0);
#endif
            }
        }
        return true;
    }

    // ==================== WINDOWS ====================
#if defined(_WIN32)
    static SERVICE_STATUS g_ServiceStatus;
    static SERVICE_STATUS_HANDLE g_StatusHandle;
    bool Install()
    {
        if (!CheckRoot())
        {
            std::cerr << "Admin required." << std::endl;
            return false;
        }
        StopService();
        fs17::path originPath = m_current_dir;
        originPath.append(m_exe);
        fs17::path clonePath = m_current_dir;
        std::string cloneName = m_exe;
        size_t extPos = cloneName.rfind(".exe");
        if (extPos != std::string::npos)
            cloneName = cloneName.substr(0, extPos);
        cloneName += CLONE_SUFFIX + ".exe";
        clonePath.append(cloneName);
        try
        {
            if (fs17::exists(clonePath))
                fs17::remove(clonePath);
            fs17::copy_file(originPath, clonePath, fs17::copy_options::overwrite_existing);
        }
        catch (...)
        {
            return false;
        }
        LoadEnv(true);
        SC_HANDLE hMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (!hMgr)
            return false;
        std::string binPath = "\"" + clonePath.string() + "\"";
        SC_HANDLE hSvc = CreateServiceA(hMgr, m_name.c_str(), m_name.c_str(), SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, binPath.c_str(), NULL, NULL, NULL, NULL, NULL);
        if (!hSvc && GetLastError() == ERROR_SERVICE_EXISTS)
        {
            hSvc = OpenServiceA(hMgr, m_name.c_str(), SERVICE_ALL_ACCESS);
            if (hSvc)
                ChangeServiceConfigA(hSvc, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, binPath.c_str(), NULL, NULL, NULL, NULL, NULL, NULL);
        }
        if (hSvc)
        {
            SERVICE_DESCRIPTIONA sd;
            std::string desc = m_name + " Service";
            sd.lpDescription = (LPSTR)desc.c_str();
            ChangeServiceConfig2A(hSvc, SERVICE_CONFIG_DESCRIPTION, &sd);
            StartService(hSvc, 0, NULL);
            CloseServiceHandle(hSvc);
        }
        CloseServiceHandle(hMgr);
        return true;
    }
    bool Uninstall()
    {
        if (!CheckRoot())
            return false;
        StopService();
        SC_HANDLE hMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (hMgr)
        {
            SC_HANDLE hSvc = OpenServiceA(hMgr, m_name.c_str(), DELETE);
            if (hSvc)
            {
                DeleteService(hSvc);
                CloseServiceHandle(hSvc);
            }
            CloseServiceHandle(hMgr);
        }
        return true;
    }
    void StopService()
    {
        SC_HANDLE hMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (!hMgr)
            return;
        SC_HANDLE hSvc = OpenServiceA(hMgr, m_name.c_str(), SERVICE_STOP | SERVICE_QUERY_STATUS);
        if (hSvc)
        {
            SERVICE_STATUS_PROCESS ssp;
            DWORD dw;
            if (QueryServiceStatusEx(hSvc, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(ssp), &dw) && ssp.dwCurrentState != SERVICE_STOPPED)
                ControlService(hSvc, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp);
            CloseServiceHandle(hSvc);
        }
        CloseServiceHandle(hMgr);
    }
    void RunAsServiceHook()
    {
        LoadEnv(true);
        std::thread t([]() {
            SERVICE_TABLE_ENTRYA Ste[] = {{(LPSTR) "", (LPSERVICE_MAIN_FUNCTIONA)ServiceMain}, {NULL, NULL}};
            StartServiceCtrlDispatcherA(Ste);
        });
        t.detach();
    }
    static void WINAPI ServiceMain(DWORD, LPSTR*)
    {
        g_StatusHandle = RegisterServiceCtrlHandlerA("", ServiceCtrlHandler);
        if (!g_StatusHandle)
            return;
        g_ServiceStatus = {SERVICE_WIN32_OWN_PROCESS, SERVICE_RUNNING, SERVICE_ACCEPT_STOP, 0, 0, 0, 0};
        SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
    }
    static void WINAPI ServiceCtrlHandler(DWORD c)
    {
        if (c == SERVICE_CONTROL_STOP)
            exit(0);
    }
#else
    // ==================== LINUX ====================
    bool Install()
    {
        if (!CheckRoot())
            return false;
        StopService();
        fs17::path originPath = m_current_dir;
        originPath.append(m_exe);
        fs17::path clonePath = m_current_dir;
        clonePath.append(m_exe + CLONE_SUFFIX);
        try
        {
            if (fs17::exists(clonePath))
                fs17::remove(clonePath);
            fs17::copy_file(originPath, clonePath, fs17::copy_options::overwrite_existing);
            fs17::permissions(clonePath, fs17::status(originPath).permissions());
        }
        catch (...)
        {
            return false;
        }
        LoadEnv(true);
        std::ofstream f("/etc/systemd/system/" + m_name + ".service");
        f << "[Unit]\nDescription=" << m_name << "\nAfter=network.target\n[Service]\nExecStart=" << clonePath.string() << "\nWorkingDirectory=" << m_current_dir.string() << "\nRestart=always\n[Install]\nWantedBy=multi-user.target\n";
        if (!m_EnvKV.empty())
            for (const auto& [k, v] : m_EnvKV)
                f << "Environment=\"" << k << "=" << v << "\"\n";
        system("systemctl daemon-reload");
        system(("systemctl enable " + m_name).c_str());
        StartServiceCmd();
        return true;
    }
    bool Uninstall()
    {
        if (!CheckRoot())
            return false;
        StopService();
        system(("systemctl disable " + m_name).c_str());
        fs17::remove("/etc/systemd/system/" + m_name + ".service");
        fs17::remove(m_current_dir / (m_exe + CLONE_SUFFIX));
        system("systemctl daemon-reload");
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
        LoadEnv(true);
    }
#endif
};

#if defined(_WIN32)
suAutoInstall* suAutoInstall::m_instance = nullptr;
SERVICE_STATUS suAutoInstall::g_ServiceStatus;
SERVICE_STATUS_HANDLE suAutoInstall::g_StatusHandle;
std::atomic<bool> suAutoInstall::g_stop_requested(false);
std::atomic<int> suAutoInstall::g_child_pid(0);
#else
suAutoInstall* suAutoInstall::m_instance = nullptr;
std::atomic<bool> suAutoInstall::g_stop_requested(false);
std::atomic<int> suAutoInstall::g_child_pid(0);
#endif
#endif  // SILLY_AUTO_INSTALL_H