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
		bool create(size_t r, size_t c, bool reset = false)
		{
			if (data && !reset)
			{
				return false;
			}
			if (data && reset)	// 重复调用此函数, 会重置
			{
				destroy();
			}
			rows = r;
			cols = c;
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

}


#endif //SILLY_UTILS_SILLY_MATRIX_H
