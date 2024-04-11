//
// Created by dell on 2024/4/11.
//

#include "silly_captcha.h"
#include <ctime>
#include <cairo/cairo.h>
#include <cairo/cairo-ft.h>
#include <ft2build.h>
#include <vector>
#include <string>
#include FT_FREETYPE_H

// const char SILLY_POSSIBLE_CHAR[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
const char SILLY_POSSIBLE_CHAR[] = "ABCDEFGHJKMNPQRSTUVWXYZabcdefghjkmnpqrstuvwxyz23456789"; // 删除常见容易混淆的字母和数字
const int SILLY_POSSIBLE_NUM[]= {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
const char SILLY_POSSIBLE_OPERATOR[] = "+-×";


static cairo_status_t cairo_write_func_mine (void *closure, const unsigned char *data, unsigned int length)
{
    std::vector<char>& vecData = *((std::vector<char>*)closure);
    for (unsigned int i = 0; i < length; ++i)
    {
        vecData.push_back(data[i]);
    }
    return CAIRO_STATUS_SUCCESS;
}

static  bool cairo_draw(std::string& result, const std::string text, const std::string font_path, const int font_size=100)
{
    bool status = false;
    double width = font_size * text.size();
    if(width == 0 )
    {
        return status;
    }

    double height = font_size;
    cairo_surface_t* surface;
    cairo_t* cr;
    FT_Library ft_library;
    FT_Face ft_face;

    FT_Error ft_error;
    bool valid_font = false;
    if (!font_path.empty())
    {
        if (!FT_Init_FreeType(&ft_library))
        {
            if (!FT_New_Face(ft_library, font_path.c_str(), 0, &ft_face))
            {
                valid_font = true;
            }
        }
    }
    
    surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
    cr = cairo_create(surface);
    cairo_set_source_rgb(cr, 1, 1, 1); // RGB 白色
    cairo_paint(cr); //
    cairo_font_face_t* cairo_font_face = nullptr;
    if (valid_font) // 
    {
        cairo_font_face = cairo_ft_font_face_create_for_ft_face(ft_face, 0);
        cairo_set_font_face(cr, cairo_font_face);
    }
    
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_set_font_size(cr, font_size); // 设置字体大小
    cairo_text_extents_t extents;
    cairo_text_extents(cr, text.c_str(), &extents);
    double font_size_scale = std::min((width / extents.width), (height / extents.height)) * 0.95;
    cairo_set_font_size(cr, font_size_scale * font_size);
    double x = (width - extents.width * font_size_scale) / 2.0;
    double y = height - (height - extents.height * font_size_scale) / 2.0;
    cairo_move_to(cr, x, y); // 设置文本起始位置
    cairo_show_text(cr, text.c_str()); // 绘制文本
    

    std::vector<unsigned char> vecData;
    cairo_surface_write_to_png_stream(surface, cairo_write_func_mine, &vecData);
    result.resize(vecData.size());
    memcpy(&result[0], &vecData[0], vecData.size());

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    if (valid_font)
    {
        cairo_font_face_destroy(cairo_font_face);
        FT_Done_Face(ft_face);
        FT_Done_FreeType(ft_library);
    }
   
    return true;
}

bool silly_captcha::get_letter_captcha(std::string& answer, std::string& image, const size_t& letter_len, const std::string font_path)
{
    answer = generate_letter_captcha(letter_len);
    return cairo_draw(image, answer, font_path);
}
bool silly_captcha::get_math_calc_captcha(std::string& answer, std::string& image, const std::string font_path)
{
    std::string text = generate_math_calc_captcha(answer);
    return cairo_draw(image, text, font_path);
}
std::string silly_captcha::generate_letter_captcha(const size_t& letter_len)
{
    std::string result;
    result.resize(letter_len);
    srand(time(0));
    for (int i = 0; i < letter_len; i++) {
        int index = rand() % (sizeof(SILLY_POSSIBLE_CHAR) - 1);
        result[i]= SILLY_POSSIBLE_CHAR[index];
    }
    return result;
}
std::string silly_captcha::generate_math_calc_captcha(std::string& result)
{
    std::string text;
    srand(time(0));
    int ai =  rand() % 10;
    int bi =  rand() % 10;
    int oi =  rand() % 3;

    int ir = 0;
    switch (oi)
    {
        case 0:
            ir = ai + bi;
            text.append(std::to_string(ai)).append("+").append(std::to_string(bi));
            break ;
        case 1:
            if (ai < bi)
            {
                ir = bi - ai;
                text.append(std::to_string(bi)).append("-").append(std::to_string(ai));
            }
            else
            {
                ir = ai - bi;
                text.append(std::to_string(ai)).append("-").append(std::to_string(bi));
            }
            break;
        case 2:
            ir = ai * bi;
            text.append(std::to_string(ai)).append("×").append(std::to_string(bi));
            break;
    }
    result = std::to_string(ir);
    return text;
}
