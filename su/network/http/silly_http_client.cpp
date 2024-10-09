/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_http_client.cpp
 * @description: silly_http_client实现
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#include "silly_http_client.h"
#include <curl/curl.h>
#include <files/silly_file.h>

#define SILLY_CURL_ERR_BREAK(v)                         \
    if (CURLE_OK != v)                                  \
    {                                                   \
        memcpy(&m_err[0], err_buffer, CURL_ERROR_SIZE); \
        break;                                          \
    }

struct silly_curl_progress
{
    curl_off_t last_runtime; /* type depends on version, see above */
    CURL* hnd;
};

static int silly_curl_xferinfo_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    if (dltotal > 0)
    {
        struct silly_curl_progress* p = (struct silly_curl_progress*)clientp;
        curl_off_t byte_per_second;
        curl_easy_getinfo(p->hnd, CURLINFO_SPEED_DOWNLOAD_T, &byte_per_second);
        double fractionDownloaded = (double)dlnow / (double)dltotal;
        // int percent = (int)(fractionDownloaded * 100);
        printf("下载进度: %.2f%% 下载速度: %.2f%M/s\r", fractionDownloaded * 100, byte_per_second / 1e6);  // \r 返回行首，但不换行
        fflush(stdout);                                                                                    // 刷新输出缓冲区，确保立即打印
    }
    return 0;  // 返回非0值会中断传输
}

static size_t silly_curl_write_file_callback(void* ptr, size_t size, size_t nmemb, void* stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
    return written;
}

static size_t silly_curl_resp_header_callback(char* buffer, size_t size, size_t nitems, void* userdata)
{
    size_t total_size = size * nitems;
    std::unordered_map<std::string, std::string>* headers = static_cast<std::unordered_map<std::string, std::string>*>(userdata);

    // 解析响应头
    std::string header(buffer, total_size);
    size_t colon_pos = header.find(':');
    if (colon_pos != std::string::npos)
    {
        std::string key = header.substr(0, colon_pos);
        std::string value = header.substr(colon_pos + 1);
        // 去除前后的空白字符
        key = std::string(key.begin(), key.end());
        value = std::string(value.begin(), value.end());
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        (*headers)[key] = value;
    }

    return total_size;
}

silly_http_client::silly_http_client()
{
    // 初始化 libcurl
    // curl_global_init(CURL_GLOBAL_DEFAULT);
}

silly_http_client::silly_http_client(const silly_http_client::req_type& type)
{
    silly_http_client();
    m_type = type;
}

bool silly_http_client::get(const std::string& url, std::string& resp)
{
    bool status = false;
    return status;
}
bool silly_http_client::post(const std::string& url, std::string& resp)
{
    bool status = false;
    return status;
}
bool silly_http_client::request(const std::string& url, std::string& resp)
{
    bool status = false;
    return status;
}

bool silly_http_client::download(const std::string& url, const std::string& file, const std::string& filename)
{
    bool status = false;
    m_err.clear();
    m_err.resize(CURL_ERROR_SIZE);
    FILE* out = fopen(file.c_str(), "wb");
    if (nullptr == out)
    {
        m_err = "无法打开写入文件:";
        m_err.append(file);
        return status;
    }

    CURL* hnd = curl_easy_init();
    if (nullptr == hnd)
    {
        m_err = "初始化curl错误";
        return status;
    }
    char err_buffer[CURL_ERROR_SIZE] = {0};
    do
    {
        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_URL, url.c_str()))
        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_ERRORBUFFER, err_buffer))
        if (m_verbose)
        {
            /* Switch on full protocol/debug output while testing */
            SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1L))
        }

        if (!m_agent.empty())
        {
            SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_USERAGENT, m_agent.c_str()))
        }

        // Set write file data callback function.
        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, silly_curl_write_file_callback))
        // Set write file data callback function param.
        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_WRITEDATA, out))
        // 设置重定向的最大次数.
        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 20L))
        // 设置301、302跳转跟随location.
        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_FOLLOWLOCATION, 1L))

        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L))
        // 验证服务器端发送的证书，默认是 2(高)，1（中），0（禁用）.
        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, 0L))
        // 设置cURL允许执行的最长秒数.
        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_TIMEOUT, m_timeout))
        //

        // SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_PROGRESSFUNCTION, silly_curl_progress_callback);
        struct silly_curl_progress prog;
        prog.last_runtime = 0;
        prog.hnd = hnd;
