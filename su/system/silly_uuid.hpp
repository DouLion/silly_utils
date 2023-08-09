/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/9 11:39
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_UUID_HPP
#define SILLY_UTILS_SILLY_UUID_HPP

#include <stdint.h>
#include <array>
#include <string>

#include <sstream>
#include <iomanip>
#include <ios>
#ifdef IS_WIN32
#include <objbase.h>
#else
#include <uuid/uuid.h>
#endif

class silly_uuid
{
public:
	static std::string random_uuid()
	{
		char buff[64] = { 0 };
#ifdef IS_WIN32

		GUID guid;
		HRESULT hr = CoCreateGuid(&guid);
		sprintf(buff, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			guid.Data1, guid.Data2,
			guid.Data3, guid.Data4[0],
			guid.Data4[1], guid.Data4[2],
			guid.Data4[3], guid.Data4[4],
			guid.Data4[5], guid.Data4[6],
			guid.Data4[7]);

#else
		uuid_t uu;
		uuid_generate_random(uu);
		uuid_unparse_upper(uu, buff);
#endif
		return std::string(buff);
	}
};

#endif //SILLY_UTILS_SILLY_UUID_HPP
