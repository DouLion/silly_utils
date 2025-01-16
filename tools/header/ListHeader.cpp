
#include "su_marco.h"
#include "string/silly_format.h"
#include <su.h>

int main()
{
    std::string output = "all_header.txt";
    std::string headerFmt = "#include <{}/{}>\n";

    std::map<std::string, std::vector<std::string>> dir_files;

    // su 目录
    std::filesystem::path suDir = std::filesystem::path(DEFAULT_SU_ROOT_DIR).append("su").string();
    // 遍历 su 目录下的目录, 获取所有头文件
    for (const auto& entry : std::filesystem::directory_iterator(suDir))
    {
        if (entry.is_directory())
        {
            std::string dirName = entry.path().filename().string();
            // 递归的获取目录下的头文件
            for (const auto& file : std::filesystem::recursive_directory_iterator(entry.path()))
            {
                if (file.is_regular_file() && file.path().extension() == ".h")
                {
                    // 获取相对entry 的路径
                    std::string relativePath = std::filesystem::relative(file.path(), entry.path()).string();
                    std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
                    std::string fileName = relativePath;
                    dir_files[dirName].push_back(fileName);
                }
            }
        }
    }

    // 输出所有头文件
    std::ofstream outFile(output);
    for (const auto& [dirName, files] : dir_files)
    {
        std::string tempDir = silly_format::format("// {}\n", dirName);
        outFile << tempDir;
        for (const auto& file : files)
        {
            std::string tempLine = silly_format::format(headerFmt, dirName, file);
            outFile << tempLine;
        }
        outFile << std::endl;
    }
    outFile.close();
    std::cout << "all_header.txt 生成成功" << std::endl;

    return 0;
}