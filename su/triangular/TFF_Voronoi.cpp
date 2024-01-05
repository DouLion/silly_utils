////////////////////////////////////////////////////////////////////////////////////
//
//  Copyright(c)2003-2018 北京天智祥信息科技有限公司(http://www.tianzhixiang.net)
//  All rights reserved.
//
//	@file       TFF_Voronoi.cpp
//	@brief		TFF_Voronoi source file
//	@author		gaoyang
//	@date		2018-06-21
//  @version
//    - v1.0	2018-06-21	gaoyang
//
////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "TFF_Voronoi.h"
// GDAL.
#include "ogr_spatialref.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "gdal_alg.h"
#include "ogr_api.h"

//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Public:

Voronoi::Voronoi(const vertexSet& vecTsaiKeyPts, const CSimpleRect& rectTsaiBound)
{
    m_setTsaiKeyPts = vecTsaiKeyPts;
    m_rectTsaiBound = rectTsaiBound;
}

void Voronoi::SetTsaiKeyPoints(const vertexSet& setTsaiKeyPts)
{
    m_setTsaiTriangles.clear();
    m_mapKeyPtToTris.clear();
    m_mapKeyPtToTsaiPolys.clear();
    m_mapKeyPtToIsGoodPolygon.clear();

    m_setTsaiKeyPts = setTsaiKeyPts;
}

void Voronoi::SetTsaiBound(const CSimpleRect& rectTsaiBound)
{
    m_setTsaiTriangles.clear();
    m_mapKeyPtToTris.clear();
    m_mapKeyPtToTsaiPolys.clear();
    m_mapKeyPtToIsGoodPolygon.clear();

    m_rectTsaiBound = rectTsaiBound;
    m_setTsaiKeyPts.insert(vertex(rectTsaiBound.x0, rectTsaiBound.y0));
    m_setTsaiKeyPts.insert(vertex(rectTsaiBound.x1, rectTsaiBound.y0));
    m_setTsaiKeyPts.insert(vertex(rectTsaiBound.x1, rectTsaiBound.y1));
    m_setTsaiKeyPts.insert(vertex(rectTsaiBound.x0, rectTsaiBound.y1));
}

const std::map<vertex, ThiessenPolygon>& Voronoi::MakeVoronoi()
{
    // 1.创建Delaunay网.
    CreateDelaunayTriangle();

    // 2.按顺时针或逆时针对关键点相邻的Delaunay三角形排序.
    SortDelaunayByEveryKeyPoints();

    // 3.计算各个Delaunay三角形的外接圆圆心.
    // triangle初始化时已计算完成，此步骤略去.

    // 4.遍历与关键点相关联的所有三角形的圆心，构成泰森多边形（注意处理边界的特殊情况）.
    MakeTsaiPolygon();

    std::map<vertex, ThiessenPolygon>::iterator it = m_mapKeyPtToTsaiPolys.begin();
    //{

    //	GDALAllRegister();
    //	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
    //	OGRRegisterAll();
    //	CPLSetConfigOption("SHAPE_ENCODING", "");

    //	GDALDriverH poDriver = nullptr;
    //	poDriver = GDALGetDriverByName("ESRI Shapefile");
    //	GDALDataset* poDSw = (GDALDataset *) GDALCreate(poDriver, "./tisrec.shp", 0, 0, 0, GDT_Unknown, nullptr);
    //	if (nullptr == poDSw)
    //	{
    //		//return false;
    //		int a = 0;
    //	}
    //	OGRSpatialReference p_spRef;

    //	p_spRef.SetGeogCS( "Mygeographic coordinate system",
    //		"WGS_1984",
    //		"My WGS84 Spheroid",
    //		SRS_WGS84_SEMIMAJOR, SRS_WGS84_INVFLATTENING,
    //		"Greenwich", 0.0,
    //		"degree");
    //	p_spRef.SetWellKnownGeogCS("WGS84");

    //	OGRLayer* poLayer_w = nullptr;
    //	poLayer_w = poDSw->CreateLayer("GLASS", &p_spRef);
    //	for (; it != m_mapKeyPtToTsaiPolys.end(); ++it)
    //	{
    //		OGRFeature* pFeature_w = nullptr;
    //		pFeature_w = OGRFeature::CreateFeature(poLayer_w->GetLayerDefn());
    //		OGRPolygon tmpPoly;
    //		OGRLinearRing linerRing;
    //		for (int i = 0; i < it->second.vecPolygonPts.size(); ++i)
    //		{
    //			linerRing.addPoint( it->second.vecPolygonPts[i].GetX(),it->second.vecPolygonPts[i].GetY());
    //		}
    //		//linerRing.setPoints(pntNum, padx, pady);
    //		linerRing.closeRings();

    //		tmpPoly.addRing(&linerRing);
    //		pFeature_w->SetGeometry(&tmpPoly);

    //		if (poLayer_w->CreateFeature(pFeature_w) != OGRERR_NONE)
    //		{
    //			printf("Failed to create feature in shapefile\n");
    //			continue;;
    //		}
    //		OGRFeature::DestroyFeature(pFeature_w);
    //	}

    //	if (nullptr != poDSw)
    //	{
    //		GDALClose(poDSw);
    //	}
    //}

    return m_mapKeyPtToTsaiPolys;
}

