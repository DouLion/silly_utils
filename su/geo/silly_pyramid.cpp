#include "silly_pyramid.h"

google_pyramid_block google_pyramid_util::get_block_rect(size_t l, size_t c, size_t r)
{

	google_pyramid_block ret_block(l, c, r);
	int ceil_num = pow(2, l);	// 该层的行数列数
	double x_delta = (SILLY_GLOBAL_RIGHT - SILLY_GLOBAL_LEFT) / ceil_num;
	double y_delta = (SILLY_GLOBAL_TOP - SILLY_GLOBAL_BOTTOM) / ceil_num;
	ret_block.rect.left = c * x_delta + (0 - SILLY_GLOBAL_LEFT); // 原点x方向上的偏移量
	ret_block.rect.right = ret_block.rect.left + x_delta;
	ret_block.rect.top = SILLY_GLOBAL_TOP - y_delta * r; // 方向上由上到下, 与经纬度表示的相反, 并且减去y方向上的偏移量
	ret_block.rect.bottom = ret_block.rect.top - y_delta;

	return ret_block;

}

google_pyramid_block google_pyramid_util::get_point_block(const size_t layer, const silly_point& point)
{
	// TODO : 完成函数, 这两个可能不太好用, 再考虑
	google_pyramid_block ret_block;

	int ceil_num = pow(2, layer);	// 该层的行数列数
	double x_delta = (SILLY_GLOBAL_RIGHT - SILLY_GLOBAL_LEFT) / ceil_num;
	double y_delta = (SILLY_GLOBAL_TOP - SILLY_GLOBAL_BOTTOM) / ceil_num;

	// int col = 

	return ret_block;

}
