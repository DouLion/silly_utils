//////////////////////////////////////////////////////////////////////////////////////
////
////  Copyright(c)2003-2017 北京天智祥信息科技有限公司(http://www.tianzhixiang.net)
////  All rights reserved.
////
////	@file       TFF_FileUtils.cpp
////	@brief		TFF_FileUtils source file
////	@author		gaoyang
////	@date		2017-02-09
////  @version
////    - v1.0	2017-02-09	gaoyang
////
//////////////////////////////////////////////////////////////////////////////////////
//#include <boost/foreach.hpp>
//#include <boost/progress.hpp>
//#include <boost/algorithm/string.hpp>
//#include <boost/xpressive/xpressive_dynamic.hpp>
//#include <boost/filesystem/operations.hpp>
//#include "encode/silly_encode.h"
//#include "TFF_FileUtils.h"
//
//using namespace boost::xpressive;
//using namespace std::filesystem;
//// std::filesystem::path FileUtils::GetPath(const std::string& path)
////{
////	std::filesystem::path fspath(path);
////	return GetPath(fspath);
////}
//// std::filesystem::path FileUtils::GetPath(const std::filesystem::path& path)
////{
////	bool isAbs = false;	// 是否是绝对路径
////	std::string filepathstr = path.string();
////#if IS_WIN32
////	for (int i = 0; i < filepathstr.size(); ++i)
////	{
////		if (filepathstr[i]== ':') // 文件名中存在盘符符号,window也不允许文件名中存在 \ / : * | ? " < >特殊字符
////		{
////			isAbs = true;
////			break;
////		}
////	}
////#elif IS_LINUX
////	if (!filepathstr.empty())
////	{
////		if (filepathstr[0] == '/')	//
////		{
////			isAbs = true;
////		}
////	}
////#endif
////	if (isAbs)
////	{
////		return std::filesystem::path(filepathstr);
////	}
////	std::filesystem::path fspath(DEFAULT_ROOT_DIR);
////	fspath.append(filepathstr);
////	return fspath;
////}
////
//// std::filesystem::path FileUtils::GetPath(const char* path)
////{
////	std::filesystem::path fspath(path);
////	return GetPath(fspath);
////}
////
//// std::string FileUtils::GetPathStr(const std::string& path)
////{
////	return GetPath(path).string();
////}
//// std::string FileUtils::GetPathStr(const std::filesystem::path& path)
////{
////	return GetPath(path).string();
////}
////
//// std::string FileUtils::GetPathStr(const char* path)
////{
////	return GetPath(path).string();
////}
//
//bool FileUtils::IsDirectory(const char* dirPath)
//{
//#if defined(IS_WIN32)
//#pragma region C
//#endif
//
//    // C.
//    // struct _stat f_stat;
//    // bool ret = false;
//    // if (_stat(dirPath, &f_stat) == 0)
//    //{
//    //    ret = (S_IFDIR & f_stat.st_mode) == S_IFDIR;
//    //}
//    // return ret;
//
//#if defined(IS_WIN32)
//#pragma endregion
//#endif
//
//#if defined(IS_WIN32)
//#pragma region Boost
//#endif
//
//    try
//    {
//#ifdef UNICODE
//        std::wstring dirPathWStr = silly_encode::cxx11_string_wstring(dirPath);
//        bool bRet = std::filesystem::is_directory(std::filesystem::path(dirPathWStr));
//        return bRet;
//#else
//        bool bRet = std::filesystem::is_directory(std::filesystem::path(dirPath));
//        return bRet;
//#endif
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        return false;
//    }
//
//#if defined(IS_WIN32)
//#pragma endregion
//#endif
//}
//
//bool FileUtils::IsDirectory(const std::string& dirPath)
//{
//    return IsDirectory(dirPath.c_str());
//}
//
//bool FileUtils::IsRegularFile(const char* filePath)
//{
//    // Boost.
//    try
//    {
//#ifdef UNICODE
//        std::wstring filePathWStr = silly_encode::cxx11_string_wstring(filePath);
//        bool bRet = std::filesystem::is_regular_file(std::filesystem::path(filePathWStr));
//        return bRet;
//#else
//        bool bRet = std::filesystem::is_regular_file(std::filesystem::path(filePath));
//        return bRet;
//#endif
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//        return false;
//    }
//}
//
//bool FileUtils::IsRegularFile(const std::string& filePath)
//{
//    return IsRegularFile(filePath.c_str());
//}
//
//bool FileUtils::IsSymlinkFile(const char* filePath)
//{
//    // Boost.
//    try
//    {
//#ifdef UNICODE
//        std::wstring filePathWStr = silly_encode::cxx11_string_wstring(filePath);
//        bool bRet = std::filesystem::is_symlink(std::filesystem::path(filePathWStr));
//        return bRet;
//#else
//        bool bRet = std::filesystem::is_symlink(std::filesystem::path(filePath));
//        return bRet;
//#endif
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//        return false;
//    }
//}
//
//bool FileUtils::IsSymlinkFile(const std::string& filePath)
//{
//    return IsSymlinkFile(filePath.c_str());
//}
//
//std::string FileUtils::GetInitialPath()
//{
//    // Boost.
//    try
//    {
//        return "";  // std::filesystem::initial_path().string();
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//        return "";
//    }
//}
//
//std::string FileUtils::GetCurrentPath()
//{
//    // Boost.
//    try
//    {
//        return std::filesystem::current_path().string();
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//        return "";
//    }
//}
//
//void FileUtils::GetLastWriteTime(const char* filePath, std::filesystem::file_time_type& time)
//{
//    // Boost.
//    //	try
//    //	{
//    //#ifdef UNICODE
//    //	std::wstring filePathWStr = silly_encode::cxx11_string_wstring(filePath);
//    //	time = std::filesystem::last_write_time(std::filesystem::path(filePathWStr));
//    //#else
//    //	time = std::filesystem::last_write_time(std::filesystem::path(filePath));
//    //#endif
//    //	}
//    //	catch(std::filesystem::filesystem_error& err)
//    //	{
//    //
//    //	}
//}
//
//void FileUtils::GetLastWriteTime(const std::string& filePath, std::filesystem::file_time_type& time)
//{
//    GetLastWriteTime(filePath.c_str(), time);
//}
//
//bool FileUtils::ModifyLastWriteTime(const char* dstPath, const std::filesystem::file_time_type& time)
//{
//    // Boost.
//    try
//    {
//#ifdef UNICODE
//        std::wstring dstPathWStr = silly_encode::cxx11_string_wstring(dstPath);
//        std::filesystem::last_write_time(std::filesystem::path(dstPathWStr), time);
//#else
//        std::filesystem::last_write_time(std::filesystem::path(dstPath), time);
//#endif
//
//        return true;
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//        return false;
//    }
//}
//
//bool FileUtils::ModifyLastWriteTime(const std::string& dstPath, const std::filesystem::file_time_type& time)
//{
//    return ModifyLastWriteTime(dstPath.c_str(), time);
//}
//
//std::filesystem::space_info FileUtils::GetSpaceInfo(const char* dstPath)
//{
//    // Boost.
//    std::filesystem::space_info si;
//    std::error_code _Ec;
//    try
//    {
//#ifdef UNICODE
//        std::wstring dstPathWStr = silly_encode::cxx11_string_wstring(dstPath);
//        si = std::filesystem::space(std::filesystem::path(dstPathWStr), _Ec);
//#else
//        si = std::filesystem::space(std::filesystem::path(dstPath));
//#endif
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//    }
//
//    return si;
//}
//
//std::filesystem::space_info FileUtils::GetSpaceInfo(const std::string& dstPath)
//{
//    return GetSpaceInfo(dstPath.c_str());
//}
//
//bool FileUtils::IsExists(const char* path, const bool bDir)
//{
//    try
//    {
//        std::filesystem::path pathFile;
//#ifdef UNICODE
//        std::wstring pathWStr = silly_encode::cxx11_string_wstring(path);
//        pathFile = pathWStr;
//#else
//        pathFile = path;
//#endif
//
//        if (!bDir)
//        {
//            if (std::filesystem::exists(pathFile) && std::filesystem::is_regular_file(pathFile))
//            {
//                return true;
//            }
//        }
//        else
//        {
//            if (std::filesystem::exists(pathFile) && std::filesystem::is_directory(pathFile))
//            {
//                return true;
//            }
//        }
//
//        return false;
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//        return false;
//    }
//}
//
//bool FileUtils::IsExists(const std::string& path, const bool bDir)
//{
//    return IsExists(path.c_str(), bDir);
//}
//
//int FileUtils::GetFileSize(const char* filePath)
//{
//    // C.
//    // int ret = 0;
//    // FILE* pFileStream = fopen(filePath, "rb");
//    // if (pFileStream)
//    //{
//    //	int res = fseek(pFileStream, 0, SEEK_END);
//    //	if (0 == res)
//    //	{
//    //		ret = ftell(pFileStream);
//    //	}
//    //}
//    // return ret;
//
//    // struct _stat f_stat;
//    // if (_stat(filePath, &f_stat) == -1)
//    //{
//    //	return 0;
//    //}
//    // return (int)f_stat.st_size;
//
//    // Boost.
//    try
//    {
//#ifdef UNICODE
//        std::wstring filePathWStr = silly_encode::cxx11_string_wstring(filePath);
//        std::filesystem::path path(filePathWStr);
//        return (int)std::filesystem::file_size(path);
//#else
//        std::filesystem::path path(filePath);
//        return (int)std::filesystem::file_size(path);
//#endif
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//        return 0;
//    }
//}
//
//int FileUtils::GetFileSize(const std::string& filePath)
//{
//    return GetFileSize(filePath.c_str());
//}
//
//std::string FileUtils::GetParentDirectory(const char* currentPath)
//{
//#if BOOST_VERSION > 104100
//    try
//    {
//#ifdef UNICODE
//        std::wstring currentPathWStr = silly_encode::cxx11_string_wstring(currentPath);
//        std::filesystem::path path(currentPathWStr);
//        return path.parent_path().string();
//#else
//        std::filesystem::path path(currentPath);
//        return path.parent_path().string();
//#endif
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//        return "";
//    }
//#else
//    return "";
//#endif
//}
//
//std::string FileUtils::GetParentDirectory(const std::string& currentPath)
//{
//    return GetParentDirectory(currentPath.c_str());
//}
//
//bool FileUtils::Mkdir(const char* dirPath)
//{
//    try
//    {
//        bool bRet = true;
//        if (!IsExists(dirPath))
//        {
//#ifdef UNICODE
//            std::wstring dirPathWStr = silly_encode::cxx11_string_wstring(dirPath);
//            bRet = std::filesystem::create_directories(std::filesystem::path(dirPathWStr));
//#else
//            bRet = std::filesystem::create_directories(std::filesystem::path(dirPath));
//#endif
//        }
//
//        return bRet;
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//        return false;
//    }
//}
//
//bool FileUtils::Mkdir(const std::string& dirPath)
//{
//    return Mkdir(dirPath.c_str());
//}
//
//std::string FileUtils::ExtractDirectory(const char* filePath)
//{
//    std::string ret = "";
//
//    try
//    {
//#if BOOST_VERSION > 104100
//        if (!IsDirectory(filePath))
//        {
//            std::deque<std::string> tokens;
//            std::string path = filePath;
//            boost::replace_all(path, "\\", "/");
//            // std::transform(path.begin(),path.end(),path.begin(),tolower);
//            path = boost::to_lower_copy(path);
//            boost::split(tokens, path, boost::algorithm::is_any_of("/"));
//            if (tokens.size() > 0)
//            {
//                tokens.pop_back();
//                ret = boost::algorithm::join(tokens, "/");
//            }
//        }
//#endif
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//    }
//
//    return ret;
//}
//
//std::string FileUtils::ExtractDirectory(const std::string& filePath)
//{
//    return ExtractDirectory(filePath.c_str());
//}
//
//std::string FileUtils::GetLeaf(const char* dstPath)
//{
//#ifdef UNICODE
//    std::wstring dstPathWStr = silly_encode::cxx11_string_wstring(dstPath);
//    std::filesystem::path p(dstPathWStr);
//    return p.filename().string();
//#else
//    std::filesystem::path p(dstPath);
//    return p.filename().string();
//#endif
//}
//
//std::string FileUtils::GetLeaf(const std::string& dstPath)
//{
//    return GetLeaf(dstPath.c_str());
//}
//
//void FileUtils::ListFilesRecursive(const char* currentPath, const std::string& filter, std::vector<std::string>& filePathVec)
//{
//    //	try
//    //	{
//    //		filePathVec.clear();
//    //
//    //#if BOOST_VERSION > 104100
//    //		std::filesystem::path currPath;
//    //		std::filesystem::path filterPath;
//    //
//    //#ifdef UNICODE
//    //		std::wstring currentPathWStr = silly_encode::cxx11_string_wstring(currentPath);
//    //		currPath = currentPathWStr;
//    //		std::wstring filterWStr = silly_encode::cxx11_string_wstring(filter);
//    //		filterPath = currPath / filterWStr;
//    //#else
//    //		currPath = currentPath;
//    //		filterPath = currPath / filter;
//    //#endif
//    //
//    //		std::filesystem::path filterDir1 = filterPath.parent_path();
//    //		std::string filename = filterPath.filename().string();
//    //		// 转换文件通配符为正则表达式.
//    //		std::string str = boost::replace_all_copy(boost::replace_all_copy(filename, ".", "\\."), "*", ".*");
//    //		sregex reg = sregex::compile(str);
//    //		if (!std::filesystem::exists(filterDir1) || !std::filesystem::is_directory(filterDir1))
//    //		{
//    //			return;
//    //		}
//    //
//    //		// 查找当前目录及子目录的所有文件，如果符合正则表达式则添加到列表中.
//    //		rd_iterator end;
//    //		for (rd_iterator pos(filterDir1); pos != end; pos++)
//    //		{
//    //		if (!std::filesystem::is_directory(*pos) && regex_match(pos->path().filename().string(), reg))
//    //		{
//    //			filePathVec.push_back(pos->path().string());
//    //		}
//    //	}
//    //#endif
//    //}
//    // catch(std::filesystem::filesystem_error& err)
//    //{
//    //
//    //	}
//}
//
//void FileUtils::ListFilesRecursive(const std::string& currentPath, const std::string& filter, std::vector<std::string>& filePathVec)
//{
//    ListFilesRecursive(currentPath.c_str(), filter, filePathVec);
//}
//
//void FileUtils::ListDirsRecursive(const char* currentPath, std::vector<std::string>& dirPathVec)
//{
//    //	try
//    //	{
//    //		dirPathVec.clear();
//    //
//    //#if BOOST_VERSION > 104100
//    //		std::filesystem::path currPath;
//    //
//    //		#ifdef UNICODE
//    //			std::wstring currentPathWStr = silly_encode::cxx11_string_wstring(currentPath);
//    //			currPath = currentPathWStr;
//    //		#else
//    //			currPath = currentPath;
//    //		#endif
//    //
//    //		rd_iterator end;
//    //		for (rd_iterator pos(currPath); pos != end; pos++)
//    //		{
//    //			if (std::filesystem::is_directory(*pos))
//    //			{
//    //				dirPathVec.push_back(pos->path().string());
//    //			}
//    //		}
//    //#endif
//    //	}
//    //	catch(std::filesystem::filesystem_error& err)
//    //	{
//    //
//    //	}
//}
//
//void FileUtils::ListDirsRecursive(const std::string& currentPath, std::vector<std::string>& dirPathVec)
//{
//    ListDirsRecursive(currentPath.c_str(), dirPathVec);
//}
//
//void FileUtils::ListFiles(const char* currentPath, const std::string& filter, std::vector<std::string>& filePathVec)
//{
//    try
//    {
//        filePathVec.clear();
//
//#if BOOST_VERSION > 104100
//        std::vector<std::filesystem::path> paths;
//
//        std::filesystem::path currPath;
//        std::filesystem::path filterPath;
//
//#ifdef UNICODE
//        std::wstring currentPathWStr = silly_encode::cxx11_string_wstring(currentPath);
//        currPath = currentPathWStr;
//        std::wstring filterWStr = silly_encode::cxx11_string_wstring(filter);
//        filterPath = currPath / filterWStr;
//#else
//        currPath = currentPath;
//        filterPath = currPath / filter;
//#endif
//
//        std::filesystem::path filterDir1 = filterPath.parent_path();
//        std::string filename = filterPath.filename().string();
//        // 转换文件通配符为正则表达式.
//        std::string str = boost::replace_all_copy(boost::replace_all_copy(filename, ".", "\\."), "*", ".*");
//
//#ifdef UNICODE
//        wsregex reg = wsregex::compile(silly_encode::cxx11_string_wstring(str));
//        std::copy(directory_iterator(currPath), directory_iterator(), back_inserter(paths));
//        size_t pathLen = paths.size();
//        for (unsigned int i = 0; i < pathLen; ++i)
//        {
//            std::wstring tmpFileNameWStr = silly_encode::cxx11_string_wstring(paths[i].filename().string());
//
//            if (is_regular_file(paths[i]) && regex_match(tmpFileNameWStr, reg))
//            {
//                filePathVec.push_back(paths[i].string());
//            }
//        }
//#else
//        sregex reg = sregex::compile(str);
//        std::copy(directory_iterator(currPath), directory_iterator(), back_inserter(paths));
//        size_t pathLen = paths.size();
//        for (unsigned int i = 0; i < pathLen; ++i)
//        {
//            std::string tmpFileName = paths[i].filename().string();
//
//            if (is_regular_file(paths[i]) && regex_match(tmpFileName, reg))
//            {
//                filePathVec.push_back(paths[i].string());
//            }
//        }
//#endif
//
//#endif
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//    }
//}
//
//void FileUtils::ListFiles(const std::string& currentPath, const std::string& filter, std::vector<std::string>& filePathVec)
//{
//    ListFiles(currentPath.c_str(), filter, filePathVec);
//}
//
//void FileUtils::ListFiles(const std::string& currentPath, const std::string& givenFileName, const std::string& filter, std::vector<std::string>& filePathVec, const bool bSplitName)
//{
//    try
//    {
//        filePathVec.clear();
//
//#if BOOST_VERSION > 104100
//        std::vector<std::filesystem::path> paths;
//
//        std::filesystem::path currPath;
//        std::filesystem::path filterPath;
//
//#ifdef UNICODE
//        std::wstring currentPathWStr = silly_encode::cxx11_string_wstring(currentPath);
//        currPath = currentPathWStr;
//        std::wstring filterWStr = silly_encode::cxx11_string_wstring(filter);
//        filterPath = currPath / filterWStr;
//#else
//        currPath = currentPath;
//        filterPath = currPath / filter;
//#endif
//
//        std::filesystem::path filterDir1 = filterPath.parent_path();
//        std::string filename = filterPath.filename().string();
//        // 转换文件通配符为正则表达式.
//        std::string str = boost::replace_all_copy(boost::replace_all_copy(filename, ".", "\\."), "*", ".*");
//        sregex reg = sregex::compile(str);
//        std::copy(directory_iterator(currPath), directory_iterator(), back_inserter(paths));
//        size_t pathLen = paths.size();
//        for (unsigned int i = 0; i < pathLen; ++i)
//        {
//            std::string leaf = GetLeaf(paths[i].filename().string());
//            if (bSplitName)
//            {
//                size_t index = leaf.find(".");
//                leaf = leaf.substr(0, index);
//            }
//            if (is_regular_file(paths[i]) && regex_match(paths[i].filename().string(), reg) && leaf == givenFileName)
//            {
//                filePathVec.push_back(paths[i].string());
//            }
//        }
//#endif
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//    }
//}
//
//void FileUtils::ListDirs(const char* currentPath, std::vector<std::string>& dirPathVec)
//{
//    try
//    {
//        dirPathVec.clear();
//
//#if BOOST_VERSION > 104100
//        std::vector<std::filesystem::path> paths;
//        std::filesystem::path p;
//#ifdef UNICODE
//        std::wstring currentPathWStr = silly_encode::cxx11_string_wstring(currentPath);
//        p = currentPathWStr;
//#else
//        p = currentPath;
//#endif
//        std::copy(directory_iterator(p), directory_iterator(), back_inserter(paths));
//        size_t pathLen = paths.size();
//        for (unsigned int i = 0; i < pathLen; ++i)
//        {
//            if (is_directory(paths[i]))
//            {
//                std::string dirFullpath = paths[i].string();
//                dirPathVec.push_back(dirFullpath);
//            }
//        }
//#endif
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//    }
//}
//
//void FileUtils::ListDirs(const std::string& currentPath, std::vector<std::string>& dirPathVec)
//{
//    ListDirs(currentPath.c_str(), dirPathVec);
//}
//
//bool FileUtils::CopyFile(const char* srcFilePath, const char* dstFilePath, const bool bCover)
//{
//    try
//    {
//        // If target file parent directory is not exist, make directory.
//        std::string parentDirPath = GetParentDirectory(dstFilePath);
//        if (!Mkdir(parentDirPath))
//        {
//            return false;
//        }
//
//        if (bCover)
//        {
//            DelFile(dstFilePath);
//        }
//
//#ifdef UNICODE
//        std::wstring srcFilePathWStr = silly_encode::cxx11_string_wstring(srcFilePath);
//        std::filesystem::path srcPath(srcFilePathWStr);
//        std::wstring dstFilePathWStr = silly_encode::cxx11_string_wstring(dstFilePath);
//        std::filesystem::path dstPath(dstFilePathWStr);
//        std::filesystem::copy_file(srcPath, dstPath);
//#else
//        std::filesystem::path srcPath(srcFilePath);
//        std::filesystem::path dstPath(dstFilePath);
//        std::filesystem::copy_file(srcPath, dstPath);
//#endif
//
//        return true;
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//        return false;
//    }
//}
//
//bool FileUtils::CopyFile(const std::string& srcFilePath, const std::string& dstFilePath, const bool bCover)
//{
//    return CopyFile(srcFilePath.c_str(), dstFilePath.c_str(), bCover);
//}
//
//bool FileUtils::MoveFile(const char* srcPath, const char* dstPath)
//{
//    return Rename(srcPath, dstPath);
//}
//
//bool FileUtils::MoveFile(const std::string& srcPath, const std::string& dstPath)
//{
//    return MoveFile(srcPath.c_str(), dstPath.c_str());
//}
//
//bool FileUtils::CopyDirectory(const char* srcDirPath, const char* dstDirPath, const std::string& filter)
//{
//    try
//    {
//        if (!IsDirectory(srcDirPath))
//        {
//            return false;
//        }
//
//        std::filesystem::path srcPath;
//#ifdef UNICODE
//        std::wstring srcDirPathWStr = silly_encode::cxx11_string_wstring(srcDirPath);
//        srcPath = srcDirPathWStr;
//#else
//        srcPath = srcDirPath;
//#endif
//        std::vector<std::string> filePathVec;
//        ListFilesRecursive(srcDirPath, filter, filePathVec);
//        if (filePathVec.empty())
//        {
//            Mkdir(dstDirPath);
//            return true;
//        }
//
//        std::string srcDirName = GetLeaf(srcDirPath);
//        std::filesystem::path dstPath(dstDirPath);
//#ifdef UNICODE
//        std::wstring dstDirPathWStr = silly_encode::cxx11_string_wstring(dstDirPath);
//        dstPath = dstDirPathWStr;
//#else
//        dstPath = dstDirPath;
//#endif
//        dstPath.append(srcDirName.c_str());
//#include <boost/progress.hpp>
//        std::filesystem::path tmp;
//        std::filesystem::path p;
//        boost::progress_display pd((unsigned long)filePathVec.size());
//        BOOST_FOREACH (std::string& filePath, filePathVec)
//        {
//#ifdef UNICODE
//            std::wstring filePathWStr = silly_encode::cxx11_string_wstring(filePath);
//            p = filePathWStr;
//            std::wstring nameWStr = silly_encode::cxx11_string_wstring(p.string().substr(srcPath.string().length()));
//            tmp = dstPath / nameWStr;
//#else
//            p = filePath;
//            tmp = dstPath / p.string().substr(srcPath.string().length());
//#endif
//
//            Mkdir(tmp.parent_path().string());
//
//            CopyFile(filePath, tmp.string());
//            ++pd;
//        }
//
//        return true;
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//        return false;
//    }
//}
//
//bool FileUtils::CopyDirectory(const std::string& srcDirPath, const std::string& dstDirPath, const std::string& filter)
//{
//    return CopyDirectory(srcDirPath.c_str(), dstDirPath.c_str(), filter);
//}
//
//bool FileUtils::MoveDirectory(const char* srcDirPath, const char* dstDirPath, const std::string& filter)
//{
//    try
//    {
//        if (!IsDirectory(srcDirPath))
//        {
//            return false;
//        }
//
//        std::filesystem::path srcPath;
//#ifdef UNICODE
//        std::wstring srcDirPathWStr = silly_encode::cxx11_string_wstring(srcDirPath);
//        srcPath = srcDirPathWStr;
//#else
//        srcPath = srcDirPath;
//#endif
//        std::vector<std::string> filePathVec;
//        ListFilesRecursive(srcDirPath, filter, filePathVec);
//        if (filePathVec.empty())
//        {
//            Mkdir(dstDirPath);
//            return true;
//        }
//
//        std::string srcDirName = GetLeaf(srcDirPath);
//        std::filesystem::path dstPath(dstDirPath);
//#ifdef UNICODE
//        std::wstring dstDirPathWStr = silly_encode::cxx11_string_wstring(dstDirPath);
//        dstPath = dstDirPathWStr;
//#else
//        dstPath = dstDirPath;
//#endif
//        dstPath.append(srcDirName.c_str());
//
//        std::filesystem::path tmp;
//        std::filesystem::path p;
//        boost::progress_display pd((unsigned long)filePathVec.size());
//        BOOST_FOREACH (std::string& filePath, filePathVec)
//        {
//#ifdef UNICODE
//            std::wstring filePathWStr = silly_encode::cxx11_string_wstring(filePath);
//            p = filePathWStr;
//            std::wstring nameWStr = silly_encode::cxx11_string_wstring(p.string().substr(srcPath.string().length()));
//            tmp = dstPath / nameWStr;
//#else
//            p = filePath;
//            tmp = dstPath / p.string().substr(srcPath.string().length());
//#endif
//
//            Mkdir(tmp.parent_path().string());
//
//            MoveFile(filePath, tmp.string());
//            ++pd;
//        }
//
//        return true;
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//        return false;
//    }
//}
//
//bool FileUtils::MoveDirectory(const std::string& srcDirPath, const std::string& dstDirPath, const std::string& filter)
//{
//    return MoveDirectory(srcDirPath.c_str(), dstDirPath.c_str(), filter);
//}
//
//bool FileUtils::DeleteDirectory(const char* dirPath)
//{
//    try
//    {
//        std::filesystem::path path;
//
//#ifdef UNICODE
//        std::wstring dirPathWStr = silly_encode::cxx11_string_wstring(dirPath);
//        path = dirPathWStr;
//#else
//        path = dirPath;
//#endif
//        boost::system::error_code errCode;
//        std::filesystem::remove_all(path, errCode);
//
//        return true;
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        return false;
//    }
//}
//
//bool FileUtils::DeleteDirectory(const std::string& dirPath)
//{
//    return DeleteDirectory(dirPath.c_str());
//}
//
//bool FileUtils::DelFile(const char* filePath)
//{
//    try
//    {
//        std::filesystem::path path;
//
//#ifdef UNICODE
//        std::wstring filePathWStr = silly_encode::cxx11_string_wstring(filePath);
//        path = filePathWStr;
//#else
//        path = filePath;
//#endif
//        boost::system::error_code errCode;
//        std::filesystem::remove(path, errCode);
//
//        return true;
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//        return false;
//    }
//}
//
//bool FileUtils::DelFile(const std::string& filePath)
//{
//    return DelFile(filePath.c_str());
//}
//
//bool FileUtils::DelGivenFile(const std::string& dirPath, const std::string& givenFileName, const std::string& filter)
//{
//    try
//    {
//        std::vector<std::string> filePathVec;
//        ListFiles(dirPath, givenFileName, filter, filePathVec);
//        size_t fileNum = filePathVec.size();
//        for (unsigned int i = 0; i < fileNum; ++i)
//        {
//            if (!DelFile(filePathVec[i]))
//            {
//                return false;
//            }
//        }
//
//        return true;
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//        return false;
//    }
//}
//
//bool FileUtils::Rename(const char* srcPath, const char* dstPath)
//{
//    try
//    {
//#ifdef UNICODE
//        std::wstring srcPathWStr = silly_encode::cxx11_string_wstring(srcPath);
//        std::filesystem::path src(srcPathWStr);
//        std::wstring dstPathWStr = silly_encode::cxx11_string_wstring(dstPath);
//        std::filesystem::path dst(dstPathWStr);
//        std::filesystem::rename(src, dst);
//#else
//        std::filesystem::path src(srcPath);
//        std::filesystem::path dst(dstPath);
//        std::filesystem::rename(src, dst);
//#endif
//
//        return true;
//    }
//    catch (std::filesystem::filesystem_error& err)
//    {
//        printf("err: %s", err.what());
//        return false;
//    }
//}
//
//bool FileUtils::Rename(const std::string& srcPath, const std::string& dstPath)
//{
//    return Rename(srcPath.c_str(), dstPath.c_str());
//}
//
//bool FileUtils::ReadAll(const char* filePath, std::string& fileContent)
//{
//    // C.
//    FILE* pf = fopen(filePath, "rb");
//    if (pf)
//    {
//        char buffer[1024 * 10] = {0};
//        size_t cur = fread(buffer, 1, sizeof(buffer), pf);
//        size_t pos = 0;
//        while (cur > 0)
//        {
//            fileContent.resize(fileContent.size() + cur);
//            memcpy(&fileContent[pos], buffer, cur);
//            pos += cur;
//            memset(buffer, 0, sizeof(buffer));
//            cur = fread(buffer, 1, sizeof(buffer), pf);
//        }
//        fclose(pf);
//
//        return true;
//    }
//
//    std::cerr << "Open " << filePath << "failed" << std::endl;
//
//    // C++.
//    // TODO: 补全代码.
//
//    return false;
//}
//
//bool FileUtils::ReadAll(const std::string& filePath, std::string& fileContent)
//{
//    return ReadAll(filePath.c_str(), fileContent);
//}
//
//bool FileUtils::ReadAll(const char* filePath, char** fileContent, int& fileSize, bool bTxt)
//{
//    // C.
//    FILE* pf = fopen(filePath, "rb");
//    if (pf)
//    {
//        char buffer[1024 * 10] = {0};
//        size_t cur = fread(buffer, 1, sizeof(buffer), pf);
//        size_t pos = 0;
//        size_t size = cur;
//        while (cur > 0)
//        {
//            if (nullptr == *fileContent)
//            {
//                *fileContent = (char*)malloc(size);
//            }
//            else
//            {
//                *fileContent = (char*)realloc(*fileContent, size);
//            }
//            // fileContent.resize(fileContent.size() + cur);
//            memcpy(&(*fileContent)[pos], buffer, cur);
//            pos += cur;
//            memset(buffer, 0, sizeof(buffer));
//            cur = fread(buffer, 1, sizeof(buffer), pf);
//            size += cur;
//        }
//        fileSize = (int)size;
//
//        if (bTxt)
//        {
//            fileSize = int(size) + 1;
//            *fileContent = (char*)realloc(*fileContent, fileSize);
//            *(*fileContent + size) = '\0';
//        }
//
//        fclose(pf);
//
//        return true;
//    }
//
//    // C++.
//    // TODO: 补全代码.
//
//    return false;
//}
//
//bool FileUtils::ReadAll(const std::string& filePath, char** fileContent, int& fileSize, bool bTxt)
//{
//    return ReadAll(filePath.c_str(), fileContent, fileSize, bTxt);
//}
//
//bool FileUtils::WriteAll(const char* filePath, std::string& fileContent)
//{
//    std::string dirPath = ExtractDirectory(filePath);
//    if (!Mkdir(dirPath))
//    {
//        return false;
//    }
//
//    // C.
//    FILE* pf = fopen(filePath, "wb");
//    if (pf)
//    {
//        fwrite(fileContent.c_str(), 1, fileContent.length(), pf);
//        fclose(pf);
//    }
//
//    // C++.
//    // setlocale(LC_ALL, "Chinese-simplified");
//
//    // ofstream os;
//    // os.open(filePath, ios_base::binary | ios_base::trunc);
//    // if (os.fail())
//    //{
//    //	return false;
//    //}
//    // os.write(fileContent.c_str(), fileContent.length());
//    // os.flush();
//    // os.close();
//
//    // setlocale(LC_ALL, "C");
//
//    return true;
//}
//
//bool FileUtils::WriteAll(const std::string& filePath, std::string& fileContent)
//{
//    return WriteAll(filePath.c_str(), fileContent);
//}
//
//bool FileUtils::WriteAll(const char* filePath, char* fileContent, int fileSize)
//{
//    std::string dirPath = ExtractDirectory(filePath);
//    if (!Mkdir(dirPath))
//    {
//        return false;
//    }
//
//    // C.
//    FILE* pf = fopen(filePath, "wb");
//    if (pf)
//    {
//        fwrite(fileContent, 1, fileSize, pf);
//        fclose(pf);
//    }
//
//    // C++.
//    // setlocale(LC_ALL, "Chinese-simplified");
//
//    // ofstream os;
//    // os.open(filePath, ios_base::binary | ios_base::trunc);
//    // if (os.fail())
//    //{
//    //	return false;
//    //}
//    // os.write(fileContent, fileSize);
//    // os.flush();
//    // os.close();
//
//    // setlocale(LC_ALL, "C");
//
//    return true;
//}
//
//bool FileUtils::WriteAll(const std::string& filePath, char* fileContent, int fileSize)
//{
//    return WriteAll(filePath.c_str(), fileContent, fileSize);
//}
