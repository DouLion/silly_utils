/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_ftp_client.cpp
 * @description: silly_ftp_client实现
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#include "silly_ftp_client.h"

#include <curl/curl.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <memory>

namespace
{
struct CurlGlobalInitializer
{
    CurlGlobalInitializer()
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    ~CurlGlobalInitializer()
    {
        curl_global_cleanup();
    }
};

CurlGlobalInitializer globalCurlInitializer;

size_t writeFileCallback(void* ptr, size_t size, size_t nmemb, void* userdata)
{
    FILE* file = static_cast<FILE*>(userdata);
    return fwrite(ptr, size, nmemb, file);
}

size_t readFileCallback(void* ptr, size_t size, size_t nmemb, void* userdata)
{
    FILE* file = static_cast<FILE*>(userdata);
    return fread(ptr, size, nmemb, file);
}

size_t writeStringCallback(void* ptr, size_t size, size_t nmemb, void* userdata)
{
    auto* data = static_cast<std::string*>(userdata);
    size_t totalSize = size * nmemb;
    data->append(static_cast<char*>(ptr), totalSize);
    return totalSize;
}

struct UploadMemoryContext
{
    const char* data = nullptr;
    size_t size = 0;
    size_t offset = 0;
};

size_t readMemoryCallback(void* ptr, size_t size, size_t nmemb, void* userdata)
{
    auto* ctx = static_cast<UploadMemoryContext*>(userdata);

    size_t bufferSize = size * nmemb;
    size_t remaining = ctx->size - ctx->offset;
    size_t copySize = remaining < bufferSize ? remaining : bufferSize;

    if (copySize > 0)
    {
        std::memcpy(ptr, ctx->data + ctx->offset, copySize);
        ctx->offset += copySize;
    }

    return copySize;
}

std::string curlErrorToString(CURLcode code, const char* errorBuffer)
{
    if (errorBuffer && errorBuffer[0] != '\0')
    {
        return std::string(errorBuffer);
    }

    return curl_easy_strerror(code);
}
} // namespace

suFTPClient::suFTPClient()
    : m_Port(21),
      m_SecurityMode(SecurityMode::None),
      m_TransferMode(TransferMode::Passive),
      m_TimeoutSeconds(0),
      m_ConnectTimeoutSeconds(30),
      m_Verbose(false),
      m_VerifyPeer(true),
      m_VerifyHost(true),
      m_Connected(false)
{
}

suFTPClient::~suFTPClient()
{
    Disconnect();
}

bool suFTPClient::Connect(const std::string& host,
                          const int& port,
                          const std::string& username,
                          const std::string& password,
                          const SecurityMode& security)
{
    m_Host = host;
    m_Port = port;
    m_UserName = username;
    m_Password = password;
    m_SecurityMode = security;
    m_Connected = true;
    m_LastError.clear();

    return true;
}

void suFTPClient::Disconnect()
{
    m_Connected = false;
}

void suFTPClient::SetTimeout(long seconds)
{
    m_TimeoutSeconds = seconds;
}

void suFTPClient::SetConnectTimeout(long seconds)
{
    m_ConnectTimeoutSeconds = seconds;
}

void suFTPClient::SetTransferMode(TransferMode mode)
{
    m_TransferMode = mode;
}

void suFTPClient::SetVerbose(bool enabled)
{
    m_Verbose = enabled;
}

void suFTPClient::SetVerifyPeer(bool enabled)
{
    m_VerifyPeer = enabled;
}

void suFTPClient::SetVerifyHost(bool enabled)
{
    m_VerifyHost = enabled;
}

