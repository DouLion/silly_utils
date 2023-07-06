#pragma once


#include <cmath>

namespace common {

//class Matrix44;

/** @class Float4
 	@brief Float4 class implements float 4 vector caculations on top of D3DX9 functions.
	@remark
*/
class  Float4
{
public:
	typedef char cmpresult;

	/// Default constructor.
	TFF_INLINE_SPECIFIER Float4();
	/// Construct from given values.
	TFF_INLINE_SPECIFIER Float4(float x, float y, float z, float w);
	/// Copy constructor.
	TFF_INLINE_SPECIFIER Float4(const Float4& rhv);

	/// Assign operator.
	TFF_INLINE_SPECIFIER void operator = (const Float4& rhv);
	/// Flip sign.
	TFF_INLINE_SPECIFIER Float4 operator - () const;
	/// Add assign operator.
	TFF_INLINE_SPECIFIER void operator += (const Float4& rhv);
	/// Substract assign operator.
	TFF_INLINE_SPECIFIER void operator -= (const Float4& rhv);
	/// Multiply assign operator.
	TFF_INLINE_SPECIFIER void operator *= (float scaler);
	/// Add operator.
	TFF_INLINE_SPECIFIER Float4 operator + (const Float4& rhv) const;
	/// Substract operator.
	TFF_INLINE_SPECIFIER Float4 operator - (const Float4& rhv) const;
	/// Multiply operator.
	TFF_INLINE_SPECIFIER Float4 operator * (float scaler) const;
	/// Equal operator.
	TFF_INLINE_SPECIFIER bool operator == (const Float4& rhv) const;
	/// Inequal operator.
	TFF_INLINE_SPECIFIER bool operator != (const Float4& rhv) const;

	/// Load from 16 byte aligned buffer.
	TFF_INLINE_SPECIFIER void LoadAligned(const float* ptr);
	/// Load from 16 byte unaligned buffer.
	TFF_INLINE_SPECIFIER void LoadUnaligned(const float* ptr);
	/// Write into 16 byte aligned buffer.
	TFF_INLINE_SPECIFIER void StoreAligned(float* ptr) const;
	/// Write into byte unaligned buffer.
	TFF_INLINE_SPECIFIER void StoreUnaligned(float* ptr) const;
	/// Stream into 16 byte aligned buffer.
	TFF_INLINE_SPECIFIER void StreamAligned(float* ptr) const;

	/// Set values.
	TFF_INLINE_SPECIFIER void Set(float x, float y, float z, float w);
	/// Read/write access to x.
	TFF_INLINE_SPECIFIER float& X();
	/// Read/write access to y.
	TFF_INLINE_SPECIFIER float& Y();
	/// Read/write access to z.
	TFF_INLINE_SPECIFIER float& Z();
	/// Read/write access to w.
	TFF_INLINE_SPECIFIER float& W();
	/// Read access to x.
	TFF_INLINE_SPECIFIER float X() const;
	/// Read access to y.
	TFF_INLINE_SPECIFIER float Y() const;
	/// Read access to z.
	TFF_INLINE_SPECIFIER float Z() const;
	/// Read access to w.
	TFF_INLINE_SPECIFIER float W() const;

	/// Return length of 4d vector.
	TFF_INLINE_SPECIFIER float Length() const;
	/// Return squae length of 4d vector.
	TFF_INLINE_SPECIFIER float LengthSq() const;
	/// Return component-wise absolute.
	TFF_INLINE_SPECIFIER Float4 Abs() const;

