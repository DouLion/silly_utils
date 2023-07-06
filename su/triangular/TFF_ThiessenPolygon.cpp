////////////////////////////////////////////////////////////////////////////////////
//
//  Copyright(c)2003-2018 北京天智祥信息科技有限公司(http://www.tianzhixiang.net)
//  All rights reserved.
//
//	@file       TFF_ThiessenPolygon.cpp
//	@brief		TFF_ThiessenPolygon source file
//	@author		gaoyang
//	@date		2018-06-21
//  @version 
//    - v1.0	2018-06-21	gaoyang	 
//
////////////////////////////////////////////////////////////////////////////////////


#include <common/triangular/TFF_ThiessenPolygon.h>
#include <common/triangular/TFF_Delaunay.h>
#include <common/math/TFF_GeometryCalc.h>

//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Public:
ThiessenPolygon::ThiessenPolygon(void)
{
	this->dblWeight = 0.0;
	this->dblArea = 0.0;
}

ThiessenPolygon::ThiessenPolygon(const std::vector<vertex>& vecPolygonPts, const double dblWeight)
{
	this->dblArea = 0.0;
	this->dblWeight = dblWeight;
	this->vecPolygonPts = vecPolygonPts;
}

ThiessenPolygon::~ThiessenPolygon(void)
= default;

/// 判断点在多边形内.
bool ThiessenPolygon::IsPointOnPoly(const vertex& ver) const
{
	size_t i, j;
	i = j = 0;
	bool c = false;
	const std::vector<vertex>& vecVer = this->vecPolygonPts;
	size_t pointNum = vecVer.size();
	double x = ver.GetX();
	double y = ver.GetY();
	for (i = 0, j = pointNum - 1; i < pointNum; j = i++)
	{
		if (fabs(vecVer.at(j).GetY() - vecVer.at(i).GetY()) <1e-6)
		{
			continue;
		}

		if ((((vecVer.at(i).GetY() <= y) && (y < vecVer.at(j).GetY()) ||
			((vecVer.at(j).GetY() <= y) && (y < vecVer.at(i).GetY()))) &&
			(x < (vecVer.at(j).GetX() - vecVer.at(i).GetX()) * (y - vecVer.at(i).GetY()) / (vecVer.at(j).GetY() - vecVer.at(i).GetY()) + vecVer.at(i).GetX())))
		{
			c = !c;
		}
	}
	return c;
}

/// 判断两个多边形的位置关系.
ThiessenPolygon::PositionalRelationship ThiessenPolygon::JudgePositionalRelationship(const ThiessenPolygon& poly)
{
	// 判断是否无效.
	if (this->vecPolygonPts.size() < 3 || poly.vecPolygonPts.size() < 3)
	{
		return POS_INVALID;
	}

	// 判断是否重合.
	if (this->vecPolygonPts.size() == poly.vecPolygonPts.size())
	{
		size_t i = 0;
		size_t nSize = poly.vecPolygonPts.size();
		for (; i < poly.vecPolygonPts.size(); ++i)
		{
			if (this->vecPolygonPts[0] == poly.vecPolygonPts[i])
			{
				break;
			}
		}
		if (i != nSize)
		{
			bool bSame = true;
			for (size_t k = 0; k < nSize; ++k)
			{
				if (this->vecPolygonPts[k].GetPoint() != poly.vecPolygonPts[(k + i) % nSize].GetPoint())
				{
					bSame = false;
					break;
				}
			}
			if (bSame)
			{
				return POS_SAME;
			}
		}
	}

	// 判断是否相交.
	vertex vP;
	segcrossCalc calc;
	size_t nPolyPtsA = this->vecPolygonPts.size();
	size_t nPolyPtsB = poly.vecPolygonPts.size();
	for (size_t i = 0; i < nPolyPtsA; ++i)
	{
		for (size_t j = 0; j < nPolyPtsB; ++j)
		{
			if (1 == calc.segcross(this->vecPolygonPts[i], this->vecPolygonPts[(i + 1) % nPolyPtsA],
				poly.vecPolygonPts[j], poly.vecPolygonPts[(j + 1) % nPolyPtsB], vP))
			{
				return POS_INTERSECT;
			}
		}
	}

	// 判断是否包含(两种情况).
	// 1.A in B.
	bool bAinB = true;
	for (size_t i = 0; i < nPolyPtsA; ++i)
	{
		if (!poly.IsPointOnPoly(this->vecPolygonPts[i]))
		{
			bAinB = false;
			break;
		}
	}
	if (bAinB)
	{
		return POS_A_IN_B;
	}
	// 2.B in A.
	bool bBinA = true;
	for (size_t i = 0; i < nPolyPtsA; ++i)
	{
		if (!this->IsPointOnPoly(poly.vecPolygonPts[i]))
		{
			bBinA = false;
			break;
		}
	}
	if (bBinA)
	{
		return POS_B_IN_A;
	}

	// 否则确定二者相离.
	return POS_AWAY;
}

