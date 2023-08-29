/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/29 16:59
 * @version: 1.0.1
 * @software: Project
 * @description:
 */
#pragma once


template<typename T>
class silly_singleton
{
public:
	static T& instance()
	{
		static T instance;
		return instance;
	}

	silly_singleton(T&&) = delete;
	silly_singleton(const T&) = delete;
	void operator= (const T&) = delete;

protected:
	silly_singleton() = default;
	virtual ~silly_singleton() = default;
};