#if LIBCURL_VERSION_NUM >= 0x072000
        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_XFERINFOFUNCTION, silly_curl_xferinfo_callback))  // 新版本推荐使用 XFERINFOFUNCTION
#else
        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_PROGRESSFUNCTION, silly_curl_xferinfo_callback))
#endif

        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_XFERINFODATA, &prog))
        // Set progress callback.
        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 0L))
        // 设置响应头回调函数
        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_HEADERFUNCTION, silly_curl_resp_header_callback))
        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_HEADERDATA, &m_resp_headers))

        if (m_max_recv_speed > 0)
        {
            curl_off_t mrs = m_max_recv_speed * 1024;  // 10 KB/s
            SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_MAX_RECV_SPEED_LARGE, mrs))
        }

        ////////////////////////////////////
        /// 执行请求, 请求设置在此之前设置完成
        ////////////////////////////////////
        SILLY_CURL_ERR_BREAK(curl_easy_perform(hnd))

        // Get response code.
        int resp_code = 0;
        SILLY_CURL_ERR_BREAK(curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &resp_code))

        if (resp_code != silly_resp_code::OK_200)
        {
            memcpy(&m_err[0], err_buffer, CURL_ERROR_SIZE);
            break;
        }
        curl_off_t byte_per_second;
        curl_off_t microseconds;

        // check for total download time
        SILLY_CURL_ERR_BREAK(curl_easy_getinfo(hnd, CURLINFO_TOTAL_TIME_T, &microseconds))
        SILLY_CURL_ERR_BREAK(curl_easy_getinfo(hnd, CURLINFO_SPEED_DOWNLOAD_T, &byte_per_second))
        m_total_seconds = microseconds / 1.e6;
        m_speed_mps = byte_per_second / 1.e6;
        curl_off_t p_file_len = 0;
        SILLY_CURL_ERR_BREAK(curl_easy_getinfo(hnd, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &p_file_len))
        fflush(out);
        fclose(out);
        out = nullptr;
        /*  size_t c_file_len = silly_file::size(file);
          if (c_file_len != static_cast<size_t>(p_file_len))
          {
              m_err = "文件大小不一致: 响应长度: " + std::to_string(p_file_len) + " 实际长度: " + std::to_string(c_file_len);
              break;
          }
  */
        status = true;
        break;
    } while (0);

    if (out)
        fclose(out);
    curl_easy_cleanup(hnd);
    return status;
}
bool silly_http_client::upload(const std::string& url, const std::string& file, const std::string& filename)
{
    bool status = false;
    return status;
}
void silly_http_client::body(const std::string& body)
{
    m_body = body;
}
std::string silly_http_client::err() const
{
    return m_err;
}

void silly_http_client::type(const silly_http_client::req_type& type)
{
    m_type = type;
}
void silly_http_client::req_header(const std::string& key, const std::string& val)
{
    m_req_headers.insert({key, val});
}
void silly_http_client::req_headers(const std::unordered_map<std::string, std::string>& headers)
{
    m_req_headers = headers;
}
std::unordered_map<std::string, std::string> silly_http_client::req_headers() const
{
    return m_req_headers;
}
std::unordered_map<std::string, std::string> silly_http_client::resp_headers() const
{
    return m_resp_headers;
}
silly_resp_code silly_http_client::resp_code()
{
    return m_resp_code;
}
void silly_http_client::agent(const std::string& agent)
{
    m_agent = agent;
}
void silly_http_client::timeout(const int64_t& seconds)
{
    m_timeout = seconds;
}
double silly_http_client::speed_mps() const
{
    return m_speed_mps;
}
double silly_http_client::total_seconds() const
{
    return m_total_seconds;
}
void silly_http_client::verbose(const bool& vb)
{
    m_verbose = vb;
}
void silly_http_client::max_recv_speed(const size_t& mrs)
{
    m_max_recv_speed = mrs;
}
