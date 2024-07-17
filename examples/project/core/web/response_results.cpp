//
// Created by dell on 2024/7/17.
//

#include "response_results.h"

Json::Value get_latest_data_resp_result::dump() {
     Json::Value jv_data;
    jv_data["rect"] = Json::objectValue;
    jv_data["rect"]["left"] = left;
    jv_data["rect"]["top"] = top;
    jv_data["rect"]["right"] = right;
    jv_data["rect"]["bottom"] = bottom;

    return jv_data;
}

Json::Value get_latest_data_resp_result::example() {

    Json::Value jv_data;
    jv_data["rect"] = Json::objectValue;
    jv_data["rect"]["left"] = "网格数据的left";
    jv_data["rect"]["top"] = "网格数据的top";
    jv_data["rect"]["right"] = "网格数据的right";
    jv_data["rect"]["bottom"] = "网格数据的bottom";
    return jv_data;
}
