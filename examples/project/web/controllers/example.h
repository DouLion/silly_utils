#pragma once

#include <drogon/HttpController.h>
#include <drogon/drogon.h>

using namespace drogon;
using namespace std;

namespace project_name // 服务或者项目名称
{
namespace api  // 指明为API接口
{
class example : public drogon::HttpController<example>
{
public:
    METHOD_LIST_BEGIN
    // url路径尽量使用RUSTFUL风格   /资源名称/动作/限制副词   /宾语/谓语/形容副词

    // 完整url  ip:port/project_name/api/data/fetch/latest
    METHOD_ADD(example::get_data, "/data/fetch/latest",  Get, Post, Options);
    // 完整url  ip:port/project_name/api/data/set/by_id
    METHOD_ADD(example::set_data_by_id, "/data/set/by_id",  Get, Post, Options);

    METHOD_LIST_END


   void get_data(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
    void set_data_by_id(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
};
} // namespace controllers
} // namespace web


