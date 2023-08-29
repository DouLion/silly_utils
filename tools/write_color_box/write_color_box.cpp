

#include <iostream>
#include <cairo/cairo.h>
#include <vector>
#include <string>

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

std::vector<DataValue> data_values =
{
    DataValue("1-10mm", 140, 247, 148),  // 使用有参构造函数创建一个具有文本和 RGB 值的 DataValue 对象
    DataValue("10-25mm", 0, 174, 33),
    DataValue("25-50mm", 33, 190, 255),
    DataValue("50-100mm", 0, 4, 255),
    DataValue("100-250mm", 255, 0, 255),
    DataValue("250-400mm", 132, 0, 66),
    DataValue(">400mm", 255, 170, 0),
};


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

    surface = cairo_image_surface_create_from_png(in_png_path.c_str()); // (CAIRO_FORMAT_A8, _param_width, _param_height);
    int w = cairo_image_surface_get_width(surface);
    int h = cairo_image_surface_get_height(surface);
    cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "LiSu", CAIRO_FONT_SLANT_NORMAL,
        CAIRO_FONT_WEIGHT_BOLD);

    float scale = 5.0;
    cairo_set_font_size(cr, 40.0 * scale);
    cairo_move_to(cr, 0.5 * w - 140 * scale, 80 * scale);
    //cairo_show_text(cr, GBK2UTF8("雷达站实测降雨 ").c_str());
    cairo_show_text(cr, "雷达站实测降雨 ");
    cairo_set_font_size(cr, 20.0 * scale);
    cairo_move_to(cr, 0.5 * w - 200 * scale, 120 * scale);
    std::string time_strrr = "2023年08月01日08时 至 2023年08月02日08时"; // TimeToFormatString(btm, "%Y年%m月%d日%H时 至 ") + TimeToFormatString(etm, "%Y年%m月%d日%H时");
    cairo_show_text(cr, time_strrr.c_str());
    cairo_stroke(cr);
    std::string new_path = out_png_path;
    cairo_surface_write_to_png(surface, new_path.c_str()); cairo_destroy(cr);
    cairo_surface_destroy(surface);

    int symobl_width = 40 * scale;
    int symbol_height = 15 * scale;
    int symbol_step = 5 * scale;
    int symbol_start_x = 1000 * scale;
    int symbol_start_y = 900 * scale;

    surface = cairo_image_surface_create_from_png(new_path.c_str());
    cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "LiSu", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 20.0 * scale);


    for (int i = 0; i < data_values.size(); i++)
    {
        cairo_move_to(cr, symbol_start_x - 100 * scale, symbol_start_y + symbol_height * i + symbol_step * i + symbol_height * 1);
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_show_text(cr, data_values[i].text.c_str()); // 使用相应的文本
        cairo_rectangle(cr, symbol_start_x, symbol_start_y + symbol_height * i + symbol_step * i, symobl_width, symbol_height);
        cairo_set_source_rgb(cr, (data_values[i].r) / 255.0, (data_values[i].g) / 255.0, (data_values[i].b) / 255.0);
        cairo_fill(cr);
    }

    cairo_surface_write_to_png(surface, new_path.c_str());
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    res = true;
    return res;
}


int main()
{
    std::string in_png_path = "D:/1_wangyingjie/code/20230829115818.png ";
    std::string out_png_path = "D:/1_wangyingjie/code/202308010800_3.title.png";
    bool res = write_png_color_box(in_png_path, out_png_path, data_values);
    return 0;
}