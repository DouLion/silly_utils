/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-26
 * @file: silly_argument
 * @description: silly_argument实现
 * @version: v1.0.1 2025-12-26 dou li yang
 */
#include "silly_argument.h"

suArgs::Option& suArgs::Option::Bind(bool& var)
{
    pParser = [&var](const std::string& str) -> bool {
        if (str.empty() || str == "true" || str == "1")
        {
            var = true;
            return true;
        }
        if (str == "false" || str == "0")
        {
            var = false;
            return true;
        }
        return false;
    };
    return *this;
}
std::string suArgs::Option::DescNormal() const
{
    std::string ret = " ";
    ret.append(pKey).append(" ");
    if (pMust)
    {
        ret.append("<..>");
    }
    else
    {
        ret.append("[..]");
    }
    return ret;
}
std::string suArgs::Option::DescEqual() const

{
    std::string ret = " ";
    ret.append(pKey).append("=");
    if (pMust)
    {
        ret.append("<..>");
    }
    else
    {
        ret.append("[..]");
    }
    return ret;
}
std::string suArgs::Option::Help() const
{
    std::string help = "  ";
    help.append(pKey).append("  ");
    help.append(pDesc).append("  ");
    if (pMust)
    {
        help.append("必选项\n");
    }
    else
    {
        help.append("可选项\n");
    }
    return help;
}
bool suArgs::Add(Option& opt)
{
    auto key = opt.Key();
    if (key.size() < 3 || key.substr(0, 2) != "--")
    {
        std::cerr << "参数: " << key << " 必须以 -- 开头且长度 >= 3\n";
        return false;
    }
    if (m_options.count(key))
    {
        std::cerr << "参数: " << key << " 已经绑定\n";
        return false;
    }
    m_options[key] = opt;
    return true;
}
bool suArgs::Parse(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg.substr(0, 2) != "--")
            continue;

        std::string key, value;
        size_t eq_pos = arg.find('=');
        if (eq_pos != std::string::npos)
        {
            key = arg.substr(0, eq_pos);
            value = arg.substr(eq_pos + 1);
        }
        else
        {
            key = arg;
            // 尝试取下一个参数作为值（如果不是下一个也是 -- 开头）
            if (i + 1 < argc && std::string(argv[i + 1]).substr(0, 2) != "--")
            {
                value = argv[++i];
            }
            else
            {
                value = "";  // 可能是布尔开关
            }
        }

        auto it = m_options.find(key);
        if (it == m_options.end())
        {
            std::cerr << "未知参数: " << key << "\n";
            return false;
        }

        if (!it->second.Parse(value))
        {
            std::cerr << "参数 " << key << " 的值 [" << value << "] 无法解析" << std::endl;
            return false;
        }
    }

    // 检查必填参数
    for (const auto& [key, opt] : m_options)
    {
        if (!opt.Provided())
        {
            std::cerr << "缺少必填参数: " << key << "\n";
            return false;
        }
    }

    return true;
}
void suArgs::Usage()
{
    std::cout << "用法:\n";
    std::cout << " <program>";
    for (const auto& [key, opt] : m_options)
    {
        std::cout <<opt.DescNormal();
    }
    std::cout << "\n 或\n ";
    std::cout << " <program>";
    for (const auto& [key, opt] : m_options)
    {
        std::cout <<opt.DescEqual();
    }

    std::cout << "\n参数:\n";
    for (const auto& [key, opt] : m_options)
    {
        std::cout << opt.Help();
    }
}

/*
bool suArgs::Bind(const std::string& key, bool& var, const bool& must)
{
    if (key.size() < 3 || key.substr(0, 2) != "--")
    {
        std::cerr << "参数: " << key << " 必须以 -- 开头且长度 >= 3\n";
        return false;
    }
    if (m_options.count(key))
    {
        std::cerr << "参数: " << key << " 已经绑定\n";
        return false;
    }
    m_options[key] = Option(
        [&var](const std::string& str) -> bool {
            if (str.empty() || str == "true" || str == "1")
            {
                var = true;
                return true;
            }
            if (str == "false" || str == "0")
            {
                var = false;
                return true;
            }
            return false;
        },
        must);
    return true;
}*/