const std::map<vertex, ThiessenPolygon> Voronoi::CutVoronoi(const ThiessenPolygon& polyBound)
{
    // 1.筛选polyBound内的泰森多边形，与polyBound相交的情况取相交部分的多边形，结果保存在mapPoly中.
    ThiessenPolygon polyIntersect;
    std::map<vertex, ThiessenPolygon> mapPoly;
    std::map<vertex, ThiessenPolygon>::iterator it = m_mapKeyPtToTsaiPolys.begin();
    for (; it != m_mapKeyPtToTsaiPolys.end(); ++it)
    {
        polyIntersect = it->second.CalcIntersectPolygon(polyBound);
        if (!polyIntersect.vecPolygonPts.empty())
        {
            mapPoly[it->first] = polyIntersect;
        }
    }

    // 2.计算mapPoly中每个多边形的权重(包围框上四个点所在的多边形除外).
    vertex P1(m_rectTsaiBound.x0, m_rectTsaiBound.y0);
    vertex P2(m_rectTsaiBound.x1, m_rectTsaiBound.y1);
    vertex P3(m_rectTsaiBound.x1, m_rectTsaiBound.y0);
    vertex P4(m_rectTsaiBound.x0, m_rectTsaiBound.y1);
    double dblSum = 0.0;
    for (std::map<vertex, ThiessenPolygon>::iterator it0 = mapPoly.begin(); it0 != mapPoly.end(); ++it0)
    {
        it0->second.CalcArea();
        if (it0->first == P1 || it0->first == P2 || it0->first == P3 || it0->first == P4)
        {
            continue;
        }
        dblSum += it0->second.dblArea;
    }
    for (std::map<vertex, ThiessenPolygon>::iterator it0 = mapPoly.begin(); it0 != mapPoly.end(); ++it0)
    {
        if (it0->first == P1 || it0->first == P2 || it0->first == P3 || it0->first == P4)
        {
            it0->second.dblWeight = 0.0;
            continue;
        }
        it0->second.dblWeight = it0->second.dblArea / dblSum;
    }

    return mapPoly;
}

//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Protected:
void Voronoi::CreateDelaunayTriangle()
{
    Delaunay dela;
    dela.Triangulate(m_setTsaiKeyPts, m_setTsaiTriangles);
}

