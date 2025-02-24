/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-02-24
 * @file: silly_depict_river_channel.cpp
 * @description: silly_depict_river_channel实现
 * @version: v1.0.1 2025-02-24 dou li yang
 */
#include "silly_depict_river_channel.h"

using namespace silly::tzx::river_channel;

// 预处理横断面数据，确保形成U形槽
void ValidateCrossSection(std::vector<depict::cross_section>& csections)
{
    // 计算所有横断面的最大和最小起点距
    double dmin = std::numeric_limits<double>::max();
    double dmax = std::numeric_limits<double>::lowest();

    for (const auto& section : csections)
    {
        double sectionMin = *std::min_element(section.dists.begin(), section.dists.end());
        double sectionMax = *std::max_element(section.dists.begin(), section.dists.end());
        dmin = std::min(dmin, sectionMin);
        dmax = std::max(dmax, sectionMax);
    }

    // 计算目标长度
    double len = dmax - dmin;

    // 处理每个横断面
    for (auto& section : csections)
    {
        std::vector<double> newDistances = section.dists;
        std::vector<double> newElevations = section.elevs;

        // 计算当前横断面的长度
        double currentLength = section.dists.back() - section.dists.front();

        // 如果当前长度小于目标长度，则添加点
        if (currentLength < len)
        {
            // 计算需要添加的点
            double leftDistance = dmin;   // 添加到最小起点距
            double rightDistance = dmax;  // 添加到最大起点距

            // 添加左侧点
            if (std::find(newDistances.begin(), newDistances.end(), leftDistance) == newDistances.end())
            {
                double leftElevation = section.elevs.front();  // 使用第一个高程值
                newDistances.insert(newDistances.begin(), leftDistance);
                newElevations.insert(newElevations.begin(), leftElevation);
            }

            // 添加右侧点
            if (std::find(newDistances.begin(), newDistances.end(), rightDistance) == newDistances.end())
            {
                double rightElevation = section.elevs.back();  // 使用最后一个高程值
                newDistances.push_back(rightDistance);
                newElevations.push_back(rightElevation);
            }
        }

        // 更新横断面数据
        section.dists = newDistances;
        section.elevs = newElevations;
    }
}

// 线性插值函数
double linearInterpolation(double x, double x0, double y0, double x1, double y1)
{
    return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}
// 线性插值函数
double linearInterpolationh(double t, double y0, double y1)
{
    return y0 + t * (y1 - y0);
}

// 计算 DEM 范围
void calculateDEMRange(const std::vector<std::vector<depict::point_z>>& centerlines, const std::vector<depict::cross_section>& csections, double& xMin, double& xMax, double& yMin, double& yMax)
{
    xMin = std::numeric_limits<double>::max();
    xMax = std::numeric_limits<double>::lowest();
    yMin = std::numeric_limits<double>::max();
    yMax = std::numeric_limits<double>::lowest();

    for (const auto& line : centerlines)
    {
        for (const auto& point : line)
        {
            xMin = std::min(xMin, point.x);
            xMax = std::max(xMax, point.x);
            yMin = std::min(yMin, point.y);
            yMax = std::max(yMax, point.y);
        }
    }

    double dmin = std::numeric_limits<double>::max();
    double dmax = std::numeric_limits<double>::lowest();

    for (const auto& section : csections)
    {
        for (const auto& distance : section.dists)
        {
            dmin = std::min(dmin, distance);
            dmax = std::max(dmax, distance);
        }
    }
    xMin += dmin;
    yMin += dmin;
    xMax += dmax;
    yMax += dmax;
}

// 计算两点之间的距离
double distanceBetweenPoints(const depict::point_z& p1, const depict::point_z& p2)
{
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    return std::sqrt(dx * dx + dy * dy);
}

