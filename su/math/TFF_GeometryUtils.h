////////////////////////////////////////////////////////////////////////////////////
//
//  Copyright(c)2003-2018 ������������Ϣ�Ƽ����޹�˾(http://www.tianzhixiang.net)
//  All rights reserved.
//
//	@file       TFF_GeometryUtils.h
//	@brief		TFF_GeometryUtils header file
//	@author		gaoyang
//	@date		2017-02-14
//  @version 
//    - v1.0	2017-02-14	gaoyang	 
//
////////////////////////////////////////////////////////////////////////////////////

#pragma once
// GDAL.
#include "ogr_spatialref.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "gdal_alg.h"
#include "ogr_api.h"
#include "TFF_Rectangle.h"
#include "TFF_Float2.h"
#include "TFF_Double2.h"
#include "TFF_MathUtils.h"


/** @class GeometryUtils
	@brief The tool class of geometry operation.
	@remark
*/
class  GeometryUtils
{
public:
	/**
	 *  @name			IsPointOnPoly
	 *  @brief			Whether point is on point
	 *  @param[in]		x: source x value
	 *					y: source y value
	 *					x1: target x value
	 *					y1: target y value
	 *					radius: vague radius value
	 *  @param[out]		
	 *  @return			 
	 *  @retval			
	 *  @exception		
	 *  @attention      
	 *  @see          
	**/
	template<typename T1>
	static bool IsPointOnPoint(const T1 x, const T1 y, const T1 x1, const T1 y1, const T1 radius);

	/**
	 *  @name			IsPointOnPoly
	 *  @brief			Whether point is on line
	 *  @param[in]		x: source x value
	 *					y: source y value
	 *					pointList: points list
	 *					pointNum: points number
	 *					radius: vague radius value
	 *  @param[out]		
	 *  @return			 
	 *  @retval			
	 *  @exception		
	 *  @attention      
	 *  @see          
	**/
	template<typename T1, typename T2>
	static bool IsPointOnLine(const T1 x, const T1 y, const T2* pointList, const size_t pointNum, const T2 lineWidth);

	/**
	 *  @name			IsPointOnPoly
	 *  @brief			Whether point is on polygon
	 *  @param[in]		x: x value
	 *					y: y value
	 *					pointList: points list
	 *					pointNum: point number
	 *  @param[out]		
	 *  @return			 
	 *  @retval			
	 *  @exception		
	 *  @attention      pointNum is (x, y) count
	 *  @see          
	**/
	//static bool IsPointOnPoly(const unsigned char x, const unsigned char y, const unsigned char* pointList, const size_t pointNum);

	//static bool IsPointOnPoly(const float x, const float y, const float* pointList, const size_t pointNum);
	//static bool IsPointOnPoly(const double x, const double y, const double* pointList, const size_t pointNum);

	//static bool IsPointOnPoly(const float x, const float y, const std::vector<float>& pointList);
	//static bool IsPointOnPoly(const double x, const double y, const std::vector<double>& pointList);

	template<typename T1, typename T2>
	static bool IsPointOnPoly(const T1 x, const T1 y, const T2* pointList, const size_t pointNum);

    static bool IsPointOnPoly(const float& x, const float& y, const OGRPolygon& ClipPolygon2);

    static bool IsLineOnPoly(const float* pointList1, const int pointNum1, const OGRPolygon& ClipPolygon2, const int vacuateFactor = 5, const bool bVacuate = false);

    static bool IsPolyOnPoly(const float* pointList1, const int pointNum1, const OGRPolygon& ClipPolygon2const, const int vacuateFactor = 5, bool bVacuate = false);

    static void GetBoundryRing(const float* pointList2, const int pointNum2, OGRPolygon& pClipPolygon2, const int vacuateFactor = 5, const bool bVacuate = false);

    template<typename T1>
    static bool IsLineOnPoly(const T1* pointList1, const size_t pointNum1, const T1* pointList2, const size_t pointNum2);

    template<typename T2>
    static bool IsPolyOnPoly(const T2* pointList1, const size_t pointNum1, const T2* pointList2, const size_t pointNum2);

    /**
	 *  @name			CalcPolygonArea
	 *  @brief			Calculate polygon area
	 *  @param[in]		x: x list
	 *					y: y list
	 *					num: point number
	 *  @param[out]		
	 *  @return			 
	 *  @retval			
	 *  @exception		
	 *  @attention      
	 *  @see          
	**/
	static float CalcPolygonArea(const float* x, const float* y, const size_t num);
	static double CalcPolygonArea(const double* x, const double* y, const size_t num);

