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
	//��ע�͵�������Դ�ӡ��������cookie����Ϣ
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

	// ע���������ÿ�α����û�õ��������¶�̬���仺�����Ĵ�С
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
	//curl��ʼ��  
	CURL* curl = curl_easy_init();
	// curl����ֵ 
	struct MemoryStruct chunk;
	CURLcode res;
	if (curl)
	{
		//����curl������ͷ
		struct curl_slist* header_list = NULL;
		header_list = curl_slist_append(header_list, "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

		//��������Ӧͷ����0�������� 1�������
		curl_easy_setopt(curl, CURLOPT_HEADER, 0);

		//���������URL��ַ 
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		//����ssl��֤
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);

		//CURLOPT_VERBOSE��ֵΪ1ʱ������ʾ��ϸ�ĵ�����Ϣ
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);

		//�������ݽ��պ���
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

		//���ó�ʱʱ��
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 6); // set transport and time out time  
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 6);

		// ��������  
		res = curl_easy_perform(curl);
	}
	// �ͷ�curl 
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

	//	// TODO: ���ַ�ʽ��֪��Ϊʲô����.
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
	* ������̶߳�ʹ�ó�ʱ�����ʱ��ͬʱ���߳�����sleep����wait�Ȳ�����
	* ������������ѡ�libcurl���ᷢ�źŴ�����wait�Ӷ����³����˳���
	*/
	curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1);
	// ����������SSL��֤����ʹ��CA֤��.
	curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
	// ��֤�������˷��͵�֤�飬Ĭ���� 2(��)��1���У���0�����ã�.
	curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, 0L);
	// �ڷ�������ǰ�ȴ���ʱ�䣬�������Ϊ0�������޵ȴ�.
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
