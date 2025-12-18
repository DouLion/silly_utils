//
// Created by dly on 2023/8/3.
//
#include "silly_file.h"

int suFile::open(const suPath &file, const int &flags, const int &mode)
{
    int fd = -1;
#if _WIN32
    const errno_t err = ::_sopen_s(&fd, file.string().c_str(), flags | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);

    if (err != 0) {
        // 详细错误处理
        switch (err) {
            case EINVAL:
                std::cerr << "错误: 无效参数 - 文件: " << file.string()
                         << ", 标志: 0x" << std::hex << flags
                         << ", 模式: 0x" << mode << std::dec << std::endl;
                break;
            case EACCES:
                std::cerr << "错误: 访问被拒绝 - 文件: " << file.string()
                         << " (权限不足或文件被锁定)" << std::endl;
                break;
            case ENOENT:
                std::cerr << "错误: 文件不存在 - " << file.string() << std::endl;
                break;
            case EEXIST:
                std::cerr << "错误: 文件已存在 - " << file.string() << std::endl;
                break;
            case EMFILE:
                std::cerr << "错误: 打开文件太多 - " << file.string() << std::endl;
                break;
            case ENODEV:
                std::cerr << "错误: 设备不存在 - " << file.string() << std::endl;
                break;
            case ENOMEM:
                std::cerr << "错误: 内存不足 - " << file.string() << std::endl;
                break;
            default:
                std::cerr << "错误: _sopen_s 失败，错误码: " << err
                         << " - 文件: " << file.string() << std::endl;
                break;
        }
        return -1;
    }

    // 验证文件句柄有效性
    if (fd == -1) {
        std::cerr << "错误: _sopen_s 返回无效文件句柄 - " << file.string() << std::endl;
        return -1;
    }
#else
    if (mode == 0)
    {
        fd = ::open(file.string().c_str(), flags);
    }
    else
    {
        fd = ::open(file.string().c_str(), flags, mode);
    }
    if (fd == -1) {
        // 使用 errno 获取具体错误原因
        switch (errno) {
            case ENOENT:
                std::cerr << "文件不存在: " << file.string() << std::endl;
                break;
            case EACCES:
                std::cerr << "权限不足: " << file.string() << std::endl;
                break;
            case EEXIST:
                std::cerr << "文件已存在: " << file.string() << std::endl;
                break;
            default:
                perror("open failed");
                break;
        }
    }

#endif
    return fd;
}
int suFile::close(const int &fd)
{
    if (-1 == fd)
    {
#if _WIN32
        return ::_close(fd);
#else
        return ::close(fd);
#endif
    }
    return -1;
}
size_t suFile::read(const suPath &fp, std::string &content, const size_t &offset, const size_t &len)
{
    size_t ret_read_size = 0;
    if (fp.exists() && fp.is_dir())
    {
        std::cerr << fp.u8string() << " 是目录,无法读取" << std::endl;
        return false;
    }
    content.clear();

    std::fstream input(fp.path(), std::ios::binary | std::ios::in);

    if (!input.is_open())
    {
        return ret_read_size;
    }
    size_t file_size = fp.file_size();
    if (offset >= file_size)  // 保证读值不为空
    {
        input.close();
        return ret_read_size;
    }
    input.seekg(0, std::ios::beg);
    ret_read_size = SU_MIN(len, file_size - offset);
    content.resize(ret_read_size);

    input.seekg(offset, std::ios::beg);
    input.read(&content[0], ret_read_size);
    input.close();
    return ret_read_size;
}
std::string suFile::read(const suPath &fp, const size_t &offset, const size_t &len)
{
    std::string ret;
    suFile::read(fp, ret, offset, len);
    return ret;
}
bool suFile::readlines(const suPath& fp, std::vector<std::string>& lines, const suFile::filter_func& func)
{
    if (fp.exists() && fp.is_dir())
    {
        std::cerr << fp.u8string() << " 是目录,无法读取" << std::endl;
        return false;
    }
    std::fstream input(fp.path(), std::ios::binary | std::ios::in);
    if (input.is_open())
    {
        std::string line;
        while (std::getline(input, line))
        {
            if (func)
            {
                if (!func(line))
                    continue;
            }
            lines.push_back(line);
            
        }
    }
    else
    {
        return false;
    }
    input.close();
    return true;

}
std::vector<std::string> suFile::readlines(const suPath &fp, const suFile::filter_func& func)
{
    std::vector<std::string> ret;
    readlines(fp, ret, func);
    return ret;
}
size_t suFile::write(const suPath &fp, const std::string &content)
{
    size_t ret = 0;
    if (fp.exists() && !fp.is_file())
    {
        std::cerr << fp.u8string() << " 是已存在的目录,无法写入" << std::endl;
        return 0;
    }
    suPath::mkdir(fp.parent());
    std::fstream output(fp.path(), std::ios::binary | std::ios::out);

    if (!output.is_open())
    {
        return 0;
    }
    output.write(content.c_str(), content.size());
    return content.size();
}
size_t suFile::write(const suPath &fp, const std::vector<std::string> &lines)
{
    size_t ret = 0;
    if (fp.exists() && !fp.is_file())
    {
        return ret;
    }
    suPath::mkdir(fp.parent());
    std::fstream output(fp.path(), std::ios::binary | std::ios::out);

    if (!output.is_open())
    {
        return ret;
    }
    for (auto& l : lines)
    {

        output.write(l.c_str(), l.size());
        ret += l.size();
        if (l.back() != '\n')
        {
            output.write("\n", 1);
            ret++;
        }
    }
    return ret;
}
size_t suFile::append(const suPath &fp, const std::string &content)
{
    if (fp.exists())
    {
        if (!fp.is_file())
        {
            return 0;
        }
    }
    else
    {
        if (!fp.parent().mkdir())
        {
            return 0;
        }
    }
    // std::ios::app 会自动定位到文件末尾
    std::fstream output(fp.path(), std::ios::binary | std::ios::out | std::ios::app/*| std::ios::ate*/);
    if (!output.is_open())
    {
        return 0;
    }
    output.write(content.c_str(), content.size());
    if (output.fail())
    {
        return 0;
    }
    return content.size();

}
size_t suFile::append(const suPath &fp, const std::vector<std::string> &lines)
{
    if (fp.exists())
    {
        if (!fp.is_file())
        {
            return 0;
        }
    }
    else
    {
        if (!fp.parent().mkdir())
        {
            return 0;
        }
    }

    std::fstream output(fp.path(), std::ios::binary | std::ios::out | std::ios::app/*| std::ios::ate*/);
    if (!output.is_open())
    {
        return 0;
    }
    size_t ret = 0;
    for (const auto& l : lines)
    {
        output.write(l.c_str(), l.size());
        ret+= l.size();
    }
    if (output.fail())
    {
        return 0;
    }
    return ret;

}
