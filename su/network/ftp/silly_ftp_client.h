/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_ftp_client.h
 * @description: silly_ftp_client 类声明
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#ifndef SILLY_FTP_CLIENT_H
#define SILLY_FTP_CLIENT_H
#include <string>
#include <vector>
#include <cstdint>

class suFTPClient
{
public:
    enum class SecurityMode
    {
        None,       // ftp://
        ExplicitTLS // ftps via AUTH TLS
    };

    enum class TransferMode
    {
        Passive,
        Active
    };

public:
    suFTPClient();

    ~suFTPClient();

    suFTPClient(const suFTPClient&) = delete;

    suFTPClient& operator=(const suFTPClient&) = delete;

    bool Connect(const std::string& host,
                 const int& port,
                 const std::string& username,
                 const std::string& password,
                 const SecurityMode& security = SecurityMode::None);

    void Disconnect();

    void SetTimeout(long seconds);

    void SetConnectTimeout(long seconds);

    void SetTransferMode(TransferMode mode);

    void SetVerbose(bool enabled);

    void SetVerifyPeer(bool enabled);

    void SetVerifyHost(bool enabled);

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
    SecurityMode m_SecurityMode;
    TransferMode m_TransferMode;

    long m_TimeoutSeconds;
    long m_ConnectTimeoutSeconds;

    bool m_Verbose;
    bool m_VerifyPeer;
    bool m_VerifyHost;
    bool m_Connected;

    std::string m_LastError;
};

#endif  // SILLY_FTP_CLIENT_H