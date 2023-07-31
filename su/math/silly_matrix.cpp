//
// Created by dell on 2023/7/26.
//

#include "silly_matrix.h"
using namespace silly_math;

template<typename T>
matrix_2d<T> matrix_utils::init(const size_t& rows, const size_t& cols, const T* v)
{
	return matrix_2d<T>();
}

template<typename T>
void matrix_utils::destroy(matrix_2d<T>& mtx)
{

}

template<typename T>
void matrix_utils::reset(matrix_2d<T>& mtx, const T& v)
{

}

template<typename T>
bool matrix_utils::median_blur(const matrix_2d<T>& src, matrix_2d<T>& dst)
{
	return 0;
}

template<typename T>
bool matrix_utils::resize(const matrix_2d<T>& src, const size_t& t_rows, const size_t& t_cols, matrix_2d<T>& dst)
{
	return 0;
}

template<typename T>
bool matrix_utils::gauss_blur(const matrix_2d<T>& src, matrix_2d<T>& dst)
{
	return 0;
}

template<typename T>
bool matrix_utils::mean_blur(const matrix_2d<T>& src, matrix_2d<T>& dst)
{
	return 0;
}

template<typename T>
bool matrix_utils::bilateral_blur(const matrix_2d<T>& src, matrix_2d<T>& dst)
{
	return 0;
}

template<typename T>
bool matrix_utils::dilate(const matrix_2d<T>& src, matrix_2d<T>& dst)
{
	return 0;
}

template<typename T>
bool matrix_utils::erode(const matrix_2d<T>& src, matrix_2d<T>& dst)
{
	return 0;
}