	/// Return reciprocal vector.
	TFF_INLINE_SPECIFIER static Float4 Reciprocal(const Float4& v);
	/// Multiply 2 given vectors.
	TFF_INLINE_SPECIFIER static Float4 Multiply(const Float4& v1, const Float4& v2);
	/// Perform 3d vector cross product vector(w is set to 0).
	TFF_INLINE_SPECIFIER static Float4 Cross3(const Float4& v1, const Float4& v2);
	/// Perform 3d vector dot product.
	TFF_INLINE_SPECIFIER static float Dot3(const Float4& v1, const Float4& v2);
	/// Return barycentric of given vectors.
	TFF_INLINE_SPECIFIER static Float4 Barycentric(const Float4& v0, const Float4& v1, const Float4& v2, float f, float g);
	/// Perform 4d catmull-rom interpolation.
	TFF_INLINE_SPECIFIER static Float4 CatmullRom(const Float4& v0, const Float4& v1, const Float4& v2, const Float4& v3, float s);
	/// Perform 4d hermite spline interpolation.
	TFF_INLINE_SPECIFIER static Float4 Hermite(const Float4& v1, const Float4& t1, const Float4& v2, const Float4& t2, float s);
	/// Perform 4d lerp of 2 vectors.
	TFF_INLINE_SPECIFIER static Float4 Lerp(const Float4& v0, const Float4& v1, float s);
	/// Return 4d vector made up of largest components of 2 vectors.
	TFF_INLINE_SPECIFIER static Float4 Maximize(const Float4& v0, const Float4& v1);
	/// Return 4d vector made up of smallest components of 2 vectors.
	TFF_INLINE_SPECIFIER static Float4 Minimize(const Float4& v0, const Float4& v1);
	/// Return normalized version.
	TFF_INLINE_SPECIFIER static Float4 Normalize(const Float4& v);
	/// Transform given vector by Matrix44.
	//static Float4 Transform(const Float4& v, const Matrix44& m);
	/// Reflect given vector from given normal vector.
	TFF_INLINE_SPECIFIER static Float4 Reflect(const Float4& v, const Float4& normal);
	/// Clamp given vector to given range.
	static Float4 Clamp(const Float4& vClamp, const Float4& vMin, const Float4& vMax);
	/// Return angle between two given vectors.
	static float Angle(const Float4& v0, const Float4& v1);

	//-------------------------------
	// Compare methods.
	/// Perform less comparison.
	TFF_INLINE_SPECIFIER static cmpresult Less3(const Float4& v0, const Float4& v1);
	/// Perform less-or-equal comparison.
	TFF_INLINE_SPECIFIER static cmpresult LessEqual3(const Float4& v0, const Float4& v1);
	/// Perform greater-then comparison.
	TFF_INLINE_SPECIFIER static cmpresult Greater3(const Float4& v0, const Float4& v1);
	/// Perform greater-or-equal comparison.
	TFF_INLINE_SPECIFIER static cmpresult GreaterEqual3(const Float4& v0, const Float4& v1);
	/// Perform equal comparison.
	TFF_INLINE_SPECIFIER static cmpresult Equal3(const Float4& v0, const Float4& v1);
	/// Perform near equal comparison with given epsilon.
	TFF_INLINE_SPECIFIER static cmpresult NearEqual3(const Float4& v0, const Float4& v1, const Float4& epsilon);
	/// Check comparison result for all-condition.
	TFF_INLINE_SPECIFIER static bool AllPass3(cmpresult res);
	/// Check comparison result for any-condition.
	TFF_INLINE_SPECIFIER static bool AnyPass3(cmpresult res);

	/// Perform less comparison.
	TFF_INLINE_SPECIFIER static cmpresult Less(const Float4& v0, const Float4& v1);
	/// Perform less-or-equal comparison.
	TFF_INLINE_SPECIFIER static cmpresult LessEqual(const Float4& v0, const Float4& v1);
	/// Perform greater-then comparison.
	TFF_INLINE_SPECIFIER static cmpresult Greater(const Float4& v0, const Float4& v1);
	/// Perform greater-or-equal comparison.
	TFF_INLINE_SPECIFIER static cmpresult GreaterEqual(const Float4& v0, const Float4& v1);
	/// Perform equal comparison.
	TFF_INLINE_SPECIFIER static cmpresult Equal(const Float4& v0, const Float4& v1);
	/// Perform near equal comparison with given epsilon.
	TFF_INLINE_SPECIFIER static cmpresult NearEqual(const Float4& v0, const Float4& v1, const Float4& epsilon);
	/// Check comparison result for all-condition.
	TFF_INLINE_SPECIFIER static bool AllPass(cmpresult res);
	/// Check comparison result for any-condition.
	TFF_INLINE_SPECIFIER static bool AnyPass(cmpresult res);

protected:
	//friend class Matrix44;

