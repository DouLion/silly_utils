/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/10
 * @version: 2.0.2 (最终修复版)
 * @software: silly_utils
 * @description: 7Z 压缩/解压缩工具实现 (基于 libarchive)
 */

#include "silly_7z.h"
#if SU_THIRD_SUPPORT_LIBARCHIVE
#include <archive.h>
#include <archive_entry.h>
#endif
#include <log/silly_log.h>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cstring>
#include <ctime>

#define SEVENZ_BLOCK_SIZE (1024 * 1024)

std::string silly_7z::getVersionInfo() {
#if SU_THIRD_SUPPORT_LIBARCHIVE
    return archive_version_string();
#else
    return "libarchive not supported";
#endif
}

eCompressErr silly_7z::compressFile(const suPath& srcPath, const suPath& dstPath, bool append) {
#if !SU_THIRD_SUPPORT_LIBARCHIVE
    return eCompressErr::Z7zDecompressErr;
#endif

    if (!srcPath.exists()) {
        return eCompressErr::FileNotExistErr;
    }

    suPath outPath = dstPath;
    if (dstPath.string().empty()) {
        outPath = srcPath.parent().append(srcPath.stem().append(".7z"));
    }

    if (!append && outPath.exists()) {
        std::filesystem::remove(outPath.string());
    }

    try {
        struct archive* a = archive_write_new();
        if (!a) return eCompressErr::Z7zDecompressErr;

        int r = archive_write_set_format_by_name(a, "7zip");
        if (r != ARCHIVE_OK) {
            archive_write_free(a);
            return eCompressErr::Z7zDecompressErr;
        }

        archive_write_add_filter_lzma(a);
        archive_write_open_filename(a, outPath.string().c_str());

        std::function<void(const suPath&, const suPath&)> addEntry = [&](const suPath& path, const suPath& base) {
            std::string rel_path = path.relative(base).string();
            
            if (path.is_file()) {
                std::ifstream ifs(path.string(), std::ios::binary);
                if (!ifs) return;

                ifs.seekg(0, std::ios::end);
                std::streamsize size = ifs.tellg();
                ifs.seekg(0, std::ios::beg);

                struct archive_entry* entry = archive_entry_new();
                archive_entry_set_pathname(entry, rel_path.c_str());
                archive_entry_set_size(entry, size);
                archive_entry_set_filetype(entry, AE_IFREG);
                archive_entry_set_perm(entry, 0644);
                archive_entry_set_mtime(entry, time(nullptr), 0);

                archive_write_header(a, entry);

                std::vector<char> buffer(SEVENZ_BLOCK_SIZE);
                std::streamsize remaining = size;
                while (remaining > 0) {
                    std::streamsize toRead = std::min(static_cast<std::streamsize>(SEVENZ_BLOCK_SIZE), remaining);
                    ifs.read(buffer.data(), toRead);
                    std::streamsize bytesRead = ifs.gcount();
                    archive_write_data(a, buffer.data(), bytesRead);
                    remaining -= bytesRead;
                }

                archive_entry_free(entry);
                ifs.close();
            } else if (path.is_dir()) {
                struct archive_entry* entry = archive_entry_new();
                archive_entry_set_pathname(entry, rel_path.c_str());
                archive_entry_set_filetype(entry, AE_IFDIR);
                archive_entry_set_perm(entry, 0755);
                archive_write_header(a, entry);
                archive_entry_free(entry);

                for (const auto& entry : std::filesystem::directory_iterator(path.string())) {
                    addEntry(suPath(entry.path()), base);
                }
            }
        };

        addEntry(srcPath, srcPath.is_file() ? srcPath.parent() : srcPath);

        archive_write_close(a);
        archive_write_free(a);

        return eCompressErr::Ok;
    } catch (const std::exception& e) {
        SLOG_ERROR("7Z 压缩异常：{}", e.what());
        return eCompressErr::Z7zDecompressErr;
    }
}

