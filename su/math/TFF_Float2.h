////////////////////////////////////////////////////////////////////////////////////
//
//  Copyright(c)2003-2017 北京天智祥信息科技有限公司(http://www.tianzhixiang.net)
//  All rights reserved.
//
//	@file       TFF_Float2.h
//	@brief		TFF_Float2 header file
//	@author		gaoyang
//	@date		2017-02-27
//  @version	
//    - v1.0	2017-02-27	gaoyang		 
//
////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "marco.h"
#ifndef TFF_INLINE_SPECIFIER
#define TFF_INLINE_SPECIFIER inline
#endif
namespace silly_utils {
	/** @class Float2
		@brief 2-component float vector.
		@remark
	*/
	class  Float2
	{
	public:
		/// Default constructor. NOTE! No initialize conponents.
		TFF_INLINE_SPECIFIER Float2();
		/// Copy constructor.
		TFF_INLINE_SPECIFIER Float2(const Float2& rhv);
		/// Construct from given x,y.
		TFF_INLINE_SPECIFIER Float2(const float x, const float y);

		/// Assign operator.
		TFF_INLINE_SPECIFIER void operator = (const Float2& rhv);
		/// Equal operator.
		TFF_INLINE_SPECIFIER bool operator == (const Float2& rhv) const;
		/// Inequal operator.
		TFF_INLINE_SPECIFIER bool operator != (const Float2& rhv) const;
		/// Flip assign.
		TFF_INLINE_SPECIFIER Float2 operator - () const;
		/// Inplace plus.
		TFF_INLINE_SPECIFIER void operator += (const Float2& rhv);
		/// Inplace sub.
		TFF_INLINE_SPECIFIER void operator -= (const Float2& rhv);
		/// Inplace scaler multiply.
		TFF_INLINE_SPECIFIER void operator *= (const float s);
		/// Plus operator.
		TFF_INLINE_SPECIFIER Float2 operator + (const Float2& rhv) const;
		/// Substract operator.
		TFF_INLINE_SPECIFIER Float2 operator - (const Float2& rhv) const;
		/// Scaler multiply operator.
		TFF_INLINE_SPECIFIER Float2 operator * (const float s) const;

		/// Set conponents.
		TFF_INLINE_SPECIFIER void Set(const float x, const float y);
		/// Read/write access to x.
		TFF_INLINE_SPECIFIER float& X();
		/// Read/write access to y.
		TFF_INLINE_SPECIFIER float& Y();
		/// Read only access to x.
		TFF_INLINE_SPECIFIER float X() const;
		/// Read only access to y.
		TFF_INLINE_SPECIFIER float Y() const;

		/// Get length of vector.
		TFF_INLINE_SPECIFIER float Length() const;
		/// Get square length.
		TFF_INLINE_SPECIFIER float LenthSq() const;
		/// Get a component-wise absolute vector.
		TFF_INLINE_SPECIFIER Float2 Abs() const;
		/// Return true if any component is non-zero.
		TFF_INLINE_SPECIFIER bool IfAnyNonZero() const;
		/// Return true if all component are non-zero.
		TFF_INLINE_SPECIFIER bool IfAllNonZero() const;

		/// Return vector made up of max value of two.
		TFF_INLINE_SPECIFIER static Float2 Maximize(const Float2& val1, const Float2& val2);
		/// Return vector made up of min value of two.
		TFF_INLINE_SPECIFIER static Float2 Minimize(const Float2& val1, const Float2& val2);
		/// Return normalized vertion.
		TFF_INLINE_SPECIFIER static Float2 Normalize(const Float2& val);
		/// Set less than components to non-zero.
		TFF_INLINE_SPECIFIER static Float2 Less(const Float2& val1, const Float2& val2);
		/// Set less equal components to non-zero.
		TFF_INLINE_SPECIFIER static Float2 LessEqual(const Float2& val1, const Float2& val2);
		/// Set greater than components to non-zero.
		TFF_INLINE_SPECIFIER static Float2 Greater(const Float2& val1, const Float2& val2);
		/// Set greater equal components to non-zero.
		TFF_INLINE_SPECIFIER static Float2 GreaterEqual(const Float2& val1, const Float2& val2);

	protected:
		/// X value.
		float				m_x;
		/// Y value.
		float				m_y;

	};

	//------------------------------------------------------------------------------
	// Implementation:
	//------------------------------------------------------------------------------
	// Public:
	Float2::Float2() :
		m_x(0.0f),
		m_y(0.0f)
	{
		// Empty.
	}

