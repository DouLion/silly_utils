/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/26 14:55
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_MATRIX_H
#define SILLY_UTILS_SILLY_MATRIX_H
#include <cstddef>
// TODO: finish
namespace silly_math
{

	template<typename T>
	struct matrix_2d
	{
		T** data;
		size_t rows;
		size_t cols;
	};

	typedef matrix_2d<int> IMatrix;
	typedef matrix_2d<unsigned int> UIMatrix;
	typedef matrix_2d<float> FMatrix;
	typedef matrix_2d<double> DMatrix;
	typedef matrix_2d<short> SMatrix;
	typedef matrix_2d<unsigned short> USMatrix;
	typedef matrix_2d<char> CMatrix;
	typedef matrix_2d<unsigned char> UCMatrix;
	typedef matrix_2d<long long> LMatrix;
	typedef matrix_2d<unsigned long long> UMatrix;

	class matrix_utils
	{
	public:
		template<typename T>
		static matrix_2d<T> init(const size_t& rows, const size_t& cols, const T* v = nullptr);

		template<typename T>
		static void destroy(matrix_2d<T>& mtx);

		template<typename T>
		static void reset(matrix_2d<T>& mtx, const T& v);

		template<typename T>
		static bool resize(const matrix_2d<T>& src, const size_t& t_rows, const size_t& t_cols, matrix_2d<T>& dst );

		// TODO: 需要添加滤波核
		template<typename T>
		// 中值滤波
		static bool median_blur(const matrix_2d<T>& src, matrix_2d<T>& dst );

		template<typename T>
		// 高斯滤波
		static bool gauss_blur(const matrix_2d<T>& src, matrix_2d<T>& dst );

		template<typename T>
		// 均值滤波
		static bool mean_blur(const matrix_2d<T>& src, matrix_2d<T>& dst );

		template<typename T>
		// 双边滤波
		static bool bilateral_blur(const matrix_2d<T>& src, matrix_2d<T>& dst );

		template<typename T>
		// 膨胀
		static bool dilate(const matrix_2d<T>& src, matrix_2d<T>& dst );

		template<typename T>
		// 腐蚀
		static bool erode(const matrix_2d<T>& src, matrix_2d<T>& dst );
	};


}


#endif //SILLY_UTILS_SILLY_MATRIX_H
