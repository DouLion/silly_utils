//
// Created by dell on 2023/11/17.
//
#pragma once
#include "silly_grib2.h"

#if SU_ECCODES_ENABLED
#include <eccodes.h>
#define SILL_ECCODES_ENV_DEFINITION_PATH "ECCODES_DEFINITION_PATH"
#endif

bool silly_grib2_utils::read(const std::string& path, silly_grib2_frame& grb, const size_t& fidx)
{
    bool status = false;
#if SU_ECCODES_ENABLED
    int i = 0;
    FILE* file_h = nullptr;
    codes_context* grb2_c = nullptr;
    codes_handle* grb2_h = nullptr;
    if (!silly_grib2_utils::open_grib2_handle(path, (void**)&file_h, (void**)&grb2_c, (void**)&grb2_h))
    {
        SU_ERROR_PRINT("Open GRB2 handle failed: %s", path.c_str());
        return status;
    }
    while (grb2_h)
    {
        if (fidx == i++)
        {
            if (silly_grib2_utils::load_grib2_frame((const void**) &grb2_h, grb, false))
            {
                status = true;
            }
            break;
        }
        else
        {
            if (!silly_grib2_utils::load_grib2_frame((const void**)&grb2_h, grb, true))
            {
            }
        }
    }
    silly_grib2_utils::close_grib2_handle(file_h, grb2_c, grb2_h);
#endif
    return status;
}

bool silly_grib2_utils::read(const std::string& path, std::map<size_t, silly_grib2_frame>& msgf_grb)
{
    bool status = false;
#if SU_ECCODES_ENABLED
    int i = 0;
    FILE* file_h = nullptr;
    codes_context* grb2_c = nullptr;
    codes_handle* grb2_h = nullptr;
    if (!silly_grib2_utils::open_grib2_handle(path, (void**)&file_h, (void**)&grb2_c,(void**) &grb2_h))
    {
        SU_ERROR_PRINT("Open GRB2 handle failed: %s", path.c_str());
        return status;
    }
    while (grb2_h)
    {
        silly_grib2_frame grb_tmp;
        silly_grib2_utils::load_grib2_frame((const void*)grb2_h, grb_tmp, false);
        codes_handle_delete(grb2_h);
        int err_code;
        grb2_h = codes_handle_new_from_file(nullptr, (FILE*)file_h, PRODUCT_GRIB, &err_code);
        msgf_grb[i] = grb_tmp;
        i++;
        break;
    }
    silly_grib2_utils::close_grib2_handle(file_h, grb2_c, grb2_h);
#endif
    return status;
}

bool silly_grib2_utils::open_grib2_handle(const std::string& path, void** file_h, void** grb2_c, void** grb2_h)
{
    bool status = false;
#if SU_ECCODES_ENABLED
    char* codes_def_path = std::getenv(SILL_ECCODES_ENV_DEFINITION_PATH);
    if (!codes_def_path || !strlen(codes_def_path))
    {
        SU_ERROR_PRINT("需要GRIB2的definition目录,并且设置到系统环境变量<codes_DEFINITION_PATH>(Windows) 或者 <ECCODES_DEFINITION_PATH>(LINUX)中.");
        return status;
    }
    codes_bufr_header* bheader;
    int mssnum = 0;
    codes_bufr_extract_headers_malloc(nullptr, path.c_str(), &bheader, &mssnum, 1);
    
    // TODO: 检查这个codes_context是否可以为null
    // *grb2_c = codes_context_get_default();
    /* turn off support for GRIB2 multi-field messages */
    //codes_grib_multi_support_off(NULL);
    //codes_grib_multi_support_on(NULL);  // 多波段读取支持
    int err_code = 0;
    FILE* in = fopen(path.c_str(), "rb");
    if (nullptr == in)
    {
        SU_ERROR_PRINT("Open GRB2 in failed: %s", path.c_str())
        fclose(in);
        return status;
    }
   /* int mcount           = 0;
    CODES_CHECK(codes_count_in_file(NULL, in, &mcount), 0);
    assert(mcount == 56);
    printf("count_in_file counted %d messages\n", mcount);*/

    codes_handle* handle = codes_handle_new_from_file(nullptr, in, PRODUCT_ANY, &err_code);
    if (handle && in)
    {
        *file_h = in;
        *grb2_h = (codes_handle*)handle;
        status = true;
    }
#endif
    return status;
}

