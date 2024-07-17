//
// Created by dell on 2024/7/17.
//

#ifndef RESPONSE_RESULT_H
#define RESPONSE_RESULT_H

// web返回值构造
#include <project_marco.h>

class response_result {
public:
    virtual Json::Value dump() = 0;

    virtual Json::Value example() = 0;

public:
    std::string err{"ok"};
    int status{1};
};


class get_latest_data_resp_result : public response_result
{
public:
    Json::Value dump() override;
    Json::Value example() override;

public:
    double left;
    double top;
    double right;
    double bottom;
};




#endif //RESPONSE_RESULT_H