// 计算路径的总距离
double totalDistance(const std::vector<depict::point_z>& points)
{
    double total = 0.0;
    for (size_t i = 0; i < points.size() - 1; ++i)
    {
        total += distanceBetweenPoints(points[i], points[i + 1]);
    }
    return total;
}
// 沿路径修改高程
void modifyElevationAlongPath(const std::vector<depict::point_z>& gridPoints, std::vector<std::vector<double>>& dem, double lineWidth, double bevel, double eelev)
{
    // 计算总路径距离
    double totalPathDistance = totalDistance(gridPoints);
    double accumulatedDistance = 0.0;

    // 遍历路径上的每个段
    for (size_t i = 0; i < gridPoints.size() - 1; ++i)
    {
        const depict::point_z& p1 = gridPoints[i];
        const depict::point_z& p2 = gridPoints[i + 1];

        // 计算当前段的距离
        double segmentDistance = distanceBetweenPoints(p1, p2);

        for (int j = 0; j <= static_cast<int>(segmentDistance); ++j)
        {
            double t = static_cast<double>(j) / segmentDistance;
            int xi = static_cast<int>(std::round(p1.x + t * (p2.x - p1.x)));
            int yi = static_cast<int>(std::round(p1.y + t * (p2.y - p1.y)));

            // 计算累计距离
            double currentDistance = accumulatedDistance + distanceBetweenPoints(p1, depict::point_z{static_cast<double>(xi), static_cast<double>(yi)});

            // 用总的累计距离计算同比变化的插值高程
            double alongPathRatio = currentDistance / totalPathDistance;
            double elevation = bevel + alongPathRatio * (eelev - bevel);

            // 更新路径上宽度范围内的高程值
            int halfLineWidth = static_cast<int>(std::floor(lineWidth / 2));
            for (int dx = -halfLineWidth; dx <= halfLineWidth; ++dx)
            {
                for (int dy = -halfLineWidth; dy <= halfLineWidth; ++dy)
                {
                    int nx = xi + dx;
                    int ny = yi + dy;

                    // 确保在 DEM 数据的有效范围内
                    if (nx >= 0 && nx < dem[0].size() && ny >= 0 && ny < dem.size() && dem[ny][nx] != std::numeric_limits<double>::infinity())
                    {
                        if (dem[ny][nx] > -9999)
                            dem[ny][nx] = std::min(dem[ny][nx], elevation);
                        else
                            dem[ny][nx] = elevation;
                    }
                }
            }
        }

        // 更新累计距离
        accumulatedDistance += segmentDistance;
    }
}

// 生成平行于河流中心线的折线
std::vector<std::vector<depict::point_z>> generateParallelLines(const std::vector<std::vector<depict::point_z>>& centerlines, double offset)
{
    std::vector<std::vector<depict::point_z>> parallelLines;

    for (const auto& line : centerlines)
    {
        std::vector<depict::point_z> parallelLine;

        for (size_t i = 0; i < line.size() - 1; ++i)
        {
            const depict::point_z& p1 = line[i];
            const depict::point_z& p2 = line[i + 1];

            // 计算方向向量
            double dx = p2.x - p1.x;
            double dy = p2.y - p1.y;
            double length = std::sqrt(dx * dx + dy * dy);

            // 单位方向向量
            dx /= length;
            dy /= length;

            // 计算垂直方向向量
            double perpX = -dy;  // 逆时针旋转90度
            double perpY = dx;

            // 创建平行点
            parallelLine.push_back({p1.x + perpX * offset, p1.y + perpY * offset});
            parallelLine.push_back({p2.x + perpX * offset, p2.y + perpY * offset});
        }

        parallelLines.push_back(parallelLine);
    }

    return parallelLines;
}

// 在单个横断面上根据距离插值高程
double interpolateElevationAtDistance(const depict::cross_section& crossSection, double targetDistance)
{
    // 检查输入有效性
    if (crossSection.dists.empty() || crossSection.elevs.empty())
    {
        throw std::invalid_argument("depict::cross_section dists and elevs must not be empty.");
    }

    // 如果目标距离在范围内，直接返回对应的高程
    if (targetDistance <= crossSection.dists.front())
    {
        return crossSection.elevs.front();
    }
    if (targetDistance >= crossSection.dists.back())
    {
        return crossSection.elevs.back();
    }

    // 查找目标距离所在的区间
    for (size_t i = 0; i < crossSection.dists.size() - 1; ++i)
    {
        if (targetDistance >= crossSection.dists[i] && targetDistance <= crossSection.dists[i + 1])
        {
            return linearInterpolation(targetDistance, crossSection.dists[i], crossSection.elevs[i], crossSection.dists[i + 1], crossSection.elevs[i + 1]);
        }
    }

    // 如果没有找到合适的区间，抛出异常
    throw std::out_of_range("Target distance is out of range of the provided cross section.");
}

