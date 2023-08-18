#include "silly_pyramid.h"

google_pyramid_block google_pyramid_util::get_block_rect(size_t l, size_t c, size_t r)
{

	google_pyramid_block ret_block(l, c, r);
	int ceil_num = pow(2, l);	// �ò����������
	double x_delta = (SILLY_GLOBAL_RIGHT - SILLY_GLOBAL_LEFT) / ceil_num;
	double y_delta = (SILLY_GLOBAL_TOP - SILLY_GLOBAL_BOTTOM) / ceil_num;
	ret_block.rect.left = c * x_delta + (0 - SILLY_GLOBAL_LEFT); // ԭ��x�����ϵ�ƫ����
	ret_block.rect.right = ret_block.rect.left + x_delta;
	ret_block.rect.top = SILLY_GLOBAL_TOP - y_delta * r; // ���������ϵ���, �뾭γ�ȱ�ʾ���෴, ���Ҽ�ȥy�����ϵ�ƫ����
	ret_block.rect.bottom = ret_block.rect.top - y_delta;

	return ret_block;

}

google_pyramid_block google_pyramid_util::get_point_block(const size_t layer, const silly_point& point)
{
	// TODO : ��ɺ���, ���������ܲ�̫����, �ٿ���
	google_pyramid_block ret_block;

	int ceil_num = pow(2, layer);	// �ò����������
	double x_delta = (SILLY_GLOBAL_RIGHT - SILLY_GLOBAL_LEFT) / ceil_num;
	double y_delta = (SILLY_GLOBAL_TOP - SILLY_GLOBAL_BOTTOM) / ceil_num;

	// int col = 

	return ret_block;

}
