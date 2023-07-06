
#ifndef PI
#define PI (3.1415926535897932384626433832795028841971693993751f)
#endif
#define TFF_PI PI

#define TFF_2PI (TFF_PI * 2.)

#ifndef BASE_E
#define BASE_E 2.71828182845904523536
#endif
#define TFF_BASE_E BASE_E

#ifndef TINY
#define TINY (0.0000001f)
#endif
#define T_TINY TINY

/** earth radius */
#ifndef EARTH_RADIUS
#define EARTH_RADIUS 6378.137
#endif
#define TFF_EARTH_RADIUS EARTH_RADIUS

#define LN_2   (0.693147180559945f)
#define SQRT_2   (1.4142135623730950488016887242097f)

#define M_PI	3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_2PI (M_PI*2.)


#define TFF_RGB(r, g, b)				((unsigned int)(((unsigned char)(r)|((unsigned int)((unsigned char)(g))<<8))|(((unsigned int)(unsigned char)(b))<<16)))
#define TFF_ARGB(a, r, g, b)			(unsigned int)( ((a)&0xff) << 24 | ((r)&0xff) << 16 | ((g)&0xff) << 8 | (b&0xff) )
#define TFF_RGBA(r, g, b, a)			TFF_ARGB( a, r, g, b )
#define TFF_XRGB(r, g, b)				TFF_ARGB( 0xff, r, g, b )

