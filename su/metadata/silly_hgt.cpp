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

static int16_t swap_bytes(int16_t val)
{
    return (val << 8) | ((val >> 8) & 0xFF);
}

bool suHGT::parse_metadata(const std::string& hgt_filepath, HgtData& out_hgt_data)
{
    out_hgt_data.clear();
    out_hgt_data.filepath = hgt_filepath;

    suPath p = suPath::suPath(hgt_filepath);
    std::string filename = p.name();

    // 验证文件名长度和后缀
    if (filename.length() < 10 || filename.substr(filename.length() - 4) != ".hgt")
    {
        SLOG_WARN("警告: 跳过无效的 HGT 文件名格式: {}", filename)
        return false;
    }

    try
    {
        // 提取纬度：文件名第一个字符为 N/S，后两位为纬度值
        char lat_hemisphere = filename[0];
        std::string lat_str = filename.substr(1, 2);
        out_hgt_data.southwest_lat = std::stoi(lat_str);
        if (lat_hemisphere == 'S')
            out_hgt_data.southwest_lat *= -1;

        // 提取经度：文件名第四个字符为 E/W，后三位为经度值
        char lon_hemisphere = filename[3];
        std::string lon_str = filename.substr(4, 3);
        out_hgt_data.southwest_lon = std::stoi(lon_str);
        if (lon_hemisphere == 'W')
            out_hgt_data.southwest_lon *= -1;
    }
    catch (const std::exception& e)
    {
        SLOG_WARN("警告: 解析 HGT 文件名 {} 失败 {}", filename, e.what())
        return false;
    }

    return true;
}

bool suHGT::open(const std::string& root_path)
{
    if (!suPath::exists(root_path) || !suPath::is_dir(root_path))
    {
        SLOG_ERROR("错误: 指定的根目录不存在或不是一个目录: {}", root_path)
        return false;
    }

    hgt_tiles_.clear();

    int hgt_files_found = 0;
    for (const auto& entry : suPath::list(root_path, "*.hgt"))
    {
        if (entry.is_file())
        {
            suHGT::HgtData hgt_data;
            if (parse_metadata(entry.path().string(), hgt_data))
            {
                // 使用西南角坐标作为 key 存储瓦片
                hgt_tiles_[{hgt_data.southwest_lat, hgt_data.southwest_lon}] = std::move(hgt_data);
                hgt_files_found++;
            }
        }
    }

    SLOG_INFO("已加载 {} 个 HGT 瓦片元数据。", hgt_files_found)
    return hgt_files_found > 0;
}

bool suHGT::load_elevations_for_tile(HgtData& hgt_data)
{
    // 如果已加载，直接返回
    if (!hgt_data.elevations.empty())
    {
        return true;
    }

    // 以 ate 模式打开，同时获取文件大小
    std::ifstream file(hgt_data.filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        SLOG_ERROR("错误: 无法打开 HGT 文件 {}", hgt_data.filepath)
        return false;
    }

    std::streampos file_size = file.tellg();
    file.seekg(0);  // 重新定位到文件开头

    // 根据文件大小判断分辨率和网格尺寸
    // SRTM1: 3601 x 3601 (1 弧秒), SRTM3: 1201 x 1201 (3 弧秒)
    if (file_size == (long long)3601 * 3601 * sizeof(int16_t))
    {
        hgt_data.resolution_arc_seconds = 1;
        hgt_data.grid_size = 3601;
    }
    else if (file_size == (long long)1201 * 1201 * sizeof(int16_t))
    {
        hgt_data.resolution_arc_seconds = 3;
        hgt_data.grid_size = 1201;
    }
    else
    {
        SLOG_ERROR("错误: 无法根据文件大小判断 HGT 文件分辨率: {}", hgt_data.filepath)
        file.close();
        return false;
    }

    // 分配内存并读取数据
    hgt_data.elevations.resize(static_cast<size_t>(hgt_data.grid_size) * hgt_data.grid_size);
    file.read(reinterpret_cast<char*>(hgt_data.elevations.data()), hgt_data.elevations.size() * sizeof(int16_t));

    if (!file)
    {
        SLOG_ERROR("错误: 读取 HGT 文件数据时发生错误: {}", hgt_data.filepath)
        file.close();
        hgt_data.unload_elevations();
        return false;
    }

    file.close();

    // 转换字节序（从大端到小端或反之）
    for (int16_t& elevation : hgt_data.elevations)
    {
        elevation = swap_bytes(elevation);
    }

    SLOG_INFO("已按需加载 HGT 瓦片数据: {}", hgt_data.filepath)
    return true;
}

