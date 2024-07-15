
#include "example_tool/project.h"
#include "example_tool/config.hpp"

#include <filesystem>
#include "singleton/silly_singleton.h"

/// <summary>
/// 初始化配置文件的类
/// </summary>
class TxtToTifConfig : public silly_singleton<TxtToTifConfig>
{
  public:
    std::string input_txt_dir;
    std::string output_tif_dir;

    bool read_config(std::string config_path)
    {
        bool status = false;
        std::fstream input;
        input.open(config_path, std::ios::binary | std::ios::in);
        if (input.is_open())
        {
            Json::Reader reader;
            Json::Value root;

            if (reader.parse(input, root))
            {
                input_txt_dir = root["input_txt_dir"].asString();
                output_tif_dir = root["output_tif_dir"].asString();
            }
            status = true;
        }
        else
        {
            std::cout << "error fail open:" << config_path << std::endl;
        }
        input.close();
        return status;
    }
};

int main()
{
#ifndef NDEBUG
    std::string configPath = "../../../../config/config.json";
#else
    std::string configPath = "./config/config.json";
#endif
    if (!TxtToTifConfig::instance().read_config(configPath))
    {
        std::cout << "error read config: " << configPath << std::endl;
        std::cout << "Please check if the configuration file exists or if the path name in the configuration file has been changed to/" << std::endl;
        return -1;
    }

    return 0;
}
