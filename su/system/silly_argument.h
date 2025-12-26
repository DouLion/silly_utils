/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-26
 * @file: silly_argument.h
 * @description: silly_argument 头文件
 * @version: v1.0.1 2025-12-26 dou li yang
 */
#ifndef SILLY_ARGUMENT_H
#define SILLY_ARGUMENT_H
#include <system/silly_system.h>
#include <su_marco.h>
class suArgs
{
public:
    struct Option
    {
        Option() = default;
        Option(const std::string& k)
        {
            pKey = k;
        }

        /**
         * @brief: 绑定参数
         * @param {T&} var
         * @return {*}
         */
        template <typename T>
        Option& Bind(T& var)
        {
           pParser =  [&var](const std::string& str) -> bool {
                std::istringstream iss(str);
                T temp;
                if (!(iss >> temp))
                    return false;  // 转换失败
                var = std::move(temp);
                return true;
            };
            return *this;
        }

        /**
         * @brief 绑定bool类型参数
         * @param var
         * @return
         */
        Option& Bind(bool& var);

        Option& Desc(const std::string& desc)
        {
            pDesc = desc;
            return *this;
        }

        /**
         * @brief 设置是否为必选项
         * @param must
         * @return
         */
        Option& Must(const bool& must)
        {
            pMust = must;
            return *this;
        }

        /**
         * @brief 解析参数
         * @param val
         * @return
         */
        bool Parse(const std::string& val)
        {
            pProvided = pParser(val);
            return pProvided;
        }

        std::string DescNormal() const;

        std::string DescEqual() const;

        /**
         * @brief 获取参数是否被正确提供
         * @return
         */
        bool Provided() const
        {
            if (!pMust)
            {
                return true;
            }
            return pProvided;
        }

        /**
         * @brief 提示信息
         * @return
         */
        std::string Help() const;

        std::string Key() const
        {
            return pKey;
        }
    private:
        std::string pKey;
        std::string pDesc;
        bool pMust = true;
        bool pProvided = false;
        std::function<bool(const std::string&)> pParser;
    };

  public:
    /**
     * @brief 添加一个参数选项
     * @param opt
     * @return
     */
    bool Add(Option& opt);

    /**
     * @brief 解析参数
     * @param argc
     * @param argv
     * @return
     */
    bool Parse(int argc, char* argv[]);

    /**
     * @brief 显示用法
     */
    void Usage();

  private:
    std::map<std::string, Option> m_options;
};

/*template <typename T>
bool suArgs::Bind(const std::string& key, T& var, const bool& must)
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
    // 注册一个 lambda，用于将字符串转换为 T 并赋值给 var
    m_options[key] = Option(
        [&var](const std::string& str) -> bool {
            std::istringstream iss(str);
            T temp;
            if (!(iss >> temp))
                return false;  // 转换失败
            var = std::move(temp);
            return true;
        },
        must);
    return true;
}*/
#endif  // SILLY_ARGUMENT_H