	Float2::Float2(const Float2& rhv) :
		m_x(rhv.m_x),
		m_y(rhv.m_y)
	{
		// Empty.
	}

	Float2::Float2(const float x, const float y) :
		m_x(x),
		m_y(y)
	{
		// Empty.
	}

	void Float2::operator = (const Float2& rhv)
	{
		m_x = rhv.m_x;
		m_y = rhv.m_y;
	}

	bool Float2::operator == (const Float2& rhv) const
	{
		return (m_x == rhv.m_x) && (m_y == rhv.m_y);
	}

	bool Float2::operator != (const Float2& rhv) const
	{
		return (m_x != rhv.m_x) || (m_y != rhv.m_y);
	}

	Float2 Float2::operator - () const
	{
		return Float2(-m_x, -m_y);
	}

	void Float2::operator += (const Float2& rhv)
	{
		m_x += rhv.m_x;
		m_y += rhv.m_y;
	}

	void Float2::operator -= (const Float2& rhv)
	{
		m_x -= rhv.m_x;
		m_y -= rhv.m_y;
	}

	void Float2::operator *= (const float s)
	{
		m_x *= s;
		m_y *= s;
	}

	Float2 Float2::operator + (const Float2& rhv) const
	{
		return Float2(m_x + rhv.m_x, m_y + rhv.m_y);
	}

	Float2 Float2::operator - (const Float2& rhv) const
	{
		return Float2(m_x - rhv.m_x, m_y - rhv.m_y);
	}

	Float2 Float2::operator * (const float s) const
	{
		return Float2(m_x * s, m_y * s);
	}

	void Float2::Set(const float x, const float y)
	{
		m_x = x;
		m_y = y;
	}

	float& Float2::X()
	{
		return m_x;
	}

	float& Float2::Y()
	{
		return m_y;
	}

	float Float2::X() const
	{
		return m_x;
	}

	float Float2::Y() const
	{
		return m_y;
	}

	float Float2::Length() const
	{
		return std::sqrt(m_x * m_x + m_y * m_y);
	}

	float Float2::LenthSq() const
	{
		return m_x * m_x + m_y * m_y;
	}

	Float2 Float2::Abs() const
	{
		return Float2(fabs(m_x), fabs(m_y));
	}

	bool Float2::IfAnyNonZero() const
	{
		return (m_x != 0.0f) || (m_y != 0.0f);
	}

	bool Float2::IfAllNonZero() const
	{
		return (m_x != 0.0f) && (m_y != 0.0f);
	}

	Float2 Float2::Maximize(const Float2& val1, const Float2& val2)
	{
		return Float2(std::max(val1.m_x, val2.m_x), std::max(val1.m_y, val2.m_y));
	}

	Float2 Float2::Minimize(const Float2& val1, const Float2& val2)
	{
		return Float2(std::min(val1.m_x, val2.m_x), std::min(val1.m_y, val2.m_y));
	}

	Float2 Float2::Normalize(const Float2& val)
	{
		float l = val.Length();
		if (l > 0.0f)
		{
			return Float2(val.m_x / l, val.m_y / l);
		}
		else
		{
			return Float2(1.0f, 0.0f);
		}
	}

	Float2 Float2::Less(const Float2& val1, const Float2& val2)
	{
		Float2 res;
		res.m_x = (val1.m_x < val2.m_x) ? 1.0f : 0.0f;
		res.m_y = (val1.m_y < val2.m_y) ? 1.0f : 0.0f;
		return res;
	}

	Float2 Float2::LessEqual(const Float2& val1, const Float2& val2)
	{
		Float2 res;
		res.m_x = (val1.m_x <= val2.m_x) ? 1.0f : 0.0f;
		res.m_y = (val1.m_y <= val2.m_y) ? 1.0f : 0.0f;
		return res;
	}

	Float2 Float2::Greater(const Float2& val1, const Float2& val2)
	{
		Float2 res;
		res.m_x = (val1.m_x > val2.m_x) ? 1.0f : 0.0f;
		res.m_y = (val1.m_y > val2.m_y) ? 1.0f : 0.0f;
		return res;
	}

	Float2 Float2::GreaterEqual(const Float2& val1, const Float2& val2)
	{
		Float2 res;
		res.m_x = (val1.m_x >= val2.m_x) ? 1.0f : 0.0f;
		res.m_y = (val1.m_y >= val2.m_y) ? 1.0f : 0.0f;
		return res;
	}

}