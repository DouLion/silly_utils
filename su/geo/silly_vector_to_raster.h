/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/6 16:15
 * @version: 1.0.1
 * @description: 矢量光栅化
 */
#ifndef SILLY_UTILS_SILLY_VECTOR_TO_RASTER_H
#define SILLY_UTILS_SILLY_VECTOR_TO_RASTER_H

#include <geo/silly_geo.h>


struct SV2RPoint
{
	SV2RPoint() = default;
	SV2RPoint(int xx, int yy)
	{
		x = xx;
		y = yy;
	}
	int x{ 0 }, y{ 0 };
};

struct SV2REdge
{
	int x{ 0 };
	float slopeInverse{ 0. };
};

struct SV2RPair
{
	size_t beg;
	size_t end;
};
/// X扫描线算法
class xscan_line_raster
{
public:
	bool init();

	// 光栅化  输入 一个矢量面(线? 点?) 输出一组 坐标
	bool rasterization(const silly_multi_poly& m_polys);

	bool rasterization(const std::vector<std::vector<SV2RPoint>> vertices_arr);

	bool rasterization(const silly_multi_silly_line& m_lines);

	void print();
public:
	/* 参照dem文件的表达方式
	ncols         9478
	nrows         7183
	xllcorner     218990.98442769
	yllcorner     3278474.0715599
	cellsize      9.8080330000000
	NODATA_value  -9999
	*/
	// 数据记录以经纬度左下角为原点, 向东为列的正方形,向上为行的正方向
	size_t ncols;
	size_t nrows;
	float xllcorner;
	float yllcorner;
	// 经纬度小数点后6位能精确到1米,更加精确意义不大
	float cell_size{ 0.000001 };
	// 记录每一行在矢量内的多对起始列号
	std::map<size_t, std::vector<SV2RPair>> row_pairs;
};

#endif //SILLY_UTILS_SILLY_VECTOR_TO_RASTER_H
