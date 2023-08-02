/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/2 13:38
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_FTP_H
#define SILLY_UTILS_SILLY_FTP_H
#include <sys/types.h>
#if defined(_WIN32)

#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

#include <time.h>
#endif

#ifndef _WIN32
#include <unistd.h>
#include <sys/time.h>
#else
#include <WinSock2.h>
#include <ws2tcpip.h>
#endif

#ifdef NOLFS
#define off64_t long
#define fseeko64 fseek
#define fopen64 fopen
#endif

#ifndef off64_t
#define off64_t long
#endif

#ifndef fseeko64
#define fseeko64 fseek
#endif

#ifndef fopen64
#define fopen64 fopen
#endif

#if defined(__APPLE__)
#define off64_t __darwin_off_t
#define fseeko64 fseeko
#define fopen64 fopen
#endif

 //SSL
typedef struct ssl_st SSL;
typedef struct ssl_ctx_st SSL_CTX;
typedef struct bio_st BIO;
typedef struct x509_st X509;

#include <sys/types.h>

#ifndef _silly_ftp_SSL_CLIENT_METHOD_
#define _silly_ftp_SSL_CLIENT_METHOD_ TLSv1_2_client_method
#endif

using namespace std;

//SSL
typedef struct ssl_st SSL;
typedef struct ssl_ctx_st SSL_CTX;
typedef struct bio_st BIO;
typedef struct x509_st X509;


typedef int (*FtpCallbackXfer)(off64_t xfered, void* arg);
typedef int (*FtpCallbackIdle)(void* arg);
typedef void (*FtpCallbackLog)(char* str, void* arg, bool out);
//SSL
typedef bool (*FtpCallbackCert)(void* arg, X509* cert);


struct ftphandle {
	char* cput, * cget;
	int handle;
	int cavail, cleft;
	char* buf;
	int dir;
	ftphandle* ctrl;
	int cmode;
	struct timeval idletime;
	FtpCallbackXfer xfercb;
	FtpCallbackIdle idlecb;
	FtpCallbackLog logcb;
	void* cbarg;
	off64_t xfered;
	off64_t cbbytes;
	off64_t xfered1;
	char response[256];
	//SSL
	SSL* ssl;
	SSL_CTX* ctx;
	BIO* sbio;
	int tlsctrl;
	int tlsdata;
	FtpCallbackCert certcb;

	off64_t offset;
	bool correctpasv;
};

class silly_ftp
{
public:

	enum accesstype
	{
		dir = 1,
		dirverbose,
		fileread,
		filewrite,
		filereadappend,
		filewriteappend
	};

	enum transfermode
	{
		ascii = 'A',
		image = 'I'
	};

	enum connmode
	{
		pasv = 1,
		port
	};

	enum fxpmethod
	{
		defaultfxp = 0,
		alternativefxp
	};

	enum dataencryption
	{
		unencrypted = 0,
		secure
	};

	silly_ftp();
	~silly_ftp();
	char* LastResponse();
	int Connect(const char* host);
	int Login(const char* user, const char* pass);
	int Site(const char* cmd);
	int Raw(const char* cmd);
	int SysType(char* buf, int max);
	int Mkdir(const char* path);
	int Chdir(const char* path);
	int Cdup();
	int Rmdir(const char* path);
	int Pwd(char* path, int max);
	int Nlst(const char* outputfile, const char* path);
	int Dir(const char* outputfile, const char* path);
	int Size(const char* path, int* size, transfermode mode);
	int ModDate(const char* path, char* dt, int max);
	int Get(const char* outputfile, const char* path, transfermode mode, off64_t offset = 0);
	int Put(const char* inputfile, const char* path, transfermode mode, off64_t offset = 0);
	int Rename(const char* src, const char* dst);
	int Delete(const char* path);
	int Quit();
	void SetCallbackIdleFunction(FtpCallbackIdle pointer);
	void SetCallbackLogFunction(FtpCallbackLog pointer);
	void SetCallbackXferFunction(FtpCallbackXfer pointer);
	void SetCallbackArg(void* arg);
	void SetCallbackBytes(off64_t bytes);
	void SetCorrectPasv(bool b) { mp_ftphandle->correctpasv = b; };
	void SetCallbackIdletime(int time);
	void SetConnmode(connmode mode);
	static int Fxp(silly_ftp* src, silly_ftp* dst, const char* pathSrc, const char* pathDst, transfermode mode, fxpmethod method);
	ftphandle* RawOpen(const char* path, accesstype type, transfermode mode);
	int RawClose(ftphandle* handle);
	int RawWrite(void* buf, int len, ftphandle* handle);
	int RawRead(void* buf, int max, ftphandle* handle);
	// SSL
	int SetDataEncryption(dataencryption enc);
	int NegotiateEncryption();
	void SetCallbackCertFunction(FtpCallbackCert pointer);

private:
	ftphandle* mp_ftphandle;

	int FtpXfer(const char* localfile, const char* path, ftphandle* nControl, accesstype type, transfermode mode);
	int FtpOpenPasv(ftphandle* nControl, ftphandle** nData, transfermode mode, int dir, char* cmd);
	int FtpSendCmd(const char* cmd, char expresp, ftphandle* nControl);
	int FtpAcceptConnection(ftphandle* nData, ftphandle* nControl);
	int FtpOpenPort(ftphandle* nControl, ftphandle** nData, transfermode mode, int dir, char* cmd);
	int FtpRead(void* buf, int max, ftphandle* nData);
	int FtpWrite(void* buf, int len, ftphandle* nData);
	int FtpAccess(const char* path, accesstype type, transfermode mode, ftphandle* nControl, ftphandle** nData);
	int FtpClose(ftphandle* nData);
	int socket_wait(ftphandle* ctl);
	int readline(char* buf, int max, ftphandle* ctl);
	int writeline(char* buf, int len, ftphandle* nData);
	int readresp(char c, ftphandle* nControl);
	void sprint_rest(char* buf, off64_t offset);
	void ClearHandle();
	int CorrectPasvResponse(unsigned char* v);
};

#endif //SILLY_UTILS_SILLY_FTP_H
