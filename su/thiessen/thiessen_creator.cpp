//
// Created by dly on 2023/1/16.
//
#pragma once
#include "thiessen/thiessen_creator.h"
#include "thiessen/jc_voronoi.h"

// 计算面积
static double PolygonArea(const std::vector<jcv_point>& polygon)
{
	int i, j;
	double area = 0;
	auto N = polygon.size();
	for (i = 0; i < N; i++) {
		j = (i + 1) % N;
		area += polygon[i].x * polygon[j].y;
		area -= polygon[i].y * polygon[j].x;
	}
	area /= 2;
	return(area < 0 ? -area : area);
}

// 切割多边形
ClipperLib::Paths PolyClip(const ClipperLib::Paths& beClip, const ClipperLib::Paths& clip)
{
	ClipperLib::Clipper clipPlatform;
	clipPlatform.Clear();
	clipPlatform.AddPaths(beClip, ClipperLib::ptSubject, true);	// 被切割的
	clipPlatform.AddPaths(clip, ClipperLib::ptClip, true);		// 切割的
	ClipperLib::Paths clipResult;
	// 第一个参数 设置切割方式为 取相交部分
	clipPlatform.Execute(ClipperLib::ctIntersection, clipResult, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
	return clipResult;

}

thiessen_creator::~thiessen_creator()
{
	jcv_diagram_free(&m_pDiagram);
}
bool thiessen_creator::set_marked_ctrl_points(const std::map<std::string, jcv_point>& markedPoints)
{
	int index = 0;
	m_pCtrlPoint.clear();
	m_pIndexToMark.clear();
	for (auto [name, point] : markedPoints)
	{
		m_pCtrlPoint.push_back(point);
		m_pIndexToMark[index] = name;
		index++;
	}
	return !m_pCtrlPoint.empty();
}

bool thiessen_creator::set_simple_ctrl_points(const std::vector<jcv_point>& points)
{
	m_pCtrlPoint.clear();
	m_pIndexToMark.clear();
	m_pCtrlPoint = points;
	return !m_pCtrlPoint.empty();

}

bool thiessen_creator::set_bounding_box(const jcv_rect& box)
{
	memcpy(&m_pRect, &box, sizeof(jcv_rect));
	return m_pRect.min.x < m_pRect.max.x && m_pRect.min.y < m_pRect.max.y;
}


bool thiessen_creator::set_clipping_path(const ClipperLib::Paths& clipPaths)
{
	m_pClipPaths = clipPaths;
	return !m_pClipPaths.empty();
}

void thiessen_creator::ClipAndCalcArea(const int& index)
{
	thiessen_polygon polygon;
	jcv_graphedge* edge  = m_pSites[index].edges;
	ClipperLib::Path pp;
	while (edge) {		// 这里取的时每个边, 连续的两个边之间有一个点是重复的,所以只取一个
		polygon.push_back(edge->pos[0]);
		pp.push_back(ClipperLib::IntPoint((ClipperLib::cInt)(edge->pos[0].x * 10e6), (ClipperLib::cInt)(edge->pos[0].y * 10e6)));
		edge = edge->next;
	}
	polygon.push_back(polygon.at(0));	// 闭合面, 首尾两个点相同
	pp.push_back(ClipperLib::IntPoint((ClipperLib::cInt)(polygon.at(0).x * 10e6), (ClipperLib::cInt)(polygon.at(0).y * 10e6)));
	ClipperLib::Paths clipPaths = {pp};
	ClipperLib::Paths clippedPaths = PolyClip(clipPaths, m_pClipPaths);
	double area = 0;
	for (auto& path: clippedPaths)
	{
		thiessen_polygon jcv_path;
		for (auto point: path)
		{
			jcv_point p;
			p.x = (double)point.X*10e-6;
			p.y = (double)point.Y*10e-6;
			jcv_path.push_back(p);
		}
		m_pIndexThiessenPolygons[index].push_back(jcv_path);
		area += PolygonArea(jcv_path);
	}
	m_pIndexAreas[m_pSites[index].index] = area;
	m_pTotalArea += area;
}

void thiessen_creator::ClipNotCalcArea(const int& index)
{
	thiessen_polygon polygon;
	jcv_graphedge* edge  = m_pSites[index].edges;
	ClipperLib::Path pp;
	while (edge) {		// 这里取的时每个边, 连续的两个边之间有一个点是重复的,所以只取一个

		polygon.push_back(edge->pos[0]);
		pp.push_back(ClipperLib::IntPoint((ClipperLib::cInt)(edge->pos[0].x * 10e6), (ClipperLib::cInt)(edge->pos[0].y * 10e6)));
		edge = edge->next;
	}
	polygon.push_back(polygon.at(0));
	pp.push_back(ClipperLib::IntPoint((ClipperLib::cInt)(polygon.at(0).x * 10e6), (ClipperLib::cInt)(polygon.at(0).y * 10e6)));
	ClipperLib::Paths clipPaths = {pp};
	ClipperLib::Paths clippedPaths = PolyClip(clipPaths, m_pClipPaths);
	for (auto& path: clippedPaths)
	{
		thiessen_polygon jcv_path;
		for (auto point: path)
		{
			jcv_point p;
			p.x = (double)point.X*10e-6;
			p.y = (double)point.Y*10e-6;
			jcv_path.push_back(p);
		}
		m_pIndexThiessenPolygons[index].push_back(jcv_path);
	}
}

void thiessen_creator::NotClipButCalcArea(const int& index)
{
	thiessen_polygon polygon;
	jcv_graphedge* edge  = m_pSites[index].edges;
	while (edge) {		// 这里取的时每个边, 连续的两个边之间有一个点是重复的,所以只取一个

		polygon.push_back(edge->pos[0]);
		edge = edge->next;
	}
	polygon.push_back(polygon.at(0));

	double area = PolygonArea(polygon);
	m_pIndexAreas[index] = area;
	m_pIndexThiessenPolygons[index].push_back(polygon);
	m_pTotalArea += area;
}


bool thiessen_creator::generate_thiessen(const bool& isClip, const bool& isCalcArea)
{

	jcv_diagram_generate(m_pCtrlPoint.size(), (const jcv_point *)&m_pCtrlPoint, &m_pRect, nullptr, &m_pDiagram);
	m_pSites = (jcv_site*)jcv_diagram_get_sites(&m_pDiagram);
	m_pTotalArea = 0;
	// 这里为了运行效率高一点, 减少大循环内的判断, 有部分代码会重复,
	if (isClip && isCalcArea)
	{
		for (int i = 0; i < m_pDiagram.numsites; i++) {
			ClipAndCalcArea(i);
		}
		for (auto& [index, area]: m_pIndexAreas)
		{
			m_pIndexAreaPercent[index]= area/m_pTotalArea;
		}
	}
	else if (isClip && !isCalcArea)
	{
		for (int i = 0; i < m_pDiagram.numsites; i++) {
			ClipNotCalcArea(i);
		}
	}
	else if (!isClip && isCalcArea)
	{
		for (int i = 0; i < m_pDiagram.numsites; i++) {
			NotClipButCalcArea(i);
		}
		for (auto& [index, area]: m_pIndexAreas)
		{
			m_pIndexAreaPercent[index]= area/m_pTotalArea;
		}
	}

	return true;
}

std::map<int, double> thiessen_creator::GetIndexAreas()
{
	return m_pIndexAreas;
}
std::map<int, double> thiessen_creator::GetIndexAreaPercent()
{
	return m_pIndexAreaPercent;
}

std::map<std::string, double> thiessen_creator::GetMarkedAreas()
{
	std::map<std::string, double> markedAreas;
	for (auto [index, mark]: m_pIndexToMark)
	{
		markedAreas[mark] = m_pIndexAreas[index];
	}
	return markedAreas;
}
std::map<std::string, double> thiessen_creator::GetMarkedAreaPercent()
{
	std::map<std::string, double> markedAreaPercent;
	for (auto [index, mark]: m_pIndexToMark)
	{
		markedAreaPercent[mark] = m_pIndexAreaPercent[index];
	}
	return markedAreaPercent;
}

thiessen_polygons thiessen_creator::GetThiessenPolygonByName(const std::string& markName)
{
	for (auto [index, mark]:m_pIndexToMark)
	{
		if (mark == markName)
		{
			return  m_pIndexThiessenPolygons[index];
		}

	}
	return {};
}

std::map<std::string, thiessen_polygons> thiessen_creator::GetThiessenPolygon()
{
	std::map<std::string, thiessen_polygons> markedThiessen;
	for (auto [index, mark]:m_pIndexToMark)
	{
		markedThiessen[mark] = m_pIndexThiessenPolygons[index];
	}

	return markedThiessen;
}

