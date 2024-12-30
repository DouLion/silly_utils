//
// Created by dell on 2024/7/17.
//

#ifndef RESPONSE_RESULT_H
#define RESPONSE_RESULT_H

#include <project_marco.h>
#include "network/http/silly_http_server.h"

namespace fupload
{
namespace web
{
namespace result
{

class upload_single final : public silly::http::post_result
{
  public:
    Json::Value jsonify() override
    {
        // 实现
        return Json::Value();
    }

    Json::Value example() override
    {
        // 实现
        return Json::Value();
    }
};

}  // namespace result
}  // namespace web
}  // namespace fupload


#endif //RESPONSE_RESULT_H
