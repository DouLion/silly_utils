#pragma once

#include <cmath>
#include <algorithm>
#ifndef TFF_INLINE_SPECIFIER
#define TFF_INLINE_SPECIFIER inline
#endif
namespace common {

/** @class Double2
	@brief 2-component double vector.
	@remark
*/
class  Double2
{
public:
	/// Default constructor. NOTE! No initialize conponents.
	TFF_INLINE_SPECIFIER Double2();
	/// Copy constructor.
	TFF_INLINE_SPECIFIER Double2(const Double2& rhv);
	/// Construct from given x,y.
	TFF_INLINE_SPECIFIER Double2(const double x, const double y);

	/// Assign operator.
	TFF_INLINE_SPECIFIER void operator = (const Double2& rhv);
	/// Equal operator.
	TFF_INLINE_SPECIFIER bool operator == (const Double2& rhv) const;
	/// Less operator
	TFF_INLINE_SPECIFIER bool operator < (const Double2& rhv) const;
	/// Inequal operator.
	TFF_INLINE_SPECIFIER bool operator != (const Double2& rhv) const;
	/// Flip assign.
	TFF_INLINE_SPECIFIER Double2 operator - () const;
	/// Inplace plus.
	TFF_INLINE_SPECIFIER void operator += (const Double2& rhv);
	/// Inplace sub.
	TFF_INLINE_SPECIFIER void operator -= (const Double2& rhv);
	/// Inplace scaler multiply.
	TFF_INLINE_SPECIFIER void operator *= (const double s);
	/// Plus operator.
	TFF_INLINE_SPECIFIER Double2 operator + (const Double2& rhv) const;
	/// Substract operator.
	TFF_INLINE_SPECIFIER Double2 operator - (const Double2& rhv) const;
	/// Scaler multiply operator.
	TFF_INLINE_SPECIFIER Double2 operator * (const double s) const;

	/// Set conponents.
	TFF_INLINE_SPECIFIER void Set(const double x, const double y);
	/// Read/write access to x.
	TFF_INLINE_SPECIFIER double& X();
	/// Read/write access to y.
	TFF_INLINE_SPECIFIER double& Y();
	/// Read only access to x.
	TFF_INLINE_SPECIFIER double X() const;
	/// Read only access to y.
	TFF_INLINE_SPECIFIER double Y() const;

	/// Get length of vector.
	TFF_INLINE_SPECIFIER double Length() const;
	/// Get square length.
	TFF_INLINE_SPECIFIER double LenthSq() const;
	/// Get a component-wise absolute vector.
	TFF_INLINE_SPECIFIER Double2 Abs() const;
	/// Return true if any component is non-zero.
	TFF_INLINE_SPECIFIER bool IfAnyNonZero() const;
	/// Return true if all component are non-zero.
	TFF_INLINE_SPECIFIER bool IfAllNonZero() const;

