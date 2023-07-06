////////////////////////////////////////////////////////////////////////////////////
//
//  Copyright(c)2003-2018 北京天智祥信息科技有限公司(http://www.tianzhixiang.net)
//  All rights reserved.
//
//	@file       TFF_Voronoi.h
//	@brief		TFF_Voronoi header file
//	@author		gaoyang
//	@date		2018-06-21
//  @version 
//    - v1.0	2018-06-21	gaoyang	 
//
////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "triangular/TFF_Delaunay.h"
#include "triangular/TFF_ThiessenPolygon.h"
/** @class Voronoi
	@brief The class of building voronoi.
	@note		
	@attention	
*/
class  Voronoi
{
public:
	/// Constructor.
	Voronoi(void) = default;
	Voronoi(const vertexSet& setTsaiKeyPts, const CSimpleRect& rectTsaiBound);
	/// Destructor.
	~Voronoi(void) = default;

	/// 设置关键点.
	void SetTsaiKeyPoints(const vertexSet& setTsaiKeyPts);
	/// 设置区域边界.
	void SetTsaiBound(const CSimpleRect& rectTsaiBound);
	/// 设置泰森多边形数据.
	TFF_INLINE_SPECIFIER void SetThiessenData(const std::map<vertex, ThiessenPolygon>& vertexToThiessenPolygonMap);
	/// 生成泰森多边形.
	const std::map<vertex, ThiessenPolygon>& MakeVoronoi();
	/// 获取生成的Delaunay三角形集.
	TFF_INLINE_SPECIFIER const triangleSet& GetTriangle();
	/// 切割泰森多边形，并求权值.
	const std::map<vertex, ThiessenPolygon> CutVoronoi(const ThiessenPolygon& polyBound);

protected:
	/// 创建Delaunay三角形.
	void CreateDelaunayTriangle();
	/// 按顺时针或逆时针对关键点相邻的Delaunay三角形排序.
	void SortDelaunayByEveryKeyPoints();
	/// 构成泰森多边形.
	void MakeTsaiPolygon();
	/// 处理泰森多边形边界.
	void DealTsaiPolyBound(const vertex& ver0, vector<vertex>& vecPolyPts);

protected:
	/// 泰森区域边界.
	CSimpleRect												m_rectTsaiBound;
	/// 泰森多边形的关键点集.
	vertexSet												m_setTsaiKeyPts;
	/// Delaunay三角形集.
	triangleSet												m_setTsaiTriangles;
	/// 从关键点到泰森多边形集合的映射.
	std::map<vertex, ThiessenPolygon>						m_mapKeyPtToTsaiPolys;
	/// 从关键点到与其相邻的D三角形集合的映射.
	std::map<vertex, std::vector<triangle>>					m_mapKeyPtToTris;
	/// 从关键点到其所在泰森多边形是否为“旋转一周多边形”的映射.
	std::map<vertex, bool>								m_mapKeyPtToIsGoodPolygon;

};

//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Public:
void Voronoi::SetThiessenData(const std::map<vertex, ThiessenPolygon>& vertexToThiessenPolygonMap)
{
	m_mapKeyPtToTsaiPolys = vertexToThiessenPolygonMap;
}

const triangleSet& Voronoi::GetTriangle()
{
	return m_setTsaiTriangles;
}


