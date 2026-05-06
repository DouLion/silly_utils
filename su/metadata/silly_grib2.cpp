//
// Created by dell on 2023/11/17.
//
#pragma once
#include "silly_grib2.h"
#include <files/silly_file.h>
#include <log/silly_log.h>
#if SU_THIRD_SUPPORT_ECCODES
#define SILL_ECCODES_ENV_DEFINITION_PATH "ECCODES_DEFINITION_PATH"
#endif


// 读取GRIB2文件中指定索引的帧数据
bool suGrib2::read(const suPath& file, suGrib2::Frame& grb, const size_t& fidx)
{
    bool status = false;
#if SU_THIRD_SUPPORT_ECCODES
    int current_frame_idx = 0;
    FILE* fileHdl = nullptr;
    codes_context* grbCtx = nullptr;
    codes_handle* grbHdl = nullptr;

    // 打开GRIB2文件句柄
    if (!suGrib2::open_grib2_handle(file, (void**)&fileHdl, (void**)&grbCtx, (void**)&grbHdl))
    {
        // SLOG_ERROR("打开GRIB2文件失败: {}", file.u8string()); // 忽略日志
        return status;
    }

    // 遍历GRIB2消息，直到找到目标帧或者文件结束
    while (grbHdl)
    {
        if (current_frame_idx == fidx)
        {
            // 找到目标帧，加载数据
            if (suGrib2::load_grib2_frame(grbHdl, grb, false))
            {
                status = true;
            }
            // 找到并处理完目标帧后，退出循环
            break;
        }
        else
        {
            // 不是目标帧，删除当前句柄并尝试获取下一个
            codes_handle_delete(grbHdl);  // 删除当前的句柄
            grbHdl = nullptr;             // 置空，防止下次循环误用
            int err_code = 0;
            // 获取下一个GRIB2消息的句柄
            grbHdl = codes_handle_new_from_file(nullptr, (FILE*)fileHdl, PRODUCT_ANY, &err_code);
            if (grbHdl)
            {
                current_frame_idx++;  // 成功获取下一个句柄才增加索引
            }
            else
            {
                // 如果没有更多句柄，退出循环
                break;
            }
        }
    }
    // 关闭GRIB2文件句柄
    suGrib2::close_grib2_handle(fileHdl, grbCtx, grbHdl);
#endif
    return status;
}

// 读取GRIB2文件中所有帧数据
bool suGrib2::read(const suPath& file, std::map<size_t, suGrib2::Frame>& msgf_grb)
{
    bool status = false;
#if SU_THIRD_SUPPORT_ECCODES
    int i = 0;
    FILE* fileHdl = nullptr;
    codes_context* grbCtx = nullptr;
    codes_handle* grbHdl = nullptr;

    // 打开GRIB2文件句柄
    if (!suGrib2::open_grib2_handle(file, (void**)&fileHdl, (void**)&grbCtx, (void**)&grbHdl))
    {
        // SLOG_ERROR("打开GRIB2文件失败: {}", file.u8string()); // 忽略日志
        return status;
    }

    // 遍历GRIB2消息
    while (grbHdl)
    {
        suGrib2::Frame grb_tmp;
        // 加载当前帧数据
        suGrib2::load_grib2_frame(grbHdl, grb_tmp, false);  // 传递 codes_handle* 类型
        // 删除当前GRIB2句柄
        codes_handle_delete(grbHdl);
        grbHdl = nullptr;  // 置空
        int err_code;
        // 获取下一个GRIB2句柄
        grbHdl = codes_handle_new_from_file(nullptr, (FILE*)fileHdl, PRODUCT_ANY, &err_code);  // 统一使用 PRODUCT_ANY
        msgf_grb[i] = grb_tmp;
        i++;
    }
    // 关闭GRIB2文件句柄
    suGrib2::close_grib2_handle(fileHdl, grbCtx, grbHdl);
    status = true;  // 如果循环完成，表示成功读取所有帧
#endif
    return status;
}

