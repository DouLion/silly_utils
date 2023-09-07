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
		/// ���Ŀǰ���̲߳���ȫ��,ʹ��ʱ��Ҫע��
		/// </summary>
	public:
		matrix_2d<T>() = default;

		/// <summary>
		/// ����������ָ��
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
		/// ����һ��ָ����С�Ķ�ά����
		/// </summary>
		/// <param name="r"></param>
		/// <param name="c"></param>
		/// <param name="reset">true: ��������ݾ���ղ������³�ʼ�� false: ���κβ���,���ҷ���false</param>
		/// <returns></returns>
		bool create(const size_t& row, const size_t& col, bool reset = false)
		{
			if (data && !reset)
			{
				return false;
			}
			if (data && reset)	// �ظ����ô˺���, ������
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

		matrix_2d<T>& operator += (const matrix_2d<T>& other)
		{
			if (data && other.data() && rows == other.rows() && cols == other.cols())
			{
				for (size_t r = 0; r < rows; r++)
				{
					for (size_t c = 0; c < cols; ++c)
					{
						data[r][c] += other.data()[r][c];
					}
				}
			}
			return *this;
		}

		matrix_2d<T>& operator -= (const matrix_2d<T>& other)
		{
			if (data && other.data() && rows == other.rows() && cols == other.cols())
			{
				for (size_t r = 0; r < rows; r++)
				{
					for (size_t c = 0; c < cols; ++c)
					{
						data[r][c] -= other.data()[r][c];
					}
				}
			}
			return *this;
		}

		/// <summary>
		/// �����������ݵ��µ�ָ���ַ
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

		/// <summary>
		/// ����������Ϊһ��ֵ
		/// </summary>
		/// <param name="val"></param>
		void set(T val)
		{
			if (data)
			{
				for (size_t r = 0; r < rows; r++)
				{
					for (size_t c = 0; c < cols; ++c)
					{

						data[r][c] = val;
					}
				}
			}
		}

		/// <summary>
		/// �ͷ��ڴ�,��Ҫ�ֶ�����
		/// </summary>
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
		// ����
		size_t rows{ 0 };
		// ����
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

	/// ����opencv
	enum InterpolationFlags {
		/** nearest neighbor interpolation ���ٽ���ֵ */
		INTER_NEAREST = 0,
		/** bilinear interpolation �����Բ�ֵ */
		INTER_LINEAR = 1,
	};

	class matrix_tools
	{
	public:

		/// <summary>
		/// ������ͬ���͵ľ����໥ת��
		/// </summary>
		/// <typeparam name="T1"></typeparam>
		/// <typeparam name="T2"></typeparam>
		/// <param name="src">Դ����</param>
		/// <param name="dst">Ŀ�����</param>
		/// <returns></returns>
		template <typename T1, typename T2>
		static bool convert_matrix(silly_math::matrix_2d<T1>& src, silly_math::matrix_2d<T2>& dst)
		{
			size_t rows = src.row();
			size_t cols = src.col();

			bool is_convert = dst.create(rows, cols);
			if (!is_convert)
			{
				return is_convert;
			}

			for (size_t i = 0; i < rows; ++i)
			{
				for (size_t j = 0; j < cols; ++j)
				{
					dst.at(i, j) = static_cast<T2>(src.at(i, j));
				}
			}
			return true;
		}

		/// <summary>
		/// �Ծ���������Ų���
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="src">Դ����</param>
		/// <param name="dst">Ŀ�����</param>
		/// <param name="dst_row">Ŀ���������</param>
		/// <param name="dst_col">Ŀ���������</param>
		/// <param name="flag">��ֵ�㷨</param>
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

			float row_ratio = (float)(src_row) / dst_row; // ���ű��� ������ٱ���֮һ
			float col_ratio = (float)(src_col) / dst_col;

			dst.create(dst_row, dst_col);

			for (size_t i = 0; i < dst_row; ++i)
			{
				float src_i_float = i * row_ratio;  // ��չ��x����ԭ�����λ��
				size_t src_i_floor = static_cast<size_t>(std::floor(src_i_float));
				size_t src_i_ceil = std::min(src_i_floor + 1, src_row - 1);

				float y_diff = src_i_float - src_i_floor;

				for (size_t j = 0; j < dst_col; ++j)
				{
					float src_j_float = j * col_ratio;	// ��չ��y����ԭ�����λ��
					size_t src_j_floor = static_cast<size_t>(std::floor(src_j_float));
					size_t src_j_ceil = std::min(src_j_floor + 1, src_col - 1);

					float x_diff = src_j_float - src_j_floor;

					T src_top_left = src.at(src_i_floor, src_j_floor);
					T src_top_right = src.at(src_i_floor, src_j_ceil);
					T src_bottom_left = src.at(src_i_ceil, src_j_floor);
					T src_bottom_right = src.at(src_i_ceil, src_j_ceil);

					T interpolated_value = static_cast<T>(
						(1 - x_diff) * (1 - y_diff) * src_top_left +
						x_diff * (1 - y_diff) * src_top_right +
						(1 - x_diff) * y_diff * src_bottom_left +
						x_diff * y_diff * src_bottom_right
						);

					dst.at(i, j) = interpolated_value;
				}
			}

			return true;
		}

	};

}

#endif // SILLY_UTILS_SILLY_MATRIX_H