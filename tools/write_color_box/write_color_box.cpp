

#include <iostream>
#include <cairo/cairo.h>
#include <vector>
#include <string>
#include "encode/convert.hpp"
#include "geo/silly_geo.h"
#include "geo/silly_geo_convert.h"
#include "files/TFF_FileUtils.h"
#include "geo/silly_projection.h"

#define COLOR_ALPHA 0.75

struct st_png_data
{
    char* data;
    unsigned int offset;
    unsigned int length;
};

static cairo_status_t cairo_read_func_mine(void* closure, unsigned char* data, unsigned int length)
{
    st_png_data* pPngData = (st_png_data*)closure;
    if (pPngData->length - pPngData->offset < length)
    {
        length = pPngData->length - pPngData->offset;
    }

    memcpy(data, pPngData->data + pPngData->offset, length);
    pPngData->offset += length;

    return CAIRO_STATUS_SUCCESS;
}

class DataValue
{
public:
    std::string text;
    int r;
    int g;
    int b;

    DataValue() = default;

    DataValue(const std::string& in_text, int in_r, int in_g, int in_b)
    {

        text = in_text;
        r = in_r;
        g = in_g;
        b = in_b;
    }
};

//std::vector<DataValue> data_values =
//{
//    DataValue("1-10mm", 140, 247, 148),  // 使用有参构造函数创建一个具有文本和 RGB 值的 DataValue 对象
//    DataValue("10-25mm", 0, 174, 33),
//    DataValue("25-50mm", 33, 190, 255),
//    DataValue("50-100mm", 0, 4, 255),
//    DataValue("100-250mm", 255, 0, 255),
//    DataValue("250-400mm", 132, 0, 66),
//    DataValue(">400mm", 255, 170, 0),
//};
std::vector<DataValue> data_values =
{
    DataValue("可能性很大", 255, 0, 0), 
    DataValue("可能性大", 255, 160, 0),
    DataValue("可能性较大", 255, 255, 0),
    DataValue("可能发生", 0, 0, 255),
};

void draw_polygon(cairo_t* cr, std::vector<silly_poly>& g2p)
{
    double left= 108.5568806, top= 30.7777413, right= 114.4857615, bottom= 24.0583430;
    double m_left, m_top, m_right, m_bottom;
    silly_projection::geo_to_mercator(left, top, m_left, m_top);
    silly_projection::geo_to_mercator(right, bottom, m_right, m_bottom);
    int w =7087, h = 8031;
    for (auto& polygon : g2p)
    {
        if (polygon.props["grade"] == "4")
        {
            std::cout << "蓝" << std::endl;
            cairo_set_source_rgba(cr, 0, 0, 1, COLOR_ALPHA);
        }
        else if (polygon.props["grade"] == "3")
        {
            std::cout << "黄" << std::endl;
            cairo_set_source_rgba(cr, 1, 1, 0, COLOR_ALPHA);
        }
        else if (polygon.props["grade"] == "2")
        {
            std::cout << "橙" << std::endl;
            cairo_set_source_rgba(cr, 1, 0.55, 0, COLOR_ALPHA);
        }
        else if (polygon.props["grade"] == "1")
        {
            std::cout << "红" << std::endl;
            cairo_set_source_rgba(cr, 1, 0, 0, COLOR_ALPHA);
        }

        if (polygon.outer_ring.points.empty())
        {
            continue;
        }
        double m_x, m_y;
        silly_projection::geo_to_mercator(polygon.outer_ring.points[0].lgtd, polygon.outer_ring.points[0].lttd, m_x, m_y);
        cairo_move_to(cr, (m_x - m_left) * w / (m_right - m_left), (m_top - m_y) * h / (m_top - m_bottom));
        for (int j = 1; j < polygon.outer_ring.points.size(); ++j)
        {
            silly_projection::geo_to_mercator(polygon.outer_ring.points[j].lgtd, polygon.outer_ring.points[j].lttd, m_x, m_y);
            cairo_line_to(cr, (m_x - m_left) * w / (m_right - m_left), (m_top - m_y) * h / (m_top - m_bottom));
        }
        cairo_close_path(cr);
        for (int i = 1; i < polygon.inner_rings.size(); ++i)
        {
            cairo_new_sub_path(cr);
            silly_projection::geo_to_mercator(polygon.inner_rings[i].points[0].lgtd, polygon.inner_rings[i].points[0].lttd, m_x, m_y);
            cairo_move_to(cr, (m_x - m_left) * w / (m_right - m_left), (m_top - m_y) * h / (m_top - m_bottom));
            for (int j = 1; j < polygon.inner_rings[i].points.size(); ++j)
            {
                silly_projection::geo_to_mercator(polygon.inner_rings[i].points[j].lgtd, polygon.inner_rings[j].points[0].lttd, m_x, m_y);
                cairo_line_to(cr, (m_x - m_left) * w / (m_right - m_left), (m_top - m_y) * h / (m_top - m_bottom));
            }

            cairo_close_path(cr);

        }
        cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);
        cairo_fill_preserve(cr);
        cairo_stroke(cr);
    }

}

