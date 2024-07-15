

#include "config_tool/config_tool.h"

int main()
{
    std::string core_cfg_name = "core_config.json";
#ifndef NDEBUG
    std::filesystem::path core_config(DEFAULT_CONFIG_DIR);
#else
    std::filesystem::path core_config("./config");
#endif
    core_config.append(core_cfg_name);


    if (!Config::instance().read_config(core_config.string()))
    {
        std::cout << "error read config: " << core_config.string() << std::endl;
        return -1;
    }

    std::cout << Config::instance().input_txt_dir << std::endl;
    std::cout << Config::instance().output_tif_dir << std::endl;

    return 0;
}
