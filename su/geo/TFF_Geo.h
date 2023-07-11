#pragma once

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "math/TFF_Double2.h"
#include "math/TFF_Rectangle.h"
#include "math/TFF_MathUtils.h"

#define MIN_LNG_VAL -180.0
#define MAX_LNG_VAL 180.0
#define MIN_LAT_VAL -90.0
#define MAX_LAT_VAL 90.0

/** @enum EGeoType
	@brief The enum of geo type.
	@remark
*/
enum EGeoType
{
	GeoPointType,            /**< geo point */
	GeoRectType,            /**< geo rect */

	NumberOfGeoType,
	InvalidGeoType
};

/** @class GeoPoint
	@brief The class of geo point.
	@remark
*/
class GeoPoint : public silly_utils::Double2
{
public:
	/// Constructor.
	GeoPoint()
	{
		m_x = 0;
		m_y = 0;
	}

	GeoPoint(const double x, const double y) :
			Double2(x, y)
	{
		//if(latitude<MIN_LAT_VAL||latitude>MAX_LAT_VAL){
		//	this->latitude = 0.0;
		//}else{
		//	this->latitude = latitude;
		//}
		//
		//if(longitude<MIN_LNG_VAL||longitude>MAX_LNG_VAL){
		//	this->longitude = 0.0;
		//}else{
		//	this->longitude = longitude;
		//}
	}

	/// Destructor.
	virtual ~GeoPoint()
	{

	}

};

/** @class GeoRect
	@brief The class of geo rect.
	@remark
*/
class GeoRect : public silly_utils::Rectangle<double>
{
public:
	/// Default constructor.
	GeoRect()
	{
		g_left = g_top = g_right = g_bottom = 0.0;
	}

	GeoRect(const double l, const double t, const double r, const double b)
	{
		g_left = l;
		g_top = t;
		g_right = r;
		g_bottom = b;
	}

	/// Destructor.
	virtual ~GeoRect() = default;

	bool IsValid()
	{
		return !(MathUtils::IsZero(g_left)
				 && MathUtils::IsZero(g_top)
				 && MathUtils::IsZero(g_right)
				 && MathUtils::IsZero(g_bottom)
				 && g_left <= g_right
				 && g_top <= g_bottom);
	}

	std::string ToString()
	{
		return str(boost::format("%1%,%2%,%3%,%4%") % g_left % g_top % g_right % g_bottom);
	}

	bool FromString(const std::string& bound_str)
	{
		std::vector<std::string> tmpVec;
		boost::split(tmpVec, bound_str, boost::is_any_of(","));
		size_t num = tmpVec.size();
		if (num >= 4)
		{
			g_left = std::stof(tmpVec[0]);
			g_top = std::stof(tmpVec[1]);
			g_right = std::stof(tmpVec[2]);
			g_bottom = std::stof(tmpVec[3]);
		}
		return IsValid();
	}

	bool IsEqual(const GeoRect& tmpGeoRect)
	{
		return (MathUtils::IsEqual(g_left, tmpGeoRect.g_left)
				&& MathUtils::IsEqual(g_top, tmpGeoRect.g_top)
				&& MathUtils::IsEqual(g_right, tmpGeoRect.g_right)
				&& MathUtils::IsEqual(g_bottom, tmpGeoRect.g_bottom));
	}
};


