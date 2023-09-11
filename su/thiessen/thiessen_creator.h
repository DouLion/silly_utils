/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/1/16 11:34
 * @version: 1.0.1
 * @software: ThiessenServer
 * @description:
 */
#pragma once

#ifndef THIESSEN_CREATOR_H
#define THIESSEN_CREATOR_H
// 由于 jc_voronoi_clip 仅支持凸多边形的裁剪,所以放弃使用其自带的多边形裁剪方式.
#define USING_DOUBLE
#define JC_VORONOI_IMPLEMENTATION

#include <thiessen/jvc_struct.h>
#include <vector>
#include <polyclipping/clipper.hpp>
#include <map>

typedef std::vector<jcv_point> thiessen_polygon;
typedef std::vector<thiessen_polygon> thiessen_polygons;


// 1 有无边界   2 切割  3 是否需要计算面积和面积百分比
class thiessen_creator
{
public:
	thiessen_creator() = default;

	~thiessen_creator();

	// 设置被标记的控制点
	bool set_marked_ctrl_points(const std::map<std::string, jcv_point>& markedPoints);

	bool set_simple_ctrl_points(const std::vector<jcv_point>& points);

	bool set_bounding_box(const jcv_rect& box);

	bool set_clipping_path(const ClipperLib::Paths& clipPaths);


	/**
	 * @desc : 生成切割后的泰森多边形
	 * @author: dou li yang
	 * @date : 2023-01-16
	 * @param : 
	 * @param null:
	 * @return : 
	 */
	bool generate_thiessen(const bool& isClip = true, const bool& isCalcArea = true);

	std::map<int, double> GetIndexAreas();

	std::map<int, double> GetIndexAreaPercent();

	std::map<std::string, double> GetMarkedAreas();

	std::map<std::string, double> GetMarkedAreaPercent();

	// 存在情况, 切割的不规则多变形可能会将一个泰森多边形切割为多个部分,之前没有考虑这种情况导致有些计算不是很准确
	thiessen_polygons GetThiessenPolygonByName(const std::string& markName);

	std::map<std::string, thiessen_polygons> GetThiessenPolygon();

private:
	void ClipAndCalcArea(const int& index);

	void ClipNotCalcArea(const int& index);

	void NotClipButCalcArea(const int& index);


private:

	jcv_rect m_pRect;
	jcv_diagram m_pDiagram;
	std::vector<jcv_point> m_pCtrlPoint;
	// 切割用的 边界数据
	ClipperLib::Paths m_pClipPaths;
	// 顶点索引号,与标记名称的关系
	std::map<int, std::string> m_pIndexToMark;
	// 生成泰森多边形的信息
	jcv_site* m_pSites;
	// 按照索引号标记的面积
	std::map<int, double> m_pIndexAreas;
	// 按照索引号标记的面积百分比
	std::map<int, double> m_pIndexAreaPercent;
	// 按照标记名称标记的面积
	std::map<std::string, double> m_pMarkedAreas;
	// 按照标记名称标记的面积百分比
	std::map<std::string, double> m_pMarkedAreaPercent;
	// 按索引标记的泰森多边形, 如果是切割过的,可能会是泰森多边形
	std::map<int, thiessen_polygons> m_pIndexThiessenPolygons;
	double m_pTotalArea;


};

#endif //THIESSEN_CREATOR_H
