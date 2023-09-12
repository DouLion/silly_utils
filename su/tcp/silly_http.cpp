//
// Created by dly on 2023/7/10.
//

#include "silly_http.h"
#include "curl/curl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t req_reply(void* ptr, size_t size, size_t nmemb, void* stream)
{
	//在注释的里面可以打印请求流，cookie的信息
	//cout << "----->reply" << endl;
	std::string* str = (std::string*)stream;
	//cout << *str << endl;
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
struct MemoryStruct {
	char* memory{ nullptr };
	size_t size{0};
};

static size_t
WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct* mem = (struct MemoryStruct*)userp;

	// 注意这里根据每次被调用获得的数据重新动态分配缓存区的大小
	char* ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
	if (ptr == NULL) {
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

std::string silly_http::request_get(const std::string& url, const std::map <std::string, std::string>& headers)
{
	std::string ret_content;
	//curl初始化  
	CURL* curl = curl_easy_init();
	// curl返回值 
	struct MemoryStruct chunk;
	CURLcode res;
	if (curl)
	{
		//设置curl的请求头
		struct curl_slist* header_list = NULL;
		header_list = curl_slist_append(header_list, "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

		//不接收响应头数据0代表不接收 1代表接收
		curl_easy_setopt(curl, CURLOPT_HEADER, 0);

		//设置请求的URL地址 
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		//设置ssl验证
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);

		//CURLOPT_VERBOSE的值为1时，会显示详细的调试信息
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);

		//设置数据接收函数
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

		//设置超时时间
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 6); // set transport and time out time  
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 6);

		// 开启请求  
		res = curl_easy_perform(curl);
	}
	// 释放curl 
	curl_easy_cleanup(curl);
	if (chunk.size && chunk.memory)
	{
		ret_content.resize(chunk.size);
		memcpy(&ret_content[0], chunk.memory, chunk.size);
	}
	return ret_content;
}

std::string silly_http::request_post(const std::string& url, const std::string& body, const std::map <std::string, std::string>& headers)
{
	std::string ret_content;
	CURLcode retCode;
	CURL* pCurl = curl_easy_init();
	if (NULL == pCurl)
	{
		return ret_content;
	}
	curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str());
	//if (!httpVerifyUserName.empty() && !httpVerifyUserPwd.empty())
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

std::string silly_http::request_download(const std::string& url, const std::string& body, const std::map <std::string, std::string>& headers)
{
	return "";
}

std::string silly_http::request_upload(const std::string& url, const std::string& body, const std::vector <std::string> files, const std::map <std::string, std::string>& headers)
{
	return "";
}