/*static float std::abs(const float a)
{
	return (a < 0.0f) ? -a : a;
}

static double std::abs(const double a)
{
	return (a < 0.0f) ? -a : a;
}

static int std::abs(const int a)
{
	return (a < 0.0f) ? -a : a;
}

static float t_sgn(const float a)
{
	return (a < 0.0f) ? -1.0f : 1.0f;
}

static double t_sgn(const double a)
{
	return (a < 0.0) ? -1.0 : 1.0;
}



static float t_rad2deg(const float r)
{
	return (r * 180.0f) / PI;
}

static double t_rad2deg(const double r)
{
	return (r * 180.0) / PI;
}

static float t_sin(const float x)
{
	return std::sinf(x);
}

static double t_sin(const double x)
{
	return std::sin(x);
}

static float t_cos(const float x)
{
	return std::cosf(x);
}

static double t_cos(const double x)
{
	return std::cos(x);
}

static float t_tan(const float x)
{
	return std::tanf(x);
}

static double t_tan(const double x)
{
	return std::tan(x);
}

static float t_asin(const float x)
{
	return std::asinf(x);
}

static double t_asin(const double x)
{
	return std::asin(x);
}

static float t_acos(const float x)
{
	return acosf(x);
}

static double t_acos(const double x)
{
	return acos(x);
}

static float t_atan(const float x)
{
	return atanf(x);
}

static double t_atan(const double x)
{
	return atan(x);
}

static float t_log2(const float f)
{
	return logf(f) / LN_2;
}

static double t_log2(const double f)
{
	return log(f) / LN_2;
}

static float t_log(const float f)
{
	return logf(f);
}

static double t_log(const double f)
{
	return log(f);
}

*//**
	integer clamping.
*//*
static int t_iclamp(const int val, const int minVal, const int maxVal)
{
	if (val < minVal)      return minVal;
	else if (val > maxVal) return maxVal;
	else return val;
}

static float std::sqrt(const float x)
{
	return  std::sqrtf(x);
}

static double std::sqrt(const double x)
{
	return  std::sqrt(x);
}

static float t_exp(const float x)
{
	return  std::expf(x);
}

static double t_exp(const double x)
{
	return std::exp(x);
}

*//**
	A fuzzy floating point equality check
*//*
static bool t_fequal(const float f0, const float f1, const float tol)
{
	float f = f0 - f1;
	return ((f > (-tol)) && (f < tol));
}

*//**
	A fuzzy floating point less-then check.
*//*
static bool t_fless(const float f0, const float f1, const float tol)
{
	return ((f0 - f1) < tol);
}

*//**
	A fuzzy floating point greater-then check.
*//*
static bool t_fgreater(const float f0, const float f1, const float tol)
{
	return ((f0 - f1) > tol);
}

*//**
	Clamp a value against lower und upper boundary.
*//*
static float t_clamp(const float val, const float lower, const float upper)
{
	if (val < lower)      return lower;
	else if (val > upper) return upper;
	else                  return val;
}

*//**
	Saturate a value (clamps between 0.0f and 1.0f)
*//*
static float t_saturate(const float val)
{
	if (val < 0.0f)      return 0.0f;
	else if (val > 1.0f) return 1.0f;
	else return val;
}

*//**

*//*
static float t_floorf(const float val)
{
	return floorf(val);
}

static double t_floor(const double val)
{
	return floor(val);
}

*//**

*//*
static float std::ceilf(const float val)
{
	return ceilf(val);
}

static double t_ceil(const double val)
{
	return ceil(val);
}

*//**
	Return a pseudo random number between 0 and 1.
*//*
static float t_randf()
{
	return float(rand()) / float(RAND_MAX);
}

static double t_rand()
{
	return double(rand()) / double(RAND_MAX);
}

*//**
	Return a pseudo random number between min and max.
*//*
static float t_rand(const float min, const float max)
{
	float unit = float(rand()) / RAND_MAX;
	float diff = max - min;
	return min + unit * diff;
}

static double t_rand(const double min, const double max)
{
	double unit = double(rand()) / RAND_MAX;
	double diff = max - min;
	return min + unit * diff;
}

*//**
	Chop float to int.
*//*
static int t_fchop(const float f)
{
	/// @todo type cast to int is slow!
	return int(f);
}

*//**
	Round float to integer.
*//*
static int t_fround(const float f)
{
	return t_fchop(floorf(f + 0.5f));
}

*//**
	Linearly interpolate between 2 values: ret = x + l * (y - x)
*//*
static float t_lerp(const float x, const float y, const float l)
{
	return x + l * (y - x);
}

*//**
*//*
static float t_fmod(const float x, const float y)
{
	return fmodf(x, y);
}

*//**
	Normalize an angular value into the range rad(0) to rad(360).
*//*
static float t_modangle(float a)
{
	if (a < 0.0f)
	{
		float valDevideTwoPi = a * 0.5f / TFF_PI;
		valDevideTwoPi = ceilf(-valDevideTwoPi);
		a += valDevideTwoPi * 2.0f * TFF_PI;
	}
	if (a >= t_deg2rad(360.0f))
	{
		a = t_fmod(a, t_deg2rad(360.0f));
	}
	return a;
}

*//**
	Get angular distance.
*//*
static float t_angulardistance(const float from, const float to)
{
	float normFrom = t_modangle(from);
	float normTo = t_modangle(to);
	float dist = normTo - normFrom;
	if (dist < t_deg2rad((float)-180.0f))
	{
		dist += t_deg2rad((float)360.0f);
	}
	else if (dist > t_deg2rad((float)180.0f))
	{
		dist -= t_deg2rad((float)360.0f);
	}
	return dist;
}*/
#include <cmath>
#ifndef SCALER_HEADER
#define SCALER_HEADER
class Scaler
{

public:
static bool FIsEqual(const float& f1, const float& f2)
{
	return std::abs(f1 - f2) < 1e-6;
}

static bool FIsEqual(const double& d1, const double& d2)
{
	return std::abs(d1 - d2) < 1e-6;
}
static float t_deg2rad(const float d)
{
    return (d * PI) / 180.0f;
}

static double t_deg2rad(const double d)
{
    return (d * PI) / 180.0;
}
};
#endif
/*

static float t_pow(const float x, const float y)
{
	return powf(x, y);
}

static double t_pow(const double x, const double y)
{
	return pow(x, y);
}

static float t_round(const float dVal, const short iPlaces)
{
	float dRetval = 0.0f;
	float dMod = 0.0000001f;
	if (dVal < 0.0f)
	{
		dMod = -0.0000001f;
	}
	dRetval = dVal;
	dRetval += (5.0f / t_pow(10.0f, (float)(iPlaces + 1.0)));
	dRetval *= t_pow(10.0f, (float)iPlaces);
	dRetval = t_floorf(dRetval + dMod);
	dRetval /= t_pow(10.0f, (float)iPlaces);

	return dRetval;
}

static double t_round(const double dVal, const short iPlaces)
{
	double dRetval = 0.0;
	double dMod = 0.0000001;
	if (dVal < 0.0)
	{
		dMod = -0.0000001;
	}
	dRetval = dVal;
	dRetval += (5.0 / t_pow(10.0, iPlaces + 1.0));
	dRetval *= t_pow(10.0, iPlaces);
	dRetval = t_floor(dRetval + dMod);
	dRetval /= t_pow(10.0, iPlaces);

	return dRetval;
}

static double t_round2(const double dVal, const short iPlaces)
{
	//char s[20] = {0};
	//double dRetval;

	//sprintf(s,"%.*lf", iPlaces, dVal);
	//sscanf(s, "%lf", &dRetval);
	//return (dRetval);

	double dRetval = dVal;
	double base = t_pow(10.0, iPlaces);
	dRetval *= base;
	dRetval += 0.5;
	dRetval = t_floor(dRetval);
	dRetval /= base;

	return dRetval;
}

static float t_cross_product(const float& p1X, const float& p1Y, const float& p2X, const float& p2Y)
{
	return p1X * p2Y - p1Y * p2X;
}

static double t_cross_product(const double& p1X, const double& p1Y, const double& p2X, const double& p2Y)
{
	return p1X * p2Y - p1Y * p2X;
}

static float t_distance_sqrd(const float x1, const float y1, const float x2, const float y2)
{
	float xd = x1 - x2;
	float yd = y1 - y2;
	return std::sqrt((xd * xd) + (yd * yd));
}
*/
