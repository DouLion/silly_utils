/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/3 10:00
 * @version: 1.0.1
 * @description: DLL/so工具类
 */
#ifndef SILLY_UTILS_SILLY_DLL_H
#define SILLY_UTILS_SILLY_DLL_H
#include <su_marco.h>
#if IS_WIN32
#include <Windows.h>
#else
#include <dlfcn.h> 
#endif

#ifndef SU_HANDLER
#if IS_WIN32
#define SU_HANDLER	HMODULE
#else
#define SU_HANDLER	void*
#endif 
#endif

#ifndef SU_FUNC_ADDR
#define SU_FUNC_ADDR void*
#endif


class silly_dll
{
public:
	/// <summary>
	/// 从指定路径加载动态库
	/// </summary>
	/// <param name="path"></param>
	/// <returns></returns>
	static SU_HANDLER load(const std::string& path);

	/// <summary>
	/// 卸载动态库
	/// </summary>
	/// <param name="handler"></param>
	/// <returns></returns>
	static bool close(SU_HANDLER& handler);

	/// <summary>
	/// 从加载的动态库中根据函数名获取函数地址
	/// </summary>
	/// <param name="handler"></param>
	/// <param name="name"></param>
	/// <returns></returns>
	static SU_FUNC_ADDR func(SU_HANDLER handler, const std::string& name);
	 
};

#endif //SILLY_UTILS_SILLY_DLL_H
