#pragma once
#include "math/TFF_Vector.h"
namespace common {

/** @class Point
	@brief A point in homogenous space. A point describes a position in space,
	and has its W component set to 1.0.
	@remark
*/
class  Point : public Float4
{
public:
	/// Default constructor.
	TFF_INLINE_SPECIFIER Point();
	/// Copy constructor.
	TFF_INLINE_SPECIFIER Point(const Point& rhv);
	/// Construct from Float4.
	TFF_INLINE_SPECIFIER Point(const Float4& rhv);
	/// Construct from given values.
	TFF_INLINE_SPECIFIER Point(float x, float y, float z);

	/// Assign operator.
	TFF_INLINE_SPECIFIER void operator = (const Point& rhv);
	/// Inplace add operator with given Vector.
	TFF_INLINE_SPECIFIER void operator += (const Vector& v);
	/// Inplace substract operator with given Vector.
	TFF_INLINE_SPECIFIER void operator -= (const Vector& v);
	/// Add operator from given Vector.
	TFF_INLINE_SPECIFIER Point operator + (const Vector& v) const;
	/// Substract operator from given Vector.
	TFF_INLINE_SPECIFIER Point operator - (const Vector& v) const;
	/// Substract operator from given Point into a Vector.
	TFF_INLINE_SPECIFIER Vector operator - (const Point& p) const;
	/// Equal operator.
	TFF_INLINE_SPECIFIER bool operator == (const Point& rhv) const;
	/// Inequal operator.
	TFF_INLINE_SPECIFIER bool operator != (const Point& rhv) const;

	/// Set value.
	TFF_INLINE_SPECIFIER void Set(float x, float y, float z);
	/// Return an origin point.
	TFF_INLINE_SPECIFIER static Point Origin();
};

//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Public:
Point::Point() :
	Float4(0.0f, 0.0f, 0.0f, 1.0f)
{

}

Point::Point(const Point& rhv) :
	Float4(rhv.m_x, rhv.m_y, rhv.m_z, 1.0f)
{

}

Point::Point(const Float4& rhv) :
	Float4(rhv)
{
	m_w	= 1.0f;
}

Point::Point(float x, float y, float z) :
	Float4(x, y, z, 1.0f)
{

}


void Point::operator = (const Point& rhv)
{
	m_x = rhv.m_x;
	m_y = rhv.m_y;
	m_z = rhv.m_z;
	m_w	= 1.0f;
}

void Point::operator += (const Vector& v)
{
	m_x += v.m_x;
	m_y += v.m_y;
	m_z += v.m_z;
}

void Point::operator -= (const Vector& v)
{
	m_x -= v.m_x;
	m_y -= v.m_y;
	m_z -= v.m_z;
}

Point Point::operator + (const Vector& v) const
{
	return Point(m_x + v.m_x, m_y + v.m_y, m_z + v.m_z);
}

Point Point::operator - (const Vector& v) const
{
	return Point(m_x - v.m_x, m_y - v.m_y, m_z - v.m_z);
}

Vector Point::operator - (const Point& p) const
{
	return Vector(m_x - p.m_x, m_y - p.m_y, m_z - p.m_z);
}

bool Point::operator == (const Point& rhv) const
{
	return m_x == rhv.m_x && m_y == rhv.m_y && m_z == rhv.m_z;
}

bool Point::operator != (const Point& rhv) const
{
	return m_x != rhv.m_x || m_y != rhv.m_y || m_z != rhv.m_z;
}


void Point::Set(float x, float y, float z)
{
	Float4::Set(x, y, z, 1.0f);
}


Point Point::Origin()
{
	return Point(0.0f, 0.0f, 0.0f);
}

}
