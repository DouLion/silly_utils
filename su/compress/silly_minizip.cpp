//
// Created by dell on 2023/8/10.
//

#include "silly_minizip.h"
#include "minizip/zip.h"
#include "minizip/unzip.h"
#include "su_marco.h"
#include <log/silly_log.h>
using namespace silly_compress;

//  压缩使用的最大内存 500M
#define SILLY_MINIZ_MAX_BUFF_SIZE (0x1F400000)

#define SILLY_MINIZ_FILE_EXTENSION ".zip"

/// <summary>
/// 将一个文件内容写入到一个 ZIP 文件中
/// </summary>
/// <param name="zip_handle"></param>
/// <param name="file">需要读取的文件</param>
/// <returns></returns>
int write_into_zip_handle(const zipFile& zip_handle, const std::string& file)
{
    int ret = -1;

    std::ifstream input(file.c_str(), std::ios::binary | std::ios::in);
    if (!input.is_open())
    {
        SLOG_ERROR("打开文件错误:{}", file);
        return ret;
    }
    size_t predicte = 100 * 1024 * 1024;

    size_t total = 0;
    while (input)
    {
        std::vector<char> buffer(predicte);
        input.read(buffer.data(), buffer.size());
        size_t read_size = input.gcount();  // 获取实际读取的字节数
        if (read_size == 0)
        {
            break;  // 如果没有读取到任何字节，说明已到达文件末尾或出错
        }
        int zs = zipWriteInFileInZip(zip_handle, buffer.data(), read_size);
        if (ZIP_OK != zs)
        {
            break;
        }
        total += read_size;
        SLOG_DEBUG("已处理{}M", total * 100 / predicte)
    }
    if (input.eof())
    {
        ret = 0;
    }
    else if (input.fail())
    {
        SLOG_ERROR("读取文件时发生错误");
    }
    input.close();
    return ret;
}