bool depict::process(const cross_section& cs0, const cross_section& cs1, std::vector<depict::point_z>& vsections, options& opt)
{
    //std::vector<cross_section> csections;
    //csections.push_back(cs0);
    //csections.push_back(cs1);
    //ValidateCrossSection(csections);

    // double xMin, xMax, yMin, yMax;
    //// 生成DEM的范围，遍历centerlines的所有点取最大和最小
    // calculateDEMRange({vsections}, csections, xMin, xMax, yMin, yMax);
    //xllcorner = xMin;
    //yllcorner = yMin;
    //// 计算行列数
    //int numCols = static_cast<int>(std::ceil((xMax - xMin) / opt.cell_size));
    //int numRows = static_cast<int>(std::ceil((yMax - yMin) / opt.cell_size));

    //// 初始化 channelData  DEM
    //std::vector<std::vector<double>> channelData(numRows, std::vector<double>(numCols, -9999));

    //for (size_t i = 0; i < vsections.size(); ++i)
    //{
    //    const auto& line = vsections[i];

    //    // 获取起点和终点的高程
    //    double startElevation = csections[i].elevs.front();        // 起点高程
    //    double endElevation = csections[i + 1].elevs.front();  // 终点高程

    //    // 将路径点转换为 DEM 格点坐标
    //    std::vector<point_z> gridPoints;
    //    for (const auto& point : line)
    //    {
    //        int gridX = static_cast<int>(std::round((point.x - xMin) / opt.cell_size));
    //        int gridY = static_cast<int>(std::round((yMax - point.y) / opt.cell_size));                // 注意这里是 yMax - point.y
    //        gridPoints.push_back(point_z{static_cast<double>(gridX), static_cast<double>(gridY)});  //
    //    }

    //    // 修改沿路径的高程
    //    modifyElevationAlongPath(gridPoints, channelData, opt.cell_size, startElevation, endElevation);
    //    gridPoints.clear();

    //    // 计算当前横断面的左侧长度
    //    double sectionLength = 0 - csections[i].dists.front();
    //    // 从起点开始，每隔 opt.cell_size 生成平行折线
    //    for (double d = -opt.cell_size; d >= -sectionLength; d -= opt.cell_size)
    //    {
    //        startElevation = interpolateElevationAtDistance(csections[i], d);
    //        endElevation = interpolateElevationAtDistance(csections[i + 1], d);
    //        std::vector<point_z> parallelLine = generateParallelLines({line}, d)[0];  // 偏移量设为 opt.cell_size

    //        for (const auto& point : parallelLine)
    //        {
    //            int gridX = static_cast<int>(std::round((point.x - xMin) / opt.cell_size));
    //            int gridY = static_cast<int>(std::round((yMax - point.y) / opt.cell_size));                // 注意这里是 yMax - point.y
    //            gridPoints.push_back(point_z{static_cast<double>(gridX), static_cast<double>(gridY)});  //
    //        }

    //        modifyElevationAlongPath(gridPoints, channelData, opt.cell_size, startElevation, endElevation);
    //        gridPoints.clear();
    //    }
    //    // 计算当前横断面的右侧长度
    //    sectionLength = csections[i].dists.back();
    //    // 从起点开始，每隔 opt.cell_size 生成平行折线
    //    for (double d = opt.cell_size; d <= sectionLength; d += opt.cell_size)
    //    {
    //        startElevation = interpolateElevationAtDistance(csections[i], d);
    //        endElevation = interpolateElevationAtDistance(csections[i + 1], d);
    //        std::vector<point_z> parallelLine = generateParallelLines({line}, d)[0];  // 偏移量设为 opt.cell_size

    //        for (const auto& point : parallelLine)
    //        {
    //            int gridX = static_cast<int>(std::round((point.x - xMin) / opt.cell_size));
    //            int gridY = static_cast<int>(std::round((yMax - point.y) / opt.cell_size));                // 注意这里是 yMax - point.y
    //            gridPoints.push_back(point_z{static_cast<double>(gridX), static_cast<double>(gridY)});  //
    //        }

    //        modifyElevationAlongPath(gridPoints, channelData, opt.cell_size, startElevation, endElevation);
    //        gridPoints.clear();
    //    }
    //}

    //return channelData;
    return false;
}

