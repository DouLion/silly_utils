/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/24 15:29
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_ISO_LINE_H
#define SILLY_UTILS_SILLY_ISO_LINE_H

#include <gpc/gpc.h>
#include <geo/silly_geo.h>

struct iso_point
{
    iso_point() = default;
    iso_point(double xx, double yy)
    {
        x = xx;
        y = yy;
    }
    double x{0};
    double y{0};
    iso_point operator=(const iso_point& ip)
    {
        this->x = ip.x;
        this->y = ip.y;
        return *this;
    }

    bool too_near(const iso_point& ip, double distance = 1e-4)
    {
        return std::abs(x - ip.x) < distance && std::abs(y - ip.y) < distance;
    }
};
struct iso_ring  // 闭合环
{
    size_t outer{0};                // 0 内环  1 外环
    size_t marked{0};               // 如果是内环的话,有没有找到他的外环 0表示没有
    std::vector<iso_point> cpoint;  // 控制点
    std::vector<iso_point> ppoint;  // 填充点

    iso_ring operator=(const iso_ring& ir)
    {
        this->outer = ir.outer;
        this->marked = ir.marked;
        this->cpoint = ir.cpoint;
        this->ppoint = ir.ppoint;
        return *this;
    }

    void add_c(iso_point point, double& dist)
    {
        if (cpoint.back().too_near(point, dist))
        {
            return;
        }
        cpoint.push_back(point);
    }
};

struct iso_polygon  // 普通面
{
    iso_ring outer;
    std::vector<iso_ring> inners;
};

struct iso_polygon_m  // 多面
{
    std::vector<iso_polygon> polys;
};

struct iso_panels  //
{
    size_t num{0};
    size_t color[64]{0};
    iso_polygon_m mpolys[64];
};

class silly_iso_line
{
    enum class enum_iso_type
    {
        eitLine = 0,  //等值线
        eitArea = 1   // 等值面
    };

    enum class enum_clip_type
    {
        ectRect = 1,
        ectBoundy = 0
    };

    struct iso_triangle
    {
        size_t i;

        size_t traced{0};
        size_t nBian[3] = {0};  // 是否从当前边穿过
        iso_point dv[3];        // 从当前边穿过的话,交点坐标
    };

  public:
    silly_iso_line(const int& type);

    // Attributes
  public:
    double m_near{1e-4};

    // 三角形追踪的中间结果
  private:
    std::map<size_t, std::vector<iso_triangle>> m_level_tris;
    std::map<size_t, std::vector<iso_polygon>> m_level_polys;
    // 等级阈值
    std::vector<double> m_thresholds;
    // 0 等值线  1 等值面
    enum_iso_type m_Type;

  private:
    double XXMax = -999., YYMax = -999., XXMin = 999., YYMin = 999.;  //数值点的x，y最大值

    // 视口范围
    double dMinLGTD{1e6};   //最小经度
    double dMaxLGTD{-1e6};  //最大经度
    double dMinLTTD{1e6};   //最小纬度
    double dMaxLTTD{-1e6};  //最大纬度

    enum_clip_type nClipType{0};
    //三角形信息
    size_t m_tri_num;
    double* m_tax{nullptr};
    double* m_tay{nullptr};
    double* m_tbx{nullptr};
    double* m_tby{nullptr};
    double* m_tcx{nullptr};
    double* m_tcy{nullptr};
    double* m_taz{nullptr};
    double* m_tbz{nullptr};
    double* m_tcz{nullptr};  //三角形数据结构

    //数据点信息
    size_t m_data_num;
    double* m_ddx{nullptr};
    double* m_ddy{nullptr};
    double* m_ddz{nullptr};
    double m_max_dz{0};
    double m_min_dz{0};  //数据点坐标(ddx,ddy)及取值(ddz)
                         // SF1 showF[ARR];

  public:
    /// <summary>
    /// 设置视口范围
    /// </summary>
    /// <param name="rect"></param>
    void set_rect(const silly_geo_rect& rect);