	/**
	 *  @name			CalcPolygonArea
	 *  @brief			Calculate polygon area
	 *  @param[in]		pointList: points list
	 *  @param[out]		
	 *  @return			 
	 *  @retval			
	 *  @exception		
	 *  @attention      
	 *  @see          
	**/
	static float CalcPolygonArea(const float* pointList, const size_t num);
	static double CalcPolygonArea(const double* pointList, const size_t num);

	/**
	 *  @name			CalcPolygonArea
	 *  @brief			Calculate polygon area
	 *  @param[in]		pointList: points list
	 *  @param[out]		
	 *  @return			 
	 *  @retval			
	 *  @exception		
	 *  @attention      
	 *  @see          
	**/
	static float CalcPolygonArea(const std::vector<float>& pointList);
	static double CalcPolygonArea(const std::vector<double>& pointList);

	/**
	 *  @name			GetCircleRect
	 *  @brief			Get circle coordinate rect
	 *  @param[in]		center: center point coordinate of a circle
	 *					radius: radius of a circle
	 *  @param[out]		rect: coordinate scope
	 *  @return			 
	 *  @retval			
	 *  @exception		
	 *  @attention      
	 *  @see          
	**/
	static void GetCircleRect(const silly_utils::Double2& center, const double radius, silly_utils::Rectangle<double>& rect);

	/**
	 *  @name			GetDistance
	 *  @brief			Get distance between point1 and point2(Unit: m)
	 *  @param[in]		point1: point1 coordinate
	 *					point2: point2 coordinate
	 *  @param[out]		
	 *  @return			 
	 *  @retval			
	 *  @exception		
	 *  @attention      
	 *  @see          
	**/
	static double GetDistance(const silly_utils::Double2& point1, const silly_utils::Double2& point2);

	/**
	 *  @name			GetPolygonCenterPoint
	 *  @brief			Get center point of polygon
	 *  @param[in]		vertices: polygon vertices
	 *  @param[out]		
	 *  @return			 
	 *  @retval			
	 *  @exception		
	 *  @attention      
	 *  @see          
	**/
	static  silly_utils::Float2 GetPolygonCenterPoint(const std::vector<silly_utils::Float2>& vertices)
	{
		silly_utils::Float2 centerPoint;


		double area = 0.0;//��������
		double Gx = 0.0, Gy = 0.0;// ���ĵ�x��y
		for (int i = 1; i <= vertices.size(); i++)
		{
			double iLat = vertices[(i % vertices.size())].Y();
			double iLng = vertices[(i % vertices.size())].X();
			double nextLat = vertices[(i - 1)].Y();
			double nextLng = vertices[(i - 1)].X();
			double temp = (iLat * nextLng - iLng * nextLat) / 2.0;
			area += temp;
			Gx += temp * (iLat + nextLat) / 3.0;
			Gy += temp * (iLng + nextLng) / 3.0;
		}
		if (!MathUtils::IsZero(area))
		{
			Gx = Gx / area;
			Gy = Gy / area;
		}
		centerPoint.Set(Gy, Gx);


		return centerPoint;
	}

	/**
	 *  @name			GetTextPositionOfBrokenLine
	 *  @brief			Get text position of broken line
	 *  @param[in]		vertices: polygon vertices
	 *  @param[out]		
	 *  @return			 
	 *  @retval			
	 *  @exception		
	 *  @attention      
	 *  @see          
	**/
	static int GetTextPositionOfBrokenLine(const char* pCaption, const int PointNum, float* pPos, const float percentage, const int FontWidth, const int FontDistance, int* Result);

	static int GetTextPositionOfBrokenLine(const char* pCaption, int PointNum, float* pPos, const float percentage, const float FontWidth, const float FontDistance, float* Result);

	static void CalcPolygonText(float xmin, float ymin, float xmax, float ymax, int Number, float* PointList, int ytest, float* x,float* y);

	static silly_utils::Float2 CalcPointPos(const silly_utils::Float2& pos, const silly_utils::Float2& widthAndHeight, std::vector<silly_utils::Rectangle<float>>& rectBeingUsed, const silly_utils::Float2& miss = silly_utils::Float2(0.0f, 0.0f));

	static bool RectIsBeingUsed(const silly_utils::Rectangle<float>& rc, const std::vector<silly_utils::Rectangle<float>>& rectBeingUsed);

	static float* CalcEccentricityPoint(const float* pPointList, const int pointNum, const int offsetFactor, const bool isPolygon, int& newPointNum);

protected:
	/// 
	static float CalDisp(float x1, float y1, float x2, float y2);
	/// 
	static float PointLineLength(float xx, float yy, float x1, float y1, float x2, float y2);
	/// 
	static float GetMaxXRange(float x, float y, int Numble, float* PointList, float* xx, int quan);

};

//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Public:


