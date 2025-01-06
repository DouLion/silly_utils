//
// Created by dell on 2023/8/10.
//

#include "silly_rar.h"
#if ENABLE_LIBARCHIVE
#include <archive.h>
#include <archive_entry.h>
#endif
#include <log/silly_log.h>
#include "encode/silly_encode.h"

using namespace silly_compress;

//  压缩使用的最大内存 500M
#define SILLY_RAR_MAX_BUFF_SIZE (0x1F400000)

#define SILLY_RAR_FILE_EXTENSION ".rar"

/// 将文件或目录压缩为ZIP文件
CPS_ERR silly_rar::compress(const std::string& s_src, const std::string& s_dst, const bool& append)
{
    auto status = CPS_ERR::MiniZUnknowErr;
#if ENABLE_LIBARCHIVE
    try
    {
        if (!std::filesystem::exists(s_src))
        {
            return CPS_ERR::FileNotExistErr;
        }
        std::string out_dst = s_dst;
        if (out_dst.empty())  // 补充默认压缩路径
        {
            auto sfp_src = std::filesystem::path(s_src);
            out_dst = sfp_src.parent_path().append(sfp_src.stem().string().append(SILLY_RAR_FILE_EXTENSION)).string();
        }
        if (!append)  // 非追加,先删除原文件
        {
            std::filesystem::remove(out_dst);
        }

        //////////////

        //////////////
    }
    catch (std::exception& e)
    {
        SLOG_ERROR("错误{}", e.what())
    }
    catch (...)
    {
        SLOG_ERROR("未知错误")
    }
#endif

    return status;
}

CPS_ERR silly_rar::decompress(const std::string& s_src, const std::string& s_dst)
{
#if ENABLE_LIBARCHIVE
    if (!std::filesystem::exists(s_src))  // 解压文件不存在
    {
        SLOG_ERROR("not exist {}", s_src.c_str());
        return CPS_ERR::FileNotExistErr;
    }

    std::filesystem::path outputDir;
    if (s_dst.empty())  // 如果解压路径为空,创建一个和压缩包名称相同的目录,解压到该目录下
    {
        outputDir = std::filesystem::path(s_src).parent_path();
    }
    else
    {
        outputDir = s_dst;
    }

    if (!std::filesystem::exists(outputDir))
    {
        if (!std::filesystem::create_directories(outputDir))
        {
            return CPS_ERR::RARCreatDirErr;  // 创建目录失败
        }
    }

    struct archive* archive_ptr = archive_read_new();
    if (archive_read_support_format_all(archive_ptr) != ARCHIVE_OK)
    {
        SLOG_ERROR("无法支持所有格式: {}", archive_error_string(archive_ptr));
        archive_read_free(archive_ptr);
        return CPS_ERR::RARSuportFormatErr;
    }

    if (archive_read_open_filename(archive_ptr, s_src.c_str(), 10240) != ARCHIVE_OK)
    {
        SLOG_ERROR("无法打开压缩文件: {}", archive_error_string(archive_ptr));
        archive_read_free(archive_ptr);
        return CPS_ERR::RAROpenErr;
    }

    struct archive_entry* entry;
    int r;

    // 定义单个文件的最大读取块大小为100MB
    const size_t predicte = 100 * 1024 * 1024;

    while ((r = archive_read_next_header(archive_ptr, &entry)) == ARCHIVE_OK)
    {
        // 获取文件名和路径
        std::string entry_name = archive_entry_pathname_utf8(entry);
        std::string gbk_entry_name;
        if (silly_encode::is_utf8(entry_name))
        {
            gbk_entry_name = silly_encode::utf8_gbk(entry_name);
        }
        else
        {
            gbk_entry_name = entry_name;
        }
        std::filesystem::path temp_path(outputDir);
        temp_path.append(gbk_entry_name);
        std::string full_path = temp_path.string();
        // 处理文件或目录
        std::filesystem::path f_full_path(full_path);
        if (!std::filesystem::exists(f_full_path.parent_path()))  // 解压文件不存在
        {
            std::filesystem::create_directories(f_full_path.parent_path());  // 创建目录
        }

        if (archive_entry_filetype(entry) == AE_IFDIR)
        {
            std::filesystem::create_directory(full_path);  // 创建目录
        }
        else if (archive_entry_filetype(entry) == AE_IFREG)
        {
            // 创建文件并写入内容
            std::ofstream out_file(full_path, std::ios::binary | std::ios::trunc);
            if (!out_file)
            {
                std::cerr << "Failed to create file: " << full_path << std::endl;
                archive_read_free(archive_ptr);
                return CPS_ERR::RARWriteErr;
            }

            // 写入文件内容
            const void* buff;
            size_t size;
            int64_t offset;
            off_t entry_size = archive_entry_size(entry);

            // 对于大于 predicte 的文件进行分块读取和写入
            if (entry_size > static_cast<off_t>(predicte))
            {
                size_t bytes_written = 0;
                while ((r = archive_read_data_block(archive_ptr, &buff, &size, &offset)) == ARCHIVE_OK)
                {
                    while (size > 0)
                    {
                        size_t chunkSize = std::min(size, predicte - (bytes_written % predicte));
                        out_file.write(static_cast<const char*>(buff), chunkSize);
                        if (!out_file)
                        {
                            std::cerr << "Error writing to file: " << full_path << std::endl;
                            archive_read_free(archive_ptr);
                            return CPS_ERR::RARWriteErr;
                        }
                        bytes_written += chunkSize;
                        size -= chunkSize;
                        buff = static_cast<const char*>(buff) + chunkSize;
                    }
                }
            }
            else
            {
                // 对于小于 predicte 的文件一次性读取和写入
                while ((r = archive_read_data_block(archive_ptr, &buff, &size, &offset)) == ARCHIVE_OK)
                {
                    out_file.write(static_cast<const char*>(buff), size);
                }
            }

            out_file.close();
        }

        // 下一个文件
        if (archive_read_data_skip(archive_ptr) != ARCHIVE_OK)
        {
            SLOG_ERROR("跳过数据失败: {}", archive_error_string(archive_ptr));
            archive_read_free(archive_ptr);
            return CPS_ERR::RARReadErr;
        }
    }

    if (r != ARCHIVE_EOF)
    {
        SLOG_ERROR("提取存档时出错: {}", archive_error_string(archive_ptr));
        archive_read_free(archive_ptr);
        return CPS_ERR::RARReadErr;
    }

    // 清理资源
    archive_read_free(archive_ptr);
#endif
    return CPS_ERR::Ok;
}

CPS_ERR silly_rar::compress(const char* c_in_val, const size_t& i_in_len, char** c_out_val, size_t& i_out_len)
{
    if (c_in_val == nullptr || i_in_len == 0)
    {
        SLOG_ERROR("Empty input data.");
        return CPS_ERR::InValidInputErr;
    }

    return CPS_ERR::Ok;
}

CPS_ERR silly_rar::decompress(const char* c_in_val, const size_t& i_in_len, char** c_out_val, size_t& i_out_len)
{
    // 检查输入参数
    if (c_in_val == nullptr || i_in_len == 0)
    {
        SLOG_ERROR("Empty input data.");
        return CPS_ERR::InValidInputErr;
    }
    if (*c_out_val)
    {
        SLOG_ERROR("Clean output and set null.");
        return CPS_ERR::InValidOutputErr;
    }

    return CPS_ERR::Ok;
}