	float				m_x;
	float				m_y;
	float				m_z;
	float				m_w;
};

//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Public:
Float4::Float4()
{
	// Empty.
}

Float4::Float4(float x, float y, float z, float w) :
	m_x(x), 
	m_y(y), 
	m_z(z), 
	m_w(w)
{

}

Float4::Float4(const Float4& rhv) :
	m_x(rhv.m_x),
	m_y(rhv.m_y),
	m_z(rhv.m_z),
	m_w(rhv.m_w)
{

}


void Float4::operator = (const Float4& rhv)
{
	m_x = rhv.m_x;
	m_y = rhv.m_y;
	m_z = rhv.m_z;
	m_w = rhv.m_w;
}

Float4 Float4::operator - () const
{
	return Float4(-m_x, -m_y, -m_z, -m_w);
}

void Float4::operator += (const Float4& rhv)
{
	m_x += rhv.m_x;
	m_y += rhv.m_y;
	m_z += rhv.m_z;
	m_w += rhv.m_w;
}

void Float4::operator -= (const Float4& rhv)
{
	m_x -= rhv.m_x;
	m_y -= rhv.m_y;
	m_z -= rhv.m_z;
	m_w -= rhv.m_w;
}

void Float4::operator *= (float scaler)
{
	m_x *= scaler;
	m_y *= scaler;
	m_z *= scaler;
	m_w *= scaler;
}

Float4 Float4::operator + (const Float4& rhv) const
{
	return Float4(m_x + rhv.m_x, m_y + rhv.m_y, m_z + rhv.m_z, m_w + rhv.m_w);
}

Float4 Float4::operator - (const Float4& rhv) const
{
	return Float4(m_x - rhv.m_x, m_y - rhv.m_y, m_z - rhv.m_z, m_w - rhv.m_w);
}

Float4 Float4::operator * (float scaler) const
{
	return Float4(m_x * scaler, m_y * scaler, m_z * scaler, m_w * scaler);
}

bool Float4::operator == (const Float4& rhv) const
{
	return (m_x == rhv.m_x) && (m_y == rhv.m_y) && (m_z == rhv.m_z) && (m_w == rhv.m_w);
}

bool Float4::operator != (const Float4& rhv) const
{
	return (m_x != rhv.m_x) || (m_y != rhv.m_y) || (m_z != rhv.m_z) || (m_w != rhv.m_w);
}


void Float4::LoadAligned(const float* ptr)
{
	m_x = ptr[0];
	m_y = ptr[1];
	m_z = ptr[2];
	m_w = ptr[3];
}

void Float4::LoadUnaligned(const float* ptr)
{
	m_x = ptr[0];
	m_y = ptr[1];
	m_z = ptr[2];
	m_w = ptr[3];
}

void Float4::StoreAligned(float* ptr) const
{
	ptr[0] = m_x;
	ptr[1] = m_y;
	ptr[2] = m_z;
	ptr[3] = m_w;
}

void Float4::StoreUnaligned(float* ptr) const
{
	ptr[0] = m_x;
	ptr[1] = m_y;
	ptr[2] = m_z;
	ptr[3] = m_w;
}

void Float4::StreamAligned(float* ptr) const
{
	ptr[0] = m_x;
	ptr[1] = m_y;
	ptr[2] = m_z;
	ptr[3] = m_w;
}


void Float4::Set(float x, float y, float z, float w)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_w = w;
}

float& Float4::X() 
{
	return m_x;
}

float& Float4::Y() 
{
	return m_y;
}

float& Float4::Z() 
{
	return m_z;
}

float& Float4::W() 
{
	return m_w;
}

float Float4::X() const
{
	return m_x;
}

float Float4::Y() const
{
	return m_y;
}

float Float4::Z() const
{
	return m_z;
}

float Float4::W() const
{
	return m_w;
}


float Float4::Length() const
{
	//return D3DXVec4Length((CONST D3DXVECTOR4*)this);
	return 0;
}

float Float4::LengthSq() const
{
	//return D3DXVec4LengthSq((CONST D3DXVECTOR4*)this);
	return 0;
}

Float4 Float4::Abs() const
{
	return Float4(std::abs(m_x), std::abs(m_y), std::abs(m_z), std::abs(m_w));
}


Float4 Float4::Reciprocal(const Float4& v)
{
	return Float4(1.0f / v.m_x, 1.0f / v.m_y, 1.0f / v.m_z, 1.0f / v.m_w);
}

Float4 Float4::Multiply(const Float4& v1, const Float4& v2)
{
	return Float4(v1.m_x * v2.m_x, v1.m_y * v2.m_y, v1.m_z * v2.m_z, v1.m_w * v2.m_w);
}

Float4 Float4::Cross3(const Float4& v1, const Float4& v2)
{
	Float4 res;
	//D3DXVec3Cross((D3DXVECTOR3*)&res, (D3DXVECTOR3*)&v1, (D3DXVECTOR3*)&v2);
	//res.m_w = 0.0f;
	return res;
}

