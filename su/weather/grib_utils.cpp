//
// Created by dly on 2023/7/10.
//

#include "grib_utils.h"
#if SU_GRIB_ENABLED
#include "grib_api.h"
#endif
using namespace grib_data;
#include <filesystem>

bool grib_utils::read(const std::string& grib_file, std::vector<DMatrix>& matrixs, int& type)
{
#if IS_WIN32
    char* grib_def_path = getenv("GRIB_DEFINITION_PATH");
#else
    char* grib_def_path = getenv("ECCODES_DEFINITION_PATH");
#endif
    if (!strlen(grib_def_path))
    {
        perror("cannot find Environment: <GRIB_DEFINITION_PATH> or <ECCODES_DEFINITION_PATH>.");
        return false;
    }

    std::filesystem::path targetPath(grib_file);
    if (!std::filesystem::exists(targetPath))
    {
        return false;
    }
#if SU_GRIB_ENABLED
    grib_context* c = grib_context_get_default();
    // 多波段读取支持
    grib_multi_support_on(nullptr);
    int err_code = 0;
    FILE* file = nullptr;
    file = fopen(targetPath.string().c_str(), "rb");
    if (nullptr == file)
    {
        std::cout << "Open file failed :" << targetPath << std::endl;
        fclose(file);
        return false;
    }

    grib_handle* gh = grib_handle_new_from_file(c, file, &err_code);
    while (gh)
    {
        if (nullptr == gh)
        {
            std::cout << "Open grib_handle failed :" << targetPath << std::endl;
            fclose(file);
            return false;
        }

        grib_keys_iterator* kiter = grib_keys_iterator_new(gh, 0, nullptr);
        if (nullptr == kiter)
        {
            std::cout << "Open grib_keys_iterator failed :" << targetPath << std::endl;
            fclose(file);
            return false;
        }
        while (grib_keys_iterator_next(kiter))
        {
            const char* name = grib_keys_iterator_get_name(kiter);
            int type = 0;
            grib_get_native_type(gh, name, &type);
            if (strcmp(name, "codedValues") == 0 || strcmp(name, "values") == 0)
            {
                continue;
            }
            if (GRIB_TYPE_LONG == type)
            {
                long vlong = 0;
                grib_get_long(gh, name, &vlong);
                if (strcmp(name, "numberOfValues") == 0)
                {
                    m_geo_info.dnum = vlong;
                }
                if (strcmp(name, "Ni") == 0)
                {
                    m_geo_info.cols = vlong;
                }
                if (strcmp(name, "Nj") == 0)
                {
                    m_geo_info.rows = vlong;
                }
                if (strcmp(name, "year") == 0)
                {
                    m_time_info.year = vlong;
                }
                if (strcmp(name, "month") == 0)
                {
                    m_time_info.month = vlong;
                }
                if (strcmp(name, "day") == 0)
                {
                    m_time_info.day = vlong;
                }
                if (strcmp(name, "hour") == 0)
                {
                    m_time_info.hour = vlong;
                }
                if (strcmp(name, "minute") == 0)
                {
                    m_time_info.miniute = vlong;
                }
            }
            else if (GRIB_TYPE_DOUBLE == type)
            {
                double vdouble = 0;

                grib_get_double(gh, name, &vdouble);

                if (strcmp(name, "latitudeOfFirstGridPointInDegrees") == 0)
                {
                    m_geo_info.bottom = vdouble;
                }
                if (strcmp(name, "longitudeOfFirstGridPointInDegrees") == 0)
                {
                    m_geo_info.left = vdouble;
                }
                if (strcmp(name, "latitudeOfLastGridPointInDegrees") == 0)
                {
                    m_geo_info.top = vdouble;
                }
                if (strcmp(name, "longitudeOfLastGridPointInDegrees") == 0)
                {
                    m_geo_info.right = vdouble;
                }
                if (strcmp(name, "iDirectionIncrementInDegrees") == 0)
                {
                    m_geo_info.xstep = vdouble;
                }
                if (strcmp(name, "jDirectionIncrementInDegrees") == 0)
                {
                    m_geo_info.ystep = vdouble;
                }
                if (strcmp(name, "maximum") == 0)
                {
                    m_normal_info.max = vdouble;
                }
                if (strcmp(name, "minimum") == 0)
                {
                    m_normal_info.min = vdouble;
                }
                // std::cout << name << " : " << vdouble << std::endl;
            }
            else if (GRIB_TYPE_STRING == type)
            {
                size_t length = 0;
                grib_get_length(gh, name, &length);
                char* buf = (char*)malloc(length);
                grib_get_string(gh, name, buf, &length);
                if (strcmp(name, "name") == 0)
                {
                    m_normal_info.name = std::string(buf);
                }
                if (strcmp(name, "units") == 0)
                {
                    m_normal_info.units = std::string(buf);
                }
                if (strcmp(name, "shortName") == 0)
                {
                    m_normal_info.short_name = std::string(buf);
                }
                free(buf);
                buf = nullptr;
            }
        }
        grib_keys_iterator_delete(kiter);

        // 
        if (m_geo_info.rows * m_geo_info.cols != m_geo_info.dnum || 0 == m_geo_info.rows || 0 == m_geo_info.cols)
        {
            grib_handle_delete(gh);
            continue;
        }
		
        double* data = (double*)malloc(m_geo_info.dnum * sizeof(double));
        size_t tmp_size = m_geo_info.dnum;
        size_t aa = 0;
        grib_get_size(gh, "values", &aa);
        grib_get_double_array(gh, "values", data, &tmp_size);
		
        DMatrix matrix;
        matrix.create(m_geo_info.rows, m_geo_info.cols);
        for (std::uint16_t r = 0; r < m_geo_info.rows; ++r)
        {
            for (std::uint16_t c = 0; c < m_geo_info.cols; ++c)
            {
                matrix.at(r, c) = (double)data[r * m_geo_info.cols + c];
            }
        }

        free(data);
        data = nullptr;
        grib_handle_delete(gh);
        gh = grib_handle_new_from_file(c, file, &err_code);
        matrixs.emplace_back(matrix);
    }

    fclose(file);
    return true;
#else

    return false;
#endif
}
