/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2026-05-20
 * @file: silly_sftp_client.h
 * @description: silly_sftp_client 头文件
 * @version: v1.0.1 2026-05-20 dou li yang
 */
#ifndef SILLY_SFTP_CLIENT_H
#define SILLY_SFTP_CLIENT_H

#include <cstdint>
#include <string>
#include <vector>

class suSFTPClient {
public:
    enum class AuthMode {
        Password,
        PrivateKey
    };

    enum class HostKeyVerifyMode {
        Disabled,
        Strict
    };

public:
    suSFTPClient();
    ~suSFTPClient();

    suSFTPClient(const suSFTPClient&) = delete;
    suSFTPClient& operator=(const suSFTPClient&) = delete;

    bool connectWithPassword(const std::string& host,
                             int port,
                             const std::string& username,
                             const std::string& password);

    bool connectWithPrivateKey(const std::string& host,
                               int port,
                               const std::string& username,
                               const std::string& privateKeyFile,
                               const std::string& publicKeyFile = "",
                               const std::string& passphrase = "");

    void disconnect();

    void setTimeout(long seconds);
    void setConnectTimeout(long seconds);
    void setVerbose(bool enabled);

    void setHostKeyVerifyMode(HostKeyVerifyMode mode);
    void setKnownHostsFile(const std::string& knownHostsFile);

    bool uploadFile(const std::string& localPath,
                    const std::string& remotePath);

    bool downloadFile(const std::string& remotePath,
                      const std::string& localPath);

    bool uploadData(const std::string& data,
                    const std::string& remotePath);

    bool downloadData(const std::string& remotePath,
                      std::string& outData);

    bool listDirectory(const std::string& remoteDir,
                       std::vector<std::string>& outLines);

    bool createDirectory(const std::string& remoteDir);
    bool removeDirectory(const std::string& remoteDir);
    bool deleteFile(const std::string& remotePath);

    bool rename(const std::string& oldRemotePath,
                const std::string& newRemotePath);

    bool getFileSize(const std::string& remotePath,
                     std::int64_t& outSize);

    std::string lastError() const;

private:
    std::string buildUrl(const std::string& remotePath) const;
    bool performQuoteCommand(const std::string& command);
    void resetCommonOptions(void* curl);

private:
    std::string host_;
    int port_;

    std::string username_;
    std::string password_;

    std::string privateKeyFile_;
    std::string publicKeyFile_;
    std::string passphrase_;

    AuthMode authMode_;
    HostKeyVerifyMode hostKeyVerifyMode_;

    std::string knownHostsFile_;

    long timeoutSeconds_;
    long connectTimeoutSeconds_;

    bool verbose_;
    bool connected_;

    std::string lastError_;
};
#endif //SILLY_SFTP_CLIENT_H