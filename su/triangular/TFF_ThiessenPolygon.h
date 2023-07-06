////////////////////////////////////////////////////////////////////////////////////
//
//  Copyright(c)2003-2018 北京天智祥信息科技有限公司(http://www.tianzhixiang.net)
//  All rights reserved.
//
//	@file       TFF_ThiessenPolygon.h
//	@brief		TFF_ThiessenPolygon header file
//	@author		gaoyang
//	@date		2018-06-21
//  @version 
//    - v1.0	2018-06-21	gaoyang	 
//
////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common/triangular/TFF_Delaunay.h"
// GDAL.
#include "ogr_spatialref.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "gdal_alg.h"
#include "ogr_api.h"

/** @class ThiessenPolygon
	@brief The class of building ThiessenPolygon.
	@note		
	@attention	
*/
class  ThiessenPolygon
{
public:
	/// 位置关系枚举变量.
	enum PositionalRelationship
	{
		POS_INTERSECT,	//相交
		POS_AWAY,		//相离
		POS_A_IN_B,		//A含于B
		POS_B_IN_A,		//B含于A
		POS_SAME,		//重合
		POS_INVALID		//多边形无效
	};

public:
	/// Constructor.
	ThiessenPolygon(void);
	ThiessenPolygon(const std::vector<vertex>& vecPolygonPts, const double dblWeight = 0.0);
	/// Destructor.
	virtual ~ThiessenPolygon(void);

	bool IsPointOnPoly(const vertex& ver) const;
	PositionalRelationship JudgePositionalRelationship(const ThiessenPolygon& poly);
	ThiessenPolygon CalcIntersectPolygon(const ThiessenPolygon& polyBound);
	void CalcArea();
	void GetGeoPoints(OGRGeometry* pGeometry, ThiessenPolygon& polyIntersect);

public:
	/// 多边形点集.
	std::vector<vertex>					vecPolygonPts;	
	/// 多边形权值.
	double							dblWeight;
	/// 多边形面积.
	double							dblArea;

};

typedef std::set<ThiessenPolygon> polygonSet;

//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Public:


