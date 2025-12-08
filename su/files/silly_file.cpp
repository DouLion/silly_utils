//
// Created by dly on 2023/8/3.
//
#include "silly_file.h"

size_t suFile::read(const suPath &fp, std::string &content, const size_t &offset, const size_t &len)
{
    size_t ret_read_size = 0;
    if (fp.exists() && fp.is_dir())
    {
        return false;
    }
    content.clear();

    std::fstream input(fp.path(), std::ios::binary | std::ios::in);

    if (!input.is_open())
    {
        return ret_read_size;
    }
    size_t file_size = fp.size();
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
        return false;
    }
    std::fstream input(fp.path(), std::ios::binary | std::ios::in);
    if (input.is_open())
    {
        std::string line;
        while (std::getline(input, line))
        {
            if (func && func(line))
            {
                lines.push_back(line);
            }
            
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
    if (fp.exists() && fp.is_dir())
    {
        return 0;
    }
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
    if (fp.exists() && fp.is_dir())
    {
        return ret;
    }
    std::fstream output(fp.path(), std::ios::binary | std::ios::out);

    if (!output.is_open())
    {
        return ret;
    }
    for (auto& l : lines)
    {

        output.write(l.c_str(), l.size());
        ret += l.size();
        if (l.back() == '\n')
        {
            output.write("\n", 1);
            ret++;
        }
    }
    return ret;
}
