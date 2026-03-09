/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/18 10:34
 * @version: 1.0.1
 * @description: png 图片渲染
 */
#ifndef SILLY_GRID_RENDER_H
#define SILLY_GRID_RENDER_H
#include <log/silly_log.h>
#include <graphics/silly_png.h>
#include <geo/proj/silly_proj.h>
#include <geo/silly_geo.h>
#include <math/silly_matrix.h>

template <typename T>
class silly_val2color
{
  public:
    silly_val2color() = default;
    silly_val2color(T v, suColor c)
    {
        val = v;
        color = c;
    }

  public:
    T val;
    suColor color;
};

template <typename T>
class silly_render_param
{
  public:
    suMatrix<T> mtx;
    std::vector<silly_val2color<T>> v2cs;  // 需要排好序
    suPNG pd;
    suRect rect;

    // 将 v2cs 按照 val 的值进行排序 默认升序排序
    void sort(bool desc = false)
    {
        if (desc)
        {
            // 降序
            std::sort(v2cs.rbegin(), v2cs.rend(), [](const silly_val2color<T>& a, const silly_val2color<T>& b) { return a.val < b.val; });
        }
        else
        {
            // 升序
            std::sort(v2cs.begin(), v2cs.end(), [](const silly_val2color<T>& a, const silly_val2color<T>& b) { return a.val < b.val; });
        }
    }
};

template <typename T>
class silly_grid_render
{
  public:
    friend class suMatrix<T>;
    friend class suPNG;

    void normal_render_greater(silly_render_param<T>& srp)
    {
        int color_num = srp.v2cs.size();
        T* ptr = srp.mtx.data();
        if (!ptr)
        {
            SLOG_ERROR("获取矩阵数据块失败")
            return;
        }
        srp.pd.create(srp.mtx.cols(), srp.mtx.rows(), eColorType::RGBA);
        for (size_t r = 0; r < srp.mtx.rows(); ++r)
        {
            for (size_t c = 0; c < srp.mtx.cols(); ++c)
            {
                T v = ptr[0];
                ptr++;
                if (v < srp.v2cs[0].val)
                {
                    continue;
                }
                int i = 1;
                for (; i < color_num; ++i)
                {
                    if (v < srp.v2cs[i].val)
                    {
                        break;
                    }
                }
                i--;
                srp.pd.pixel(r, c, srp.v2cs[i].color);
            }
        }
    }

    void geo_mc_render_greater(silly_render_param<T>& srp)
    {
        suMatrix<T> mc_mtx;
        if (!matrix_geo_to_mercator(srp.mtx, srp.rect, mc_mtx))
        {
            SLOG_ERROR("经纬坐标转换墨卡托坐标失败")
            return;
        }
        // srp.mtx.release();
        silly_render_param<T> srp2;
        srp2.v2cs = srp.v2cs;
        srp2.rect = srp.rect;
        srp2.mtx = mc_mtx;
        normal_render_greater(srp2);
        mc_mtx.release();
        srp.pd = srp2.pd;
    }

    void normal_render(silly_render_param<T>& srp, std::function<suColor(T, std::vector<silly_val2color<T>>)> func)
    {
        int color_num = srp.v2cs.size();
        T* ptr = srp.mtx.data();
        if (!ptr)
        {
            SLOG_ERROR("获取矩阵数据块失败")
            return;
        }
        srp.pd.create(srp.mtx.cols(), srp.mtx.rows(), eColorType::RGBA);
        for (size_t r = 0; r < srp.mtx.rows(); ++r)
        {
            for (size_t c = 0; c < srp.mtx.cols(); ++c)
            {
                T v = ptr[0];
                ptr++;
                suColor tmp_color = func(v, srp.v2cs);
                srp.pd.pixel(r, c, tmp_color);
            }
        }
    }

    void geo_mc_render(silly_render_param<T>& srp, std::function<suColor(T, std::vector<silly_val2color<T>>)> func)
    {
        suMatrix<T> mc_mtx;
        if (!matrix_geo_to_mercator(srp.mtx, srp.rect, mc_mtx))
        {
            SLOG_ERROR("经纬坐标转换墨卡托坐标失败")
            return;
        }
        silly_render_param<T> srp2;
        srp2.v2cs = srp.v2cs;
        srp2.rect = srp.rect;
        srp2.mtx = mc_mtx;
        normal_render(srp2, func);
        mc_mtx.release();
        srp.pd = srp2.pd;
    }
protected:
bool matrix_geo_to_mercator(suMatrix<T> src, const suRect& rect, suMatrix<T>& dst)
    {
        // 防止传参数进来是src与dst是同一个对象
        suMatrix<T> tmp = src.copy();
        if (!(tmp.rows() && tmp.cols() && tmp.data()))
        {
            return false;
        }

        if (!dst.create(tmp.rows(), tmp.cols(), true))
        {
            return false;
        }
        double m_left{0}, m_top{0}, m_right{0}, m_bottom{0};
        suGeoProj::Tlonlat_to_mercator(rect.max.x, rect.min.y, m_right, m_bottom);
        suGeoProj::Tlonlat_to_mercator(rect.min.x, rect.max.y, m_left, m_top);
        double mc_xdelta = (m_right - m_left) / tmp.cols();
        double mc_ydelta = (m_top - m_bottom) / tmp.rows();

        double geo_xdelta = (rect.max.x - rect.min.x) / tmp.cols();
        double geo_ydelta = (rect.max.y - rect.min.y) / tmp.rows();

        /*   T g_width = rect.max.x - rect.min.x;
           T g_height = rect.max.y - rect.min.y;*/
        int max_r = tmp.rows() - 1;
        int max_c = tmp.cols() - 1;
        // 为dst即墨卡托上每个位置找到geo上对应的位置, 然后取值, 防止图片有撕裂的情况
        for (int r = 0; r < tmp.rows(); ++r)
        {
            for (int c = 0; c < tmp.cols(); ++c)
            {
                double m_x = c * mc_xdelta + m_left;  // 每个matrix网格点对应的mecator坐标
                double m_y = m_top - r * mc_ydelta;
                double lgtd, lttd;
                suGeoProj::Tmercator_to_lonlat(m_x, m_y, lgtd, lttd);
                int dst_c = std::round((lgtd - rect.min.x) / geo_xdelta);
                int dst_r = std::round((rect.max.y - lttd) / geo_ydelta);
                // TODO: 这一步是不是有问题,是否是必须的,防止访问溢出
                dst_c = std::min(std::max(0, dst_c), max_c);
                dst_r = std::min(std::max(0, dst_r), max_r);
                dst[r][c] = tmp[dst_r][dst_c];
            }
        }
        tmp.release();
        return true;
    }
};

#endif  // SILLY_GRID_RENDER_H