    /// <summary>
    /// 添加站点数据
    /// </summary>
    /// <param name="PointNum"></param>
    /// <param name="pLgtd"></param>
    /// <param name="pLttd"></param>
    /// <param name="pData"></param>
    void set_data(const size_t& PointNum, const double* pLgtd, const double* pLttd, const double* pData);

    /// <summary>
    /// 分级设置阈值
    /// </summary>
    /// <param name="thresholds"></param>
    void set_level(const std::vector<double>& thresholds);

    /// <summary>
    /// 构建三角网络
    /// </summary>
    void make_tri_net();

    /// <summary>
    /// 三角网格在阈值上的焦点
    /// </summary>
    void make_tri_intersect();

    /// <summary>
    /// 三角网格在指定等级阈值上的焦点
    /// </summary>
    void make_tri_intersect(const size_t& level, const bool& greater = true);

    /// <summary>
    /// 构建所有的等值面
    /// </summary>
    void make_iso_area();

    /// <summary>
    /// 指定层级构建等值面
    /// </summary>
    /// <param name="level"></param>
    void make_iso_area(const size_t& level);

    /// <summary>
    /// 重置构建结果,不包括输入数据
    /// </summary>
    void reset();

    /// <summary>
    /// 清空整个对象
    /// </summary>
    void clear();
    std::string geojson();

  public:
    /*   int point_in_poly(const double& x, const double& y, const size_t& num, const double* points);
       int point_in_poly(const double& x, const double& y, const size_t& poly_p_num, const double* poly_x, const double* poly_y);*/
    int point_in_poly(const double& x, const double& y, const std::vector<iso_point>& points);

    void iso_fill(const size_t& curr_level, const size_t& curr_tri_num, std::vector<iso_triangle>& curr_tri);  // level为当前跟踪的级别

    /// <summary>
    /// 获取与三角形相交边的顶点坐标
    /// </summary>
    /// <param name="i">第i个三角形</param>
    /// <param name="bi">第i个相交边</param>
    /// <param name="dx1"></param>
    /// <param name="dx2"></param>
    /// <param name="dy1"></param>
    /// <param name="dy2"></param>
    void tri_edge(size_t i, size_t bi, double& dx1, double& dx2, double& dy1, double& dy2);
    /// <summary>
    /// 以第tri_idx个三角形为起始点,追踪出完整的环
    /// </summary>
    /// <param name="tri_idx"></param>
    /// <param name="curr_tri"></param>
    /// <param name="ring"></param>
    void trace_ring(const size_t& tri_idx, std::vector<iso_triangle>& tri_list, iso_ring& result);
    // 根据控制点, 填充出平滑的等值面
    void draw_iso_fill(std::vector<iso_polygon>& result_polys, size_t ring_num, std::vector<iso_ring>& all_rings);

    /**
     * @desc : 根据边界切割环
     * @author: dou li yang
     * @date : 2023-07-14
     * @param_in : None
     * @param_out : None
     * @return : None
     */
    void clip_iso_ring(iso_ring* ring, gpc* gpc1, gpc_polygon* clip);

    /**
     * @desc : 三次b样条插值的一种实现
     * @author: dou li yang
     * @date : 2023-07-14
     * @param :
     * @param null:
     * @return :
     */
    bool cubic_b_spline(size_t mode, size_t ni, double* poc, size_t* num, double* buff, const bool bMultiPoint = true);

    /// <summary>
    /// 比较一个边中是否存在该边的相邻边,如果有,返回这个边的序号
    /// </summary>
    /// <param name="dx1"></param>
    /// <param name="dy1"></param>
    /// <param name="dx2"></param>
    /// <param name="dy2"></param>
    /// <param name="point"></param>
    /// <param name="nBian"></param>
    /// <returns></returns>
    bool compare_triangle_same_edge(double dx1, double dy1, double dx2, double dy2, iso_point* point, size_t& nBian);
    bool compare_triangle_same_edge(double dx1, double dy1, double dx2, double dy2, size_t& i, size_t& nBian);

  public:
    virtual ~silly_iso_line();
};

#endif  // SILLY_UTILS_SILLY_ISO_LINE_H