void suHGT::unload_all_elevations()
{
    int count = 0;
    for (auto& pair : hgt_tiles_)
    {
        if (!pair.second.elevations.empty())
        {
            pair.second.unload_elevations();
            count++;
        }
    }

    if (count > 0)
    {
        SLOG_INFO("已卸载 {} 个 HGT 瓦片的海拔数据。", count)
    }
}

bool suHGT::unload_tile_elevations(int lat, int lon)
{
    auto it = hgt_tiles_.find({lat, lon});
    if (it != hgt_tiles_.end() && !it->second.elevations.empty())
    {
        it->second.unload_elevations();
        SLOG_INFO("已卸载 HGT 瓦片: ({}, {})", lat, lon)
        return true;
    }

    return false;
}

suHGT::HgtData* suHGT::get_hgt_data(double lat, double lon)
{
    // 获取瓦片坐标（经纬度整数部分）
    int tile_lat = static_cast<int>(std::floor(lat));
    int tile_lon = static_cast<int>(std::floor(lon));

    // 查找对应瓦片
    auto it = hgt_tiles_.find({tile_lat, tile_lon});
    if (it == hgt_tiles_.end())
    {
        return nullptr;
    }

    HgtData& hgt_data = it->second;

    // 如果数据未加载，则按需加载
    if (hgt_data.elevations.empty())
    {
        if (!load_elevations_for_tile(hgt_data))
        {
            SLOG_ERROR("错误: 无法为坐标 ({}, {}) 加载瓦片数据 {}", lat, lon, hgt_data.filepath)
            return nullptr;
        }
    }

    return &hgt_data;
}

int16_t suHGT::get_elevation(double lat, double lon)
{
    const HgtData* hgt_data = get_hgt_data(lat, lon);
    if (!hgt_data)
    {
        return -32768;
    }

    // 将经纬度坐标转换为网格索引（行列）
    // 注意：行的计算为倒序（纬度高的点在上方，数组索引小）
    double float_row = (1.0 - (lat - hgt_data->southwest_lat)) * (hgt_data->grid_size - 1);
    double float_col = (lon - hgt_data->southwest_lon) * (hgt_data->grid_size - 1);

    // 最近邻插值：四舍五入到最近的整数索引
    int row = static_cast<int>(std::round(float_row));
    int col = static_cast<int>(std::round(float_col));

    return get_elevation_at_index(*hgt_data, row, col);
}

double suHGT::get_interpolated_elevation(double lat, double lon)
{
    const HgtData* hgt_data = get_hgt_data(lat, lon);
    if (!hgt_data)
    {
        return -32768.0;
    }

    // 将经纬度坐标转换为网格索引（浮点形式）
    double float_row = (1.0 - (lat - hgt_data->southwest_lat)) * (hgt_data->grid_size - 1);
    double float_col = (lon - hgt_data->southwest_lon) * (hgt_data->grid_size - 1);

    // 获取四个相邻的网格点索引
    int row1 = static_cast<int>(std::floor(float_row));
    int col1 = static_cast<int>(std::floor(float_col));
    int row2 = row1 + 1;
    int col2 = col1 + 1;

    // 计算相对权重（用于线性插值）
    double weight_row = float_row - row1;
    double weight_col = float_col - col1;

    // 获取四个角的海拔值
    int16_t e11 = get_elevation_at_index(*hgt_data, row1, col1);
    int16_t e12 = get_elevation_at_index(*hgt_data, row1, col2);
    int16_t e21 = get_elevation_at_index(*hgt_data, row2, col1);
    int16_t e22 = get_elevation_at_index(*hgt_data, row2, col2);

    // 如果任何角的值为缺失数据，返回缺失值
    if (e11 == -32768 || e12 == -32768 || e21 == -32768 || e22 == -32768)
    {
        return -32768.0;
    }

    // 双线性插值公式
    double interpolated_val = (e11 * (1.0 - weight_col) + e12 * weight_col) * (1.0 - weight_row) + (e21 * (1.0 - weight_col) + e22 * weight_col) * weight_row;

    return interpolated_val;
}

int16_t suHGT::get_elevation_at_index(const HgtData& hgt_data, int row, int col) const
{
    // 检查行列索引是否在有效范围内
    if (row < 0 || row >= hgt_data.grid_size || col < 0 || col >= hgt_data.grid_size)
    {
        return -32768;  // 返回缺失数据标记
    }

    // 计算一维数组索引（HGT 数据按行优先存储）
    size_t index = static_cast<size_t>(row * hgt_data.grid_size + col);
    if (index >= hgt_data.elevations.size())
    {
        SLOG_ERROR("内部错误: 索引超出 elevations 向量大小。")
        return -32768;
    }

    return hgt_data.elevations[index];
}