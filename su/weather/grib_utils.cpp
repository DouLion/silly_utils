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
#if DSU_GRIB_ENABLED
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
                    m_pGeoInfo.dnum = vlong;
                }
                if (strcmp(name, "Ni") == 0)
                {
                    m_pGeoInfo.cols = vlong;
                }
                if (strcmp(name, "Nj") == 0)
                {
                    m_pGeoInfo.rows = vlong;
                }
                if (strcmp(name, "year") == 0)
                {
                    m_pTimeInfo.year = vlong;
                }
                if (strcmp(name, "month") == 0)
                {
                    m_pTimeInfo.month = vlong;
                }
                if (strcmp(name, "day") == 0)
                {
                    m_pTimeInfo.day = vlong;
                }
                if (strcmp(name, "hour") == 0)
                {
                    m_pTimeInfo.hour = vlong;
                }
                if (strcmp(name, "minute") == 0)
                {
                    m_pTimeInfo.miniute = vlong;
                }
            }
            else if (GRIB_TYPE_DOUBLE == type)
            {
                double vdouble = 0;

                grib_get_double(gh, name, &vdouble);

                if (strcmp(name, "latitudeOfFirstGridPointInDegrees") == 0)
                {
                    m_pGeoInfo.bottom = vdouble;
                }
                if (strcmp(name, "longitudeOfFirstGridPointInDegrees") == 0)
                {
                    m_pGeoInfo.left = vdouble;
                }
                if (strcmp(name, "latitudeOfLastGridPointInDegrees") == 0)
                {
                    m_pGeoInfo.top = vdouble;
                }
                if (strcmp(name, "longitudeOfLastGridPointInDegrees") == 0)
                {
                    m_pGeoInfo.right = vdouble;
                }
                if (strcmp(name, "iDirectionIncrementInDegrees") == 0)
                {
                    m_pGeoInfo.xstep = vdouble;
                }
                if (strcmp(name, "jDirectionIncrementInDegrees") == 0)
                {
                    m_pGeoInfo.ystep = vdouble;
                }
                if (strcmp(name, "maximum") == 0)
                {
                    m_pNormalInfo.max = vdouble;
                }
                if (strcmp(name, "minimum") == 0)
                {
                    m_pNormalInfo.min = vdouble;
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
                    m_pNormalInfo.name = std::string(buf);
                }
                if (strcmp(name, "units") == 0)
                {
                    m_pNormalInfo.units = std::string(buf);
                }
                if (strcmp(name, "shortName") == 0)
                {
                    m_pNormalInfo.short_name = std::string(buf);
                }
                free(buf);
                buf = nullptr;
            }
        }
        grib_keys_iterator_delete(kiter);

        // ???????
        if (m_pGeoInfo.rows * m_pGeoInfo.cols != m_pGeoInfo.dnum || 0 == m_pGeoInfo.rows || 0 == m_pGeoInfo.cols)
        {
            grib_handle_delete(gh);
            continue;
        }
        double* data = (double*)malloc(m_pGeoInfo.dnum * sizeof(double));
        size_t tmpSize = m_pGeoInfo.dnum;
        size_t aa = 0;
        grib_get_size(gh, "values", &aa);
        grib_get_double_array(gh, "values", data, &tmpSize);

        DMatrix matrix;
        matrix.create(m_pGeoInfo.rows, m_pGeoInfo.cols);
        for (std::uint16_t r = 0; r < m_pGeoInfo.rows; ++r)
        {
            for (std::uint16_t c = 0; c < m_pGeoInfo.cols; ++c)
            {
                matrix.at(r, c) = (double)data[r * m_pGeoInfo.cols + c];
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
