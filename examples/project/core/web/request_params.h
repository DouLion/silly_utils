//
// Created by dell on 2024/7/17.
//

#ifndef REQUEST_PARAM_H
#define REQUEST_PARAM_H

// web返回值构造
#include <project_marco.h>
#include "network/http/silly_http_server.h"

namespace fupload
{
namespace web
{
namespace param
{

class upload_single final : public silly::http::post_param
{
  public:
    bool parse(const std::string& str) override
    {
        bool status = false;
        // 实现

        // status = true;
        return status;
    }

    Json::Value example() override
    {
        bool status = false;
        // 实现

        status = true;
        return status;
    }
};

}  // namespace param
}  // namespace web
}  // namespace fupload


#endif //REQUEST_PARAM_H