void recursive_list_paths(const std::string& recurse_root, std::vector<std::string>& paths)
{
    try
    {
        for (auto fiter : std::filesystem::recursive_directory_iterator(recurse_root))
        {
            paths.push_back(fiter.path().string());
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        SLOG_ERROR("遍历目录失败: {}", e.what());
    }
}

// 压缩文件
int minizip_compress_file(const zipFile& zip_handle, const std::string& src)
{
    std::string srcFileName = std::filesystem::path(src).filename().string();
    zip_fileinfo zFileInfo = {0};
    int ret = zipOpenNewFileInZip3_64(zip_handle, srcFileName.c_str(), &zFileInfo, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION, 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, nullptr, 0, 1);
    if (ret != ZIP_OK)
    {
        SLOG_ERROR("Failed to open file in zip: {}", src);
        return ret;
    }
    ret = write_into_zip_handle(zip_handle, src);
    if (ret != ZIP_OK)
    {
        SLOG_ERROR("ailed to write file to zip: {}", src);
    }
    return ret;
}

// 压缩文件夹
int minizip_compress_dir(const zipFile& zip_handle, const std::string& root)
{
    std::vector<std::string> paths;
    auto sfp_relate_root = std::filesystem::path(root).parent_path();
    std::string relate_root = sfp_relate_root.string();
    recursive_list_paths(root, paths);

    for (const auto& path : paths)
    {
        bool is_dir = std::filesystem::is_directory(path);
        std::string tmp_relate = std::filesystem::relative(path, sfp_relate_root).string();

        // 首先处理目录
        if (is_dir)
        {
            tmp_relate.append("/");  // 添加尾随斜线并在ZIP中创建条目
            zip_fileinfo tmp_zinfo = {};
            int ret = zipOpenNewFileInZip3_64(zip_handle, tmp_relate.c_str(), &tmp_zinfo, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION, 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, nullptr, 0, 1);
            if (ret != ZIP_OK)
            {
                SLOG_ERROR("创建ZIP目录条目失败: {}", path);
                return ret;
            }
            continue;
        }

        // 处理文件
        zip_fileinfo tmp_zinfo = {};
        int ret = zipOpenNewFileInZip3_64(zip_handle, tmp_relate.c_str(), &tmp_zinfo, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION, 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, nullptr, 0, 1);
        if (ret != ZIP_OK)
        {
            SLOG_ERROR("打开ZIP文件条目失败: {}", path);
            return ret;
        }

        ret = write_into_zip_handle(zip_handle, path);
        if (ret != ZIP_OK)
        {
            SLOG_ERROR("写入ZIP文件内容失败: {}", path);
            return ret;
        }
    }

    return ZIP_OK;
}

/// 将文件或目录压缩为ZIP文件
CPS_ERR MiniZip::compress(const std::string& s_src, const std::string& s_dst, const bool& append)
{
    auto status = CPS_ERR::MiniZUnknowErr;
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
            out_dst = sfp_src.parent_path().append(sfp_src.stem().string().append(SILLY_MINIZ_FILE_EXTENSION)).string();
        }
        if (!append)  // 非追加,先删除原文件
        {
            std::filesystem::remove(out_dst);
        }
        zipFile zip_handle = zipOpen64(out_dst.c_str(), APPEND_STATUS_CREATE);
        if (!zip_handle)
        {
            return CPS_ERR::MiniZCreatZipErr;  //  创建写入的zip失败
        }

        do
        {
            if (std::filesystem::is_directory(s_src))  // 压缩文件夹
            {
                if (ZIP_OK != minizip_compress_dir(zip_handle, s_src))
                {
                    status = CPS_ERR::MiniZOpenFileErr;
                    break;
                }
            }
            else  // 压缩单个文件
            {
                if (ZIP_OK != minizip_compress_file(zip_handle, s_src))
                {
                    status = CPS_ERR::MiniZOpenFileErr;
                    break;
                }
            }
            status = CPS_ERR::Ok;
        } while (0);
        zipClose(zip_handle, nullptr);
    }
    catch (std::exception& e)
    {
        SLOG_ERROR("错误{}", e.what())
    }
    catch (...)
    {
        SLOG_ERROR("未知错误")
    }

    return status;
}

