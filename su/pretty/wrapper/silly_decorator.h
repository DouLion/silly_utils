/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/4/9 11:24
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_DECORATOR_H
#define SILLY_UTILS_SILLY_DECORATOR_H

#include <functional>
#include <string>

template <typename Func, typename Obj, typename... Args>
auto exception_decorator_member_func(Func&& func, Obj&& obj, std::string&& err, Args&&... args) -> decltype(std::invoke(std::forward<Func>(func), std::forward<Obj>(obj), std::forward<Args>(args)...))
{
    try
    {
        auto result = std::invoke(std::forward<Func>(func), std::forward<Obj>(obj), std::forward<Args>(args)...);
    }
    catch (const std::exception& e)
    {
        // 捕获标准异常
        err = e.what();
    }
    catch (...)
    {
        // 捕获所有其他类型的异常
        err = "未知异常";
    }
    return decltype(std::invoke(std::forward<Func>(func), std::forward<Obj>(obj), std::forward<Args>(args)...))();  // 返回默认值
}

#endif  // SILLY_UTILS_SILLY_DECORATOR_H
