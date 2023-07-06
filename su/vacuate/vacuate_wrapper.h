/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/1/16 17:10
 * @version: 1.0.1
 * @software: ThiessenServer
 * @description:
 */
#pragma once

#ifndef VACUATE_WRAPPER_H
#define VACUATE_WRAPPER_H

#include <vector>

enum  VacuateAlgorithmType
{
		NTH_POINT,			// N点: 只保留第一个、最后一个和每个第n个点, 该算法非常快，但不幸的是，它不太擅长保留直线的几何特征。
		// 以下的算法均能在不同程度保持曲线的几何特征.
		RADIAL_DISTANCE,	// 径向距离: 过于紧密的连续顶点减少为单个顶点
		PERPENDICULAR_DISTANCE,	// 垂直距离:
		REUMANN_WITKAM,		// 点到线（垂直）距离公差:
		OPHEIM,				// Opheim使用最小和最大距离公差来限制搜索区域, WITKAM 约束版本
		LANG,				// 该搜索区域的第一个和最后一个点形成一个段。该段用于计算到每个中间点的垂直距离。如果任何计算出的距离大于指定的公差，搜索区域将通过排除其最后一个点来缩小。这个过程将一直持续到所有计算的距离都低于指定的公差，或者没有更多的中间点。删除所有中间点，并从旧搜索区域的最后一个点开始定义新的搜索区域。
		DOUGLAS_PEUCKER,	// 算法使用点到边距离公差。该算法从粗略的简化开始，即连接原始折线的第一个和最后一个顶点的单边。然后计算所有中间顶点到该边的距离。距离该边最远且计算距离大于指定容差的顶点将被标记为键并添加到简化中。此过程将针对当前简化中的每条边递归，直到原始折线的所有顶点都在简化结果的公差范围内。
		DOUGLAS_PEUCKER_VARIANT	// 使用点数容差代替点到边距离容差; 是一次处理单个边缘（选择伪随机）
};


class vacuate_wrapper
{
public:
	static bool VacuateNPoint(const int& n, const std::vector<double>& inputs, std::vector<double>& outputs, const int& dims = 2);
	static bool VacuateRadialDistance(const double& radialDistance, const std::vector<double>& points, std::vector<double>& outputs, const int& dims = 2);
	static bool VacuatePerpendicularDistance(const int& repeat, const double& distance, const std::vector<double>& points, std::vector<double>& outputs, const int& dims = 2);
	static bool VacuateReumannWitkam(const double& distance, const std::vector<double>& points, std::vector<double>& outputs, const int& dims = 2);
	static bool VacuateOpheim(const double& minDistance, const double& maxDistance, const std::vector<double>& points, std::vector<double>& outputs, const int& dims = 2);
	static bool VacuateLang(const int& lookAhead, const double& distance, const std::vector<double>& points, std::vector<double>& outputs, const int& dims = 2);
	static bool VacuateDouglasPeucker(const double& distance, const std::vector<double>& points, std::vector<double>& outputs, const int& dims = 2);
	static bool VacuateDouglasPeuckerVariant(const int& pointNum, const std::vector<double>& points, std::vector<double>& outputs, const int& dims = 2);
};

#endif //VACUATE_WRAPPER_H