// 打开GRIB2文件句柄
bool suGrib2::open_grib2_handle(const suPath& file, void** fileHdl, void** grbCtx, void** grbHdl)
{
    bool status = false;
#if SU_THIRD_SUPPORT_ECCODES
    // 获取ECCODES定义文件的路径
    char* codes_def_path = std::getenv(SILL_ECCODES_ENV_DEFINITION_PATH);
    if (!codes_def_path || !strlen(codes_def_path))
    {
        // SU_ERROR_PRINT("需要GRIB2的definition目录,并且设置到系统环境变量<codes_DEFINITION_PATH>(Windows) 或者 <ECCODES_DEFINITION_PATH>(LINUX)中."); // 忽略日志
        return status;
    }

    // TODO: 检查这个codes_context是否可以为null
    *grbCtx = codes_context_get_default();
    /* turn off support for GRIB2 multi-field messages */
    // codes_grib_multi_support_off(NULL);
    codes_grib_multi_support_on(NULL);  // 开启多波段读取支持

    int err_code = 0;
    // 打开文件
    FILE* in = fopen(file.string().c_str(), "rb");
    if (nullptr == in)
    {
        // SLOG_ERROR("打开GRIB2文件失败: {}", file.u8string()); // 忽略日志
        // 如果in是nullptr，则不能fclose
        return status;
    }

    // 从文件创建GRIB2句柄
    codes_handle* handle = codes_handle_new_from_file(nullptr, in, PRODUCT_ANY, &err_code);
    if (handle && in)
    {
        *fileHdl = in;
        *grbHdl = handle;  // 直接赋值
        status = true;
    }
    else
    {
        // 如果句柄创建失败，需要关闭文件
        fclose(in);
    }
#endif
    return status;
}

// 加载GRIB2帧数据
// 参数 grbHdl 类型已更改为 codes_handle*
bool suGrib2::load_grib2_frame(codes_handle* grbHdl, suGrib2::Frame& grb, const bool& skip)
{
    bool status = skip;
#if SU_THIRD_SUPPORT_ECCODES
    // 如果是跳过模式，则直接返回
    if (skip)
    {
        return true;
    }

    codes_keys_iterator* gkiter = nullptr;
    // 创建GRIB2键迭代器，跳过版本特定和重复的键
    gkiter = codes_keys_iterator_new(grbHdl, CODES_KEYS_ITERATOR_SKIP_EDITION_SPECIFIC | CODES_KEYS_ITERATOR_SKIP_DUPLICATES, nullptr);
    if (nullptr == gkiter)
    {
        return false;
    }

    // 遍历所有键
    while (codes_keys_iterator_next(gkiter))
    {
        const char* name = codes_keys_iterator_get_name(gkiter);
        int type = 0;
        // 获取键的原始类型
        codes_get_native_type(grbHdl, name, &type);

        /*
        // 忽略codedValues和values键，因为它们包含大量数据
        if (strcmp(name, "codedValues") == 0 || strcmp(name, "values") == 0)
        {
            continue;
        }
        */

        // 根据键的类型进行处理
        if (CODES_TYPE_SECTION == type)
        {
            // std::cout << name << " :  --Section--" << std::endl; // 忽略调试输出
        }
        else if (CODES_TYPE_LABEL == type)
        {
            // std::cout << name << " :  --Label--" << std::endl; // 忽略调试输出
        }
        else if (CODES_TYPE_STRING == type)
        {
            size_t length = 0;
            codes_get_length(grbHdl, name, &length);
            std::string s_val;
            s_val.resize(length);
            codes_get_string(grbHdl, name, s_val.data(), &length);
            // std::cout << name << " : " << s_val << std::endl; // 忽略调试输出
        }
        else if (CODES_TYPE_LONG == type)
        {
            long l_val = 0;
            codes_get_long(grbHdl, name, &l_val);
            /*
            // 根据键名存储地理信息
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
            */
            // std::cout << name << " : " << l_val << std::endl; // 忽略调试输出
        }
        else if (CODES_TYPE_DOUBLE == type)
        {
            double d_val = 0;
            codes_get_double(grbHdl, name, &d_val);
            /*
            // 根据键名存储地理信息
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
            */
            // std::cout << name << " : " << d_val << std::endl; // 忽略调试输出
        }
    }
    // 删除GRIB2键迭代器
    codes_keys_iterator_delete(gkiter);

    // 获取数据格点数量
    size_t cell_num = 0;
    // 假设我们总是需要获取"values"键的数据
    codes_get_size(grbHdl, "values", &cell_num);
    grb.data.resize(cell_num);
    // 获取"values"键的双精度浮点数组数据
    codes_get_double_array(grbHdl, "values", grb.data.data(), &cell_num);
    grb.is_valid = true;  // 设置帧数据有效
    status = true;

#endif
    return status;
}

// 关闭GRIB2文件句柄
bool suGrib2::close_grib2_handle(void* fileHdl, void* grbCtx, void* grbHdl)
{
    bool status = false;
#if SU_THIRD_SUPPORT_ECCODES
    if (fileHdl)
    {
        fclose((FILE*)fileHdl);  // 关闭文件
    }
    if (grbHdl)  // grbCtx在这里没有直接用到，实际需要关闭的是grbHdl
    {
        codes_handle_delete((codes_handle*)grbHdl);  // 删除GRIB2句柄
    }
    status = true;
#endif
    return status;
}