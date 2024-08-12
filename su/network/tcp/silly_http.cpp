//
// Created by dly on 2023/7/10.
//

#include "silly_http.h"
#include "curl/curl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <filesystem>
#include <su_marco.h>

size_t req_reply(void* ptr, size_t size, size_t nmemb, void* stream)
{
    //在注释的里面可以打印请求流，cookie的信息
    // cout << "----->reply" << endl;
    std::string* str = (std::string*)stream;
    // cout << *str << endl;
    (*str).append((char*)ptr, size * nmemb);
    return size * nmemb;
}
size_t HandleWriteStrData(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
    std::string* str = dynamic_cast<std::string*>((std::string*)pParam);
    if (NULL == str || NULL == pBuffer)
    {
        return -1;
    }

    char* pData = (char*)pBuffer;
    str->append(pData, nSize * nMemByte);
    return nMemByte;
}
struct MemoryStruct
{
    char* memory{nullptr};
    size_t size{0};
};

static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

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

std::string silly_http::request_get(const std::string& url, const std::map<std::string, std::string>& headers)
{
    std::string ret_content;
    // curl初始化
    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */
    CURL* curl = curl_easy_init();
    // curl返回值
    struct MemoryStruct chunk;
    CURLcode res;
    if (curl)
    {
        /* specify URL to get */
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        /* send all data to this function  */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

        /* some servers do not like requests that are made without a user-agent
           field, so we provide one */
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/127.0.0.0 Safari/537.36");

        /* get it! */
        res = curl_easy_perform(curl);

        /* check for errors */
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        // else {
        //	/*
        //	 * Now, our chunk.memory points to a memory block that is chunk.size
        //	 * bytes big and contains the remote file.
        //	 *
        //	 * Do something nice with it!
        //	 */

        //	printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
        //}
    }
    // 释放curl
    curl_easy_cleanup(curl);

    curl_global_cleanup();

    if (chunk.size && chunk.memory)
    {
        ret_content.resize(chunk.size);
        memcpy(&ret_content[0], chunk.memory, chunk.size);
        free(chunk.memory);
    }
    return ret_content;
}

std::string silly_http::request_post(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers)
{
    std::string ret_content;
    CURLcode retCode;
    CURL* pCurl = curl_easy_init();
    if (NULL == pCurl)
    {
        return ret_content;
    }
    curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str());
    // if (!httpVerifyUserName.empty() && !httpVerifyUserPwd.empty())
    //{
    //	std::string userPwd = httpVerifyUserName + ":" + httpVerifyUserPwd;
    //	CBase64Coder base64Coder;
    //	const char* up = base64Coder.encode(userPwd);

    //	// TODO: 这种方式不知道为什么不行.
    //	//curl_easy_setopt(pCurl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    //	//curl_easy_setopt(pCurl, CURLOPT_USERPWD, up);

    //	struct curl_slist* headers = NULL;
    //	if (authType == CURLAUTH_BASIC)
    //	{
    //		headers = curl_slist_append(headers, ("Authorization: Basic " + string(up)).c_str());
    //	}
    //	curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, headers);
    //}
    curl_easy_setopt(pCurl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, HandleWriteStrData);
    curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)&ret_content);
    /**
     * 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
     * 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
     */
    curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1);
    // 跳过服务器SSL验证，不使用CA证书.
    curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
    // 验证服务器端发送的证书，默认是 2(高)，1（中），0（禁用）.
    curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, 0L);
    // 在发起连接前等待的时间，如果设置为0，则无限等待.
    curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, 0);
    curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 3600);
    retCode = curl_easy_perform(pCurl);
    if (retCode != CURLE_OK)
    {
        curl_easy_cleanup(pCurl);
        return ret_content;
    }

    // Get response code.
    int responseCode = 0;
    retCode = curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &responseCode);
    if (retCode != CURLE_OK || responseCode != 200)
    {
        curl_easy_cleanup(pCurl);
        return ret_content;
    }

    curl_easy_cleanup(pCurl);
    return ret_content;
}

static size_t su_handle_write_data(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
    std::string* str = dynamic_cast<std::string*>((std::string*)pParam);
    if (NULL == str || NULL == pBuffer)
    {
        return -1;
    }

    char* pData = (char*)pBuffer;
    str->append(pData, nSize * nMemByte);
    return nMemByte;
}

static size_t sU_handle_write(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
    FILE* fp = (FILE*)pParam;
    size_t nWrite = fwrite(pBuffer, nSize, nMemByte, fp);
    return nWrite;
}

