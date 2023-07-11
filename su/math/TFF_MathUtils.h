////////////////////////////////////////////////////////////////////////////////////
//
//  Copyright(c)2003-2017 北京天智祥信息科技有限公司(http://www.tianzhixiang.net)
//  All rights reserved.
//
//	@file       TFF_MathUtils.h
//	@brief		TFF_MathUtils header file
//	@author		gaoyang
//	@date		2017-02-22
//  @version 
//    - v1.0	2017-02-22	gaoyang	 
//
////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <boost/any.hpp>
#ifndef TFF_INLINE_SPECIFIER
#define TFF_INLINE_SPECIFIER inline
#endif
// Float error value.
#define FLOAT_ERROR 1e-6  

/** @class MathUtils
	@brief The tool class of math operation.
	@note		
	@attention	
*/
class  MathUtils
{
public:
	/// Whether floating number is equal to zero.
    TFF_INLINE_SPECIFIER static bool IsZero(double data);
	/// Whether floating number is greater than zero.
    TFF_INLINE_SPECIFIER static bool IsGreaterThanZero(double data);
	/// Whether floating number is less than zero.
    TFF_INLINE_SPECIFIER static bool IsLessThanZero(double data);
	/// Whether two floating number is equal.
    TFF_INLINE_SPECIFIER static bool IsEqual(double data1, double data2);
	/// Whether the floating number data1 is greater than data2.
    TFF_INLINE_SPECIFIER static bool IsGreater(double data1, double data2);
	/// Whether the floating number data1 is greater than data2 or equal to data2.
    TFF_INLINE_SPECIFIER static bool IsGreaterEqual(double data1, double data2);
	/// Whether the floating number data1 is less than data2.
    TFF_INLINE_SPECIFIER static bool IsLess(double data1, double data2);
	/// Whether the floating number data1 is less than data2 or equal to data2.
    TFF_INLINE_SPECIFIER static bool IsLessEqual(double data1, double data2);
	///
    TFF_INLINE_SPECIFIER static void GetIntFromDouble(double fValue, int& iValue);
    TFF_INLINE_SPECIFIER static void GetIntFromDouble(double fValue, long long & iValue);

    /**
    * 把浮点数按照单位和精度做整数化
    *
    * 注意：unit应该是1, 10, 100... 的形式，而prec指在unit下保留几位小数的精度
    */
    TFF_INLINE_SPECIFIER static long long  GetIntFromDouble(double fValue, const std::pair<int, int>& multiplier);
    TFF_INLINE_SPECIFIER static long long  GetIntFromDouble(double fValue, const int& unit, const int& pre);
	///
    template<typename T>
    TFF_INLINE_SPECIFIER static long long  GetInt64FromData(T src, const std::pair<int, int>& m);
    template<typename T>
    TFF_INLINE_SPECIFIER static long long  GetInt64FromData(const boost::any& src, T, const std::pair<int, int>& m);
	///
    template<typename T>
    TFF_INLINE_SPECIFIER static void FromInt64(const long long & src, boost::any& dst, T v, const std::pair<int, int>& divFactor);

};

//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Public:
bool MathUtils::IsZero(double data)
{ 
	return std::abs(data) <= FLOAT_ERROR;
}

bool MathUtils::IsGreaterThanZero(double data)
{ 
	return data > FLOAT_ERROR; 
}

bool MathUtils::IsLessThanZero(double data)
{ 
	return data < -1 * FLOAT_ERROR; 
}

bool MathUtils::IsEqual(double data1, double data2)
{
	return IsZero(data1 - data2);
}

bool MathUtils::IsGreater(double data1, double data2) 
{ 
	return IsGreaterThanZero(data1 - data2);
}

bool MathUtils::IsGreaterEqual(double data1, double data2) 
{ 
	return !IsLessThanZero(data1 - data2); 
}

bool MathUtils::IsLess(double data1, double data2)
{ 
	return IsLessThanZero(data1 - data2); 
}

bool MathUtils::IsLessEqual(double data1, double data2) 
{ 
	return !IsGreaterThanZero(data1 - data2); 
}

void MathUtils::GetIntFromDouble(double fValue, int& iValue)
{ 
	iValue = (int)(fValue + FLOAT_ERROR);
}

void MathUtils::GetIntFromDouble(double fValue, long long & iValue)
{ 
	iValue = (long long )(fValue + FLOAT_ERROR);
}

long long  MathUtils::GetIntFromDouble(double fValue, const std::pair<int, int>& multiplier) 
{ 
	if (fValue < 0)
	{
		return (long long )(((fValue) * multiplier.first / multiplier.second ) - 0.5);
	}        
	return (long long )(0.5 + ((fValue) * multiplier.first / multiplier.second));
}

long long  MathUtils::GetIntFromDouble(double fValue, const int& unit, const int& pre)
{ 
	return (long long )(fValue);
}

template<typename T>
long long  MathUtils::GetInt64FromData(T src, const std::pair<int, int>& m)
{
	if (src < 0)
	{
		return (long long )(((src) * m.first / m.second ) - 0.5);
	}        
	return (long long )(0.5 + ((src) * m.first / m.second));
}

template<typename T>
long long  MathUtils::GetInt64FromData(const boost::any& src, T, const std::pair<int, int>& m)
{       
	T d = boost::any_cast<const T>(src);
	if (d < 0)
	{
		return (long long )((d * m.first / m.second ) - 0.5);
	}        
	return (long long )(0.5 + (d * m.first / m.second));
}

template<typename T>
void MathUtils::FromInt64(const long long & src, boost::any& dst, T v, const std::pair<int, int>& divFactor)
{
	dst = (T)(v * src * divFactor.second / divFactor.first);
}


