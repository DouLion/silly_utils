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
#if defined(__linux__)
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
#include <unistd.h>  // geteuid, readlink
#include <limits.h>  // PATH_MAX
#include <thread>
#include <chrono>

// 确保 C++17
#if __cplusplus < 201703L
#error "This code requires C++17 or later."
#endif

namespace fs17 = std::filesystem;

class suAutoInstall
{
  public:
    // --- 主入口处理函数 ---
    void Process(int argc, char** argv)
    {
        // 1. 如果没有参数，或者参数不是 install/uninstall，直接返回，执行业务逻辑
        if (argc < 2)
            return;

        std::string cmd = argv[1];
        if (cmd != "install" && cmd != "uninstall")
        {
            return;
        }

        // 2. 初始化
        if (!Init(argc, argv))
        {
            ShowUsage(argv[0]);
            exit(-1);
        }

        // 安全检查：防止分身程序自己尝试安装自己
        // 如果当前运行的是 .srv_run 文件，禁止它执行安装操作
        if (m_exe.find(CLONE_SUFFIX) != std::string::npos)
        {
            std::cerr << "[错误] 服务运行副本无法执行安装/卸载操作。" << std::endl;
            std::cerr << "请运行原始可执行程序。" << std::endl;
            exit(-1);
        }

        // 3. 执行逻辑
        bool result = false;
        if (cmd == "install")
        {
            result = Install();
        }
        else  // uninstall
        {
            result = Uninstall();
        }

        exit(result ? 0 : -1);
    }

  private:
    const std::string CLONE_SUFFIX = ".srv_run";  // 分身后缀

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

    static fs17::path GetSelfExePath()
    {
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        if (count != -1)
            return fs17::path(std::string(result, count));
        return "";
    }