bool silly_grib2_utils::load_grib2_frame(const void* grb2_h, silly_grib2_frame& grb, const bool& skip)
{
    bool status = skip;
#if SU_ECCODES_ENABLED
    while (!skip)
    {
        status = false;
        codes_keys_iterator* gkiter = nullptr;
        gkiter = codes_keys_iterator_new((codes_handle*)grb2_h, CODES_KEYS_ITERATOR_SKIP_EDITION_SPECIFIC|CODES_KEYS_ITERATOR_SKIP_DUPLICATES, nullptr);
        if (nullptr == gkiter)
        {
            break;
        }
        while (codes_keys_iterator_next(gkiter))
        {
            const char* name = codes_keys_iterator_get_name(gkiter);
            int type = 0;
            codes_get_native_type((codes_handle*)grb2_h, name, &type);
            
            /*if (strcmp(name, "codedValues") == 0 || strcmp(name, "values") == 0)
            {
                continue;
            }*/
            if (CODES_TYPE_SECTION == type)
            {
                std::cout << name << " :  --Section--"<< std::endl;
            }

            if (CODES_TYPE_LABEL == type)
            {
                std::cout << name << " :  --Label--" << std::endl;
            }
            if (CODES_TYPE_STRING == type)
            {
                size_t length = 0;
                codes_get_length((codes_handle*)grb2_h, name, &length);
                std::string s_val;
                s_val.resize(length);
                codes_get_string((codes_handle*)grb2_h, name, s_val.data(), &length);
                std::cout << name << " : " << s_val << std::endl;
            }
            if (CODES_TYPE_LONG == type)
            {
                long l_val = 0;
                codes_get_long((codes_handle*)grb2_h, name, &l_val);
                if (strcmp(name, "numberOfValues") == 0)
                {
                    // m_pGeoInfo.dnum = l_val;
                }
                if (strcmp(name, "Ni") == 0)
                {
                    // m_pGeoInfo.cols = l_val;
                }
                if (strcmp(name, "Nj") == 0)
                {
                    // m_pGeoInfo.rows = l_val;
                }
                std::cout << name << " : " << l_val << std::endl;
            }
            else if (CODES_TYPE_DOUBLE == type)
            {
                double d_val = 0;

                codes_get_double((codes_handle*)grb2_h, name, &d_val);

                if (strcmp(name, "latitudeOfFirstGridPointInDegrees") == 0)
                {
                    // m_pGeoInfo.bottom = d_val;
                }
                if (strcmp(name, "longitudeOfFirstGridPointInDegrees") == 0)
                {
                    // m_pGeoInfo.left = d_val;
                }
                if (strcmp(name, "latitudeOfLastGridPointInDegrees") == 0)
                {
                    // m_pGeoInfo.top = d_val;
                }
                if (strcmp(name, "longitudeOfLastGridPointInDegrees") == 0)
                {
                    // m_pGeoInfo.right = d_val;
                }
                if (strcmp(name, "iDirectionIncrementInDegrees") == 0)
                {
                    // m_pGeoInfo.xstep = d_val;
                }
                if (strcmp(name, "jDirectionIncrementInDegrees") == 0)
                {
                    // m_pGeoInfo.ystep = d_val;
                }

                std::cout << name << " : " << d_val << std::endl;
            }
        }
        codes_keys_iterator_delete(gkiter);
        break;
        // 总共有多少个格子
        size_t cell_num = 0;
        grb.data.resize(cell_num);
        // TODO: 检查这一行是干什么的 codes_get_size((codes_handle*)grb2_h, "values", &aa);
        // codes_get_double_array((codes_handle*)grb2_h, "values", &grb.data[0], &cell_num);
        status = true;
    }

#endif
    return status;
}

bool silly_grib2_utils::close_grib2_handle(void* file_h, void* grb2_c, void* grb2_h)
{
    bool status = false;
#if SU_ECCODES_ENABLED
    fclose((FILE*)file_h);
    if (grb2_c)
    {
        codes_handle_delete((codes_handle*)grb2_h);
    }
#endif
    return status;
}