/// <summary>
/// 读取一个png图片,在图片上写文件,并设置颜色框和值,和图片名雷达站实测降雨
/// </summary>
/// <param name="in_png_path">读取png地址</param>
/// <param name="out_png_path">png地址</param>
/// <param name="data_values">传入vector类型的颜色框标题和该框对应的rgb值</param>
bool write_png_color_box(std::string in_png_path, std::string out_png_path, std::vector<DataValue> data_values)
{
    bool res = false;
    cairo_surface_t* surface;
    cairo_t* cr;
    std::string content;
    FileUtils::ReadAll("E:/fff.geojson", content);

    std::vector<silly_poly> polys = silly_geo::load_geojson(content);
    char* data = nullptr;
    int size;
    FileUtils::ReadAll(in_png_path, &data, size);
    st_png_data pngData = { (char*)data, 0, (unsigned int)size };
    surface = cairo_image_surface_create_from_png_stream(cairo_read_func_mine, &pngData);
    //surface = cairo_image_surface_create_from_png(in_png_path.c_str()); // (CAIRO_FORMAT_A8, _param_width, _param_height);
    int w = cairo_image_surface_get_width(surface);
    int h = cairo_image_surface_get_height(surface);
    cr = cairo_create(surface);
    
    draw_polygon(cr, polys);
    cairo_set_source_rgba(cr, 0, 0, 0, 255);
    cairo_select_font_face(cr, "SimSun", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    float scale = 7.0;
    int font_size_1 = 40;
    cairo_set_font_size(cr, 40.0 * scale);
    cairo_move_to(cr, 0.5 * w - font_size_1 * 14 * scale/2, 60 * scale);
    //cairo_show_text(cr, GBK2UTF8("雷达站实测降雨 ").c_str());
    cairo_show_text(cr, silly_conv::GBK2UTF8("湖南省山洪灾害气象风险预警图").c_str());
    // cairo_show_text(cr, " AABBBCCCCCC ");

    cairo_set_font_size(cr, 20.0 * scale);
    cairo_move_to(cr, 0.5 * w - 200 * scale, 90 * scale);
    std::string time_strrr = "2023年08月01日08时 至 2023年08月02日08时"; // TimeToFormatString(btm, "%Y年%m月%d日%H时 至 ") + TimeToFormatString(etm, "%Y年%m月%d日%H时");
    cairo_show_text(cr, silly_conv::GBK2UTF8(time_strrr).c_str());
    cairo_stroke(cr);
  

    int symobl_width = 40 * scale;
    int symbol_height = 15 * scale;
    int symbol_step = 5 * scale;
    int symbol_start_x = 100 * scale;
    int symbol_start_y = 900 * scale;

    cairo_set_source_rgba(cr, 0, 0, 0, 255);
    // cairo_select_font_face(cr, "LiSu", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_select_font_face(cr, "SimSun", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 20.0 * scale);

    for (int i = 0; i < data_values.size(); i++)
    {
        cairo_set_source_rgba(cr, (data_values[i].r) / 255.0, (data_values[i].g) / 255.0, (data_values[i].b) / 255.0, 1.0);
        cairo_rectangle(cr, symbol_start_x, symbol_start_y + symbol_height * i + symbol_step * i, symobl_width, symbol_height);
        cairo_fill(cr);
        cairo_set_line_width(cr, 2* scale);
        cairo_move_to(cr, symbol_start_x + symobl_width + 5 * scale , symbol_start_y + symbol_height * i + symbol_step * i + symbol_height * 1);
        cairo_set_source_rgba(cr, 0, 0, 0, 1);
        cairo_show_text(cr, silly_conv::GBK2UTF8(data_values[i].text.c_str()).c_str()); // 使用相应的文本
       
        cairo_fill(cr);
    }

    cairo_surface_write_to_png(surface, out_png_path.c_str());
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    free(data);
    res = true;
    return res;
}


int main()
{
    std::string in_png_path = "E:/FF2.png";
    std::string out_png_path = "E:/HunanRiskOutput.png";
    bool res = write_png_color_box(in_png_path, out_png_path, data_values);
    return 0;
}