/// 计算相交的多边形.
ThiessenPolygon ThiessenPolygon::CalcIntersectPolygon(const ThiessenPolygon& polyBound)
{
	// 使用gdal方法进行相交计算.
	ThiessenPolygon polyIntersect;

	/// 构建裁剪区域Cesium.
	OGRPolygon PolygonCesium;
	OGRLinearRing RingCesium;
	int boundpntcnt = (int)polyBound.vecPolygonPts.size();
	if (boundpntcnt < 4)
	{
		return polyIntersect;
	}
	double* padx = (double*)malloc(boundpntcnt * sizeof(double));
	double* pady = (double*)malloc(boundpntcnt * sizeof(double));
	for (size_t i = 0; i < polyBound.vecPolygonPts.size(); ++i)
	{
		const vertex& vPt = polyBound.vecPolygonPts.at(i);
		//RingCesium.addPoint(vPt.GetX(), vPt.GetY(), 0.0);
		padx[i] = vPt.GetX();
		pady[i] = vPt.GetY();
	}
	RingCesium.setPoints(boundpntcnt, padx, pady);
	free(padx);
	free(pady);
	RingCesium.closeRings();
	PolygonCesium.addRing(&RingCesium);
	PolygonCesium.closeRings();

	/// 构建裁剪区域Raster.
	OGRPolygon PolygonRaster;
	OGRLinearRing RingRaster;
	int rstpntcnt = (int)this->vecPolygonPts.size();
	if (rstpntcnt < 4)
	{
		return polyIntersect;
	}
	double* npadx = (double*)malloc(rstpntcnt * sizeof(double));
	double* npady = (double*)malloc(rstpntcnt * sizeof(double));
	for (size_t i = 0; i < this->vecPolygonPts.size(); ++i)
	{
		const vertex& vPt = this->vecPolygonPts.at(i);
		npadx[i] = (float)vPt.GetX();
		npady[i] = (float)vPt.GetY();
	}
	RingRaster.setPoints(rstpntcnt, npadx, npady);
	free(npadx);
	free(npady);
	RingRaster.closeRings();
	PolygonRaster.addRing(&RingRaster);
	PolygonRaster.closeRings();

	//计算多边形相交部分
	OGRGeometry * pClipGeometry = PolygonRaster.Intersection(&PolygonCesium);
	if (pClipGeometry == nullptr)
	{
		return polyIntersect;
	}

	////对相交部分抽稀
	//OGRGeometry * pResultGeometry = pClipGeometry->Simplify(0.001);
	//if (pResultGeometry == nullptr)
	//{
	//	return polyIntersect;
	//}

	//将抽稀点填充到vecPoints
	//GetGeoPoints(pResultGeometry, polyIntersect);
	GetGeoPoints(pClipGeometry, polyIntersect);

	return polyIntersect;
}

/// 计算多边形面积.
void ThiessenPolygon::CalcArea()
{
	double area = 0.0;
	int j = 0;
	size_t num = vecPolygonPts.size();
	for (size_t i = 0; i < num; ++i)
	{
		j = int((i + 1) % num);
		area += vecPolygonPts[i].GetX() * vecPolygonPts[j].GetY();
		area -= vecPolygonPts[i].GetY() * vecPolygonPts[j].GetX();
	}

	area /= 2.0;
	dblArea = (area < 0.0 ? -area : area);
}

