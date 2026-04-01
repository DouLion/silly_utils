/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2026-04-01
 * @file: silly_hgt
 * @description: silly_hgt实现
 * @version: v1.0.1 2026-04-01 dou li yang
 */
#include "silly_hgt.h"

short suHGT::swap_bytes(short val)
{
    return (val << 8) | ((val >> 8) & 0xFF);
}

bool suHGT::parse_filename(const std::string& hgt_filepath, const std::string& filename, HgtData& out_hgt_data)
{
    if (filename.length() < 10 || filename.substr(filename.length() - 4) != ".hgt")
    {
        SLOG_ERROR("错误: HGT 文件名格式无效: {}", filename)
        return false;
    }

    try
    {
        // 从文件名中提取纬度
        char lat_hemisphere = filename[0];            // N (北纬) 或 S (南纬)
        std::string lat_str = filename.substr(1, 2);  // 纬度数字部分，例如 "34"
        out_hgt_data.southwest_lat = std::stoi(lat_str);
        if (lat_hemisphere == 'S')
            out_hgt_data.southwest_lat *= -1;  // 南纬用负数表示

        // 从文件名中提取经度
        char lon_hemisphere = filename[3];            // E (东经) 或 W (西经)
        std::string lon_str = filename.substr(4, 3);  // 经度数字部分，例如 "118"
        out_hgt_data.southwest_lon = std::stoi(lon_str);
        if (lon_hemisphere == 'W')
            out_hgt_data.southwest_lon *= -1;  // 西经用负数表示

        // 根据文件大小判断分辨率和网格尺寸
        std::ifstream file_check_size(hgt_filepath, std::ios::binary | std::ios::ate);  // 以二进制模式打开文件并定位到文件末尾
        if (!file_check_size.is_open())
        {
            SLOG_ERROR("错误: 无法打开文件检查大小: {}", hgt_filepath)
            return false;
        }
        std::streampos file_size = file_check_size.tellg();  // 获取文件大小
        file_check_size.close();                             // 关闭文件

        if (file_size == (long long)3601 * 3601 * sizeof(short))
        {
            out_hgt_data.resolution_arc_seconds = 1;
            out_hgt_data.grid_size = 3601;
        }
        else if (file_size == (long long)1201 * 1201 * sizeof(short))
        {
            out_hgt_data.resolution_arc_seconds = 3;
            out_hgt_data.grid_size = 1201;
        }
        else
        {
            SLOG_ERROR("错误: 无法根据文件大小判断 HGT 文件分辨率")
            return false;
        }
    }
    catch (const std::exception& e)
    {
        SLOG_ERROR("错误解析 HGT 文件名或确定分辨率: {}", e.what())
        return false;
    }
    return true;
}

// read (加载) 函数的实现
// 注意：现在它接收文件路径和 HgtData 引用
bool suHGT::read(const std::string& hgt_filepath, HgtData& hgt_data)
{
    hgt_data.clear();

    size_t last_slash = hgt_filepath.find_last_of("/\\");
    std::string filename = (last_slash == std::string::npos) ? hgt_filepath : hgt_filepath.substr(last_slash + 1);

    if (!parse_filename(hgt_filepath, filename, hgt_data))
    {
        return false;
    }

    std::ifstream file(hgt_filepath, std::ios::binary);
    if (!file.is_open())
    {
        SLOG_ERROR("错误: 无法打开 HGT 文件 {}", hgt_filepath)
        return false;
    }

    // 根据网格尺寸调整 elevations 向量的大小
    hgt_data.elevations.resize(static_cast<size_t>(hgt_data.grid_size) * hgt_data.grid_size);
    // 将原始字节数据读入向量
    file.read(reinterpret_cast<char*>(hgt_data.elevations.data()), hgt_data.elevations.size() * sizeof(short));

    if (!file)
    {
        SLOG_ERROR("读取 HGT 文件数据时发生错误: {}", hgt_filepath)
        file.close();
        hgt_data.clear();
        return false;
    }
    file.close();

    // 将数据从大端序转换为本机字节序（假设本机是小端序，如 x86/x64 处理器）
    for (short& elevation : hgt_data.elevations)
    {
        elevation = swap_bytes(elevation);
    }

    SLOG_INFO("已加载 HGT 文件: {}", hgt_filepath)
    SLOG_INFO("西南角纬度: {}  经度: {}", hgt_data.southwest_lat, hgt_data.southwest_lon)
    SLOG_INFO("分辨率: {}弧秒", hgt_data.resolution_arc_seconds)
    SLOG_INFO("网格: {} x {}", hgt_data.grid_size, hgt_data.grid_size)

    return true;
}

bool suHGT::get_coordinates(const HgtData& hgt_data, double lat, double lon, double& float_row, double& float_col) const
{
    if (!is_coordinate_within_tile(hgt_data, lat, lon))
    {
        return false;  // 坐标超出瓦片范围
    }

    double lat_offset = lat - hgt_data.southwest_lat;
    double lon_offset = lon - hgt_data.southwest_lon;

    // 纬度索引: 因为数据是从北到南（纬度减小方向），所以 (1.0 - lat_offset) 将 0.0-1.0 的范围反转。
    // 乘以 (grid_size - 1) 将 [0, 1) 的范围映射到 [0, grid_size - 1]。
    float_row = (1.0 - lat_offset) * (hgt_data.grid_size - 1);

    // 经度索引: 因为数据是从西到东（经度增大方向），直接使用 lon_offset。
    float_col = lon_offset * (hgt_data.grid_size - 1);

    return true;
}