eCompressErr silly_7z::decompressFile(const suPath& srcPath, const suPath& dstPath) {
#if !SU_THIRD_SUPPORT_LIBARCHIVE
    return eCompressErr::Z7zDecompressErr;
#endif

    if (!srcPath.exists()) {
        return eCompressErr::FileNotExistErr;
    }

    suPath outputDir = dstPath;
    if (dstPath.string().empty() || (!outputDir.is_dir() && !dstPath.string().empty())) {
        outputDir = srcPath.parent();
    }

    if (!outputDir.exists()) {
        try {
            std::filesystem::create_directories(outputDir.string());
        } catch (...) {
            return eCompressErr::MiniZCreatDirErr;  // 复用错误码
        }
    }

    struct archive* archive_ptr = archive_read_new();
    if (!archive_ptr) return eCompressErr::Z7zDecompressErr;

    archive_read_support_format_all(archive_ptr);
    archive_read_support_filter_all(archive_ptr);

    if (archive_read_open_filename(archive_ptr, srcPath.string().c_str(), 10240) != ARCHIVE_OK) {
        archive_read_free(archive_ptr);
        return eCompressErr::Z7zDecompressErr;
    }

    struct archive_entry* entry = nullptr;
    int r;

    while ((r = archive_read_next_header(archive_ptr, &entry)) == ARCHIVE_OK) {
        const char* path_utf8 = archive_entry_pathname(entry);
        if (!path_utf8) {
            archive_read_data_skip(archive_ptr);
            continue;
        }

        std::string entry_name = path_utf8;
        suPath dest_path = outputDir.append(entry_name);

        suPath parent = dest_path.parent();
        if (!parent.exists() && parent.string().length() > 0) {
            try {
                std::filesystem::create_directories(parent.string());
            } catch (...) {
                archive_read_free(archive_ptr);
                return eCompressErr::MiniZCreatDirErr;
            }
        }

        if (archive_entry_filetype(entry) == AE_IFDIR) {
            if (!dest_path.exists()) {
                try {
                    std::filesystem::create_directories(dest_path.string());
                } catch (...) {}
            }
        } else if (archive_entry_filetype(entry) == AE_IFREG || archive_entry_filetype(entry) == AE_IFLNK) {
            std::ofstream out_file(dest_path.string(), std::ios::binary | std::ios::trunc);
            if (!out_file) {
                archive_read_free(archive_ptr);
                return eCompressErr::MiniZUncompressStrErr;
            }

            const void* buff = nullptr;
            size_t size = 0;
            int64_t offset = 0;

            while ((r = archive_read_data_block(archive_ptr, &buff, &size, &offset)) == ARCHIVE_OK) {
                if (size == 0) break;
                out_file.write(static_cast<const char*>(buff), size);
            }

            out_file.close();

            if (r != ARCHIVE_EOF && r != ARCHIVE_OK) {
                archive_read_free(archive_ptr);
                return eCompressErr::Z7zDecompressErr;
            }
        }

        if (archive_entry_filetype(entry) == AE_IFLNK) {
            const char* linkname = archive_entry_symlink(entry);
            if (linkname) {
                try {
                    std::filesystem::create_symlink(linkname, dest_path.string());
                } catch (...) {}
            }
        }

        archive_entry_free(entry);
        entry = nullptr;
    }

    if (r != ARCHIVE_EOF) {
        archive_read_free(archive_ptr);
        return eCompressErr::Z7zDecompressErr;
    }

    archive_read_free(archive_ptr);
    return eCompressErr::Ok;
}

eCompressErr silly_7z::compressData(const char* inData, size_t inLen, char** outData, size_t& outLen) {
    (void)inData; (void)inLen; (void)outData; (void)outLen;
    return eCompressErr::NotImplement;
}

eCompressErr silly_7z::decompressData(const char* inData, size_t inLen, char** outData, size_t& outLen) {
#if !SU_THIRD_SUPPORT_LIBARCHIVE
    return eCompressErr::Z7zDecompressErr;
#endif

    if (!inData || inLen == 0 || !outData) {
        return eCompressErr::InValidInputErr;
    }

    if (*outData) {
        delete[] *outData;
        *outData = nullptr;
    }
    outLen = 0;

    try {
        size_t estimate_size = inLen * 4;
        std::vector<char> temp_buffer(estimate_size);

        struct archive* archive_ptr = archive_read_new();
        if (!archive_ptr) return eCompressErr::Z7zDecompressErr;

        archive_read_support_format_all(archive_ptr);
        archive_read_support_filter_all(archive_ptr);

        int r = archive_read_open_memory(archive_ptr, inData, inLen);
        if (r != ARCHIVE_OK) {
            archive_read_free(archive_ptr);
            return eCompressErr::Z7zDecompressErr;
        }

        struct archive_entry* entry = nullptr;
        r = archive_read_next_header(archive_ptr, &entry);
        if (r != ARCHIVE_OK) {
            archive_read_free(archive_ptr);
            return eCompressErr::Z7zDecompressErr;
        }

        size_t entry_size = static_cast<size_t>(archive_entry_size(entry));
        if (entry_size > 0 && entry_size > estimate_size) {
            temp_buffer.resize(entry_size);
        }

        size_t total_read = 0;
        const void* buff = nullptr;
        size_t size = 0;
        int64_t offset = 0;

        while ((r = archive_read_data_block(archive_ptr, &buff, &size, &offset)) == ARCHIVE_OK) {
            if (size == 0) break;
            if (total_read + size > temp_buffer.size()) {
                temp_buffer.resize(temp_buffer.size() * 2);
            }
            std::memcpy(temp_buffer.data() + total_read, buff, size);
            total_read += size;
        }

        archive_read_free(archive_ptr);

        if (total_read == 0) {
            return eCompressErr::Z7zDecompressErr;
        }

        *outData = new char[total_read];
        std::memcpy(*outData, temp_buffer.data(), total_read);
        outLen = total_read;

        return eCompressErr::Ok;
    } catch (const std::exception& e) {
        SLOG_ERROR("7Z 解压异常：{}", e.what());
        return eCompressErr::Z7zDecompressErr;
    }
}

eCompressErr silly_7z::compressBin(const std::string& inData, std::string& outData) {
    (void)inData; (void)outData;
    return eCompressErr::NotImplement;
}

eCompressErr silly_7z::decompressBin(const std::string& inData, std::string& outData) {
    char* decompressed = nullptr;
    size_t decompressed_len = 0;
    eCompressErr result = decompressData(inData.data(), inData.size(), &decompressed, decompressed_len);

    if (result == eCompressErr::Ok && decompressed) {
        outData.assign(decompressed, decompressed_len);
        delete[] decompressed;
    } else {
        outData.clear();
    }

    return result;
}

eCompressErr silly_7z::compressBin(const std::string& inData, std::vector<char>& outData) {
    (void)inData; (void)outData;
    return eCompressErr::NotImplement;
}

eCompressErr silly_7z::decompressBin(const std::vector<char>& inData, std::string& outData) {
    char* decompressed = nullptr;
    size_t decompressed_len = 0;
    eCompressErr result = decompressData(inData.data(), inData.size(), &decompressed, decompressed_len);

    if (result == eCompressErr::Ok && decompressed) {
        outData.assign(decompressed, decompressed_len);
        delete[] decompressed;
    } else {
        outData.clear();
    }

    return result;
}