/// 解压zip文件,解压单独文件和目录文件
CPS_ERR MiniZip::decompress(const std::string& s_src, const std::string& s_dst)
{
    if (!std::filesystem::exists(s_src))  // 解压文件不存在
    {
        SU_ERROR_PRINT("not exist {}", s_src.c_str());
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
            return CPS_ERR::MiniZCreatDirErr;  // 创建目录失败
        }
    }
    unzFile zipFile = unzOpen64(s_src.c_str());  // 打开ZIP文件
    if (zipFile == nullptr)
    {
        return CPS_ERR::MiniZOpenFileErr;  // 打开ZIP文件失败
    }
    // 进入ZIP文件中的第一个文件
    if (unzGoToFirstFile(zipFile) != UNZ_OK)
    {
        unzClose(zipFile);
        return CPS_ERR::MiniZFileEmptyErr;  // ZIP文件为空
    }
    size_t currentUncompressedSize = 0;  // 已解压的总大小
    size_t predicte = 1024 * 1024;
    do  // 循环解压缩所有文件
    {
        size_t nameLen = 0;            // 该压缩分支文件名长度
        size_t uncompressed_size = 0;  // 压缩文件大小
        unz_file_info64 file_info;     // 获取当前文件信息
        if (unzGetCurrentFileInfo64(zipFile, &file_info, nullptr, 0, nullptr, 0, nullptr, 0) == UNZ_OK)
        {
            nameLen = file_info.size_filename;
            uncompressed_size = file_info.uncompressed_size;
        }
        char* filename = (char*)malloc(nameLen + 1);  // 文件名
        if (unzGetCurrentFileInfo64(zipFile, &file_info, filename, nameLen + 1, nullptr, 0, nullptr, 0) != UNZ_OK)
        {
            SU_MEM_FREE(filename);
            return CPS_ERR::MiniZGetInforErr;  // 获取文件信息失败
        }
        std::filesystem::path one_file = outputDir;
        if (filename == nullptr)
        {
            SU_MEM_FREE(filename);
            continue;
        }
        one_file.append(filename);
        if (filename[nameLen - 1] == '/' || filename[nameLen - 1] == '\\')  // 压缩分支为目录
        {
            if (!std::filesystem::exists(one_file))
            {
                std::filesystem::create_directories(one_file.string());
                if (!std::filesystem::exists(one_file))
                {
                    SU_MEM_FREE(filename);
                    return CPS_ERR::MiniZCreatDirErr;  // 创建目录失败
                }
            }
        }
        else  // 压缩分支为文件
        {
            // 创建父目录
            if (std::filesystem::is_directory(one_file))
            {
                SU_MEM_FREE(filename);
                continue;
            }
            std::filesystem::create_directories(std::filesystem::path(one_file).parent_path());
            // 写入文件
            std::ofstream outFile(one_file, std::ios::binary);
            if (!outFile.is_open())
            {
                SU_MEM_FREE(filename);
                return CPS_ERR::MiniZCreatDirErr;  // 创建文件失败
            }
            else
            {
                size_t block_size = 100 * 1024 * 1024;  // 每次解压100MB
                char* buffer = (char*)malloc(block_size);
                int readSize;
                unzOpenCurrentFile(zipFile);
                while ((readSize = unzReadCurrentFile(zipFile, buffer, block_size)) > 0)
                {
                    outFile.write(buffer, readSize);
                    currentUncompressedSize += readSize;  // 更新已解压的总大小
                    SLOG_DEBUG("已处理{}M", currentUncompressedSize / predicte)
                }
                outFile.close();
                SU_MEM_FREE(buffer);
            }
        }
        SU_MEM_FREE(filename);

    } while (unzGoToNextFile(zipFile) == UNZ_OK);
    unzClose(zipFile);
    return CPS_ERR::Ok;
}

CPS_ERR MiniZip::compress(const char* c_in_val, const size_t& i_in_len, char** c_out_val, size_t& i_out_len)
{
    if (c_in_val == nullptr || i_in_len == 0)
    {
        SLOG_ERROR("Empty input data.");
        return CPS_ERR::InValidInputErr;
    }

    uLong dest_len = compressBound(i_in_len);  // 解压后的数据大小
    if (!*c_out_val)
    {
        *c_out_val = (char*)malloc(dest_len);
    }
    else
    {
        SLOG_ERROR("Clean output and set null.");
        return CPS_ERR::InValidOutputErr;
    }
    i_out_len = 0;
    // 压缩
    int err = compress2((Bytef*)*c_out_val, &dest_len, (Bytef*)c_in_val, uLong(i_in_len), Z_DEFAULT_COMPRESSION);
    if (err != Z_OK)
    {
        SU_MEM_FREE(*c_out_val);
        return CPS_ERR::MiniZCompressStrErr;
    }

    i_out_len = dest_len;  // 解压后的数据大小
    return CPS_ERR::Ok;
}

CPS_ERR MiniZip::decompress(const char* c_in_val, const size_t& i_in_len, char** c_out_val, size_t& i_out_len)
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

    uLongf dest_len = compressBound(i_in_len);  // 解压后的数据大小
    *c_out_val = (char*)malloc(dest_len);

    i_out_len = 0;
    uLong in_len = uLong(i_in_len);
    // 解压
    int err = uncompress2((Bytef*)*c_out_val, &dest_len, (const Bytef*)c_in_val, &in_len);
    if (err != Z_OK)
    {
        SU_MEM_FREE(*c_out_val);
        return CPS_ERR::MiniZUncompressStrErr;
    }
    i_out_len = dest_len;  // 解压后的数据大小

    return CPS_ERR::Ok;
}
