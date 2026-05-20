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

    bool ConnectWithPassword(const std::string& host,
                             int port,
                             const std::string& username,
                             const std::string& password);

    bool ConnectWithPrivateKey(const std::string& host,
                               int port,
                               const std::string& username,
                               const std::string& privateKeyFile,
                               const std::string& publicKeyFile = "",
                               const std::string& passphrase = "");

    void Disconnect();

    void SetTimeout(long seconds);
    void SetConnectTimeout(long seconds);
    void SetVerbose(bool enabled);

    void SetHostKeyVerifyMode(HostKeyVerifyMode mode);
    void SetKnownHostsFile(const std::string& knownHostsFile);

    bool UploadFile(const std::string& localPath,
                    const std::string& remotePath);

    bool DownloadFile(const std::string& remotePath,
                      const std::string& localPath);

    bool UploadData(const std::string& data,
                    const std::string& remotePath);

    bool DownloadData(const std::string& remotePath,
                      std::string& outData);

    bool ListDirectory(const std::string& remoteDir,
                       std::vector<std::string>& outLines);

    bool CreateDir(const std::string& remoteDir);
    bool RemoveDir(const std::string& remoteDir);
    bool DelFile(const std::string& remotePath);

    bool Rename(const std::string& oldRemotePath,
                const std::string& newRemotePath);

    bool GetFileSize(const std::string& remotePath,
                     std::int64_t& outSize);

    std::string LastError() const;

private:
    std::string buildUrl(const std::string& remotePath) const;
    bool performQuoteCommand(const std::string& command);
    void resetCommonOptions(void* curl);

private:
    std::string m_Host;
    int m_Port;

    std::string m_UserName;
    std::string m_Password;

    std::string m_PrivateKeyFile_;
    std::string m_PublicKeyFile;
    std::string m_Passphrase;

    AuthMode m_AuthMode;
    HostKeyVerifyMode m_HostKeyVerifyMode;

    std::string m_KnownHostsFile;

    long m_TimeoutSeconds;
    long m_ConnectTimeoutSeconds;

    bool m_Verbose;
    bool m_Connected;

    std::string m_LastError;
};
#endif //SILLY_SFTP_CLIENT_H