bool silly_http::request_download(const std::string& url, const std::string& save, const std::map<std::string, std::string>& headers)
{
    bool status = false;
    FILE* pFile = fopen(save.c_str(), "wb");
    if (NULL == pFile)
    {
        return false;
    }

    // Init curl.
    CURL* pCurl = curl_easy_init();
    if (NULL == pCurl)
    {
        return false;
    }
    CURLcode retCode;
    // Set remot url.
    retCode = curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str());
    // if (retCode != CURLE_OK)
    //{
    //	fprintf(stderr, "Failed to set writer [%s]\n", errorBuffer);
    //	return false;
    //}
    // Set User-Agent.

    // Set write file data callback function.
    curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, sU_handle_write);
    // Set write file data callback function param.
    curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, pFile);
    // 设置重定向的最大次数.
    curl_easy_setopt(pCurl, CURLOPT_MAXREDIRS, 20);
    // 设置301、302跳转跟随location.
    curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1);
    // Set progress callback.
    curl_easy_setopt(pCurl, CURLOPT_NOPROGRESS, 0);
    // curl_easy_setopt(pCurl, CURLOPT_PROGRESSFUNCTION, HandleProgress);
    curl_easy_setopt(pCurl, CURLOPT_PROGRESSDATA, save.c_str());
    // 跳过服务器SSL验证，不使用CA证书.
    curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
    // 验证服务器端发送的证书，默认是 2(高)，1（中），0（禁用）.
    curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, 0L);
    // 设置cURL允许执行的最长秒数.
    curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 3600);
    //
    char errorBuffer[CURL_ERROR_SIZE] = {0};
    curl_easy_setopt(pCurl, CURLOPT_ERRORBUFFER, errorBuffer);
    // if (!httpVerifyUserName.empty() && !httpVerifyUserPwd.empty())
    //{
    //	std::string userPwd = httpVerifyUserName + ":" + httpVerifyUserPwd;
    //	CBase64Coder base64Coder;
    //	const char* up = base64Coder.encode(userPwd);

    //	// TODO: 这种方式不知道为什么不行.
    //	//curl_easy_setopt(pCurl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    //	//curl_easy_setopt(pCurl, CURLOPT_USERPWD, up);

    //	struct curl_slist* headers = NULL;
    //	if (authType == CURLAUTH_BASIC)
    //	{
    //		headers = curl_slist_append(headers, ("Authorization: Basic " + string(up)).c_str());
    //	}
    //	curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, headers);
    //}

    // Excute download operation.
    retCode = curl_easy_perform(pCurl);
    if (retCode != CURLE_OK)
    {
        fclose(pFile);
        curl_easy_cleanup(pCurl);

        return false;
    }

    // Get response code.
    int responseCode = 0;
    retCode = curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &responseCode);
    if (retCode != CURLE_OK || responseCode != 200)
    {
        fclose(pFile);
        curl_easy_cleanup(pCurl);

        return false;
    }

    // TFFFloat64 length = 0.0;
    // retCode = curl_easy_getinfo(pCurl, CURLINFO_CONTENT_LENGTH_DOWNLOAD , &length);
    fflush(pFile);
    fseek(pFile, 0, SEEK_END);
    int nDown = ftell(pFile);
    if (nDown == 0)
    {
        printf("DownloadFile %s is 0", save.c_str());
        fclose(pFile);
        curl_easy_cleanup(pCurl);
        return false;
    }

    fclose(pFile);
    curl_easy_cleanup(pCurl);
    return true;
    return status;
}

static size_t upload_write_cb(void* contents, size_t size, size_t nmemb, std::string* userp)
{
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

bool silly_http::request_upload(const std::string& url, const std::string& body, const std::vector<std::string> files, const std::map<std::string, std::string>& headers, std::string& resp)
{
    bool status = false;
    CURL* curl;
    CURLcode res;

    struct curl_httppost* formpost = NULL;
    struct curl_httppost* lastptr = NULL;
    struct curl_slist* headerlist = NULL;
    static const char buf[] = "Expect:";

    curl_global_init(CURL_GLOBAL_ALL);

    // 设置表单字段
    for (auto file : files)
    {
        std::filesystem::path sfp_tmp(file);
        if (!std::filesystem::exists(sfp_tmp))
        {
            continue;
        }

        SU_DEBUG_PRINT("正在上传 : %s", file)
        std::string s_file_name = sfp_tmp.filename().string();
        curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, s_file_name.c_str(), CURLFORM_FILE, file.c_str(), CURLFORM_END);
    }

    curl = curl_easy_init();
    headerlist = curl_slist_append(headerlist, buf);
    if (curl)
    {
        // 设置目标URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // 设置HTTP POST数据
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, upload_write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);

        // 执行请求
        res = curl_easy_perform(curl);

        // 检查错误
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            status = true;
        }

        // 清理
        curl_easy_cleanup(curl);

        // 清理表单
        curl_formfree(formpost);
        // 清理自定义头
        curl_slist_free_all(headerlist);
    }

    curl_global_cleanup();
    return status;
}
