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
			const size_t src_row = src.row();
			const size_t src_col = src.col();
			

			const float row_ratio = static_cast<float>(src_row) / dst_row;
			const float col_ratio = static_cast<float>(src_col) / dst_col;

			dst.create(dst_row, dst_col);

			for (size_t i = 0; i < dst_row; ++i)
			{
				const size_t src_i = std::min(static_cast<size_t>(i * row_ratio), src_row - 1);

				for (size_t j = 0; j < dst_col; ++j)
				{
					const size_t src_j = std::min(static_cast<size_t>(j * col_ratio), src_col - 1);
					dst.at(i, j) = src.at(src_i, src_j);
				}
			}

			return true;
		}

		template<typename T>
		bool bilinear_resize(matrix_2d<T>& src, matrix_2d<T>& dst, const size_t& dst_row, const size_t& dst_col)
		{
			size_t src_row = src.row();
			size_t src_col = src.col();
			if (!src_row || !src_col || !src.get_data() || !dst_row || !dst_col)
			{
				return  false;
			}

			float row_ratio = src_row*1.0 / dst_row; // 缩放比例 扩大多少倍分之一
			float col_ratio = src_col*1.0 / dst_col;

			dst.create(dst_row, dst_col);

			for (size_t r = 0; r < dst_row; ++r)
			{
				float src_i_float = r * row_ratio;  // 扩展后x的在原矩阵的位置
				size_t src_i_floor = static_cast<size_t>(std::floor(src_i_float));
				size_t src_i_ceil = std::min(src_i_floor + 1, src_row - 1);

				float y_diff = src_i_float - src_i_floor;

				for (size_t c = 0; c < dst_col; ++c)
				{
					float src_j_float = c * col_ratio;	// 扩展后y的在原矩阵的位置
					size_t src_j_floor = static_cast<size_t>(std::floor(src_j_float));
					size_t src_j_ceil = std::min(src_j_floor + 1, src_col - 1);

					float x_diff = src_j_float - src_j_floor;

					T src_top_left = src.at(src_i_floor, src_j_floor);
					T src_top_right = src.at(src_i_floor, src_j_ceil);
					T src_bottom_left = src.at(src_i_ceil, src_j_floor);
					T src_bottom_right = src.at(src_i_ceil, src_j_ceil);

					T interpolated_value = (T)(
						(1 - x_diff) * (1 - y_diff) * src_top_left +
						x_diff * (1 - y_diff) * src_top_right +
						(1 - x_diff) * y_diff * src_bottom_left +
						x_diff * y_diff * src_bottom_right
						);

					dst.at(r, c) = interpolated_value;
				}
			}

			return true;
		}

	};

}

#endif // SILLY_UTILS_SILLY_MATRIX_H