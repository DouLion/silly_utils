//
// Created by dell on 2023/7/10.
//

#include <string>
#include "ftp_utils.h"


// 不落地获取信息
static size_t write_info_resp_func(void* ptr, size_t size, size_t nmb, void* data)
{
	//		FILE* writehere = (FILE*)data;
	//		return fwrite(ptr, size, nmemb, writehere);

	ftp_info* info = (ftp_info*)data;
	if (!info->size || !info->cache)
	{
		info->size = size;
		info->cache = (unsigned char*)malloc(info->size);
	}
	else
	{
		info->size += size;
		info->cache = (unsigned char*)realloc(info->cache, info->size);
	}
	memcpy_s(info->cache + info->size - size, size, ptr, size);
	return size;
}

// 获取信息存入文件
static size_t write_info_resp_to_file_func(void* ptr, size_t size, size_t nmemb, void* data)
{
	FILE* writehere = (FILE*)data;
	return fwrite(ptr, size, nmemb, writehere);
}

struct FtpFile {
	const char* filename;
	FILE* stream{ nullptr };
};

static int ftp_file_write_func(void* buffer, int size, int nmemb, void* stream)
{
	FtpFile* out = (FtpFile*)stream;
	if (!out->stream) {
		/* open file for writing */
		out->stream = fopen(out->filename, "wb");
		if (!out->stream)
			return -1; /* failure, can't open file to write */
	}
	return (int)fwrite(buffer, size, nmemb, out->stream);
}

/* parse headers for Content-Length */
static int upload_get_content_len_func(void* ptr, int size, int nmemb, void* stream)
{
	int r;
	long len = 0;
	r = sscanf((const char*)ptr, "Content-Length: %ld\n", &len);
	if (r) {
		*((long*)stream) = len;
	}
	return size * nmemb;
}

/* discard downloaded data */
static int upload_discard_func(void* ptr, int size, int nmemb, void* stream)
{
	(void)ptr;
	(void)stream;
	return size * nmemb;
}

/* read data to upload */
static int upload_read_func(void* ptr, int size, int nmemb, void* stream)
{
	FILE* f = (FILE*)stream;
	int n;

	if (ferror(f))
	{
		return CURL_READFUNC_ABORT;
	}


	n = (int)fread(ptr, size, nmemb, f) * size;

	return n;
}

ftp_file_info_list ftp_utils::get_ftp_info(const std::string& url, const std::string& user, const std::string& password, ftp_file_info_callback cbfunc)
{
	ftp_file_info_list ret_list_info;
	CURL* curl;
	CURLcode res;
	/*ftp_info* header_info = new ftp_info();
	ftp_info* file_info = new ftp_info();*/

	/* local file name to store the file as */
	FILE* body_file = fopen("FTPBODY", "wb"); /* b is binary, needed on win32 */


	/* local file name to store the FTP server's response lines in */
	FILE* header_file = fopen("FTPHEADERS", "wb"); /* b is binary, needed on win32 */

	curl = curl_easy_init();
	if (curl)
	{

#ifndef  NDEBUG
		char* version_str = curl_version();
		printf("%s\n", version_str);
#endif
		/* Get a file listing from sunet */
		//curl_easy_setopt(curl, CURLOPT_URL, "ftp://10.13.100.102:21212/5km/72/");
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		/*user & pwd*/
		std::string user_pwd = user + ":" + password;
		//curl_easy_setopt(curl, CURLOPT_USERPWD, "bjtzxxxkjyxgs:Tzx1@3$");
		curl_easy_setopt(curl, CURLOPT_USERPWD, user_pwd.c_str());


		curl_easy_setopt(curl, CURLOPT_WRITEDATA, body_file);
		/* If you intend to use this on windows with a libcurl DLL, you must use
		   CURLOPT_WRITEFUNCTION as well */
		// 头部信息
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_info_resp_to_file_func);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, header_file);
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	fclose(body_file);
	fclose(header_file);
	if (cbfunc)
	{
		std::ifstream input("FTPBODY");

		if (input.is_open())
		{
			std::string row;
			while (std::getline(input, row))
			{
				ftp_file_info tmp_info;

				cbfunc(row, tmp_info);
				ret_list_info.push_back(tmp_info);
			}
		}

		input.close();
	}

	return ret_list_info;
}

