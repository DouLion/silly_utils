/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/27 13:22
 * @version: 1.0.1
 * @description: 中国气象局Micaps系统的数据
 */
#ifndef SILLY_UTILS_SILLY_DIAMOND_4_H
#define SILLY_UTILS_SILLY_DIAMOND_4_H
#include <su_marco.h>
#include <vector>

 /// <summary>
 /// 第四类数据格式：格点数据（文本文件）
 /// 文件头：
 ///     diamond 4  数据说明（字符串）  年  月  日  时次  时效  层次  经度格距  纬度格距  起始经度  终止经度  起始纬度  终止纬度  纬向格点数  经向格点数  等值线间隔  等值线起始值  终止值  平滑系数  加粗线值
 /// 数据：
 ///     数据按先纬向后经向放置，均为浮点数。
 /// 注：
 ///     1）网格必须为经纬度网格
 /// </summary>
class micaps_diamond_4
{
public:
	/// <summary>
	/// 根据内容处理请求头
	/// </summary>
	/// <param name="content"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	bool check_head(const std::string& content, size_t& offset);

	/// <summary>
	/// 判断已接受数据是否完整并且一致
	/// </summary>
	/// <returns></returns>
	bool is_valid();

	/// <summary>
	/// 重置对象
	/// </summary>
	/// <returns></returns>
	bool reset();

public:
	std::string format;
	std::string desc;
	std::string base_tm;
	size_t aging{ 1 };
	size_t layer{ 0 };
	float lng_step{ 0. };
	float lat_step{ 0. };
	float left{ 0. };
	float right{ 0. };
	float bottom{ 0. };
	float top{ 0. };
	size_t lat_size{ 0 };
	size_t lng_size{ 0 };
	float iso_step{ 0. };
	float iso_bv{ 0. };
	float iso_ev{ 0. };
	float smooth{ 0. };
	float bold{ 0. };
	std::vector<float> data;
};

class silly_diamond_4
{
public:
	/// <summary>
	/// 普通文件读取diamond 4格式的数据
	/// </summary>
	/// <param name="path"></param>
	/// <param name="md4"></param>
	/// <returns></returns>
	static bool read(const std::string& path, micaps_diamond_4& md4);

	/// <summary>
	/// 以mmap读取diamond 4格式的数据, 普通读取方式性能已经满足要求,这个暂时不实现
	/// </summary>
	/// <param name="path"></param>
	/// <param name="md4"></param>
	/// <returns></returns>
	static bool mmap_read(const std::string& path, micaps_diamond_4& md4);

	/// <summary>
	/// 写diamond 4格式的数据
	/// </summary>
	/// <param name="path"></param>
	/// <param name="md4"></param>
	/// <returns></returns>
	static bool write(const std::string& path, const micaps_diamond_4& md4);
private:

};

#endif // SILLY_UTILS_SILLY_DIAMOND_4_H
