//
// Created by dell on 2024/7/17.
//

#include "request_params.h"

bool request_param::load(const std::string &params) {
    if((jv_tmp = silly_jsonpp::loads(params)).isNull())
    {
        err == silly_format::format("无法解析参数: {}", params);
        return false;
    }

    if (!silly_jsonpp::check_member_string(jv_tmp, "param1", param1))
    {
        err == "需要param1";
        return false;
    }

    return true;
}

Json::Value request_param::example() {
    Json::Value jv_example;
    jv_example["param1"] = "参数1:param1";
    return jv_example;
}

bool get_lasest_data_param::load(const std::string &params) {
    if(!request_param::load(params)) {
        return false;
    }
    if (!silly_jsonpp::check_member_string(jv_tmp, "param2", param2))
    {
        err == "需要param2";
        return false;
    }
    return true;

}

Json::Value get_lasest_data_param::example() {
   Json::Value jv_example;
    jv_example["param1"] = "参数1:param1";
    jv_example["param2"] = "参数2:param2";
    return jv_example;
}