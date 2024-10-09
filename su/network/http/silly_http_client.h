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

enum silly_resp_code
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

class silly_http_client
{
  public:
    enum class req_type
    {
        Get = 1,
        Post = 2
    };
  public:
    silly_http_client();
    ~silly_http_client() = default;
    silly_http_client(const req_type& type);


    bool get(const std::string& url, std::string& resp);
    bool post(const std::string& url, std::string& resp);

    bool request(const std::string& url, std::string& resp);

    /// 下载文件,更改下载文件名
    bool download(const std::string& url, const std::string& file, const std::string& filename="");
    /// 上传文件
    bool upload(const std::string& url, const std::string& file, const std::string& filename="");

    void body(const std::string& body);

    std::string err() const;

    void req_header(const std::string& key, const std::string& val);
    void req_headers(const std::unordered_map<std::string, std::string>& headers);
    std::unordered_map<std::string, std::string> req_headers() const;
    std::unordered_map<std::string, std::string> resp_headers() const;
    silly_resp_code resp_code();


    void type(const req_type& type);

    void agent(const std::string& agent);

    void timeout(const int64_t& seconds);

    double speed_mps() const;
    double total_seconds() const;

    void verbose(const bool& vb);

    /// 设置最大下载速度
    void max_recv_speed(const size_t& kb_per_sec);


  private:
    std::string m_body;
    std::string m_err;
    std::unordered_map<std::string, std::string> m_req_headers{};
    std::unordered_map<std::string, std::string> m_resp_headers{};
    silly_resp_code m_resp_code;
    std::string m_agent;
    req_type m_type{req_type::Get};
    int64_t m_timeout{120L};
    double m_speed_mps{0};// 传输速度Mb/s
    double m_total_seconds{0};
    bool m_verbose{0};
    size_t m_max_recv_speed{0}; // kb/s   0为不做设置

};

#endif  // SILLY_UTILS_SILLY_HTTP_CLIENT_H
