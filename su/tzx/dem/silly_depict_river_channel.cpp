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
// 定义点结构体
struct Point
{
    Point() = default;

    Point(double xx, double yy)
    {
        x = xx;
        y = yy;
    }

    Point(silly_point sp)
    {
        x = sp.x;
        y = sp.y;
    }
    double x, y;
};
double xllcorner;
double yllcorner;
double cellsize;
// 定义横断面结构体
struct CrossSection
{
    std::vector<double> distances;   // 起点距
    std::vector<double> elevations;  // 高程
    silly_point startPoint;                // 起点的高斯坐标
    silly_point leftEndpoint;        // 断面左起点高斯坐标
    silly_point rightEndpoint;             // 断面右端点高斯坐标
};

// 计算点到线段的距离 
double pointToSegmentDistance(const Point& p, const Point& v, const Point& w)
{
    double l2 = std::pow(v.x - w.x, 2) + std::pow(v.y - w.y, 2);  // 线段的平方长度
    if (l2 == 0.0)
        return std::sqrt(std::pow(p.x - v.x, 2) + std::pow(p.y - v.y, 2));  // v == w

    double t = ((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2;
    t = std::max(0.0, std::min(1.0, t));                                // 限制 t 在 [0, 1] 范围内
    Point projection = {v.x + t * (w.x - v.x), v.y + t * (w.y - v.y)};  // 投影点

    return std::sqrt(std::pow(p.x - projection.x, 2) + std::pow(p.y - projection.y, 2));  // 返回距离
}

// 计算断面的左右端点坐标
std::pair<Point, Point> generateCrossSectionEndpoints(const std::vector<Point>& centerline, const CrossSection& crossSection)
{
    // 确保 distances 数组至少有两个元素
    if (crossSection.distances.size() < 2 || centerline.size() < 2)
    {
        throw std::invalid_argument("Distances and centerline must have at least two points.");
    }

    // 取起点
    Point startPoint = crossSection.startPoint;

    // 寻找离起点最近的线段
    double minDistance = std::numeric_limits<double>::max();
    Point closestPoint;

    for (size_t i = 0; i < centerline.size() - 1; ++i)
    {
        Point p1 = centerline[i];
        Point p2 = centerline[i + 1];

        double distance = pointToSegmentDistance(startPoint, p1, p2);

        // 计算投影点
        Point projection;
        double l2 = std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2);
        if (l2 != 0.0)
        {
            double t = ((startPoint.x - p1.x) * (p2.x - p1.x) + (startPoint.y - p1.y) * (p2.y - p1.y)) / l2;
            t = std::max(0.0, std::min(1.0, t));                                // 限制 t 在 [0, 1] 范围内
            projection = {p1.x + t * (p2.x - p1.x), p1.y + t * (p2.y - p1.y)};  // 投影点
        }
        else
        {
            projection = p1;  // 如果 p1 和 p2 重合，投影点就是 p1
        }

        if (distance < minDistance)
        {
            minDistance = distance;
            closestPoint = projection;  // 更新最近的投影点
        }
    }

    // 计算法向量
    double dx = centerline[1].x - centerline[0].x;  // 假设最近的线段是第一个线段
    double dy = centerline[1].y - centerline[0].y;
    double length = std::sqrt(dx * dx + dy * dy);
    double unit_dx = dx / length;
    double unit_dy = dy / length;

    // 计算法向量（顺时针旋转90度）
    double normal_dx = -unit_dy;
    double normal_dy = unit_dx;

    // 计算左端点和右端点坐标
    double leftDistance = fabs(crossSection.distances.front()) * 2;  // 第一个起点距
    double rightDistance = fabs(crossSection.distances.back()) * 2;  // 最后一个起点距

    Point leftEndpoint = {closestPoint.x + normal_dx * leftDistance, closestPoint.y + normal_dy * leftDistance};
    Point rightEndpoint = {closestPoint.x - normal_dx * rightDistance, closestPoint.y - normal_dy * rightDistance};

    return {leftEndpoint, rightEndpoint};
}
// 预处理横断面数据，计算DEM的范围，计算左右端点坐标
void preprocessCrossSection(const std::vector<Point>& centerline, std::vector<CrossSection>& crossSections)
{
    // 计算所有横断面的最大和最小起点距
    double minDistance = std::numeric_limits<double>::max();
    double maxDistance = std::numeric_limits<double>::lowest();

    for (const auto& section : crossSections)
    {
        double sectionMin = *std::min_element(section.distances.begin(), section.distances.end());
        double sectionMax = *std::max_element(section.distances.begin(), section.distances.end());
        minDistance = std::min(minDistance, sectionMin);
        maxDistance = std::max(maxDistance, sectionMax);
    }

    // 计算目标长度
    double targetLength = maxDistance - minDistance;

    // 处理每个横断面
    for (auto& section : crossSections)
    {
        std::vector<double> newDistances = section.distances;
        std::vector<double> newElevations = section.elevations;

        // 计算当前横断面的长度
        double currentLength = section.distances.back() - section.distances.front();

        // 如果当前长度小于目标长度，则添加点
        if (currentLength < targetLength)
        {
            // 计算需要添加的点
            double leftDistance = minDistance;   // 添加到最小起点距
            double rightDistance = maxDistance;  // 添加到最大起点距

            // 添加左侧点
            if (std::find(newDistances.begin(), newDistances.end(), leftDistance) == newDistances.end())
            {
                double leftElevation = section.elevations.front();  // 使用第一个高程值
                newDistances.insert(newDistances.begin(), leftDistance);
                newElevations.insert(newElevations.begin(), leftElevation);
            }

            // 添加右侧点
            if (std::find(newDistances.begin(), newDistances.end(), rightDistance) == newDistances.end())
            {
                double rightElevation = section.elevations.back();  // 使用最后一个高程值
                newDistances.push_back(rightDistance);
                newElevations.push_back(rightElevation);
            }
        }

        // 更新横断面数据
        section.distances = newDistances;
        section.elevations = newElevations;

        // 计算断面左右端点坐标
        auto endpoints = generateCrossSectionEndpoints(centerline, section);
        // 输出左右端点

        section.leftEndpoint.x = endpoints.first.x;
        section.leftEndpoint.y = endpoints.first.y;
        section.rightEndpoint.x = endpoints.second.x;
        section.rightEndpoint.y = endpoints.second.y;
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
void calculateDEMRange(const std::vector<std::vector<Point>>& centerlines, const std::vector<CrossSection>& crossSections, double& xMin, double& xMax, double& yMin, double& yMax)
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

    double minDistance = std::numeric_limits<double>::max();
    double maxDistance = std::numeric_limits<double>::lowest();

    for (const auto& section : crossSections)
    {
        for (const auto& distance : section.distances)
        {
            minDistance = std::min(minDistance, distance);
            maxDistance = std::max(maxDistance, distance);
        }
    }
    xMin += minDistance;
    yMin += minDistance;
    xMax += maxDistance;
    yMax += maxDistance;
}

// 计算两点之间的距离
double distanceBetweenPoints(const Point& p1, const Point& p2)
{
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    return std::sqrt(dx * dx + dy * dy);
}

// 计算路径的总距离
double totalDistance(const std::vector<Point>& points)
{
    double total = 0.0;
    for (size_t i = 0; i < points.size() - 1; ++i)
    {
        total += distanceBetweenPoints(points[i], points[i + 1]);
    }
    return total;
}
// 沿路径修改高程
void modifyElevationAlongPath(const std::vector<Point>& gridPoints, std::vector<std::vector<double>>& demData, double lineWidth, double startElevation, double endElevation)
{
    // 计算总路径距离
    double totalPathDistance = totalDistance(gridPoints);
    double accumulatedDistance = 0.0;

    // 遍历路径上的每个段
    for (size_t i = 0; i < gridPoints.size() - 1; ++i)
    {
        const Point& p1 = gridPoints[i];
        const Point& p2 = gridPoints[i + 1];

        // 计算当前段的距离
        double segmentDistance = distanceBetweenPoints(p1, p2);

        for (int j = 0; j <= static_cast<int>(segmentDistance); ++j)
        {
            double t = static_cast<double>(j) / segmentDistance;
            int xi = static_cast<int>(std::round(p1.x + t * (p2.x - p1.x)));
            int yi = static_cast<int>(std::round(p1.y + t * (p2.y - p1.y)));

            // 计算累计距离
            double currentDistance = accumulatedDistance + distanceBetweenPoints(p1, Point{static_cast<double>(xi), static_cast<double>(yi)});

            // 用总的累计距离计算同比变化的插值高程
            double alongPathRatio = currentDistance / totalPathDistance;
            double elevation = startElevation + alongPathRatio * (endElevation - startElevation);

            // 更新路径上宽度范围内的高程值
            int halfLineWidth = static_cast<int>(std::floor(lineWidth / 2));
            for (int dx = -halfLineWidth; dx <= halfLineWidth; ++dx)
            {
                for (int dy = -halfLineWidth; dy <= halfLineWidth; ++dy)
                {
                    int nx = xi + dx;
                    int ny = yi + dy;

                    // 确保在 DEM 数据的有效范围内
                    if (nx >= 0 && nx < demData[0].size() && ny >= 0 && ny < demData.size() && demData[ny][nx] != std::numeric_limits<double>::infinity())
                    {
                        if (demData[ny][nx] > -9999)
                            demData[ny][nx] = std::min(demData[ny][nx], elevation);
                        else
                            demData[ny][nx] = elevation;
                    }
                }
            }
        }

        // 更新累计距离
        accumulatedDistance += segmentDistance;
    }
}

// 计算两条线段是否相交，并返回交点
std::optional<Point> lineSegmentIntersection(const Point& p1, const Point& p2, const Point& p3, const Point& p4)
{
    const double EPSILON = 1e-6;  // 浮点数比较的容差
    double a1 = p2.y - p1.y;
    double b1 = p1.x - p2.x;
    double c1 = a1 * p1.x + b1 * p1.y;

    double a2 = p4.y - p3.y;
    double b2 = p3.x - p4.x;
    double c2 = a2 * p3.x + b2 * p3.y;

    double determinant = a1 * b2 - a2 * b1;

    if (determinant == 0)
    {
        // 平行或重合
        return std::nullopt;  // 无交点
    }
    else
    {
        double x = (b2 * c1 - b1 * c2) / determinant;
        double y = (a1 * c2 - a2 * c1) / determinant;
        Point intersection = {x, y};

        // 检查交点是否在两个线段上
        if ((std::min(p1.x, p2.x) - EPSILON <= x && x <= std::max(p1.x, p2.x) + EPSILON) && (std::min(p1.y, p2.y) - EPSILON <= y && y <= std::max(p1.y, p2.y) + EPSILON) && (std::min(p3.x, p4.x) - EPSILON <= x && x <= std::max(p3.x, p4.x) + EPSILON) &&
            (std::min(p3.y, p4.y) - EPSILON <= y && y <= std::max(p3.y, p4.y) + EPSILON))
        {
            return intersection;  // 返回交点
        }
        else
        {
            return std::nullopt;  // 交点不在两个线段上
        }
    }
}

// 计算折线与线段的交点
Point* findIntersection(const std::vector<Point>& centerline, const Point& a, const Point& b)
{
    for (size_t i = 0; i < centerline.size() - 1; ++i)
    {
        Point p1 = centerline[i];
        Point p2 = centerline[i + 1];

        auto intersection = lineSegmentIntersection(p1, p2, a, b);
        if (intersection)
        {
            // 返回交点
            Point* result = new Point(*intersection);
            return result;
        }
    }
    return nullptr;  // 没有交点
}

// 生成平行于河流中心线的折线
std::vector<std::vector<Point>> generateParallelLines(const std::vector<std::vector<Point>>& centerlines, double offset)
{
    std::vector<std::vector<Point>> parallelLines;

    for (const auto& line : centerlines)
    {
        std::vector<Point> parallelLine;

        for (size_t i = 0; i < line.size() - 1; ++i)
        {
            const Point& p1 = line[i];
            const Point& p2 = line[i + 1];

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
double interpolateElevationAtDistance(const CrossSection& crossSection, double targetDistance)
{
    // 检查输入有效性
    if (crossSection.distances.empty() || crossSection.elevations.empty())
    {
        throw std::invalid_argument("CrossSection distances and elevations must not be empty.");
    }

    // 如果目标距离在范围内，直接返回对应的高程
    if (targetDistance <= crossSection.distances.front())
    {
        return crossSection.elevations.front();
    }
    if (targetDistance >= crossSection.distances.back())
    {
        return crossSection.elevations.back();
    }

    // 查找目标距离所在的区间
    for (size_t i = 0; i < crossSection.distances.size() - 1; ++i)
    {
        if (targetDistance >= crossSection.distances[i] && targetDistance <= crossSection.distances[i + 1])
        {
            return linearInterpolation(targetDistance, crossSection.distances[i], crossSection.elevations[i], crossSection.distances[i + 1], crossSection.elevations[i + 1]);
        }
    }

    // 如果没有找到合适的区间，抛出异常
    throw std::out_of_range("Target distance is out of range of the provided cross section.");
}

// 计算两点之间的折线
std::vector<Point> generatePolyline(const std::vector<Point>& centerline, const Point& startPoint, const Point& endPoint, int numPoints)
{
    std::vector<Point> polyline;

    // 确保中心线至少有两个点
    if (centerline.size() < 2)
    {
        throw std::invalid_argument("Centerline must have at least two points.");
    }

    // 计算起点和终点之间的方向向量
    double dx = endPoint.x - startPoint.x;
    double dy = endPoint.y - startPoint.y;
    double length = std::sqrt(dx * dx + dy * dy);

    // 计算单位方向向量
    dx /= length;
    dy /= length;

    // 生成折线的中间点
    for (int i = 0; i <= numPoints; ++i)
    {
        double t = static_cast<double>(i) / numPoints;  // 计算插值因子
        Point currentPoint = {startPoint.x + t * (endPoint.x - startPoint.x), startPoint.y + t * (endPoint.y - startPoint.y)};

        // 计算当前点在河流中心线上的最近点
        double closestDistance = std::numeric_limits<double>::max();
        Point closestPoint;

        for (size_t j = 0; j < centerline.size() - 1; ++j)
        {
            const Point& p1 = centerline[j];
            const Point& p2 = centerline[j + 1];

            // 计算当前点到线段 p1p2 的最近点
            double segmentLength = std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
            if (segmentLength == 0)
                continue;  // 避免除以零

            double t = ((currentPoint.x - p1.x) * (p2.x - p1.x) + (currentPoint.y - p1.y) * (p2.y - p1.y)) / (segmentLength * segmentLength);
            t = std::max(0.0, std::min(1.0, t));  // 限制 t 在 [0, 1] 范围内

            Point projection = {p1.x + t * (p2.x - p1.x), p1.y + t * (p2.y - p1.y)};

            double distance = std::sqrt(std::pow(currentPoint.x - projection.x, 2) + std::pow(currentPoint.y - projection.y, 2));
            if (distance < closestDistance)
            {
                closestDistance = distance;
                closestPoint = projection;
            }
        }

        // 将最近点添加到折线中
        polyline.push_back(closestPoint);
    }

    return polyline;
}

// 计算两点之间的折线
std::vector<Point> generatePolyline(const std::vector<Point>& centerline, Point startPoint, Point endPoint)
{
    std::vector<Point> polyline;

    // 确保中心线至少有两个点
    if (centerline.size() < 2)
    {
        throw std::invalid_argument("Centerline must have at least two points.");
    }

    // 找到起点和终点在中心线上的最近线段
    size_t startSegmentIndex = std::numeric_limits<size_t>::max();
    size_t endSegmentIndex = std::numeric_limits<size_t>::max();
    double minStartDistance = std::numeric_limits<double>::max();
    double minEndDistance = std::numeric_limits<double>::max();

    for (size_t i = 0; i < centerline.size() - 1; ++i)
    {
        const Point& v = centerline[i];
        const Point& w = centerline[i + 1];

        // 计算起点到当前线段的距离
        double startDistance = pointToSegmentDistance(startPoint, v, w);
        if (startDistance < minStartDistance)
        {
            minStartDistance = startDistance;
            startSegmentIndex = i;
        }

        // 计算终点到当前线段的距离
        double endDistance = pointToSegmentDistance(endPoint, v, w);
        if (endDistance < minEndDistance)
        {
            minEndDistance = endDistance;
            endSegmentIndex = i + 1;  // 终点是下一个点
        }
    }

    // 检查是否找到起点和终点
    if (startSegmentIndex == std::numeric_limits<size_t>::max() || endSegmentIndex == std::numeric_limits<size_t>::max())
    {
        throw std::invalid_argument("Start point or end point not found in the centerline.");
    }

    // 确保起点索引小于终点索引
    if (startSegmentIndex > endSegmentIndex)
    {
        std::swap(startPoint, endPoint);  // 颠倒输入点
        std::swap(startSegmentIndex, endSegmentIndex);
    }

    // 截取中心线的中间部分
    for (size_t i = startSegmentIndex + 1; i <= endSegmentIndex; ++i)
    {
        polyline.push_back(centerline[i]);
    }

    // 将输入的起点添加到折线的开头
    polyline.insert(polyline.begin(), startPoint);
    // 将输入的终点添加到折线的末尾
    polyline.push_back(endPoint);

    return polyline;
}


bool depict::process(const silly_linez& cs0, const silly_linez& cs1, silly_linez& vsections, options& opt)
{
    //std::vector<silly_linez> csections;
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
    //    std::vector<silly_point_z> gridPoints;
    //    for (const auto& point : line)
    //    {
    //        int gridX = static_cast<int>(std::round((point.x - xMin) / opt.cell_size));
    //        int gridY = static_cast<int>(std::round((yMax - point.y) / opt.cell_size));                // 注意这里是 yMax - point.y
    //        gridPoints.push_back(silly_point_z{static_cast<double>(gridX), static_cast<double>(gridY)});  //
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
    //        std::vector<silly_point_z> parallelLine = generateParallelLines({line}, d)[0];  // 偏移量设为 opt.cell_size

    //        for (const auto& point : parallelLine)
    //        {
    //            int gridX = static_cast<int>(std::round((point.x - xMin) / opt.cell_size));
    //            int gridY = static_cast<int>(std::round((yMax - point.y) / opt.cell_size));                // 注意这里是 yMax - point.y
    //            gridPoints.push_back(silly_point_z{static_cast<double>(gridX), static_cast<double>(gridY)});  //
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
    //        std::vector<silly_point_z> parallelLine = generateParallelLines({line}, d)[0];  // 偏移量设为 opt.cell_size

    //        for (const auto& point : parallelLine)
    //        {
    //            int gridX = static_cast<int>(std::round((point.x - xMin) / opt.cell_size));
    //            int gridY = static_cast<int>(std::round((yMax - point.y) / opt.cell_size));                // 注意这里是 yMax - point.y
    //            gridPoints.push_back(silly_point_z{static_cast<double>(gridX), static_cast<double>(gridY)});  //
    //        }

    //        modifyElevationAlongPath(gridPoints, channelData, opt.cell_size, startElevation, endElevation);
    //        gridPoints.clear();
    //    }
    //}

    //return channelData;
    return false;
}

std::vector<std::vector<double>> depict::process(const std::vector<silly_linez> css, std::vector<silly_linez>& vsections, options& opt)
{
    std::vector<std::vector<double>> channelData;
    //std::vector<silly_linez> csections = css;
    //ValidateCrossSection(csections);

    //double xMin, xMax, yMin, yMax;
    //// 生成DEM的范围，遍历centerlines的所有点取最大和最小
    //calculateDEMRange(vsections, csections, xMin, xMax, yMin, yMax);
    //opt.rect.min.x = xMin;
    //opt.rect.min.y= yMin;
    //// 计算行列数
    //int numCols = static_cast<int>(std::ceil((xMax - xMin) / opt.cell_size));
    //int numRows = static_cast<int>(std::ceil((yMax - yMin) / opt.cell_size));

    //// 初始化 channelData  DEM
    //channelData = std::move(std::vector<std::vector<double>>(numRows, std::vector<double>(numCols, -9999)));

    //for (size_t i = 0; i < vsections.size(); ++i)
    //{
    //    const auto& line = vsections[i];

    //    // 获取起点和终点的高程
    //    double startElevation = csections[i].elevs.front();    // 起点高程
    //    double endElevation = csections[i + 1].elevs.front();  // 终点高程

    //    // 将路径点转换为 DEM 格点坐标
    //    std::vector<silly_point_z> gridPoints;
    //    for (const auto& point : line)
    //    {
    //        int gridX = static_cast<int>(std::round((point.x - xMin) / opt.cell_size));
    //        int gridY = static_cast<int>(std::round((yMax - point.y) / opt.cell_size));             // 注意这里是 yMax - point.y
    //        gridPoints.push_back(silly_point_z(static_cast<double>(gridX), static_cast<double>(gridY)));  //
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
    //        std::vector<silly_point_z> parallelLine = generateParallelLines({line}, d)[0];  // 偏移量设为 opt.cell_size

    //        for (const auto& point : parallelLine)
    //        {
    //            int gridX = static_cast<int>(std::round((point.x - xMin) / opt.cell_size));
    //            int gridY = static_cast<int>(std::round((yMax - point.y) / opt.cell_size));             // 注意这里是 yMax - point.y
    //            gridPoints.push_back(silly_point_z{static_cast<double>(gridX), static_cast<double>(gridY)});  //
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
    //        std::vector<silly_point_z> parallelLine = generateParallelLines({line}, d)[0];  // 偏移量设为 opt.cell_size

    //        for (const auto& point : parallelLine)
    //        {
    //            int gridX = static_cast<int>(std::round((point.x - xMin) / opt.cell_size));
    //            int gridY = static_cast<int>(std::round((yMax - point.y) / opt.cell_size));             // 注意这里是 yMax - point.y
    //            gridPoints.push_back(silly_point_z{static_cast<double>(gridX), static_cast<double>(gridY)});  //
    //        }

    //        modifyElevationAlongPath(gridPoints, channelData, opt.cell_size, startElevation, endElevation);
    //        gridPoints.clear();
    //    }
    //}

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
