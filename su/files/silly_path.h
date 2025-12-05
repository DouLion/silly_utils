/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-04
 * @file: silly_path.cpp
 * @description: 文件路径处理
 *                  主要是为了处理windows平台下的中文路径
 * @version: v1.0.1 2025-12-04 dou li yang
 */
#ifndef SILLY_PATH_H
#define SILLY_PATH_H
#include <su_marco.h>
class suPath
{
  public:
    suPath() = default;
    suPath(const std::string& path);
    suPath(const suPath& path);
    suPath(const std::filesystem::path& path);
    ~suPath() = default;

    static suPath current();

    static void chdir(const suPath& path);
    static void mkdir(const suPath& path);

    /**
     *
     * @param node
     * @return
     */
    suPath& append(const std::string& node);
#if _WIN32
    suPath(const std::wstring& path);
    suPath& append(const std::wstring& node);
    std::wstring wstring() const;
    std::wstring name_ws() const;
    std::wstring stem_ws() const;
    std::wstring ext_ws() const;
#endif

    /**
     *
     * @return
     */
    suPath& absolute();

    suPath& relative(const suPath& root);

    suPath parent() const;

    /**
     *  跟随系统的文件名称,包含后缀
     * @return
     */
    std::string name() const;

    /**
     *  utf8的文件名称,包含后缀
     * @return
     */
    std::string name_utf8() const;

    /**
     *  跟随系统的文件名称,不包含后缀
     * @return
     */
    std::string stem() const;

    /**
     *  utf8的文件名称,不包含后缀
     * @return
     */
    std::string stem_utf8() const;

    /**
     *  跟随系统的文件后缀
     * @return
     */
    std::string ext() const;

    /**
     * utf8的文件后缀
     * @return
     */
    std::string ext_utf8() const;

    void mkdir() const;


    std::string string() const;
    std::string u8string() const;

  protected:
    std::filesystem::path m_path = std::filesystem::current_path();
};

using supath = suPath;

#endif  // SILLY_PATH_H
