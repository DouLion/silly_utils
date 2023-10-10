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
	size_t size{ 0 };
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
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

		/* get it! */
		res = curl_easy_perform(curl);

		/* check for errors */
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
		}
		//else {
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

bool silly_http::request_download(const std::string& url, const std::string& save, const std::map <std::string, std::string>& headers)
{
	bool status = false;
	CURLcode res;
	CURL* curl_handle;
	struct MemoryStruct chunk;

	chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
	chunk.size = 0;    /* no data at this point */

	curl_global_init(CURL_GLOBAL_ALL);

	/* init the curl session */
	curl_handle = curl_easy_init();

	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);

	/* specify URL to get */
	curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());

	// timeout 10s
	curl_easy_setopt(curl_handle, CURLOPT_ACCEPTTIMEOUT_MS, 30000L);

	// 下面两个一起  30秒内小于1个字节则停止
	curl_easy_setopt(curl_handle, CURLOPT_LOW_SPEED_LIMIT, 1L);

	curl_easy_setopt(curl_handle, CURLOPT_LOW_SPEED_TIME, 30);

	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);

	/* some servers do not like requests that are made without a user-agent
	   field, so we provide one */
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	/* get it! */
	res = curl_easy_perform(curl_handle);
	/* check for errors */
	if (res != CURLE_OK) {
		//fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

	}
	else {
		// 添加状态码判断，保证数据下载完成.
		int code = 0;
		res = curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &code);
		double speed;
		res = curl_easy_getinfo(curl_handle, CURLINFO_SPEED_DOWNLOAD, &speed);
		/*LOG_F(INFO, "%s", TzxFormat::Format("Download speed {0} bytes/sec", speed).c_str());*/
		if (res != CURLE_OK || 200 != code) {
			//LOG_F(ERROR, "%s", TzxFormat::Format("CURL status: {0}, HTTP code: {1}", (int)res, code).c_str());
			//remoteStatus = false;
		}
		if (!chunk.size)
		{
			/*LOG_F(ERROR, "%s", TzxFormat::Format("File size is zero or file data empty, check url:", url).c_str());
			remoteStatus = false;*/
		}
		else
		{
			FILE* pagefile;
			pagefile = fopen(save.c_str(), "wb");
			fwrite(chunk.memory, chunk.size, 1, pagefile);
			fclose(pagefile);
			status = true;
		}

	}

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);

	free(chunk.memory);

	/* we are done with libcurl, so clean it up */
	curl_global_cleanup();
	return status;
}

std::string silly_http::request_upload(const std::string& url, const std::string& body, const std::vector <std::string> files, const std::map <std::string, std::string>& headers)
{
	return "";
}