bool suFTPClient::UploadFile(const std::string& localPath,
                             const std::string& remotePath)
{
    m_LastError.clear();

    FILE* file = std::fopen(localPath.c_str(), "rb");
    if (!file)
    {
        m_LastError = "Failed to open local file for reading: " + localPath;
        return false;
    }

    std::unique_ptr<FILE, decltype(&std::fclose)> fileGuard(file, &std::fclose);

    std::ifstream ifs(localPath, std::ios::binary | std::ios::ate);
    if (!ifs)
    {
        m_LastError = "Failed to get local file size: " + localPath;
        return false;
    }

    auto fileSize = static_cast<curl_off_t>(ifs.tellg());

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        m_LastError = "curl_easy_init failed";
        return false;
    }

    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curlGuard(curl, &curl_easy_cleanup);

    char errorBuffer[CURL_ERROR_SIZE] = {0};

    resetCommonOptions(curl);

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_URL, buildUrl(remotePath).c_str());
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, readFileCallback);
    curl_easy_setopt(curl, CURLOPT_READDATA, file);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, fileSize);

    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK)
    {
        m_LastError = curlErrorToString(code, errorBuffer);
        return false;
    }

    return true;
}

bool suFTPClient::DownloadFile(const std::string& remotePath,
                               const std::string& localPath)
{
    m_LastError.clear();

    FILE* file = std::fopen(localPath.c_str(), "wb");
    if (!file)
    {
        m_LastError = "Failed to open local file for writing: " + localPath;
        return false;
    }

    std::unique_ptr<FILE, decltype(&std::fclose)> fileGuard(file, &std::fclose);

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        m_LastError = "curl_easy_init failed";
        return false;
    }

    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curlGuard(curl, &curl_easy_cleanup);

    char errorBuffer[CURL_ERROR_SIZE] = {0};

    resetCommonOptions(curl);

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_URL, buildUrl(remotePath).c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFileCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK)
    {
        m_LastError = curlErrorToString(code, errorBuffer);
        return false;
    }

    return true;
}

bool suFTPClient::UploadData(const std::string& data,
                             const std::string& remotePath)
{
    m_LastError.clear();

    UploadMemoryContext ctx;
    ctx.data = data.data();
    ctx.size = data.size();
    ctx.offset = 0;

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        m_LastError = "curl_easy_init failed";
        return false;
    }

    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curlGuard(curl, &curl_easy_cleanup);

    char errorBuffer[CURL_ERROR_SIZE] = {0};

    resetCommonOptions(curl);

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_URL, buildUrl(remotePath).c_str());
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, readMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_READDATA, &ctx);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(data.size()));

    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK)
    {
        m_LastError = curlErrorToString(code, errorBuffer);
        return false;
    }

    return true;
}

bool suFTPClient::DownloadData(const std::string& remotePath,
                               std::string& outData)
{
    m_LastError.clear();
    outData.clear();

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        m_LastError = "curl_easy_init failed";
        return false;
    }

    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curlGuard(curl, &curl_easy_cleanup);

    char errorBuffer[CURL_ERROR_SIZE] = {0};

    resetCommonOptions(curl);

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_URL, buildUrl(remotePath).c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeStringCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outData);

    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK)
    {
        m_LastError = curlErrorToString(code, errorBuffer);
        return false;
    }

    return true;
}

bool suFTPClient::ListDirectory(const std::string& remoteDir,
                                std::vector<std::string>& outLines)
{
    m_LastError.clear();
    outLines.clear();

    std::string listing;

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        m_LastError = "curl_easy_init failed";
        return false;
    }

    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curlGuard(curl, &curl_easy_cleanup);

    char errorBuffer[CURL_ERROR_SIZE] = {0};

    resetCommonOptions(curl);

    std::string url = buildUrl(remoteDir);
    if (!url.empty() && url.back() != '/')
    {
        url += "/";
    }

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeStringCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &listing);

    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK)
    {
        m_LastError = curlErrorToString(code, errorBuffer);
        return false;
    }

    std::istringstream iss(listing);
    std::string line;
    while (std::getline(iss, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        if (!line.empty())
        {
            outLines.push_back(line);
        }
    }

    return true;
}

bool suFTPClient::CreateDir(const std::string& remoteDir)
{
    return performQuoteCommand("MKD " + remoteDir);
}

bool suFTPClient::RemoveDir(const std::string& remoteDir)
{
    return performQuoteCommand("RMD " + remoteDir);
}

bool suFTPClient::DelFile(const std::string& remotePath)
{
    return performQuoteCommand("DELE " + remotePath);
}

