
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <json/json.h>

#include "singleton/silly_singleton.h"

/// <summary>
/// 初始化配置文件的类
/// </summary>
class Config : public silly_singleton<Config>
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