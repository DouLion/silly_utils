#pragma once
#include "common/math/TFF_Float4.h"
namespace common {

/** @class Vector
 	@brief A vector in homogenous space. A vector represents a direction and lengh in 3d space and it's
	W component is alwas 0.0f.
	@remark
*/
class  Vector : public Float4
{
public:
	/// Default constructor.
	TFF_INLINE_SPECIFIER Vector();
	/// Copy constructor.
	TFF_INLINE_SPECIFIER Vector(const Vector& rhv);
	/// Construct from given Float4.
	TFF_INLINE_SPECIFIER Vector(const Float4& rhv);
	/// Construct from given values.
	TFF_INLINE_SPECIFIER Vector(float x, float y, float z);

	/// Assign operator.
	TFF_INLINE_SPECIFIER void operator = (const Vector& rhv);
	/// Flip assign.
	TFF_INLINE_SPECIFIER Vector operator - () const;
	/// Inplace add operator.
	TFF_INLINE_SPECIFIER void operator += (const Vector& v);
	/// Inplace substract operator.
	TFF_INLINE_SPECIFIER void operator -= (const Vector& v);
	/// Inplace multiply operator by given scaler.
	TFF_INLINE_SPECIFIER void operator *= (float scaler);
	/// Add operator.
	TFF_INLINE_SPECIFIER Vector operator + (const Vector& v) const;
	/// Substract operator.
	TFF_INLINE_SPECIFIER Vector operator - (const Vector& v) const;
	/// Multiply operator by scaler.
	TFF_INLINE_SPECIFIER Vector operator * (float scaler) const;

	/// Set values.
	TFF_INLINE_SPECIFIER void Set(float x, float y, float z);
	/// Return null vector.
	TFF_INLINE_SPECIFIER static Vector NullVec();
	/// Return up vector.
	TFF_INLINE_SPECIFIER static Vector UpVec();
	/// Return right vector.
	TFF_INLINE_SPECIFIER static Vector RightVec();
	/// Return forward vector.
	TFF_INLINE_SPECIFIER static Vector ForwardVec();

	friend class Point;
};

//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Public:
Vector::Vector() :
	Float4(0.0f, 0.0f, 0.0f, 0.0f)
{

}

Vector::Vector(const Vector& rhv) :
	Float4(rhv.m_x, rhv.m_y, rhv.m_z, 0.0f)
{

}

Vector::Vector(const Float4& rhv) :
	Float4(rhv)
{
	m_w = 0.0f;
}

Vector::Vector(float x, float y, float z) :
	Float4(x, y, z, 0.0f)
{

}


void Vector::operator = (const Vector& rhv)
{
	m_x = rhv.m_x;
	m_y = rhv.m_y;
	m_z = rhv.m_z;
	m_w = 0.0f;
}

Vector Vector::operator - () const
{
	return Vector(-m_x, -m_y, -m_z);
}

void Vector::operator += (const Vector& v)
{
	m_x += v.m_x;
	m_y += v.m_y;
	m_z += v.m_z;
}

void Vector::operator -= (const Vector& v)
{
	m_x -= v.m_x;
	m_y -= v.m_y;
	m_z -= v.m_z;
}

void Vector::operator *= (float scaler)
{
	m_x *= scaler;
	m_y *= scaler;
	m_z *= scaler;
}

Vector Vector::operator + (const Vector& v) const
{
	return Vector(m_x + v.m_x, m_y + v.m_y, m_z + v.m_z);
}

Vector Vector::operator - (const Vector& v) const
{
	return Vector(m_x - v.m_x, m_y - v.m_y, m_z - v.m_z);
}

Vector Vector::operator * (float scaler) const
{
	return Vector(m_x * scaler, m_y * scaler, m_z * scaler);
}


void Vector::Set(float x, float y, float z)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_w = 0.0f;
}

Vector Vector::NullVec()
{
	return Vector(0.0f, 0.0f, 0.0f);
}

Vector Vector::UpVec()
{
	return Vector(0.0f, 1.0f, 0.0f);
}

Vector Vector::RightVec()
{
	return Vector(1.0f, 0.0f, 0.0f);
}

Vector Vector::ForwardVec()
{
	return Vector(0.0f, 0.0f, 1.0f);
}

}
