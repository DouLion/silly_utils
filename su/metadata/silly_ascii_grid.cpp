/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/3 11:31
 * @version: 1.0.1
 * @description:
 */
//
// Created by dell on 2024/6/3. 实现
//

#include "silly_ascii_grid.h"

static const std::string ASC = ".asc";
static const std::string BIN = ".bin";
static const std::string PRJ = ".prj";

static const std::string NCOLS = "ncols";
static const std::string NROWS = "nrows";
static const std::string XLLCORNER = "xllcorner";
static const std::string YLLCORNER = "yllcorner";
static const std::string CELLSIZE = "cellsize";
static const std::string NODATA_VALUE = "nodata_value";

bool silly_ascii_grid::read(const std::filesystem::path& file)
{
    bool status = false;
    std::filesystem::path realfp = sufile::realpath(file);
    m_root = realfp.parent_path().string();
    m_name = realfp.stem().string();
    m_type = realfp.extension().string();
    if (ASC == m_type)
    {
        return read_asc(realfp);
    }
    else if (BIN == m_type)
    {
        return read_bin(realfp);
    }
    else
    {
        std::cerr << "不支持的格式: " << m_type << std::endl;
    }
    return status;
}

static std::string to_lower(const std::string& str)
{
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
    return lower_str;
}

bool silly_ascii_grid::read_asc(const std::filesystem::path& file)
{
    bool status = false;
    std::string content;

    if (!sufile::read(file, content))
    {
        std::cerr << "读取文件失败: " << file.u8string() << std::endl;
        return status;
    }

    std::stringstream linestream(content);
    std::string key;
    while (!linestream.eof())
    {
        linestream >> key;
        if (NCOLS == to_lower(key))
        {
            linestream >> ncols;
        }
        else if (NROWS == to_lower(key))
        {
            linestream >> nrows;
        }
        else if (XLLCORNER == to_lower(key))
        {
            linestream >> m_rect.min.x;
        }
        else if (YLLCORNER == to_lower(key))
        {
            linestream >> m_rect.min.x;
        }
        else if (CELLSIZE == to_lower(key))
        {
            linestream >> cellsize;
        }
        else if (NODATA_VALUE == to_lower(key))
        {
            linestream >> m_fill;
        }
        else
        {
            std::cerr << "Invalid key: " << key << std::endl;
            return status;
        }

        if (NODATA_VALUE == to_lower(key))
            break;  // header section ends after NODATA_value
    }

    m_data.create(nrows , ncols, true);

    double value;
    for (int r = 0; r < nrows && linestream.good(); ++r)
    {
        for (int c = 0; c < ncols && linestream.good(); ++c)
        {
            linestream >> value;
            m_data.data()[r * ncols + c] = value;
        }
    }
    std::string prj_path = std::filesystem::path(m_root).append(m_name + PRJ).string();
    read_prj(prj_path);
    status = true;
    return status;
}

bool silly_ascii_grid::write(const std::filesystem::path& file)
{
    bool status = false;
    std::string ext = file.extension().string();
    if (ASC == ext)
    {
        return write_asc(file);
    }
    if (BIN == ext)
    {
        return write_bin(file);
    }
    std::cerr << "不支持的格式: " << ext << std::endl;
    return status;
}
std::string silly_ascii_grid::stringify_ll(const int& precision)
{
    std::string ret;
    {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(15);
        ss << NCOLS << " " << ncols << "\n";
        ss << NROWS << " " << nrows << "\n";
        ss << XLLCORNER << " " << m_rect.min.x << "\n";
        ss << YLLCORNER << " " << m_rect.min.y << "\n";
        ss << CELLSIZE << " " << cellsize << "\n";
        ss << std::fixed << std::setprecision(precision);
        ss << NODATA_VALUE << " " << m_fill << "\n";
        ret.append(ss.str());
    }
    {
        for (int r = 0; r < nrows; ++r)
        {
            std::stringstream ss2;
            ss2 << std::fixed << std::setprecision(precision);
            for (int c = 0; c < ncols; ++c)
            {
                ss2 << m_data[r * ncols + c] << " ";
            }
            ss2 << "\n";
            ret.append(ss2.str());
        }
    }
    return ret;
}