	/// Return vector made up of max value of two.
	TFF_INLINE_SPECIFIER static Double2 Maximize(const Double2& val1, const Double2& val2);
	/// Return vector made up of min value of two.
	TFF_INLINE_SPECIFIER static Double2 Minimize(const Double2& val1, const Double2& val2);
	/// Return normalized vertion.
	TFF_INLINE_SPECIFIER static Double2 Normalize(const Double2& val);
	/// Set less than components to non-zero.
	TFF_INLINE_SPECIFIER static Double2 Less(const Double2& val1, const Double2& val2);
	/// Set less equal components to non-zero.
	TFF_INLINE_SPECIFIER static Double2 LessEqual(const Double2& val1, const Double2& val2);
	/// Set greater than components to non-zero.
	TFF_INLINE_SPECIFIER static Double2 Greater(const Double2& val1, const Double2& val2);
	/// Set greater equal components to non-zero.
	TFF_INLINE_SPECIFIER static Double2 GreaterEqual(const Double2& val1, const Double2& val2);

protected:
	/// X value.
	double										m_x;
	/// Y value
	double										m_y;

};

//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Public:
Double2::Double2() :
	m_x(0.0),
	m_y(0.0)
{
	// Empty.
}

Double2::Double2(const Double2& rhv) :
	m_x(rhv.m_x), 
	m_y(rhv.m_y)
{
	// Empty.
}

Double2::Double2(const double x, const double y) :
	m_x(x), 
	m_y(y)
{
	// Empty.
}

void Double2::operator = (const Double2& rhv)
{
	m_x = rhv.m_x;
	m_y = rhv.m_y;
}

bool Double2::operator == (const Double2& rhv) const
{
	return (m_x == rhv.m_x) && (m_y == rhv.m_y);
}

bool Double2::operator < (const Double2& rhv) const
{
	char buff1[20] = {0};
	char buff2[20] = {0};
	sprintf(buff1, "%.4f,%.4f", this->m_x, this->m_y);
	sprintf(buff2, "%.4f,%.4f", rhv.m_x, rhv.m_y);
	//return (m_x < rhv.m_x) ? true : (m_y < rhv.m_y);
	return strcmp(buff1, buff2) < 0 ? true : false;
}

bool Double2::operator != (const Double2& rhv) const
{
	return (m_x != rhv.m_x) || (m_y != rhv.m_y);
}

Double2 Double2::operator - () const
{
	return Double2(-m_x, -m_y);
}

void Double2::operator += (const Double2& rhv)
{
	m_x += rhv.m_x;
	m_y += rhv.m_y;
}

void Double2::operator -= (const Double2& rhv)
{
	m_x -= rhv.m_x;
	m_y -= rhv.m_y;
}

void Double2::operator *= (const double s)
{
	m_x *= s;
	m_y *= s;
}

Double2 Double2::operator + (const Double2& rhv) const
{
	return Double2(m_x + rhv.m_x, m_y + rhv.m_y);
}

Double2 Double2::operator - (const Double2& rhv) const
{
	return Double2(m_x - rhv.m_x, m_y - rhv.m_y);
}

Double2 Double2::operator * (const double s) const
{
	return Double2(m_x * s, m_y * s);
}

void Double2::Set(const double x, const double y)
{
	m_x = x;
	m_y = y;
}

double& Double2::X()
{
	return m_x;
}

double& Double2::Y()
{
	return m_y;
}

double Double2::X() const
{
	return m_x;
}

double Double2::Y() const
{
	return m_y;
}

double Double2::Length() const
{
	return sqrt(m_x * m_x + m_y * m_y);
}

double Double2::LenthSq() const
{
	return m_x * m_x + m_y * m_y;
}

Double2 Double2::Abs() const
{
	return Double2(std::fabs(m_x), std::fabs(m_y));
}

bool Double2::IfAnyNonZero() const
{
	return (m_x != 0.0f) || (m_y != 0.0f);
}

bool Double2::IfAllNonZero() const
{
	return (m_x != 0.0f) && (m_y != 0.0f);
}

Double2 Double2::Maximize(const Double2& val1, const Double2& val2)
{
	return Double2(std::max(val1.m_x, val2.m_x), std::max(val1.m_y, val2.m_y));
}

Double2 Double2::Minimize(const Double2& val1, const Double2& val2)
{
	return Double2(std::min(val1.m_x, val2.m_x), std::min(val1.m_y, val2.m_y));
}

Double2 Double2::Normalize(const Double2& val)
{
	double l = val.Length();
	if (l > 0.0f)
	{
		return Double2(val.m_x / l, val.m_y / l);
	}
	else
	{
		return Double2(1.0f, 0.0f);
	}
}

Double2 Double2::Less(const Double2& val1, const Double2& val2)
{
	Double2 res;
	res.m_x = (val1.m_x < val2.m_x) ? 1.0f : 0.0f;
	res.m_y = (val1.m_y < val2.m_y) ? 1.0f : 0.0f;
	return res;
}

Double2 Double2::LessEqual(const Double2& val1, const Double2& val2)
{
	Double2 res;
	res.m_x = (val1.m_x <= val2.m_x) ? 1.0f : 0.0f;
	res.m_y = (val1.m_y <= val2.m_y) ? 1.0f : 0.0f;
	return res;
}

Double2 Double2::Greater(const Double2& val1, const Double2& val2)
{
	Double2 res;
	res.m_x = (val1.m_x > val2.m_x) ? 1.0f : 0.0f;
	res.m_y = (val1.m_y > val2.m_y) ? 1.0f : 0.0f;
	return res;
}

Double2 Double2::GreaterEqual(const Double2& val1, const Double2& val2)
{
	Double2 res;
	res.m_x = (val1.m_x >= val2.m_x) ? 1.0f : 0.0f;
	res.m_y = (val1.m_y >= val2.m_y) ? 1.0f : 0.0f;
	return res;
}

}