int16_t suHGT::get_elevation(const HgtData& hgt_data, int row, int col) const
{
    // 确保索引在有效范围内
    if (row < 0 || row >= hgt_data.grid_size || col < 0 || col >= hgt_data.grid_size)
    {
        // std::cerr << "警告: 索引 (" << row << ", " << col << ") 超出 HGT 瓦片网格范围。" << std::endl;
        return -32768;  // 超出范围，返回缺失数据值
    }
    size_t index = static_cast<size_t>(row * hgt_data.grid_size + col);
    if (index >= hgt_data.elevations.size())
    {
        // 这应该不会发生，除非 grid_size 和 elevations.size 不匹配
        std::cerr << "内部错误: 计算出的索引超出 elevations 向量大小。(" << row << ", " << col << ") -> " << index << std::endl;
        return -32768;
    }
    return hgt_data.elevations[index];
}

int16_t suHGT::get_elevation(const HgtData& hgt_data, double lat, double lon) const
{
    if (!hgt_data.is_loaded())
    {
        throw std::runtime_error("HGT 数据未加载。请先调用 read() 函数。");
    }

    double float_row, float_col;
    if (!get_coordinates(hgt_data, lat, lon, float_row, float_col))
    {
        return -32768;  // 坐标超出瓦片范围
    }

    // 四舍五入到最近的整数索引
    int row = static_cast<int>(std::round(float_row));
    int col = static_cast<int>(std::round(float_col));

    // 确保索引在有效范围内（即使 round 导致超出，get_elevation_at_index 也会处理）
    return get_elevation(hgt_data, row, col);
}

double suHGT::get_interpolated_elevation(const HgtData& hgt_data, double lat, double lon) const
{
    if (!hgt_data.is_loaded())
    {
        throw std::runtime_error("HGT 数据未加载。请先调用 read() 函数。");
    }

    double float_row, float_col;
    if (!get_coordinates(hgt_data, lat, lon, float_row, float_col))
    {
        return -32768.0;  // 坐标超出瓦片范围
    }

    // 获取四个最近的网格点索引
    int row1 = static_cast<int>(std::floor(float_row));
    int col1 = static_cast<int>(std::floor(float_col));
    int row2 = row1 + 1;
    int col2 = col1 + 1;

    // 获取权重
    double weight_row = float_row - row1;
    double weight_col = float_col - col1;

    // 获取四个点的海拔值
    // 使用辅助函数确保索引安全，并处理 -32768
    short e11 = get_elevation(hgt_data, row1, col1);  // 左上
    short e12 = get_elevation(hgt_data, row1, col2);  // 右上
    short e21 = get_elevation(hgt_data, row2, col1);  // 左下
    short e22 = get_elevation(hgt_data, row2, col2);  // 右下

    // 如果任何一个点是缺失数据，则插值结果也可能是缺失数据
    if (e11 == -32768 || e12 == -32768 || e21 == -32768 || e22 == -32768)
    {
        // 可以选择返回 -32768.0，或者尝试用剩余的有效点进行插值
        // 这里为了简单，如果任何一个点缺失就返回缺失
        return -32768.0;
    }

    // 双线性插值
    double interpolated_val = (e11 * (1.0 - weight_col) + e12 * weight_col) * (1.0 - weight_row) + (e21 * (1.0 - weight_col) + e22 * weight_col) * weight_row;

    return interpolated_val;
}

bool suHGT::is_coordinate_within_tile(const HgtData& hgt_data, double lat, double lon) const
{
    if (!hgt_data.is_loaded())
    {
        throw std::runtime_error("HGT 数据未加载。请先调用 read() 函数。");
    }

    // 瓦片覆盖的纬度范围是 [southwest_lat, southwest_lat + 1.0)
    // 瓦片覆盖的经度范围是 [southwest_lon, southwest_lon + 1.0)
    return (lat >= hgt_data.southwest_lat && lat < hgt_data.southwest_lat + 1.0 && lon >= hgt_data.southwest_lon && lon < hgt_data.southwest_lon + 1.0);
}

void suHGT::get_tile_geographic_bounds(const HgtData& hgt_data, double& min_lat, double& max_lat, double& min_lon, double& max_lon) const
{
    if (!hgt_data.is_loaded())
    {
        throw std::runtime_error("HGT 数据未加载。请先调用 read() 函数。");
    }
    min_lat = static_cast<double>(hgt_data.southwest_lat);
    max_lat = static_cast<double>(hgt_data.southwest_lat + 1);
    min_lon = static_cast<double>(hgt_data.southwest_lon);
    max_lon = static_cast<double>(hgt_data.southwest_lon + 1);
}

void suHGT::get_min_max_elevation(const HgtData& hgt_data, short& min_elev, short& max_elev) const
{
    if (!hgt_data.is_loaded())
    {
        throw std::runtime_error("HGT 数据未加载。请先调用 read() 函数。");
    }
    if (hgt_data.elevations.empty())
    {
        min_elev = -32768;  // 或者其他表示“无数据”的值
        max_elev = -32768;
        return;
    }

    // 使用 C++ 标准库算法查找
    auto min_it = std::min_element(hgt_data.elevations.begin(), hgt_data.elevations.end());
    auto max_it = std::max_element(hgt_data.elevations.begin(), hgt_data.elevations.end());

    min_elev = *min_it;
    max_elev = *max_it;
}