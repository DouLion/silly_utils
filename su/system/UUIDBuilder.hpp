/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/5/19 15:08
 * @version: 1.0.1
 * @software: TzxIsoLine
 * @description:
 */
#pragma once

#ifndef UUID_BUILDER_HPP
#define UUID_BUILDER_HPP
#include <stdint.h>
#include <array>
#include <string>

#include <sstream>
#include <iomanip>
#include <ios>
#ifdef _WIN32
#include <objbase.h>
#else
#include <uuid/uuid.h>
#endif
class UUIDBuilder
{
public:
	UUIDBuilder() = default;
	~UUIDBuilder() =default;
	static UUIDBuilder GenerateRandom()
	{
		UUIDBuilder result;
#ifdef _WIN32

		GUID newId;
		::CoCreateGuid(&newId);

		std::array<ValueType, DATA_LENGTH> bytes =
		{ {
				// Big-Endian
				(ValueType)((newId.Data1 >> 24) & 0xFF),
				(ValueType)((newId.Data1 >> 16) & 0xFF),
				(ValueType)((newId.Data1 >> 8) & 0xFF),
				(ValueType)((newId.Data1) & 0xFF),

				(ValueType)((newId.Data2 >> 8) & 0xFF),
				(ValueType)((newId.Data2) & 0xFF),

				(ValueType)((newId.Data3 >> 8) & 0xFF),
				(ValueType)((newId.Data3) & 0xFF),

				// Little-Endian
				newId.Data4[0],
				newId.Data4[1],
				newId.Data4[2],
				newId.Data4[3],
				newId.Data4[4],
				newId.Data4[5],
				newId.Data4[6],
				newId.Data4[7]
			} };
		result._data = bytes;

#else

		uuid_t id;
		uuid_generate(id);

		std::array<ValueType, DATA_LENGTH> bytes =
		{ {
				id[0],
				id[1],
				id[2],
				id[3],
				id[4],
				id[5],
				id[6],
				id[7],
				id[8],
				id[9],
				id[10],
				id[11],
				id[12],
				id[13],
				id[14],
				id[15]
			} };
		result._data = bytes;
#endif
		return result;
	}
	std::string toString(const std::string& split = "-", bool up = true)
	{
		std::stringstream str;
		if (up)
		{
			str.setf(std::ios_base::uppercase);
		}
		else
		{
			str.unsetf(std::ios_base::uppercase);
		}
		str << std::hex << std::setfill('0')
			<< std::setw(2) << (int)_data[0]
			<< std::setw(2) << (int)_data[1]
			<< std::setw(2) << (int)_data[2]
			<< std::setw(2) << (int)_data[3]
			<< split
			<< std::setw(2) << (int)_data[4]
			<< std::setw(2) << (int)_data[5]
			<< split
			<< std::setw(2) << (int)_data[6]
			<< std::setw(2) << (int)_data[7]
			<< split
			<< std::setw(2) << (int)_data[8]
			<< std::setw(2) << (int)_data[9]
			<< split
			<< std::setw(2) << (int)_data[10]
			<< std::setw(2) << (int)_data[11]
			<< std::setw(2) << (int)_data[12]
			<< std::setw(2) << (int)_data[13]
			<< std::setw(2) << (int)_data[14]
			<< std::setw(2) << (int)_data[15];
		return str.str();
	}
private:
	typedef uint8_t ValueType;
	static const size_t DATA_LENGTH = 16;
	friend bool operator==(UUIDBuilder const& lhs, UUIDBuilder const& rhs) noexcept
	{
		return lhs._data == rhs._data;
	}
	friend bool operator<(UUIDBuilder const& lhs, UUIDBuilder const& rhs) noexcept
	{
		return lhs._data < rhs._data;
	}

	std::array<ValueType, DATA_LENGTH> _data;
};

#endif //UUID_BUILDER_HPP