void Voronoi::SortDelaunayByEveryKeyPoints()
{
    vertexSet::iterator it = m_setTsaiKeyPts.begin();
    for (; it != m_setTsaiKeyPts.end(); ++it)
    {
        /// 顺序或逆序选出以该点为中心的所有三角形.
        std::vector<triangle> vecTris;

        // 1.选出一个包含该点的三角形.
        triangleSet::iterator it2 = m_setTsaiTriangles.begin();
        for (; it2 != m_setTsaiTriangles.end(); ++it2)
        {
            if (it2->GetVertex(0)->GetPoint() == it->GetPoint() || it2->GetVertex(1)->GetPoint() == it->GetPoint() || it2->GetVertex(2)->GetPoint() == it->GetPoint())
            {
                break;
            }
        }
        if (it2 == m_setTsaiTriangles.end())
        {
            continue;  // 该语句正常来说不会被触发.
        }

        // 2.标记另外两点之一为已经用过的点.
        vertex usedPt, unusedPt;
        if (it2->GetVertex(0)->GetPoint() == it->GetPoint())
        {
            usedPt = it2->GetVertex(1)->GetPoint();
            unusedPt = it2->GetVertex(2)->GetPoint();
        }
        else if (it2->GetVertex(1)->GetPoint() == it->GetPoint())
        {
            usedPt = it2->GetVertex(0)->GetPoint();
            unusedPt = it2->GetVertex(2)->GetPoint();
        }
        else  // it2->GetVertex(2)->GetPoint() == it->GetPoint()
        {
            usedPt = it2->GetVertex(0)->GetPoint();
            unusedPt = it2->GetVertex(1)->GetPoint();
        }
        vecTris.push_back(*it2);
        vertex firstUsedPt = usedPt;
        vertex firstUnUsedPt = unusedPt;

        // 3.选出包含O和B，且不包含已标记已用点的三角形，取出未用点，更新已用点（重复此步直到选不出满足条件的三角形，进入下一层循环）
        bool bFind = false;
        bool bGoodPoly = false;     // 能旋转一周构成的泰森多边形
        bool bTurnOneSide = false;  // 旋转完一个方向但是未构成泰森多边形（改变此标志，从第一点反方向旋转）
        bool bTurnTwoSide = false;  // 两个方向都旋转完的标志
        do
        {
            bFind = false;
            triangleSet::iterator it2_2 = m_setTsaiTriangles.begin();
            for (; it2_2 != m_setTsaiTriangles.end(); ++it2_2)
            {
                if ((it2_2->GetVertex(0)->GetPoint() == it->GetPoint() || it2_2->GetVertex(1)->GetPoint() == it->GetPoint() || it2_2->GetVertex(2)->GetPoint() == it->GetPoint()) &&
                    (it2_2->GetVertex(0)->GetPoint() == unusedPt.GetPoint() || it2_2->GetVertex(1)->GetPoint() == unusedPt.GetPoint() || it2_2->GetVertex(2)->GetPoint() == unusedPt.GetPoint()) &&
                    (it2_2->GetVertex(0)->GetPoint() != usedPt.GetPoint() && it2_2->GetVertex(1)->GetPoint() != usedPt.GetPoint() && it2_2->GetVertex(2)->GetPoint() != usedPt.GetPoint()))
                {
                    bFind = true;
                    usedPt = unusedPt;
                    if (it2_2->GetVertex(0)->GetPoint() == it->GetPoint())
                    {
                        if (it2_2->GetVertex(1)->GetPoint() == unusedPt.GetPoint())
                        {
                            unusedPt = it2_2->GetVertex(2)->GetPoint();
                        }
                        else
                        {
                            unusedPt = it2_2->GetVertex(1)->GetPoint();
                        }
                    }
                    else if (it2_2->GetVertex(1)->GetPoint() == it->GetPoint())
                    {
                        if (it2_2->GetVertex(0)->GetPoint() == unusedPt.GetPoint())
                        {
                            unusedPt = it2_2->GetVertex(2)->GetPoint();
                        }
                        else
                        {
                            unusedPt = it2_2->GetVertex(0)->GetPoint();
                        }
                    }
                    else  // it2_2->GetVertex(2)->GetPoint() == it->GetPoint()
                    {
                        if (it2_2->GetVertex(0)->GetPoint() == unusedPt.GetPoint())
                        {
                            unusedPt = it2_2->GetVertex(1)->GetPoint();
                        }
                        else
                        {
                            unusedPt = it2_2->GetVertex(0)->GetPoint();
                        }
                    }

                    if (!bTurnOneSide)  // 顺序添加点
                    {
                        vecTris.push_back(*it2_2);
                    }
                    else  // 逆序添加点
                    {
                        vecTris.insert(vecTris.begin(), *it2_2);
                    }

                    // 处理旋转一圈回到起始边的情况.
                    if (firstUsedPt == unusedPt)
                    {
                        bGoodPoly = true;
                    }

                    break;
                }
            }

            if (!bFind && bTurnOneSide)
            {
                bTurnTwoSide = true;
            }

            if (!bFind && !bTurnOneSide)
            {
                bTurnOneSide = true;
                usedPt = firstUnUsedPt;
                unusedPt = firstUsedPt;
                firstUsedPt = usedPt;
                firstUnUsedPt = unusedPt;
            }

        } while (!bGoodPoly && !bTurnTwoSide);  // “旋转一圈回到起始边”或“两个方向旋转完”时退出循环

        m_mapKeyPtToTris[*it] = vecTris;
        m_mapKeyPtToIsGoodPolygon[*it] = bGoodPoly;
    }
}

void Voronoi::MakeTsaiPolygon()
{
    std::map<vertex, std::vector<triangle>>::const_iterator it = m_mapKeyPtToTris.begin();
    std::map<vertex, bool>::iterator mapKeyPtToIsGoodPolygonIt;
    for (; it != m_mapKeyPtToTris.end(); ++it)
    {
        ThiessenPolygon poly;
        std::vector<vertex>& vecPts = poly.vecPolygonPts;
        const vertex& ver0 = it->first;
        const std::vector<triangle>& vecTri = it->second;
        int cf = 0;
        for (size_t i = 0; i < vecTri.size(); ++i)
        {
            size_t ll = vecPts.size();
            if (!(i > 0 && vecTri.at(i).GetTriangleCenter() == vecPts[ll - 1]))  // 去重复点push
            {
                vecPts.push_back(vecTri.at(i).GetTriangleCenter());
            }
        }

        // 特殊情况处理.
        mapKeyPtToIsGoodPolygonIt = m_mapKeyPtToIsGoodPolygon.find(ver0);
        if (mapKeyPtToIsGoodPolygonIt != m_mapKeyPtToIsGoodPolygon.end())
        {
            if (!mapKeyPtToIsGoodPolygonIt->second)  // 若不是完整多边形
            {
                DealTsaiPolyBound(ver0, poly.vecPolygonPts);
            }
        }

        m_mapKeyPtToTsaiPolys[ver0] = poly;
    }
}

void Voronoi::DealTsaiPolyBound(const vertex& ver0, std::vector<vertex>& vecPolyPts)
{
    ThiessenPolygon poly(vecPolyPts);
    if (!poly.IsPointOnPoly(ver0))
    {
        vecPolyPts.push_back(ver0);
    }
}
