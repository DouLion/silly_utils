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
#include <string.h>
#include <stdlib.h>
#include <cstdlib>
#include <cmath>

 // TODO: finish
namespace silly_math
{

	template <typename T>
	class matrix_2d
	{
		/// <summary>
		/// 这个目前是线程不安全的,使用时需要注意
		/// </summary>
	public:
		matrix_2d<T>() = default;

		/// <summary>
		/// 仅传递数据指针
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		matrix_2d<T>& operator = (const matrix_2d<T>& other)
		{
			this->rows = other.rows;
			this->cols = other.cols;
			this->data = other.data;
			return *this;
		}

		/// <summary>
		/// 创建一个指定大小的二维数据
		/// </summary>
		/// <param name="r"></param>
		/// <param name="c"></param>
		/// <param name="reset">true: 如果有数据就清空并且重新初始化 false: 做任何操作,并且返回false</param>
		/// <returns></returns>
		bool create(const size_t& row, const size_t& col, bool reset = false)
		{
			if (data && !reset)
			{
				return false;
			}
			if (data && reset)	// 重复调用此函数, 会重置
			{
				destroy();
			}
			rows = row;
			cols = col;
			data = (T**)malloc(rows * sizeof(T*));
			size_t col_size = cols * sizeof(T);
			for (size_t r = 0; r < rows; r++)
			{
				data[r] = (T*)malloc(col_size);
				if (data[r])
				{
					memcpy(data[r], data[r], col_size);
				}
			}

			return true;
		}

		/// <summary>
		/// 复制数据内容到新的指针地址
		/// </summary>
		/// <returns></returns>
		matrix_2d<T>& copy()
		{
			matrix_2d<T> ret;
			ret.cols = cols;
			ret.rows = rows;
			if (data)
			{
				ret.data = (T**)malloc(ret.rows * sizeof(T*));
				size_t col_size = cols * sizeof(T);
				for (size_t r = 0; r < ret.rows; r++)
				{
					ret.data[r] = (T*)malloc(col_size);
					if (ret.data[r] && data[r])
					{
						memcpy(ret.data[r], data[r], col_size);
					}
				}
			}

			return ret;
		}

		T& at(size_t r, size_t c)
		{
			if (data)
			{
				return data[r][c];
			}
			return mp;
		}

		T** get_data()
		{
			return data;
		}

		void destroy()
		{
			if (data)
			{
				for (size_t r = 0; r < rows; r++)
				{
					if (data[r])
					{
						free(data[r]);
						data[r] = nullptr;
					}
				}
				free(data);
				data = nullptr;
			}

		}
		size_t row()
		{
			return rows;
		}

		size_t col()
		{
			return cols;
		}
	private:
		T** data{ nullptr };
		// 行数
		size_t rows{ 0 };
		// 列数
		size_t cols{ 0 };
		T mp{ 0 };
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

	/// 参照opencv
	enum InterpolationFlags {
		/** nearest neighbor interpolation 最临近插值 */
		INTER_NEAREST = 0,
		/** bilinear interpolation 二次性插值 */
		INTER_LINEAR = 1,
	};

	class matrix_tools
	{
	public:

		/// <summary>
		/// 对矩阵进行缩放操作
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="src">源矩阵</param>
		/// <param name="dst">目标矩阵</param>
		/// <param name="dst_row">目标矩阵行数</param>
		/// <param name="dst_col">目标矩阵列数</param>
		/// <param name="flag">插值算法</param>
		/// <returns></returns>
		template<typename T>
		bool resize(matrix_2d<T>& src, matrix_2d<T>& dst, const size_t& dst_row, const size_t& dst_col, const InterpolationFlags& flag = INTER_NEAREST)
		{
			switch(flag)
			{
			case INTER_LINEAR:
				return bilinear_resize(src,dst, dst_row, dst_col);
			default:
				return inter_nearest_resize(src,dst, dst_row, dst_col);
			}
		}
	private:
		template<typename T>
		bool inter_nearest_resize(matrix_2d<T>& src, matrix_2d<T>& dst, const size_t& dst_row, const size_t& dst_col)
		{
			return false;
		}

		template<typename T>
		bool bilinear_resize(matrix_2d<T>& src, matrix_2d<T>& dst, const size_t& dst_row, const size_t& dst_col)
		{
			return false;
		}
	};

}

#endif // SILLY_UTILS_SILLY_MATRIX_H