float Float4::Dot3(const Float4& v1, const Float4& v2)
{
	float res = 0.0f;
	//res = D3DXVec3Dot((CONST D3DXVECTOR3*)&v1, (CONST D3DXVECTOR3*)&v2);
	return res;
}

Float4 Float4::Barycentric(const Float4& v0, const Float4& v1, const Float4& v2, float f, float g)
{
	Float4 res;
	//D3DXVec4BaryCentric((D3DXVECTOR4*)&res, (CONST D3DXVECTOR4*)&v0, (CONST D3DXVECTOR4*)&v1, (CONST D3DXVECTOR4*)&v2, f, g);
	return res;
}

Float4 Float4::CatmullRom(const Float4& v0, const Float4& v1, const Float4& v2, const Float4& v3, float s)
{
	Float4 res;
	//D3DXVec4CatmullRom((D3DXVECTOR4*)&res, (CONST D3DXVECTOR4*)&v0, (CONST D3DXVECTOR4*)&v1, (CONST D3DXVECTOR4*)&v2, (CONST D3DXVECTOR4*)&v3, s);
	return res;
}

Float4 Float4::Hermite(const Float4& v1, const Float4& t1, const Float4& v2, const Float4& t2, float s)
{
	Float4 res;
	//D3DXVec4Hermite((D3DXVECTOR4*)&res, (CONST D3DXVECTOR4*)&v1, (CONST D3DXVECTOR4*)&t1, (CONST D3DXVECTOR4*)&v2, (CONST D3DXVECTOR4*)&t2, s);
	return res;
}

Float4 Float4::Lerp(const Float4& v0, const Float4& v1, float s)
{
	Float4 res;
	//D3DXVec4Lerp((D3DXVECTOR4*)&res, (CONST D3DXVECTOR4*)&v0, (CONST D3DXVECTOR4*)&v1, s);
	return res;
}

Float4 Float4::Maximize(const Float4& v0, const Float4& v1)
{
	Float4 res;
	//D3DXVec4Maximize((D3DXVECTOR4*)&res, (CONST D3DXVECTOR4*)&v0, (CONST D3DXVECTOR4*)&v1);
	return res;
}

Float4 Float4::Minimize(const Float4& v0, const Float4& v1)
{
	Float4 res;
	//D3DXVec4Minimize((D3DXVECTOR4*)&res, (CONST D3DXVECTOR4*)&v0, (CONST D3DXVECTOR4*)&v1);
	return res;
}

Float4 Float4::Normalize(const Float4& v)
{
	Float4 res;
	//D3DXVec4Normalize((D3DXVECTOR4*)&res, (CONST D3DXVECTOR4*)&v);
	return res;
}

Float4 Float4::Reflect(const Float4& v, const Float4& normal)
{
	float s = 2.0f * Dot3(v, normal);
	Float4 res = v - (normal * s);
	return res;
}


Float4::cmpresult Float4::Less3(const Float4& v0, const Float4& v1)
{
	cmpresult res = 0;
	if (v0.m_x < v1.m_x) res |= (1 << 0);
	if (v0.m_y < v1.m_y) res |= (1 << 1);
	if (v0.m_z < v1.m_z) res |= (1 << 2);
	return res;
}

Float4::cmpresult Float4::LessEqual3(const Float4& v0, const Float4& v1)
{
	cmpresult res = 0;
	if (v0.m_x <= v1.m_x) res |= (1 << 0);
	if (v0.m_y <= v1.m_y) res |= (1 << 1);
	if (v0.m_z <= v1.m_z) res |= (1 << 2);
	return res;
}

Float4::cmpresult Float4::Greater3(const Float4& v0, const Float4& v1)
{
	cmpresult res = 0;
	if (v0.m_x > v1.m_x) res |= (1 << 0);
	if (v0.m_y > v1.m_y) res |= (1 << 1);
	if (v0.m_z > v1.m_z) res |= (1 << 2);
	return res;
}

Float4::cmpresult Float4::GreaterEqual3(const Float4& v0, const Float4& v1)
{
	cmpresult res = 0;
	if (v0.m_x >= v1.m_x) res |= (1 << 0);
	if (v0.m_y >= v1.m_y) res |= (1 << 1);
	if (v0.m_z >= v1.m_z) res |= (1 << 2);
	return res;
}

