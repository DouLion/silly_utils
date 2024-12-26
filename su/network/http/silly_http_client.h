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

enum enum_http_type
{
    Get = 1,
    Post = 2,
    Option = 3,
    Delete = 4
};
enum enum_http_code
{
    Continue_100 = 100,
    SwitchingProtocol_101 = 101,
    Processing_102 = 102,
    EarlyHints_103 = 103,

    // Successful responses
    OK_200 = 200,
    Created_201 = 201,
    Accepted_202 = 202,
    NonAuthoritativeInformation_203 = 203,
    NoContent_204 = 204,
    ResetContent_205 = 205,
    PartialContent_206 = 206,
    MultiStatus_207 = 207,
    AlreadyReported_208 = 208,
    IMUsed_226 = 226,

    // Redirection messages
    MultipleChoices_300 = 300,
    MovedPermanently_301 = 301,
    Found_302 = 302,
    SeeOther_303 = 303,
    NotModified_304 = 304,
    UseProxy_305 = 305,
    unused_306 = 306,
    TemporaryRedirect_307 = 307,
    PermanentRedirect_308 = 308,

    // Client error responses
    BadRequest_400 = 400,
    Unauthorized_401 = 401,
    PaymentRequired_402 = 402,
    Forbidden_403 = 403,
    NotFound_404 = 404,
    MethodNotAllowed_405 = 405,
    NotAcceptable_406 = 406,
    ProxyAuthenticationRequired_407 = 407,
    RequestTimeout_408 = 408,
    Conflict_409 = 409,
    Gone_410 = 410,
    LengthRequired_411 = 411,
    PreconditionFailed_412 = 412,
    PayloadTooLarge_413 = 413,
    UriTooLong_414 = 414,
    UnsupportedMediaType_415 = 415,
    RangeNotSatisfiable_416 = 416,
    ExpectationFailed_417 = 417,
    ImATeapot_418 = 418,
    MisdirectedRequest_421 = 421,
    UnprocessableContent_422 = 422,
    Locked_423 = 423,
    FailedDependency_424 = 424,
    TooEarly_425 = 425,
    UpgradeRequired_426 = 426,
    PreconditionRequired_428 = 428,
    TooManyRequests_429 = 429,
    RequestHeaderFieldsTooLarge_431 = 431,
    UnavailableForLegalReasons_451 = 451,

    // Server error responses
    InternalServerError_500 = 500,
    NotImplemented_501 = 501,
    BadGateway_502 = 502,
    ServiceUnavailable_503 = 503,
    GatewayTimeout_504 = 504,
    HttpVersionNotSupported_505 = 505,
    VariantAlsoNegotiates_506 = 506,
    InsufficientStorage_507 = 507,
    LoopDetected_508 = 508,
    NotExtended_510 = 510,
    NetworkAuthenticationRequired_511 = 511,
};

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
    client(const enum_http_type& type);

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
    /// 上传
    /// </summary>
    /// <param name="url"></param>
    /// <param name="file"></param>
    /// <param name="filename"></param>
    /// <returns></returns>
    bool upload(const std::string& url, const std::string& file, const std::string& filename = "");

    /// <summary>
    /// 设置请求体
    /// </summary>
    /// <param name="body"></param>
    void body(const std::string& body);

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
    void headers(std::unordered_map<std::string, std::string>& h) const;

    /// <summary>
    /// 错误信息
    /// </summary>
    /// <returns></returns>
    std::string err() const;

    /// <summary>
    /// http错误码
    /// </summary>
    /// <returns></returns>
    enum_http_code code();

    /// <summary>
    /// http请求类型
    /// </summary>
    /// <param name="type"></param>
    void type(const enum_http_type& type);

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
    std::unordered_map<std::string, std::string> m_req_headers;
    std::unordered_map<std::string, std::string> m_resp_headers;
    enum_http_code m_code = enum_http_code::BadRequest_400;
    std::string m_agent;
    enum_http_type m_type{enum_http_type::Get};
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
