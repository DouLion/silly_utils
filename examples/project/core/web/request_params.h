//
// Created by dell on 2024/7/17.
//

#ifndef REQUEST_PARAM_H
#define REQUEST_PARAM_H
// web请求参数解析
#include <project_marco.h>
const static std::string GET_REQUEST_ERR = "GET请求仅显示用法, 请使用POST请求数据";
const static std::string GET_REQUEST_PARAM_DEMO = "请求参数示例";
const static std::string GET_REQUEST_RESULT_DEMO = "返回结果示例";
class request_param {

public:
    virtual bool load(const std::string& params);
    virtual Json::Value example();
public:
    std::string err{"ok"};
    std::string param1;
protected:
    Json::Value jv_tmp;
};

class get_lasest_data_param : public request_param {
public:
    bool load(const std::string& params) override;
    Json::Value example() override;
public:
    std::string param2;
};



#endif //REQUEST_PARAM_H