Float4::cmpresult Float4::Equal3(const Float4& v0, const Float4& v1)
{
	cmpresult res = 0;
	if (v0.m_x == v1.m_x) res |= (1 << 0);
	if (v0.m_y == v1.m_y) res |= (1 << 1);
	if (v0.m_z == v1.m_z) res |= (1 << 2);
	return res;
}

Float4::cmpresult Float4::NearEqual3(const Float4& v0, const Float4& v1, const Float4& epsilon)
{
	cmpresult res = 0;
	if (v0.m_x <= v1.m_x + epsilon.m_x && v1.m_x <= v0.m_x + epsilon.m_x) res |= (1 << 0);
	if (v0.m_y <= v1.m_y + epsilon.m_y && v1.m_y <= v0.m_y + epsilon.m_y) res |= (1 << 1);
	if (v0.m_z <= v1.m_z + epsilon.m_z && v1.m_z <= v0.m_z + epsilon.m_z) res |= (1 << 2);
	return res;
}

bool Float4::AllPass3(cmpresult res)
{
	return res == (1 << 0 | 1 << 1 | 1 << 2);
}

bool Float4::AnyPass3(cmpresult res)
{
	return res != 0;
}



Float4::cmpresult Float4::Less(const Float4& v0, const Float4& v1)
{
	cmpresult res = 0;
	if (v0.m_x < v1.m_x) res |= (1 << 0);
	if (v0.m_y < v1.m_y) res |= (1 << 1);
	if (v0.m_z < v1.m_z) res |= (1 << 2);
	if (v0.m_w < v1.m_w) res |= (1 << 3);
	return res;
}

Float4::cmpresult Float4::LessEqual(const Float4& v0, const Float4& v1)
{
	cmpresult res = 0;
	if (v0.m_x <= v1.m_x) res |= (1 << 0);
	if (v0.m_y <= v1.m_y) res |= (1 << 1);
	if (v0.m_z <= v1.m_z) res |= (1 << 2);
	if (v0.m_w <= v1.m_w) res |= (1 << 3);
	return res;
}

Float4::cmpresult Float4::Greater(const Float4& v0, const Float4& v1)
{
	cmpresult res = 0;
	if (v0.m_x > v1.m_x) res |= (1 << 0);
	if (v0.m_y > v1.m_y) res |= (1 << 1);
	if (v0.m_z > v1.m_z) res |= (1 << 2);
	if (v0.m_w > v1.m_w) res |= (1 << 3);
	return res;
}

Float4::cmpresult Float4::GreaterEqual(const Float4& v0, const Float4& v1)
{
	cmpresult res = 0;
	if (v0.m_x >= v1.m_x) res |= (1 << 0);
	if (v0.m_y >= v1.m_y) res |= (1 << 1);
	if (v0.m_z >= v1.m_z) res |= (1 << 2);
	if (v0.m_w >= v1.m_w) res |= (1 << 3);
	return res;
}

Float4::cmpresult Float4::Equal(const Float4& v0, const Float4& v1)
{
	cmpresult res = 0;
	if (v0.m_x == v1.m_x) res |= (1 << 0);
	if (v0.m_y == v1.m_y) res |= (1 << 1);
	if (v0.m_z == v1.m_z) res |= (1 << 2);
	if (v0.m_w == v1.m_w) res |= (1 << 3);
	return res;
}

Float4::cmpresult Float4::NearEqual(const Float4& v0, const Float4& v1, const Float4& epsilon)
{
	cmpresult res = 0;
	if (v0.m_x <= v1.m_x + epsilon.m_x && v1.m_x <= v0.m_x + epsilon.m_x) res |= (1 << 0);
	if (v0.m_y <= v1.m_y + epsilon.m_y && v1.m_y <= v0.m_y + epsilon.m_y) res |= (1 << 1);
	if (v0.m_z <= v1.m_z + epsilon.m_z && v1.m_z <= v0.m_z + epsilon.m_z) res |= (1 << 2);
	if (v0.m_w <= v1.m_w + epsilon.m_w && v1.m_w <= v0.m_w + epsilon.m_w) res |= (1 << 3);
	return res;
}

bool Float4::AllPass(cmpresult res)
{
	return res == (1 << 0 | 1 << 1 | 1 << 2 | 1 << 3);
}

bool Float4::AnyPass(cmpresult res)
{
	return res != 0;
}

}
