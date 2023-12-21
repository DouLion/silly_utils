//
// Created by dell on 2023/12/6.
//

#include "silly_vector_to_raster.h"
#include "su_marco.h"
#include <algorithm>
#include <math.h>

#define SILLY_SV2R_CHECK_INTERSECT_POINT(a, b)\
    SV2RPoint v1 = vertices[a];\
    SV2RPoint v2 = vertices[b];\
    if ((scanY >= v1.y && scanY < v2.y) || (scanY >= v2.y && scanY < v1.y))\
    {\
        float slope = (v2.x - v1.x) / (v2.y - v1.y);\
        float x = (scanY - v1.y) * slope + v1.x;\
        edges.push_back(std::round(x));\
    }\


bool xscan_line_raster::init() {
	return false;
}

bool xscan_line_raster::rasterization(const silly_multi_poly& m_polys)
{
	std::vector<std::vector<SV2RPoint>> vertices_arr;
	double left = 180., right = -180., top = -180.0, bottom = 180.;
	// 找出上下左右
	for (const auto& poly : m_polys)
	{
		for (const auto& ring : poly.inner_rings)
		{
			for (const auto& point : ring.points)
			{
				left = SU_MIN(point.lgtd, left);
				right = SU_MAX(point.lgtd, right);
				top = SU_MAX(point.lttd, top);
				bottom = SU_MIN(point.lttd, bottom);
			}
		}
		for (const auto& point : poly.outer_ring.points)
		{
			left = SU_MIN(point.lgtd, left);
			right = SU_MAX(point.lgtd, right);
			top = SU_MAX(point.lttd, top);
			bottom = SU_MIN(point.lttd, bottom);
		}
	}

	left = std::floor(left / cell_size) * cell_size;
	right = std::ceil(right / cell_size) * cell_size;
	top = std::ceil(top / cell_size) * cell_size;
	bottom = std::floor(bottom / cell_size) * cell_size;

	xllcorner = left;
	yllcorner = bottom;


	for (const auto& poly : m_polys)
	{
		for (const auto& ring : poly.inner_rings)
		{
			std::vector<SV2RPoint> tmp_vertices;
			for (const auto& point : ring.points)
			{
				tmp_vertices.push_back(SV2RPoint(std::round((point.lgtd - left) / cell_size), std::round((point.lttd - bottom) / cell_size)));
			}
			vertices_arr.push_back(tmp_vertices);
		}
		std::vector<SV2RPoint> tmp_vertices;
			
		for (const auto& point : poly.outer_ring.points)
		{
			tmp_vertices.push_back(SV2RPoint(std::round((point.lgtd - left) / cell_size), std::round((top - point.lttd) / cell_size)));
		}
		vertices_arr.push_back(tmp_vertices);
	}

	return rasterization(vertices_arr);
}

bool xscan_line_raster::rasterization(const std::vector<std::vector<SV2RPoint>> vertices_arr)
{
	size_t minY = SIZE_MAX, maxY = 0;
	size_t minX = SIZE_MAX, maxX = 0;

	for (const auto& part : vertices_arr)
	{
		for (const auto& point : part)
		{
			minY = SU_MIN(minY, point.y);
			maxY = SU_MAX(maxY, point.y);
			minX = SU_MIN(minX, point.x);
			maxX = SU_MAX(maxX, point.x);
		}
	}
	nrows = maxY - minY + 1;
	ncols = maxX - minX + 1;

	
	// 对每一条扫描线进行处理
	for (int scanY = minY; scanY <= maxY; ++scanY)
	{
		// 构建边缘列表
		std::vector<size_t> edges;
		for (size_t part = 0; part < vertices_arr.size(); ++part)
		{
			auto vertices = vertices_arr[part];
			size_t numVertices = vertices.size();
			for (size_t i = 0; i < numVertices - 1; ++i)
			{
				SV2RPoint v1 = vertices[i]; 
				SV2RPoint v2 = vertices[(i+1)% numVertices];
				if ((scanY >= v1.y && scanY < v2.y) || (scanY >= v2.y && scanY < v1.y))
				{
					float slope = (v2.x - v1.x) / (v2.y - v1.y); 
					float x = (scanY - v1.y) * slope + v1.x; 

					std::cout << x << std::endl;
					if (x < 0)
					{
						int a = 0;
					}
					edges.push_back(std::round(x)); 
				}
			}
			

		}
	
		// 根据X值对边缘进行排序
		std::sort(edges.begin(), edges.end());
	/*	std::cout << scanY << " : ";
		for (auto e : edges)
		{
			std::cout << e << ", ";
		}
		std::cout << std::endl;*/
		// 填充位于两条边之间的像素
		size_t old = 0;
		for (size_t i = 0; i < edges.size(); i += 2)
		{
			SV2RPair tmp_pair;
			tmp_pair.beg = edges[i];
			tmp_pair.end = edges[i + 1];
			row_pairs[scanY].push_back(tmp_pair);
		}
	}
	return true;
}

bool xscan_line_raster::rasterization(const silly_multi_silly_line& m_lines)
{
	return false;
}

void xscan_line_raster::print()
{
	// std::map<size_t, std::map<size_t, size_t>> row_record;

	printf("Display:");
	for (size_t r = 0; r < nrows; ++r)
	{
		printf("\n");
		if (row_pairs.find(r) == std::end(row_pairs))
		{
			for (size_t i = 0; i < ncols; ++i)
			{
				printf("*");
			}
			continue;
		}
		size_t old = 0;
		for (auto p: row_pairs[r])
		{
			for (size_t i = old; i < p.beg; ++i)
			{
				printf("*");
			}
			for (size_t i = p.beg; i < p.end; ++i)
			{
				printf(" ");
			}
			old = p.end;
		}
		for (size_t i = old; i < ncols; ++i)
		{
			printf("*");
		}
	}
}
