/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2026-05-20
 * @file: silly_sftp_client
 * @description: silly_sftp_client实现
 * @version: v1.0.1 2026-05-20 dou li yang
 */
#include "silly_sftp_client.h"
#include <curl/curl.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <memory>
#include <sstream>

namespace {

struct CurlGlobalInitializer {
    CurlGlobalInitializer() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    ~CurlGlobalInitializer() {
        curl_global_cleanup();
    }
};

CurlGlobalInitializer globalCurlInitializer;

size_t writeFileCallback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    FILE* file = static_cast<FILE*>(userdata);
    return fwrite(ptr, size, nmemb, file);
}

size_t readFileCallback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    FILE* file = static_cast<FILE*>(userdata);
    return fread(ptr, size, nmemb, file);
}

size_t writeStringCallback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* data = static_cast<std::string*>(userdata);
    size_t totalSize = size * nmemb;
    data->append(static_cast<char*>(ptr), totalSize);
    return totalSize;
}

struct UploadMemoryContext {
    const char* data = nullptr;
    size_t size = 0;
    size_t offset = 0;
};

size_t readMemoryCallback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* ctx = static_cast<UploadMemoryContext*>(userdata);

    size_t bufferSize = size * nmemb;
    size_t remaining = ctx->size - ctx->offset;
    size_t copySize = remaining < bufferSize ? remaining : bufferSize;

    if (copySize > 0) {
        std::memcpy(ptr, ctx->data + ctx->offset, copySize);
        ctx->offset += copySize;
    }

    return copySize;
}

std::string curlErrorToString(CURLcode code, const char* errorBuffer) {
    if (errorBuffer && errorBuffer[0] != '\0') {
        return std::string(errorBuffer);
    }

    return curl_easy_strerror(code);
}

} // namespace

suSFTPClient::suSFTPClient()
    : port_(22),
      authMode_(AuthMode::Password),
      hostKeyVerifyMode_(HostKeyVerifyMode::Disabled),
      timeoutSeconds_(0),
      connectTimeoutSeconds_(30),
      verbose_(false),
      connected_(false) {
}

suSFTPClient::~suSFTPClient() {
    disconnect();
}

bool suSFTPClient::connectWithPassword(const std::string& host,
                                     int port,
                                     const std::string& username,
                                     const std::string& password) {
    host_ = host;
    port_ = port > 0 ? port : 22;
    username_ = username;
    password_ = password;
    privateKeyFile_.clear();
    publicKeyFile_.clear();
    passphrase_.clear();
    authMode_ = AuthMode::Password;
    connected_ = true;
    lastError_.clear();

    return true;
}

bool suSFTPClient::connectWithPrivateKey(const std::string& host,
                                       int port,
                                       const std::string& username,
                                       const std::string& privateKeyFile,
                                       const std::string& publicKeyFile,
                                       const std::string& passphrase) {
    host_ = host;
    port_ = port > 0 ? port : 22;
    username_ = username;
    password_.clear();
    privateKeyFile_ = privateKeyFile;
    publicKeyFile_ = publicKeyFile;
    passphrase_ = passphrase;
    authMode_ = AuthMode::PrivateKey;
    connected_ = true;
    lastError_.clear();

    return true;
}

void suSFTPClient::disconnect() {
    connected_ = false;
}

void suSFTPClient::setTimeout(long seconds) {
    timeoutSeconds_ = seconds;
}

void suSFTPClient::setConnectTimeout(long seconds) {
    connectTimeoutSeconds_ = seconds;
}

void suSFTPClient::setVerbose(bool enabled) {
    verbose_ = enabled;
}

void suSFTPClient::setHostKeyVerifyMode(HostKeyVerifyMode mode) {
    hostKeyVerifyMode_ = mode;
}

void suSFTPClient::setKnownHostsFile(const std::string& knownHostsFile) {
    knownHostsFile_ = knownHostsFile;
}

