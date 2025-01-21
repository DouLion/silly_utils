/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_http_client.h
 * @description: http(https)客户端 类声明
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_HTTP_CLIENT_H
#define SILLY_UTILS_SILLY_HTTP_CLIENT_H
#include <su_marco.h>
#include <network/http/silly_http_headers.h>

namespace silly
{
namespace http
{

class client
{
  public:
  public:
    client();
    ~client() = default;
    client(const silly::http::type& tp);

    /// <summary>
    /// 执行Get请求
    /// </summary>
    /// <param name="url"></param>
    /// <param name="resp"></param>
    /// <returns></returns>
    bool get(const std::string& url, std::string& resp);

    /// <summary>
    /// 执行Post请求
    /// </summary>
    /// <param name="url"></param>
    /// <param name="resp"></param>
    /// <returns></returns>
    bool post(const std::string& url, std::string& resp);

    /// <summary>
    /// 根据m_type执行请求
    /// </summary>
    /// <param name="url"></param>
    /// <param name="resp"></param>
    /// <returns></returns>
    bool request(const std::string& url, std::string& resp);

    /// <summary>
    /// 下载
    /// </summary>
    /// <param name="url"></param>
    /// <param name="file"></param>
    /// <param name="filename"></param>
    /// <returns></returns>
    bool download(const std::string& url, const std::string& file, const std::string& filename = "");


    /// <summary>
    /// 上传文件 Upload files
    /// </summary>
    /// <param name="url"></param>
    /// <param name="copyname">
    ///     一般多文件为 `files[]`, 单文件为 `file`, 根据服务端的要求设置此名称
    ///     Generally, when upload multiple files, use `files[]`, otherwise, use `file`, 
    ///     set the value according to the requirements of the server
    /// </param>
    /// <param name="resp"></param>
    /// <returns></returns>
    bool upload(const std::string& url, const std::string& copyname, std::string& resp);

    /// <summary>
    /// 添加上传文件
    /// </summary>
    /// <param name="name">一般为文件名, 不应该重复, 不能确定服务端如何实现</param>
    /// <param name="path"></param>
    /// <returns></returns>
    bool add_upload(const std::string& name, std::filesystem::path path);

    /// <summary>
    /// 移除上传文件
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    bool remove_upload(const std::string& name);

    /// <summary>
    /// 清空上传文件
    /// </summary>
    /// <returns></returns>
    void clear_upload();

    /// <summary>
    /// 设置请求体
    /// </summary>
    /// <param name="body"></param>
    void body(const std::string& body);

    /// <summary>
    /// 设置用户名密码
    /// </summary>
    /// <param name="user"></param>
    void user(const std::string& user);
    void password(const std::string& pwd);

    /// <summary>
    /// 设置请求头
    /// </summary>
    /// <param name="key"></param>
    /// <param name="val"></param>
    void header(const std::string& key, const std::string& val);
    void headers(const std::unordered_map<std::string, std::string>& headers);

    /// <summary>
    /// 获取响应头
    /// </summary>
    /// <returns></returns>
    std::string header(const std::string& key) const;
    std::unordered_map<std::string, std::string> headers() const;

    /// <summary>
    /// 错误信息
    /// </summary>
    /// <returns></returns>
    std::string err() const;

    /// <summary>
    /// http 状态码
    /// </summary>
    /// <returns></returns>
    silly::http::status_code code();

    /// <summary>
    /// http请求类型
    /// </summary>
    /// <param name="type"></param>
    void type(const silly::http::type& tp);

    /// <summary>
    /// http请求代理
    /// </summary>
    /// <param name="agent"></param>
    void agent(const std::string& agent);

    /// <summary>
    /// 设置超时时间
    /// </summary>
    void timeout(const int64_t& seconds);

    /// <summary>
    /// 获取传输速度 M/S 兆每秒
    /// </summary>
    /// <returns></returns>
    double speed_mps() const;

    /// <summary>
    /// 总时长
    /// </summary>
    /// <returns></returns>
    double total_seconds() const;

    /// <summary>
    /// 是否输出详细日志
    /// </summary>
    /// <param name="vb"></param>
    void verbose(const bool& vb);

    /// <summary>
    /// 设置最大下载速度
    /// </summary>
    /// <param name="kb_per_sec"></param>
    void max_recv_speed(const size_t& kb_per_sec);

  private:
    std::string m_body;
    std::string m_err;
    std::unordered_map<std::string, std::string> m_hrequest;
    std::unordered_map<std::string, std::string> m_hresponse;
    std::unordered_map<std::string, std::filesystem::path> m_files;
    std::string m_copyname;
    std::string m_user;
    std::string m_password;
    silly::http::status_code m_code = silly::http::BadRequest_400;
    std::string m_agent;
    silly::http::type m_type{silly::http::Get};
    int64_t m_timeout{120L};
    double m_speed_mps{0};  // 传输速度Mb/s
    double m_total_seconds{0};
    bool m_verbose{false};
    size_t m_max_recv_speed{0};  // kb/s   0为不做设置

  
};
}  // namespace http
}  // namespace silly

typedef silly::http::client silly_http_client;

#endif  // SILLY_UTILS_SILLY_HTTP_CLIENT_H

/* 用例 Usecase

1. 声明客户端
silly::http::client cli;
cli.verbose(true); // 输出详细日志

2. 设置请求头
cli.header("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.132 Safari/537.36");
cli.header("Accept-Encoding", "gzip, deflate, br");

3. 发起请求
3.1 GET
    std::string resp;
    cli.get("https://su.tzx.com?token=12345", resp)

3.2 普通POST
    std::string param = "name=su&age=18"; 表单提交
    param = R"({"user":"zs"})" json数据
    cli.body(param);
    cli.post("https://su.tzx.com?token=12345", resp)

3.3 文件上传
    cli.add_upload("file1", "path/file1");
    cli.add_upload("file2", "path/file2");
    cli.copyname("files[]");
    cli.upload("https://su.tzx.co?token=12345m", resp);

3.4 文件下载
    cli.download("https://su.tzx.com/file.txt", "path/file.txt");

*/