std::vector<std::vector<double>> depict::process(const std::vector<cross_section> css, std::vector<std::vector<depict::point_z>>& vsections, options& opt)
{
    std::vector<cross_section> csections = css;
    ValidateCrossSection(csections);

    double xMin, xMax, yMin, yMax;
    // 生成DEM的范围，遍历centerlines的所有点取最大和最小
    calculateDEMRange(vsections, csections, xMin, xMax, yMin, yMax);
    opt.rect.min.x = xMin;
    opt.rect.min.y= yMin;
    // 计算行列数
    int numCols = static_cast<int>(std::ceil((xMax - xMin) / opt.cell_size));
    int numRows = static_cast<int>(std::ceil((yMax - yMin) / opt.cell_size));

    // 初始化 channelData  DEM
    std::vector<std::vector<double>> channelData(numRows, std::vector<double>(numCols, -9999));

    for (size_t i = 0; i < vsections.size(); ++i)
    {
        const auto& line = vsections[i];

        // 获取起点和终点的高程
        double startElevation = csections[i].elevs.front();    // 起点高程
        double endElevation = csections[i + 1].elevs.front();  // 终点高程

        // 将路径点转换为 DEM 格点坐标
        std::vector<point_z> gridPoints;
        for (const auto& point : line)
        {
            int gridX = static_cast<int>(std::round((point.x - xMin) / opt.cell_size));
            int gridY = static_cast<int>(std::round((yMax - point.y) / opt.cell_size));             // 注意这里是 yMax - point.y
            gridPoints.push_back(point_z{static_cast<double>(gridX), static_cast<double>(gridY)});  //
        }

        // 修改沿路径的高程
        modifyElevationAlongPath(gridPoints, channelData, opt.cell_size, startElevation, endElevation);
        gridPoints.clear();

        // 计算当前横断面的左侧长度
        double sectionLength = 0 - csections[i].dists.front();
        // 从起点开始，每隔 opt.cell_size 生成平行折线
        for (double d = -opt.cell_size; d >= -sectionLength; d -= opt.cell_size)
        {
            startElevation = interpolateElevationAtDistance(csections[i], d);
            endElevation = interpolateElevationAtDistance(csections[i + 1], d);
            std::vector<point_z> parallelLine = generateParallelLines({line}, d)[0];  // 偏移量设为 opt.cell_size

            for (const auto& point : parallelLine)
            {
                int gridX = static_cast<int>(std::round((point.x - xMin) / opt.cell_size));
                int gridY = static_cast<int>(std::round((yMax - point.y) / opt.cell_size));             // 注意这里是 yMax - point.y
                gridPoints.push_back(point_z{static_cast<double>(gridX), static_cast<double>(gridY)});  //
            }

            modifyElevationAlongPath(gridPoints, channelData, opt.cell_size, startElevation, endElevation);
            gridPoints.clear();
        }
        // 计算当前横断面的右侧长度
        sectionLength = csections[i].dists.back();
        // 从起点开始，每隔 opt.cell_size 生成平行折线
        for (double d = opt.cell_size; d <= sectionLength; d += opt.cell_size)
        {
            startElevation = interpolateElevationAtDistance(csections[i], d);
            endElevation = interpolateElevationAtDistance(csections[i + 1], d);
            std::vector<point_z> parallelLine = generateParallelLines({line}, d)[0];  // 偏移量设为 opt.cell_size

            for (const auto& point : parallelLine)
            {
                int gridX = static_cast<int>(std::round((point.x - xMin) / opt.cell_size));
                int gridY = static_cast<int>(std::round((yMax - point.y) / opt.cell_size));             // 注意这里是 yMax - point.y
                gridPoints.push_back(point_z{static_cast<double>(gridX), static_cast<double>(gridY)});  //
            }

            modifyElevationAlongPath(gridPoints, channelData, opt.cell_size, startElevation, endElevation);
            gridPoints.clear();
        }
    }

    return channelData;
}

bool depict::write(const std::string& filename, const std::vector<std::vector<double>>& channelData, const options& opt)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        return false;
    }
    file << std::setprecision(3) << std::fixed;
    // 写入DEM文件头
    file << "ncols " << channelData[0].size() << "\n";
    file << "nrows " << channelData.size() << "\n";
    file << "xllcorner " << opt.rect.min.x << "\n";
    file << "yllcorner " << opt.rect.min.y << "\n";
    file << "cellsize " << opt.cell_size << "\n";
    file << "NODATA_value -9999\n";
    // 写入数据
    for (const auto& row : channelData)
    {
        for (double value : row)
        {
            if (value == -9999.0)
            {
                file << "-9999 ";
            }
            else
            {
                file << value << " ";
            }
        }
        file << "\n";
    }
    file.close();
    return true;
}