bool suSFTPClient::uploadFile(const std::string& localPath,
                            const std::string& remotePath) {
    lastError_.clear();

    FILE* file = std::fopen(localPath.c_str(), "rb");
    if (!file) {
        lastError_ = "Failed to open local file for reading: " + localPath;
        return false;
    }

    std::unique_ptr<FILE, decltype(&std::fclose)> fileGuard(file, &std::fclose);

    std::ifstream ifs(localPath, std::ios::binary | std::ios::ate);
    if (!ifs) {
        lastError_ = "Failed to get local file size: " + localPath;
        return false;
    }

    auto fileSize = static_cast<curl_off_t>(ifs.tellg());

    CURL* curl = curl_easy_init();
    if (!curl) {
        lastError_ = "curl_easy_init failed";
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
    if (code != CURLE_OK) {
        lastError_ = curlErrorToString(code, errorBuffer);
        return false;
    }

    return true;
}

bool suSFTPClient::downloadFile(const std::string& remotePath,
                              const std::string& localPath) {
    lastError_.clear();

    FILE* file = std::fopen(localPath.c_str(), "wb");
    if (!file) {
        lastError_ = "Failed to open local file for writing: " + localPath;
        return false;
    }

    std::unique_ptr<FILE, decltype(&std::fclose)> fileGuard(file, &std::fclose);

    CURL* curl = curl_easy_init();
    if (!curl) {
        lastError_ = "curl_easy_init failed";
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
    if (code != CURLE_OK) {
        lastError_ = curlErrorToString(code, errorBuffer);
        return false;
    }

    return true;
}

bool suSFTPClient::uploadData(const std::string& data,
                            const std::string& remotePath) {
    lastError_.clear();

    UploadMemoryContext ctx;
    ctx.data = data.data();
    ctx.size = data.size();
    ctx.offset = 0;

    CURL* curl = curl_easy_init();
    if (!curl) {
        lastError_ = "curl_easy_init failed";
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
    if (code != CURLE_OK) {
        lastError_ = curlErrorToString(code, errorBuffer);
        return false;
    }

    return true;
}

bool suSFTPClient::downloadData(const std::string& remotePath,
                              std::string& outData) {
    lastError_.clear();
    outData.clear();

    CURL* curl = curl_easy_init();
    if (!curl) {
        lastError_ = "curl_easy_init failed";
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
    if (code != CURLE_OK) {
        lastError_ = curlErrorToString(code, errorBuffer);
        return false;
    }

    return true;
}

bool suSFTPClient::listDirectory(const std::string& remoteDir,
                               std::vector<std::string>& outLines) {
    lastError_.clear();
    outLines.clear();

    std::string listing;

    CURL* curl = curl_easy_init();
    if (!curl) {
        lastError_ = "curl_easy_init failed";
        return false;
    }

    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curlGuard(curl, &curl_easy_cleanup);

    char errorBuffer[CURL_ERROR_SIZE] = {0};

    resetCommonOptions(curl);

    std::string url = buildUrl(remoteDir);
    if (!url.empty() && url.back() != '/') {
        url += "/";
    }

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeStringCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &listing);

    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK) {
        lastError_ = curlErrorToString(code, errorBuffer);
        return false;
    }

    std::istringstream iss(listing);
    std::string line;
    while (std::getline(iss, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (!line.empty()) {
            outLines.push_back(line);
        }
    }

    return true;
}

bool suSFTPClient::createDirectory(const std::string& remoteDir) {
    return performQuoteCommand("mkdir " + remoteDir);
}

bool suSFTPClient::removeDirectory(const std::string& remoteDir) {
    return performQuoteCommand("rmdir " + remoteDir);
}

bool suSFTPClient::deleteFile(const std::string& remotePath) {
    return performQuoteCommand("rm " + remotePath);
}

bool suSFTPClient::rename(const std::string& oldRemotePath,
                        const std::string& newRemotePath) {
    return performQuoteCommand("rename " + oldRemotePath + " " + newRemotePath);
}

bool suSFTPClient::getFileSize(const std::string& remotePath,
                             std::int64_t& outSize) {
    lastError_.clear();
    outSize = -1;

    CURL* curl = curl_easy_init();
    if (!curl) {
        lastError_ = "curl_easy_init failed";
        return false;
    }

    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curlGuard(curl, &curl_easy_cleanup);

    char errorBuffer[CURL_ERROR_SIZE] = {0};

    resetCommonOptions(curl);

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_URL, buildUrl(remotePath).c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_HEADER, 0L);

    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK) {
        lastError_ = curlErrorToString(code, errorBuffer);
        return false;
    }

    curl_off_t contentLength = -1;
    code = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &contentLength);
    if (code != CURLE_OK || contentLength < 0) {
        lastError_ = "Failed to get remote file size";
        return false;
    }

    outSize = static_cast<std::int64_t>(contentLength);
    return true;
}

std::string suSFTPClient::lastError() const {
    return lastError_;
}

std::string suSFTPClient::buildUrl(const std::string& remotePath) const {
    std::ostringstream oss;

    oss << "sftp://";
    oss << host_;

    if (port_ > 0) {
        oss << ":" << port_;
    }

    if (!remotePath.empty() && remotePath.front() != '/') {
        oss << "/";
    }

    oss << remotePath;

    return oss.str();
}

bool suSFTPClient::performQuoteCommand(const std::string& command) {
    lastError_.clear();

    CURL* curl = curl_easy_init();
    if (!curl) {
        lastError_ = "curl_easy_init failed";
        return false;
    }

    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curlGuard(curl, &curl_easy_cleanup);

    char errorBuffer[CURL_ERROR_SIZE] = {0};

    resetCommonOptions(curl);

    struct curl_slist* commands = nullptr;
    commands = curl_slist_append(commands, command.c_str());

    std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> commandGuard(
        commands, &curl_slist_free_all
    );

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_URL, buildUrl("/").c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_QUOTE, commands);

    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK) {
        lastError_ = curlErrorToString(code, errorBuffer);
        return false;
    }

    return true;
}

void suSFTPClient::resetCommonOptions(void* handle) {
    CURL* curl = static_cast<CURL*>(handle);

    curl_easy_setopt(curl, CURLOPT_USERNAME, username_.c_str());

    if (authMode_ == AuthMode::Password) {
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password_.c_str());
    } else {
        curl_easy_setopt(curl, CURLOPT_SSH_PRIVATE_KEYFILE, privateKeyFile_.c_str());

        if (!publicKeyFile_.empty()) {
            curl_easy_setopt(curl, CURLOPT_SSH_PUBLIC_KEYFILE, publicKeyFile_.c_str());
        }

        if (!passphrase_.empty()) {
            curl_easy_setopt(curl, CURLOPT_KEYPASSWD, passphrase_.c_str());
        }
    }

    if (connectTimeoutSeconds_ > 0) {
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connectTimeoutSeconds_);
    }

    if (timeoutSeconds_ > 0) {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeoutSeconds_);
    }

    curl_easy_setopt(curl, CURLOPT_VERBOSE, verbose_ ? 1L : 0L);

    if (hostKeyVerifyMode_ == HostKeyVerifyMode::Strict) {
        if (!knownHostsFile_.empty()) {
            curl_easy_setopt(curl, CURLOPT_SSH_KNOWNHOSTS, knownHostsFile_.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_SSH_HOSTKEYFUNCTION, nullptr);
    } else {
        curl_easy_setopt(curl, CURLOPT_SSH_KNOWNHOSTS, nullptr);
    }
}