bool ftp_utils::ftp_download(const std::string& remote_file_path, const std::string& local_file_path, const std::string& user, const std::string& passwd, int timeout)
{
	CURL* curl;
	CURLcode res;
	FtpFile ftp_download_file;
	ftp_download_file.filename = local_file_path.c_str();

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if (curl) {
		/*
		* You better replace the URL with one that works!
		*/
		curl_easy_setopt(curl, CURLOPT_URL, remote_file_path.c_str());
		if (!user.empty())
		{
			char user_key[1024] = { 0 };
			sprintf(user_key, "%s:%s", user.c_str(), passwd.c_str());
			curl_easy_setopt(curl, CURLOPT_USERPWD, user_key);
		}

		/* Define our callback to get called when there's data to be written */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ftp_file_write_func);
		/* Set a pointer to our struct to pass to the callback */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftp_download_file);

		/* Switch on full protocol/debug output */
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		res = curl_easy_perform(curl);

		/* always cleanup */
		curl_easy_cleanup(curl);

		if (CURLE_OK != res) {
			/* we failed */
			fprintf(stderr, "curl told us %d\n", res);
		}
	}

	if (ftp_download_file.stream)
	{
		fclose(ftp_download_file.stream); /* close the local file */
	}


	curl_global_cleanup();

	return true;
}

bool ftp_utils::ftp_upload(const std::string& localFileFath, const std::string& remote_file_path, const std::string& user, const std::string& passwd, int timeout, bool IsDir)
{
	{
		CURL* curl_handle = nullptr;
		bool bRet = false;
		curl_global_init(CURL_GLOBAL_ALL);

		curl_handle = curl_easy_init();
		if (nullptr == curl_handle)
		{
			return bRet;
		}
		FILE* f;
		long uploaded_len = 0;
		CURLcode r = CURLE_GOT_NOTHING;
		int c;

		f = fopen(localFileFath.c_str(), "rb");
		if (!f) {
			perror(nullptr);
			return false;
		}

		curl_easy_setopt(curl_handle, CURLOPT_UPLOAD, 1L);

		curl_easy_setopt(curl_handle, CURLOPT_URL, remote_file_path.c_str());

		if (timeout)
			curl_easy_setopt(curl_handle, CURLOPT_FTP_RESPONSE_TIMEOUT, timeout);

		curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, upload_get_content_len_func);
		curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, &uploaded_len);

		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, upload_discard_func);

		curl_easy_setopt(curl_handle, CURLOPT_READFUNCTION, upload_read_func);
		curl_easy_setopt(curl_handle, CURLOPT_READDATA, f);

		/* disable passive mode */
		curl_easy_setopt(curl_handle, CURLOPT_FTPPORT, "-");
		curl_easy_setopt(curl_handle, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);

		curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

		for (c = 0; (r != CURLE_OK) && (c < 3); c++) {
			/* are we resuming? */
			if (c) { /* yes */
				/* determine the length of the file already written */

				/*
				* With NOBODY and NOHEADER, libcurl will issue a SIZE
				* command, but the only way to retrieve the result is
				* to parse the returned Content-Length header. Thus,
				* getcontentlengthfunc(). We need discardfunc() above
				* because HEADER will dump the headers to stdout
				* without it.
				*/
				curl_easy_setopt(curl_handle, CURLOPT_NOBODY, 1L);
				curl_easy_setopt(curl_handle, CURLOPT_HEADER, 1L);

				r = curl_easy_perform(curl_handle);
				if (r != CURLE_OK)
				{
					continue;
				}

				curl_easy_setopt(curl_handle, CURLOPT_NOBODY, 0L);
				curl_easy_setopt(curl_handle, CURLOPT_HEADER, 0L);

				fseek(f, uploaded_len, SEEK_SET);

				curl_easy_setopt(curl_handle, CURLOPT_APPEND, 1L);
			}
			else { /* no */
				curl_easy_setopt(curl_handle, CURLOPT_APPEND, 0L);
			}

			r = curl_easy_perform(curl_handle);
		}

		fclose(f);


		curl_easy_cleanup(curl_handle);
		curl_global_cleanup();

		if (r == CURLE_OK)
			bRet = true;
		else {
			fprintf(stderr, "%s\n", curl_easy_strerror(r));
		}

		return bRet;
	}
}