bool silly_ascii_grid::read_bin(const std::filesystem::path& file)
{
    bool status = false;
    std::string content;
    if (!sufile::read(file, content))
    {
        std::cerr << "读取文件失败: " << file.u8string() << std::endl;
        return status;
    }
    double* p = reinterpret_cast<double*>(content.data());
    nrows = static_cast<size_t>(*p);
    p++;
    ncols = static_cast<size_t>(*p);
    p++;
    if ((nrows * ncols + 2) * sizeof(double) != content.size())
    {
        std::cerr << "bin文件数据和宽高不匹配" << std::endl;
        return false;
    }
    m_data.create(nrows , ncols, true);
    std::memcpy(m_data.data(), p, nrows * ncols * sizeof(double));

    return true;
}

bool silly_ascii_grid::write_asc(const std::filesystem::path& file)
{
    std::ofstream ofs(file);
    if (!ofs.is_open())
    {
        return false;
    }
    std::string _root = file.parent_path().string();
    std::string _name = file.stem().string();
    {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(15);
        ss << NCOLS << " " << ncols << "\n";
        ss << NROWS << " " << nrows << "\n";
        ss << XLLCORNER << " " << m_rect.min.x << "\n";
        ss << YLLCORNER << " " << m_rect.min.y << "\n";
        ss << CELLSIZE << " " << cellsize << "\n";
        ss << std::fixed << std::setprecision(3);
        ss << NODATA_VALUE << " " << m_fill << "\n";
        ofs.write(ss.str().c_str(), ss.str().size());
    }

    for (int r = 0; r < nrows; ++r)
    {
        std::stringstream ss2;
        ss2 << std::fixed << std::setprecision(3);
        for (int c = 0; c < ncols; ++c)
        {
            ss2 << m_data[r * ncols + c] << " ";
        }
        ss2 << "\n";
        ofs.write(ss2.str().c_str(), ss2.str().size());
    }
    ofs.close();

    std::string prj_path = std::filesystem::path(_root).append(_name + PRJ).string();
    write_prj(prj_path);
    return true;
}

bool silly_ascii_grid::write_bin(const std::filesystem::path& file)
{
    std::ofstream ofs(file);
    if (!ofs.is_open())
    {
        return false;
    }
    std::string header;
    header.resize(2 * sizeof(double));
    std::memcpy(header.data(), &nrows, sizeof(double));
    std::memcpy(header.data() + sizeof(double), &ncols, sizeof(double));
    ofs.write(header.c_str(), header.size());
    for (int r = 0; r < nrows; ++r)
    {
        std::string content = std::string(ncols * sizeof(double), 0);
        std::memcpy(content.data(), m_data.data() + r * ncols, ncols * sizeof(double));
        ofs.write(content.c_str(), content.size());
    }
    ofs.close();
    return true;
}

bool silly_ascii_grid::write_prj(const std::filesystem::path& file)
{
    // Projection    TRANSVERSE
    // Units         METERS
    // Zunits        NO
    // Xshift        0.0
    // Yshift        0.0
    // Parameters    6378137.0  6356752.314140356
    // 1.0 /* scale factor at central meridian
    // 117  0  0.0 /* longitude of central meridian
    // 0  0  0.0 /* latitude of origin
    // 500000.0 /* false easting (meters)
    // 0.0 /* false northing (meters)
    std::string filepath = file.parent_path().append(file.stem().string() + PRJ).string();
    std::stringstream ss;
    ss << "Projection    TRANSVERSE\n";
    ss << "Units         METERS\n";
    ss << "Zunits        NO\n";
    ss << "Xshift        0.0\n";
    ss << "Yshift        0.0\n";
    ss << "Parameters    6378137.0  6356752.314140356\n";
    ss << "1.0 /* scale factor at central meridian\n";
    ss << static_cast<int>(l0) << "  0  0.0 /* longitude of central meridian\n";
    ss << "0  0  0.0 /* latitude of origin\n";
    ss << "500000.0 /* false easting (meters)\n";
    ss << "0.0 /* false northing (meters)\n";
    if (sufile::write(filepath, ss.str()))
    {
        return true;
    }
    return false;
}

bool silly_ascii_grid::read_prj(const std::filesystem::path& file)
{
    // 提取高斯分带中心线经度
    std::vector<std::string> lines;
    if (sufile::readlines(file, lines))
    {
        if (lines.size() >= 11)
        {
            std::stringstream ss(lines[7]);
            ss >> l0;
            return true;
        }
    }

    return false;
}
