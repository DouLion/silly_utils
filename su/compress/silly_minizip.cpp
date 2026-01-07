//
// Created by dell on 2023/8/10.
//

#include "silly_minizip.h"
#include "minizip/zip.h"
#include "minizip/unzip.h"
#include <log/silly_log.h>
using namespace silly_compress;

//  压缩使用的最大内存 500M
#define SILLY_MINIZ_MAX_BUFF_SIZE (0x1F400000)

#define SILLY_MINIZ_FILE_EXTENSION ".zip"

/// <summary>
/// 将一个文件内容写入到一个 ZIP 文件中
/// </summary>
/// <param name="zipHDL"></param>
/// <param name="file">需要读取的文件</param>
/// <returns></returns>
static int write_into_zipHDL(const zipFile& zipHDL, const suPath& file)
{
    int ret = -1;

    std::ifstream input(file.string().c_str(), std::ios::binary | std::ios::in);
    if (!input.is_open())
    {
        SLOG_ERROR("打开文件错误:{}", file.u8string());
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
        int zs = zipWriteInFileInZip(zipHDL, buffer.data(), read_size);
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

// 压缩文件
int minizip_compress_file(const zipFile& zipHDL, const suPath& src)
{
    std::string srcFileName = src.name();
    zip_fileinfo zFileInfo = {0};
    int ret = zipOpenNewFileInZip3_64(zipHDL, srcFileName.c_str(), &zFileInfo, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION, 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, nullptr, 0, 1);
    if (ret != ZIP_OK)
    {
        SLOG_ERROR("Failed to open file in zip: {}", src.u8string());
        return ret;
    }
    ret = write_into_zipHDL(zipHDL, src);
    if (ret != ZIP_OK)
    {
        SLOG_ERROR("ailed to write file to zip: {}", src.u8string());
    }
    return ret;
}

// 压缩文件夹
int minizip_compress_dir(const zipFile& zipHDL, const suPath& root)
{
    std::vector<suPath> paths = root.relist();
    auto sfp_relate_root =root.parent();
    std::string relate_root = sfp_relate_root.string();
    for (const auto& path : paths)
    {
        bool is_dir = path.is_dir();
        std::string tmp_relate = path.relative(sfp_relate_root).string();

        // 首先处理目录
        if (is_dir)
        {
            tmp_relate.append("/");  // 添加尾随斜线并在ZIP中创建条目
            zip_fileinfo tmp_zinfo = {};
            int ret = zipOpenNewFileInZip3_64(zipHDL, tmp_relate.c_str(), &tmp_zinfo, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION, 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, nullptr, 0, 1);
            if (ret != ZIP_OK)
            {
                SLOG_ERROR("创建ZIP目录条目失败: {}", path.u8string());
                return ret;
            }
            continue;
        }

        // 处理文件
        zip_fileinfo tmp_zinfo = {};
        int ret = zipOpenNewFileInZip3_64(zipHDL, tmp_relate.c_str(), &tmp_zinfo, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION, 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, nullptr, 0, 1);
        if (ret != ZIP_OK)
        {
            SLOG_ERROR("打开ZIP文件条目失败: {}", path.u8string());
            return ret;
        }

        ret = write_into_zipHDL(zipHDL, path);
        if (ret != ZIP_OK)
        {
            SLOG_ERROR("写入ZIP文件内容失败: {}", path.u8string());
            return ret;
        }
    }

    return ZIP_OK;
}

/// 将文件或目录压缩为ZIP文件
eCompressErr MiniZip::compress(const suPath& s_src, const suPath& s_dst, const bool& append)
{
    auto status = eCompressErr::MiniZUnknowErr;
    try
    {
        if (!s_src.exists())
        {
            return eCompressErr::FileNotExistErr;
        }
        std::string out_dst = s_dst;
        if (out_dst.empty())  // 补充默认压缩路径
        {
            auto sfp_src = suPath(s_src);
            out_dst = sfp_src.parent().append(sfp_src.stem().append(SILLY_MINIZ_FILE_EXTENSION)).string();
        }
        if (!append)  // 非追加,先删除原文件
        {
            std::filesystem::remove(out_dst);
        }
        zipFile zipHDL = zipOpen64(out_dst.c_str(), APPEND_STATUS_CREATE);
        if (!zipHDL)
        {
            return eCompressErr::MiniZCreatZipErr;  //  创建写入的zip失败
        }

        do
        {
            if (suPath::is_dir(s_src))  // 压缩文件夹
            {
                if (ZIP_OK != minizip_compress_dir(zipHDL, s_src))
                {
                    status = eCompressErr::MiniZOpenFileErr;
                    break;
                }
            }
            else  // 压缩单个文件
            {
                if (ZIP_OK != minizip_compress_file(zipHDL, s_src))
                {
                    status = eCompressErr::MiniZOpenFileErr;
                    break;
                }
            }
            status = eCompressErr::Ok;
        } while (0);
        zipClose(zipHDL, nullptr);
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
eCompressErr MiniZip::decompress(const suPath& s_src, const suPath& s_dst)
{
    if (!s_src.exists())  // 解压文件不存在
    {
        return eCompressErr::FileNotExistErr;
    }
    suPath outputDir;
    if (s_dst.is_file())  // 如果解压路径为空,创建一个和压缩包名称相同的目录,解压到该目录下
    {
        outputDir = suPath(s_src).parent();
    }
    else
    {
        outputDir = s_dst;
    }
    if (!suPath::exists(outputDir))
    {
        if (!suPath::mkdir(outputDir))
        {
            return eCompressErr::MiniZCreatDirErr;  // 创建目录失败
        }
    }
    unzFile zipFile = unzOpen64(s_src.string().c_str());  // 打开ZIP文件
    if (zipFile == nullptr)
    {
        return eCompressErr::MiniZOpenFileErr;  // 打开ZIP文件失败
    }
    // 进入ZIP文件中的第一个文件
    if (unzGoToFirstFile(zipFile) != UNZ_OK)
    {
        unzClose(zipFile);
        return eCompressErr::MiniZFileEmptyErr;  // ZIP文件为空
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
            return eCompressErr::MiniZGetInforErr;  // 获取文件信息失败
        }
        suPath one_file = outputDir;
        if (filename == nullptr)
        {
            SU_MEM_FREE(filename);
            continue;
        }
        one_file.append(filename);
        if (filename[nameLen - 1] == '/' || filename[nameLen - 1] == '\\')  // 压缩分支为目录
        {
            if (!suPath::exists(one_file))
            {
                suPath::mkdir(one_file.string());
                if (!suPath::exists(one_file))
                {
                    SU_MEM_FREE(filename);
                    return eCompressErr::MiniZCreatDirErr;  // 创建目录失败
                }
            }
        }
        else  // 压缩分支为文件
        {
            // 创建父目录
            if (suPath::is_dir(one_file))
            {
                SU_MEM_FREE(filename);
                continue;
            }
            suPath::mkdir(suPath(one_file).parent());
            // 写入文件
            std::ofstream outFile(one_file, std::ios::binary);
            if (!outFile.is_open())
            {
                SU_MEM_FREE(filename);
                return eCompressErr::MiniZCreatDirErr;  // 创建文件失败
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
    return eCompressErr::Ok;
}

eCompressErr MiniZip::compress(const char* c_in_val, const size_t& i_in_len, char** c_out_val, size_t& i_out_len)
{
    if (c_in_val == nullptr || i_in_len == 0)
    {
        SLOG_ERROR("Empty input data.");
        return eCompressErr::InValidInputErr;
    }

    uLong dest_len = compressBound(i_in_len);  // 解压后的数据大小
    if (!*c_out_val)
    {
        *c_out_val = (char*)malloc(dest_len);
    }
    else
    {
        SLOG_ERROR("Clean output and set null.");
        return eCompressErr::InValidOutputErr;
    }
    i_out_len = 0;
    // 压缩
    int err = compress2((Bytef*)*c_out_val, &dest_len, (Bytef*)c_in_val, uLong(i_in_len), Z_DEFAULT_COMPRESSION);
    if (err != Z_OK)
    {
        SU_MEM_FREE(*c_out_val);
        return eCompressErr::MiniZCompressStrErr;
    }

    i_out_len = dest_len;  // 解压后的数据大小
    return eCompressErr::Ok;
}

eCompressErr MiniZip::decompress(const char* c_in_val, const size_t& i_in_len, char** c_out_val, size_t& i_out_len)
{
    // 检查输入参数
    if (c_in_val == nullptr || i_in_len == 0)
    {
        SLOG_ERROR("Empty input data.");
        return eCompressErr::InValidInputErr;
    }
    if (*c_out_val)
    {
        SLOG_ERROR("Clean output and set null.");
        return eCompressErr::InValidOutputErr;
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
        return eCompressErr::MiniZUncompressStrErr;
    }
    i_out_len = dest_len;  // 解压后的数据大小

    return eCompressErr::Ok;
}