/// 获取Gdal数据结构里的点.
void ThiessenPolygon::GetGeoPoints(OGRGeometry* pGeometry, ThiessenPolygon& polyIntersect)
{
	std::vector<vertex>& vecPoints = polyIntersect.vecPolygonPts;
	//vecPoints.clear();

	double x, y, z;
	int i = 0, j = 0, k = 0;

	switch (pGeometry->getGeometryType())
	{
	case wkbPoint:
		{
			OGRPoint point;
			OGRPoint *poPoint = (OGRPoint *)pGeometry;
			if (poPoint == nullptr)
			{
				return;
			}
			x = poPoint->getX();
			y = poPoint->getY();
			z = poPoint->getZ();
			vecPoints.push_back(vertex(x, y, z));

			break;
		}
	case wkbMultiPoint:
		{
			OGRMultiPoint* pMultiPoint = (OGRMultiPoint*)pGeometry;
			if (pMultiPoint == nullptr)
			{
				return;
			}
			OGRMultiPoint MPoint;
			int GeometryNum = pMultiPoint->getNumGeometries();
			for (i = 0; i < GeometryNum; i++)
			{
				OGRPoint point;
				OGRPoint* pPoint = (OGRPoint*)(pMultiPoint->getGeometryRef(i));
				if (pPoint == nullptr)
				{
					return;
				}
				x = pPoint->getX();
				y = pPoint->getY();
				z = pPoint->getZ();
				vecPoints.push_back(vertex(x, y, z));
			}
			break;
		}
	case wkbLineString:
		{
			OGRPoint point;
			OGRLineString* pLineGeo = (OGRLineString*)pGeometry;
			if (pLineGeo == nullptr)
			{
				return;
			}
			int PointNum = pLineGeo->getNumPoints();

			for (i = 0; i < PointNum; i++)
			{
				pLineGeo->getPoint(i, &point);//得到第i个点的坐标
				x = point.getX();
				y = point.getY();
				z = point.getZ();
				vecPoints.push_back(vertex(x, y, z));
			}
			break;
		}
	case wkbMultiLineString:
		{
			OGRMultiLineString* pMultiLineString = (OGRMultiLineString*)pGeometry;
			if (pMultiLineString == nullptr)
			{
				return;
			}
			OGRMultiLineString MLine;
			int GeometryNum = pMultiLineString->getNumGeometries();
			for (i = 0; i < GeometryNum; i++)
			{
				OGRPoint point;
				OGRLineString* pLineGeo = (OGRLineString*)(pMultiLineString->getGeometryRef(i));
				int PointNum = pLineGeo->getNumPoints();

				for (j = 0; j < PointNum; j++)
				{
					pLineGeo->getPoint(j, &point);//得到第j个点的坐标
					x = point.getX();
					y = point.getY();
					z = point.getZ();
					vecPoints.push_back(vertex(x, y, z));
				}
			}
			break;
		}
	case wkbPolygon:
		{
			OGRPolygon *poPolygon = (OGRPolygon *)pGeometry;
			if (poPolygon == nullptr)
			{
				return;
			}
			OGRPoint point;
			OGRLinearRing *pOGRLinearRing = poPolygon->getExteriorRing();
			if (pOGRLinearRing == nullptr)
			{
				return;
			}
			int PointNum = pOGRLinearRing->getNumPoints();
			if (PointNum == 0)
			{
				return;
			}

			for (i = 0; i < PointNum; i++)
			{
				pOGRLinearRing->getPoint(i, &point);//得到第i个点的坐标
				x = point.getX();
				y = point.getY();
				z = point.getZ();
				vecPoints.push_back(vertex(x, y, z));
			}


			int InteriorRingsNum = poPolygon->getNumInteriorRings();
			for (i = 0; i< InteriorRingsNum; i++)
			{
				OGRLinearRing *pOGRInteriorRings = poPolygon->getInteriorRing(i);
				PointNum = pOGRInteriorRings->getNumPoints();
				for (j = 0; j < PointNum; j++)
				{
					pOGRInteriorRings->getPoint(j, &point);//得到第j个点的坐标
					x = point.getX();
					y = point.getY();
					z = point.getZ();
					vecPoints.push_back(vertex(x, y, z));
				}
			}
			break;
		}
	case wkbMultiPolygon:
		{
			OGRMultiPolygon* pMultiPolygon = (OGRMultiPolygon*)pGeometry;
			if (pMultiPolygon == nullptr)
			{
				return;
			}
			int GeometryNum = pMultiPolygon->getNumGeometries();
			for (k = 0; k < GeometryNum; k++)
			{
				OGRPolygon *poPolygon = (OGRPolygon *)(pMultiPolygon->getGeometryRef(k));
				if (poPolygon == nullptr)
				{
					return;
				}
				OGRPoint point;
				OGRLinearRing *pOGRLinearRing = poPolygon->getExteriorRing();
				if (pOGRLinearRing == nullptr)
				{
					return;
				}
				int PointNum = pOGRLinearRing->getNumPoints();

				for (i = 0; i < PointNum; i++)
				{
					pOGRLinearRing->getPoint(i, &point);//得到点的坐标
					x = point.getX();
					y = point.getY();
					z = point.getZ();
					vecPoints.push_back(vertex(x, y, z));
				}

				int InteriorRingsNum = poPolygon->getNumInteriorRings();
				for (i = 0; i< InteriorRingsNum; i++)
				{
					OGRLinearRing InteriorRing;
					OGRLinearRing *pOGRInteriorRings = poPolygon->getInteriorRing(i);
					PointNum = pOGRInteriorRings->getNumPoints();
					for (j = 0; j < PointNum; j++)
					{
						pOGRInteriorRings->getPoint(j, &point);//得到第j个点的坐标
						x = point.getX();
						y = point.getY();
						z = point.getZ();
						vecPoints.push_back(vertex(x, y, z));
					}
				}
			}
			break;

		}
	case wkbGeometryCollection:
		{
			/// 遍历每一个Geometry
			OGRGeometryCollection * pGeometryCollection = (OGRGeometryCollection *)pGeometry;
			if (pGeometryCollection == nullptr)
			{
				break;;
			}
			if (!pGeometryCollection->IsValid())
			{
				break;
			}
			if (!pGeometryCollection->IsEmpty())
			{
				break;
			}
			OGRPoint poPoint;
			pGeometryCollection->Centroid(&poPoint);
			for (i = 0; i < pGeometryCollection->getNumGeometries(); ++i)
			{
				OGRGeometry * pGeo = pGeometryCollection->getGeometryRef(i);
				if (pGeo == nullptr)
				{
					return;
				}
				GetGeoPoints(pGeo, polyIntersect);
			}
		}
		break;
	}
}