    bool Init(int argc, char** argv)
    {
        fs17::path selfPath = GetSelfExePath();
        if (selfPath.empty())
            return false;

        m_exe = selfPath.filename().string();
        m_current_dir = selfPath.parent_path();
        m_skip_confirm = false;

        for (int i = 2; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg == "--NAME" && i + 1 < argc)
            {
                m_name = argv[i + 1];
                size_t extPos = m_name.find(".service");
                if (extPos != std::string::npos)
                {
                    m_name = m_name.substr(0, extPos);
                }
            }
            else if (arg == "-y" || arg == "--yes")
            {
                m_skip_confirm = true;
            }
        }
        return !m_name.empty();
    }

    // --- 核心修改：安装时创建分身 ---
    bool Install()
    {
        fs17::path originPath = m_current_dir;
        originPath.append(m_exe);  // 原始程序路径

        fs17::path clonePath = m_current_dir;
        clonePath.append(m_exe + CLONE_SUFFIX);  // 分身路径

        // 1. Root 检查
        if (geteuid() != 0)
        {
            std::cerr << "[致命错误] 安装需要 ROOT 权限。" << std::endl;
            std::cerr << "尝试: sudo " << originPath.string() << " install --NAME \"" << m_name << "\"" << std::endl;
            return false;
        }

        std::cout << ">>> 正在安装服务: " << m_name << " (用户: root) <<<" << std::endl;

        // 2. 必须先停止服务，才能安全删除/覆盖旧的分身
        std::cout << "[步骤] 正在停止服务(确保释放文件占用)..." << std::endl;
        Stop();

        // 3. 创建/更新 分身文件
        // 策略：先删除旧分身(接触占用)，再复制新分身
        try
        {
            if (fs17::exists(clonePath))
            {
                fs17::remove(clonePath);  // 关键：删除文件会解除 Linux 对该文件的 inode 锁定
            }
            std::cout << "[步骤] 创建服务运行副本..." << std::endl;
            fs17::copy_file(originPath, clonePath, fs17::copy_options::overwrite_existing);

            // 复制权限 (确保 +x)
            fs17::permissions(clonePath, fs17::status(originPath).permissions());
        }
        catch (std::exception& e)
        {
            std::cerr << "[错误] 创建运行副本失败: " << e.what() << std::endl;
            return false;
        }

        // 4. 冲突检查
        fs17::path existingPath = ParseSysServicePath();
        if (!existingPath.empty())
        {
            // 如果存在且不是指向我们的分身，也不是指向原始文件，则报警
            if (!fs17::equivalent(existingPath, clonePath) && !fs17::equivalent(existingPath, originPath))
            {
                std::cerr << "[错误] 服务名称冲突! 现有路径: " << existingPath.string() << std::endl;
                return false;
            }
        }

        // 5. 生成配置文件
        if (!ParseEnv())
            return false;

        // 生成 Service 文件 (ExecStart 将指向 clonePath)
        if (!GenServiceFile(clonePath))
            return false;

        // 6. 启动
        if (!Reload())
            return false;

        std::string cmd = "systemctl enable ";
        cmd.append(m_name);
        system(cmd.c_str());

        std::cout << "[步骤] 正在启动服务 (运行副本)..." << std::endl;
        if (!Start())
        {
            std::cerr << "[错误] 服务启动失败。请检查日志。" << std::endl;
            Status();
            Journal();
            return false;
        }

        std::cout << "[成功] 安装完毕。服务运行于: " << clonePath.filename() << std::endl;
        Status();
        return true;
    }

    bool Uninstall()
    {
        if (geteuid() != 0)
        {
            std::cerr << "[致命错误] 卸载需要 ROOT 权限。" << std::endl;
            return false;
        }

        if (!m_skip_confirm)
        {
            std::cout << "警告: 即将卸载服务 [" << m_name << "]。是否继续? [y/N]: ";
            std::string input;
            std::getline(std::cin, input);
            std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c) { return std::tolower(c); });
            if (input != "y" && input != "yes")
            {
                std::cout << "操作已取消。" << std::endl;
                return false;
            }
        }

        std::cout << ">>> 正在卸载服务: " << m_name << " <<<" << std::endl;

        Stop();

        std::string cmd = "systemctl disable ";
        cmd.append(m_name);
        system(cmd.c_str());

        // 删除 Service 文件
        fs17::path svcFile = ServiceFile();
        if (fs17::exists(svcFile))
        {
            std::error_code ec;
            fs17::remove(svcFile, ec);
        }

        // 删除分身文件 (Cleanup)
        fs17::path clonePath = m_current_dir;
        clonePath.append(m_exe + CLONE_SUFFIX);
        if (fs17::exists(clonePath))
        {
            std::error_code ec;
            fs17::remove(clonePath, ec);
            if (!ec)
                std::cout << "[步骤] 运行副本已移除。" << std::endl;
        }

        Reload();
        system("systemctl reset-failed");

        std::cout << "[成功] 卸载完毕。" << std::endl;
        return true;
    }

    // --- Systemd 操作 ---
    bool Start() const
    {
        return system(("systemctl start " + m_name).c_str()) == 0;
    }
    bool Stop() const
    {
        return system(("systemctl stop " + m_name).c_str()) == 0;
    }
    bool Reload() const
    {
        return system("systemctl daemon-reload") == 0;
    }
    bool Status() const
    {
        return system(("systemctl status " + m_name).c_str()) == 0;
    }
    bool Journal() const
    {
        return system(("journalctl -n 20 -u " + m_name).c_str()) == 0;
    }

    fs17::path EnvFile() const
    {
        fs17::path p = m_current_dir;
        p.append(m_name + ".env");
        return p;
    }
    fs17::path ServiceFile() const
    {
        fs17::path p = "/etc/systemd/system/";
        p.append(m_name + ".service");
        return p;
    }

    // 参数修改：接收 clonePath
    bool GenServiceFile(const fs17::path& runPath)
    {
        std::stringstream ss;
        ss << "[Unit]\nDescription=" << m_name << " Service\nAfter=network.target\n\n";

        ss << "[Service]\nType=simple\n";
        ss << "User=root\nGroup=root\n";
        // 这里的 ExecStart 指向分身
        ss << "ExecStart=" << runPath.string() << "\n";
        ss << "WorkingDirectory=" << m_current_dir.string() << "\n";

        if (!m_EnvKV.empty())
        {
            ss << "# Environment\n";
            for (const auto& [k, v] : m_EnvKV)
                ss << "Environment=\"" << k << "=" << v << "\"\n";
        }

        ss << "\nRestart=always\nRestartSec=10\n";
        ss << "\n[Install]\nWantedBy=multi-user.target\n";

        return WriteFile(ServiceFile(), ss.str());
    }

    bool ParseEnv()
    {
        fs17::path file = EnvFile();
        m_EnvKV.clear();

        if (!fs17::exists(file))
        {
            const std::string content =
                R"(# Service Environment Config
# LD_LIBRARY_PATH=/path/to/dmdbms/bin
# DB_HOST=127.0.0.1
)";
            if (!WriteFile(file, content))
                return false;
        }

        auto parse = [this](std::string& line) {
            line = Trim(line);
            if (line.empty() || line[0] == '#')
                return false;
            size_t pos = line.find('=');
            if (pos != std::string::npos)
            {
                m_EnvKV[Trim(line.substr(0, pos))] = Trim(line.substr(pos + 1));
            }
            return false;
        };
        ReadFile(file, parse);
        return true;
    }

    fs17::path ParseSysServicePath()
    {
        fs17::path sysfile = ServiceFile();
        if (!fs17::exists(sysfile))
            return {};

        std::string execPath;
        auto extract = [&](std::string& line) {
            line = Trim(line);
            if (line.find("ExecStart=") == 0)
            {
                execPath = line.substr(10);
                size_t sp = execPath.find(' ');
                if (sp != std::string::npos)
                    execPath = execPath.substr(0, sp);
            }
            return false;
        };
        ReadFile(sysfile, extract);

        if (!execPath.empty() && fs17::exists(execPath))
            return fs17::absolute(execPath);
        return {};
    }

    bool WriteFile(const fs17::path& p, const std::string& c) const
    {
        std::ofstream f(p);
        if (!f.is_open())
            return false;
        f << c;
        fs17::permissions(p, fs17::perms::owner_all | fs17::perms::group_read | fs17::perms::others_read);
        return true;
    }

    void ReadFile(const fs17::path& p, const std::function<bool(std::string&)>& ff)
    {
        std::ifstream f(p);
        std::string l;
        while (std::getline(f, l))
            if (ff)
                ff(l);
    }

    std::unordered_map<std::string, std::string> m_EnvKV;
    std::string m_name;
    std::string m_exe;
    fs17::path m_current_dir;
    bool m_skip_confirm;
};
#endif
#endif  // SILLY_AUTO_INSTALL_H