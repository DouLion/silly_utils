//
// Created by dell on 2023/11/17.
//
#pragma once
#include "silly_grib2.h"

#if SU_GRIB_ENABLED
#if IS_WIN32
#include <grib_api.h>
#define SILL_GRIB2_ENV_DEFINITION_PATH "GRIB_DEFINITION_PATH"
#else
#include <eccodes.h>
#define SILL_GRIB2_ENV_DEFINITION_PATH "ECCODES_DEFINITION_PATH"
#endif
#endif

bool silly_grib2_utils::read(const std::string& path, silly_grib2_frame& grb, const size_t& fidx)
{
    bool status = false;
#if SU_GRIB_ENABLED
    int i = 0;
    FILE* file_h = nullptr;
    grib_context* grb2_c = nullptr;
    grib_handle* grb2_h = nullptr;
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
#if SU_GRIB_ENABLED
    int i = 0;
    FILE* file_h = nullptr;
    grib_context* grb2_c = nullptr;
    grib_handle* grb2_h = nullptr;
    if (!silly_grib2_utils::open_grib2_handle(path, (void**)&file_h, (void**)&grb2_c,(void**) &grb2_h))
    {
        SU_ERROR_PRINT("Open GRB2 handle failed: %s", path.c_str());
        return status;
    }
    while (grb2_h)
    {
        silly_grib2_frame grb_tmp;
        silly_grib2_utils::load_grib2_frame((const void*)grb2_h, grb_tmp, false);
        grib_handle_delete(grb2_h);
        int err_code;
        grb2_h = grib_handle_new_from_file(grb2_c, (FILE*)file_h, &err_code);
        msgf_grb[i] = grb_tmp;
        i++;
    }
    silly_grib2_utils::close_grib2_handle(file_h, grb2_c, grb2_h);
#endif
    return status;
}

bool silly_grib2_utils::open_grib2_handle(const std::string& path, void** file_h, void** grb2_c, void** grb2_h)
{
    bool status = false;
#if SU_GRIB_ENABLED
    char* grib_def_path = std::getenv(SILL_GRIB2_ENV_DEFINITION_PATH);
    if (!grib_def_path || !strlen(grib_def_path))
    {
        SU_ERROR_PRINT("需要GRIB2的definition目录,并且设置到系统环境变量<GRIB_DEFINITION_PATH>(Windows) 或者 <ECCODES_DEFINITION_PATH>(LINUX)中.");
        return status;
    }

    // TODO: 检查这个grib_context是否可以为null
    *grb2_c = grib_context_get_default();

    grib_multi_support_on(nullptr);  // 多波段读取支持
    int err_code = 0;
    FILE* file = fopen(path.c_str(), "rb");
    if (nullptr == file)
    {
        SU_ERROR_PRINT("Open GRB2 file failed: %s", path.c_str())
        fclose(file);
        return status;
    }

    *grb2_h = grib_handle_new_from_file((grib_context*)*grb2_c, file, &err_code);
    if (grb2_h && file)
    {
        *file_h = file;
        *grb2_h = (grib_handle*)grb2_h;
        status = true;
    }
#endif
    return status;
}

bool silly_grib2_utils::load_grib2_frame(const void* grb2_h, silly_grib2_frame& grb, const bool& skip)
{
    bool status = skip;
#if SU_GRIB_ENABLED
    while (!skip)
    {
        status = false;
        grib_keys_iterator* gkiter = nullptr;
        gkiter = grib_keys_iterator_new((grib_handle*)grb2_h, GRIB_KEYS_ITERATOR_SKIP_EDITION_SPECIFIC|GRIB_KEYS_ITERATOR_SKIP_DUPLICATES, nullptr);
        if (nullptr == gkiter)
        {
            break;
        }
        while (grib_keys_iterator_next(gkiter))
        {
            const char* name = grib_keys_iterator_get_name(gkiter);
            int type = 0;
            grib_get_native_type((grib_handle*)grb2_h, name, &type);
            if (strcmp(name, "codedValues") == 0 || strcmp(name, "values") == 0)
            {
                continue;
            }
            if (GRIB_TYPE_LONG == type)
            {
                long l_val = 0;
                grib_get_long((grib_handle*)grb2_h, name, &l_val);
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
            }
            else if (GRIB_TYPE_DOUBLE == type)
            {
                double d_val = 0;

                grib_get_double((grib_handle*)grb2_h, name, &d_val);

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

                // std::cout << name << " : " << d_val << std::endl;
            }
        }
        grib_keys_iterator_delete(gkiter);

        // 总共有多少个格子
        size_t cell_num = 0;
        grb.data.resize(cell_num);
        // TODO: 检查这一行是干什么的 grib_get_size((grib_handle*)grb2_h, "values", &aa);
        grib_get_double_array((grib_handle*)grb2_h, "values", &grb.data[0], &cell_num);
        status = true;
    }

#endif
    return status;
}

bool silly_grib2_utils::close_grib2_handle(void* file_h, void* grb2_c, void* grb2_h)
{
    bool status = false;
#if SU_GRIB_ENABLED
    fclose((FILE*)file_h);
    if (grb2_c)
    {
        grib_handle_delete((grib_handle*)grb2_h);
    }
#endif
    return status;
}
