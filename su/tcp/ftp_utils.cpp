//
// Created by dell on 2023/7/10.
//

#include <string>
#include <cstring>
#include "ftp_utils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <filesystem>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif


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
	memcpy(info->cache + info->size - size, ptr, size);
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


static size_t ftp_upload_read_callback(char* ptr, size_t size, size_t nmemb, void* stream)
{
	unsigned long nread;
	/* in real-world cases, this would probably get this data differently
	   as this fread() stuff is exactly what the library already would do
	   by default internally */

	size_t retcode = fread(ptr, size, nmemb, (FILE*)stream);

	if (retcode > 0) {
		nread = (unsigned long)retcode;
		fprintf(stderr, "*** We read %lu bytes from file\n", nread);
	}

	return retcode;
}

ftp_file_info_list ftp_utils::get_ftp_info(const std::string& url, const std::string& user, const std::string& passwd, ftp_file_info_callback cbfunc)
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
		std::string user_pwd = user + ":" + passwd;
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
	bool status = false;
	CURL* curl;
	CURLcode res;
	FILE* hd_src;
	struct stat file_info;
	unsigned long fsize;

	struct curl_slist* headerlist = NULL;
	/*static const char buf_1[] = "RNFR " UPLOAD_FILE_AS;
	static const char buf_2[] = "RNTO " RENAME_FILE_TO;*/

	std::filesystem::path sp(localFileFath);
	// const char* file_name = sp.filename().string();
	std::string UPLOAD_FILE_AS = "RNFR ";
	UPLOAD_FILE_AS.append(sp.filename().string());
	std::string RENAME_FILE_TO = "RNTO";
	RENAME_FILE_TO.append(sp.filename().string());


	/* get the file size of the local file */
	if (stat(localFileFath.c_str(), &file_info)) {
		printf("Couldn't open '%s': %s\n", localFileFath.c_str(), strerror(errno));
		return status;
	}
	fsize = (unsigned long)file_info.st_size;

	printf("Local file size: %lu bytes.\n", fsize);

	/* get a FILE * of the same file */
	hd_src = fopen(localFileFath.c_str(), "rb");

	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */
	curl = curl_easy_init();
	if (curl) {
		/* build a list of commands to pass to libcurl */
		headerlist = curl_slist_append(headerlist, UPLOAD_FILE_AS.c_str());
		headerlist = curl_slist_append(headerlist, RENAME_FILE_TO.c_str());
		std::string user_pwd = user + ":" + passwd;
		//curl_easy_setopt(curl, CURLOPT_USERPWD, "bjtzxxxkjyxgs:Tzx1@3$");
		curl_easy_setopt(curl, CURLOPT_USERPWD, user_pwd.c_str());

		/* we want to use our own read function */
		// curl_easy_setopt(curl, CURLOPT_READFUNCTION, ftp_upload_read_callback);

		/* enable uploading */
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

		/* specify target */
		curl_easy_setopt(curl, CURLOPT_URL, remote_file_path.c_str());

		/* pass in that last of FTP commands to run after the transfer */
		//curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

		/* now specify which file to upload */
		curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);

		curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);

		/* Set the size of the file to upload (optional).  If you give a *_LARGE
		   option you MUST make sure that the type of the passed-in argument is a
		   curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
		   make sure that to pass in a type 'long' argument. */
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
			(curl_off_t)fsize);

		/* Now run off and do what you have been told! */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
		}
		else
		{
			status = true;
		}
			
		/* clean up the FTP commands list */
		curl_slist_free_all(headerlist);

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	fclose(hd_src); /* close the local file */

	curl_global_cleanup();
	return status;
}