bool suFTPClient::Rename(const std::string& oldRemotePath,
                         const std::string& newRemotePath)
{
    m_LastError.clear();

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        m_LastError = "curl_easy_init failed";
        return false;
    }

    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curlGuard(curl, &curl_easy_cleanup);

    char errorBuffer[CURL_ERROR_SIZE] = {0};

    resetCommonOptions(curl);

    struct curl_slist* commands = nullptr;
    commands = curl_slist_append(commands, ("RNFR " + oldRemotePath).c_str());
    commands = curl_slist_append(commands, ("RNTO " + newRemotePath).c_str());

    std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> commandGuard(
        commands,
        &curl_slist_free_all
        );

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_URL, buildUrl("/").c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_QUOTE, commands);

    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK)
    {
        m_LastError = curlErrorToString(code, errorBuffer);
        return false;
    }

    return true;
}

bool suFTPClient::GetFileSize(const std::string& remotePath,
                              std::int64_t& outSize)
{
    m_LastError.clear();
    outSize = -1;

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        m_LastError = "curl_easy_init failed";
        return false;
    }

    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curlGuard(curl, &curl_easy_cleanup);

    char errorBuffer[CURL_ERROR_SIZE] = {0};

    resetCommonOptions(curl);

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_URL, buildUrl(remotePath).c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
    curl_easy_setopt(curl, CURLOPT_FILETIME, 0L);

    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK)
    {
        m_LastError = curlErrorToString(code, errorBuffer);
        return false;
    }

    curl_off_t contentLength = -1;
    code = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &contentLength);
    if (code != CURLE_OK || contentLength < 0)
    {
        m_LastError = "Failed to get remote file size";
        return false;
    }

    outSize = static_cast<std::int64_t>(contentLength);
    return true;
}

std::string suFTPClient::LastError() const
{
    return m_LastError;
}

std::string suFTPClient::buildUrl(const std::string& remotePath) const
{
    std::ostringstream oss;

    if (m_SecurityMode == SecurityMode::ExplicitTLS)
    {
        oss << "ftp://";
    }
    else
    {
        oss << "ftp://";
    }

    oss << m_Host;

    if (m_Port > 0)
    {
        oss << ":" << m_Port;
    }

    if (!remotePath.empty() && remotePath.front() != '/')
    {
        oss << "/";
    }

    oss << remotePath;

    return oss.str();
}

bool suFTPClient::performQuoteCommand(const std::string& command)
{
    m_LastError.clear();

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        m_LastError = "curl_easy_init failed";
        return false;
    }

    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curlGuard(curl, &curl_easy_cleanup);

    char errorBuffer[CURL_ERROR_SIZE] = {0};

    resetCommonOptions(curl);

    struct curl_slist* commands = nullptr;
    commands = curl_slist_append(commands, command.c_str());

    std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> commandGuard(
        commands,
        &curl_slist_free_all
        );

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_URL, buildUrl("/").c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_QUOTE, commands);

    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK)
    {
        m_LastError = curlErrorToString(code, errorBuffer);
        return false;
    }

    return true;
}

void suFTPClient::resetCommonOptions(void* handle)
{
    CURL* curl = static_cast<CURL*>(handle);

    curl_easy_setopt(curl, CURLOPT_USERNAME, m_UserName.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, m_Password.c_str());

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, m_ConnectTimeoutSeconds);

    if (m_TimeoutSeconds > 0)
    {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, m_TimeoutSeconds);
    }

    curl_easy_setopt(curl, CURLOPT_VERBOSE, m_Verbose ? 1L : 0L);

    if (m_TransferMode == TransferMode::Active)
    {
        curl_easy_setopt(curl, CURLOPT_FTPPORT, "-");
    }
    else
    {
        curl_easy_setopt(curl, CURLOPT_FTPPORT, nullptr);
    }

    if (m_SecurityMode == SecurityMode::ExplicitTLS)
    {
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, m_VerifyPeer ? 1L : 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, m_VerifyHost ? 2L : 0L);
    }

    curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR_NONE);
}