#pragma once

#include <drogon/HttpController.h>
#include <drogon/drogon.h>

using namespace drogon;
using namespace std;

class check_ctrl : public drogon::HttpController<check_ctrl>
{
  public:
    METHOD_LIST_BEGIN

    // URL: http://0.0.0.0:8080/check_ctrl/check?station=HNPROJ1&work=2023-11-10
    METHOD_ADD(check_ctrl::check, "/check?station={1}&work={2}", Post, Get);

    METHOD_LIST_END
    

   void check(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string&& station, std::string&& work);
};
