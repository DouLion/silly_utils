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

#include <log/silly_log.h>
#include <datetime/silly_timer.h>

static const std::string ASC = ".asc";
static const std::string BIN = ".bin";
static const std::string PRJ = ".prj";

static const std::string NCOLS = "ncols";
static const std::string NROWS = "nrows";
static const std::string XLLCORNER = "xllcorner";
static const std::string YLLCORNER = "yllcorner";
static const std::string CELLSIZE = "cellsize";
static const std::string NODATA_VALUE = "nodata_value";

bool suAsciiGrid::create(const suDem::Info& _info)
{
    info = _info;
    raster.create(info.height, info.width, true);
    raster.set(info.fill);
    return true;
}
bool suAsciiGrid::read(const suPath& file, const bool& onlyhead)
{
    m_root = file.parent().string();
    m_name = file.stem();
    m_type = file.extension();
    if (ASC == TO_LOWER(m_type))
    {
        return read_asc(file, onlyhead);
    }
    if (BIN == TO_LOWER(m_type))
    {
        return read_bin(file);
    }
    m_err = "不支持的格式: " + m_type;
    std::cerr << m_err << std::endl;
    return false;
}

bool suAsciiGrid::read_asc(const suPath& file, const bool& onlyhead)
{
    bool status = false;
    std::string prj_path = suPath(m_root).append(m_name + PRJ).string();
    read_prj(prj_path);
    std::string content;
    std::fstream input(file, std::ios::binary | std::ios::in);
    if (!input.is_open())
    {
        return false;
    }
    double cellsize = 0;

    std::string key;
    std::string line;
    while (std::getline(input, line))
    {
        std::istringstream linestream(line);
        linestream >> key;
        if (NCOLS == TO_LOWER(key))
        {
            linestream >> info.width;
        }
        else if (NROWS == TO_LOWER(key))
        {
            linestream >> info.height;
        }
        else if (XLLCORNER == TO_LOWER(key))
        {
            linestream >> info.bound.min.x;
        }
        else if (YLLCORNER == TO_LOWER(key))
        {
            linestream >> info.bound.min.y;
        }
        else if (CELLSIZE == TO_LOWER(key))
        {
            linestream >> cellsize;
        }
        else if (NODATA_VALUE == TO_LOWER(key))
        {
            linestream >> info.fill;
        }
        else
        {
            m_err = "无效的Key: " + key;
            std::cerr << m_err << std::endl;
            return status;
        }

        if (NODATA_VALUE == TO_LOWER(key))
            break;  // header section ends after NODATA_value
    }
    info.dx = cellsize;
    info.dy = cellsize;
    info.bound.max.x = info.bound.min.x + info.width * cellsize;
    info.bound.max.y = info.bound.min.y + info.height * cellsize;
    if (onlyhead)
    {
        input.close();
        return true;
    }
    raster.create(info.height, info.width, true);

    double value;
    int r = 0;
    while (std::getline(input, line) && r < info.height)
    {
        std::istringstream linestream(line);
        int c = 0;
        while (c < info.width)
        {
            linestream >> value;
            if (value == 0)
            {
                int ccc = 1;
            }
            raster[r][c] = value;
            c++;
        }
        r++;
    }
    input.close();
    status = true;
    return status;
}

bool suAsciiGrid::write(const suPath& file) const
{
    bool status = false;
    std::string ext = file.extension();
    if (ASC == TO_LOWER(ext))
    {
        return write_asc(file);
    }
    if (BIN == TO_LOWER(ext))
    {
        return write_bin(file);
    }
    std::cerr << std::string("不支持的格式: ") + ext << std::endl;
    return status;
}
std::string suAsciiGrid::stringify_ll(const int& precision) const
{
    std::string ret;
    {
        std::stringstream ssH;
        ssH << std::fixed << std::setprecision(15);
        ssH << NCOLS << " " << info.width << "\n";
        ssH << NROWS << " " << info.height << "\n";
        ssH << XLLCORNER << " " << info.bound.min.x << "\n";
        ssH << YLLCORNER << " " << info.bound.min.y << "\n";
        ssH << CELLSIZE << " " << info.dx << "\n";
        ssH << std::fixed << std::setprecision(precision);
        ssH << NODATA_VALUE << " " << info.fill << "\n";
        ret.append(ssH.str());
    }
    {
        std::stringstream ssG;
        ssG << std::fixed << std::setprecision(precision);
        for (int r = 0; r < info.height; ++r)
        {
            for (int c = 0; c < info.width; ++c)
            {
                ssG << raster[r * info.width + c] << " ";
            }
            ssG << "\n";
        }
        ret.append(ssG.str());
    }
    return ret;
}

