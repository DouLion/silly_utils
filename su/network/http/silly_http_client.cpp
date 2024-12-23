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
using namespace silly::net::http;
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

struct silly_curl_memory
{
    char* memory{nullptr};
    size_t size{0};
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

static size_t silly_curl_write_memory_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t realsize = size * nmemb;
    struct silly_curl_memory* mem = (struct silly_curl_memory*)userp;

    // 注意这里根据每次被调用获得的数据重新动态分配缓存区的大小
    char* ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL)
    {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
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

client::client()
{
    // 初始化 libcurl
    // curl_global_init(CURL_GLOBAL_DEFAULT);
}

client::client(const enum_http_type& type)
{
    client();
    m_type = type;
}

bool client::get(const std::string& url, std::string& resp)
{
    m_type = enum_http_type::Get;
    return request(url, resp);
}
bool client::post(const std::string& url, std::string& resp)
{
    m_type = enum_http_type::Post;
    return request(url, resp);
}
bool client::request(const std::string& url, std::string& resp)
{
    bool status = false;
    m_err.clear();
    m_err.resize(CURL_ERROR_SIZE);

    CURL* hnd = curl_easy_init();
    struct curl_slist* headers = NULL;
    if (nullptr == hnd)
    {
        m_err = "初始化curl错误";
        return status;
    }
    char err_buffer[CURL_ERROR_SIZE] = {0};
    struct silly_curl_memory chunk;
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

        // 设置请求头
        for (auto [k, v] : m_req_headers)
        {
            headers = curl_slist_append(headers, std::string(k + ": " + v).c_str());
        }
        curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

        if (m_type == enum_http_type::Post)
        {
            // 指定这是一个 POST 请求
            curl_easy_setopt(hnd, CURLOPT_POST, 1L);

            // 设置Post请求数据
            if (!m_body.empty())
            {
                curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, m_body.c_str());
            }
        }

        /* send all data to this function  */
        curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, silly_curl_write_memory_callback);

        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void*)&chunk);

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

        // Set progress callback.
        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L))
        // 设置响应头回调函数
        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_HEADERFUNCTION, silly_curl_resp_header_callback))
        SILLY_CURL_ERR_BREAK(curl_easy_setopt(hnd, CURLOPT_HEADERDATA, &m_resp_headers))

        ////////////////////////////////////
        /// 执行请求, 请求设置在此之前设置完成
        ////////////////////////////////////
        SILLY_CURL_ERR_BREAK(curl_easy_perform(hnd))

        // Get response code.
        int resp_code = 0;
        SILLY_CURL_ERR_BREAK(curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &resp_code))

        if (resp_code != enum_http_code::OK_200)
        {
            memcpy(&m_err[0], err_buffer, CURL_ERROR_SIZE);
            break;
        }
        curl_off_t microseconds;

        // check for total download time
        SILLY_CURL_ERR_BREAK(curl_easy_getinfo(hnd, CURLINFO_TOTAL_TIME_T, &microseconds))
        m_total_seconds = microseconds / 1.e6;
        curl_off_t p_file_len = 0;
        SILLY_CURL_ERR_BREAK(curl_easy_getinfo(hnd, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &p_file_len))
        resp.resize(chunk.size);
        memcpy(&resp[0], chunk.memory, chunk.size);

        status = true;
        break;
    } while (0);
    curl_easy_cleanup(hnd);
    curl_slist_free_all(headers);
    return status;
}

bool client::download(const std::string& url, const std::string& file, const std::string& filename)
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

        if (resp_code != enum_http_code::OK_200)
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
bool client::upload(const std::string& url, const std::string& file, const std::string& filename)
{
    bool status = false;
    return status;
}
void client::body(const std::string& body)
{
    m_body = body;
}
std::string client::err() const
{
    return m_err;
}

void client::type(const enum_http_type& type)
{
    m_type = type;
}
void client::header(const std::string& key, const std::string& val)
{
    m_req_headers.insert({key, val});
}
void client::headers(const std::unordered_map<std::string, std::string>& headers)
{
    m_req_headers = headers;
}
std::string client::header(const std::string& key) const
{
    auto it = m_resp_headers.find(key);
    if (it == m_resp_headers.end())
        return "";
    return it->second;
}
void client::headers(std::unordered_map<std::string, std::string>& h) const
{
    h = m_resp_headers;
}
enum_http_code client::code()
{
    return m_code;
}
void client::agent(const std::string& agent)
{
    m_agent = agent;
}
void client::timeout(const int64_t& seconds)
{
    m_timeout = seconds;
}
double client::speed_mps() const
{
    return m_speed_mps;
}
double client::total_seconds() const
{
    return m_total_seconds;
}
void client::verbose(const bool& vb)
{
    m_verbose = vb;
}
void client::max_recv_speed(const size_t& mrs)
{
    m_max_recv_speed = mrs;
}