bool suAsciiGrid::read_bin(const suPath& file)
{
    std::string content;
    if (!sufile::read(file, content))
    {
        m_err = "读取文件失败: " + file.u8string();
        std::cerr << m_err << std::endl;
        return false;
    }
    double* p = reinterpret_cast<double*>(content.data());
    info.height = static_cast<size_t>(*p);
    p++;
    info.width = static_cast<size_t>(*p);
    p++;
    if ((info.height * info.width + 2) * sizeof(double) != content.size())
    {
        std::cerr << "bin文件数据和宽高不匹配" << std::endl;
        return false;
    }
    raster.create(info.height, info.width, true);
    std::memcpy(raster.data(), p, info.height * info.width * sizeof(double));

    return true;
}

bool suAsciiGrid::write_asc(const suPath& file) const
{
#ifndef NDEBUG
    suTimer timer;
#endif
    std::ofstream ofs(file);
    if (!ofs.is_open())
    {
        return false;
    }
    std::string _root = file.parent().string();
    std::string _name = file.stem();
    {
        std::stringstream ssHead;
        ssHead << std::fixed << std::setprecision(15);
        ssHead << NCOLS << " " << info.width << "\n";
        ssHead << NROWS << " " << info.height << "\n";
        ssHead << XLLCORNER << " " << info.bound.min.x << "\n";
        ssHead << YLLCORNER << " " << info.bound.min.y << "\n";
        ssHead << CELLSIZE << " " << info.dx << "\n";
        ssHead << std::fixed << std::setprecision(3);
        ssHead << NODATA_VALUE << " " << info.fill << "\n";
        ofs.write(ssHead.str().c_str(), ssHead.str().size());
    }
    std::stringstream ssGrid;
    std::string fillV = SUFMT("{:.3f}", info.fill);
    ssGrid << std::fixed << std::setprecision(3);
    double* p = raster.data();
    for (int r = 0; r < info.height; ++r)
    {
        for (int c = 0; c < info.width; ++c)
        {
            if (*p == info.fill)
            {
                ssGrid << fillV << " ";
            }
            else
            {
                ssGrid << *p << " ";
            }
            p++;
        }
        ssGrid << "\n";
    }
    ofs.write(ssGrid.str().c_str(), ssGrid.str().size());
    ofs.close();
    if (IsGauss())
    {
        const auto prjFile = suPath(_root).append(_name + PRJ);
        if (!write_prj(prjFile))
        {
            std::cerr << "写回投影信息失败: " << prjFile.u8string() << std::endl;
            return false;
        }
    }
#ifndef NDEBUG
    SLOG_DEBUG("写入{}计时 : {:.3f}S", file.u8string(), timer.elapsed_ms() / 1000.0)
#endif

    return true;
}

bool suAsciiGrid::write_bin(const suPath& file) const
{
    std::ofstream ofs(file);
    if (!ofs.is_open())
    {
        return false;
    }
    std::string header;
    header.resize(2 * sizeof(double));
    std::memcpy(header.data(), &info.height, sizeof(double));
    std::memcpy(header.data() + sizeof(double), &info.width, sizeof(double));
    ofs.write(header.c_str(), header.size());
    for (int r = 0; r < info.height; ++r)
    {
        std::string content = std::string(info.width * sizeof(double), 0);
        std::memcpy(content.data(), raster.data() + r * info.width, info.width * sizeof(double));
        ofs.write(content.c_str(), content.size());
    }
    ofs.close();
    return true;
}

bool suAsciiGrid::write_prj(const suPath& file) const
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
    std::string filepath = file.parent().append(file.stem() + PRJ).string();
    std::stringstream ss;
    ss << "Projection    TRANSVERSE\n";
    ss << "Units         METERS\n";
    ss << "Zunits        NO\n";
    ss << "Xshift        0.0\n";
    ss << "Yshift        0.0\n";
    ss << "Parameters    6378137.0  6356752.314140356\n";
    ss << "1.0 /* scale factor at central meridian\n";
    ss << static_cast<int>(info.central) << "  0  0.0 /* longitude of central meridian\n";
    ss << "0  0  0.0 /* latitude of origin\n";
    ss << "500000.0 /* false easting (meters)\n";
    ss << "0.0 /* false northing (meters)\n";
    if (sufile::write(filepath, ss.str()) > 0)
    {
        return true;
    }
    return false;
}

bool suAsciiGrid::read_prj(const suPath& file)
{
    // 提取高斯分带中心线经度
    std::vector<std::string> lines;
    if (sufile::readlines(file, lines))
    {
        if (lines.size() >= 11)
        {
            std::stringstream ss(lines[7]);
            ss >> info.central;
            return true;
        }
    }

    return false;
}
