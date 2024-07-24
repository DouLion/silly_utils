//#pragma once
//
//#include <iostream>
//#include <filesystem>
//#include <vector>
//
//#define DIRENT_SEARCH_SUFFIX "*"
//
///** @class TFF_FileUtils
//        @brief The tool class of file operations.
//        @note
//        @attention
//*/
//class FileUtils
//{
//  public:
//    /// Whether is directory.
//    //	static std::filesystem::path GetPath(const char* path);
//    //	static std::filesystem::path GetPath(const std::string& path);
//    //	static std::filesystem::path GetPath(const std::filesystem::path& path);
//    //	static std::string GetPathStr(const char* path);
//    //	static std::string GetPathStr(const std::string& path);
//    //	static std::string GetPathStr(const std::filesystem::path& path);
//    static bool IsDirectory(const char* dirPath);
//    static bool IsDirectory(const std::string& dirPath);
//    /// Whether is regular file.
//    static bool IsRegularFile(const char* filePath);
//    static bool IsRegularFile(const std::string& filePath);
//    /// Whether is symlink file.
//    static bool IsSymlinkFile(const char* filePath);
//    static bool IsSymlinkFile(const std::string& filePath);
//    /// Get system current path of program running.
//    static std::string GetInitialPath();
//    /// Get system curren path.
//    static std::string GetCurrentPath();
//    /// Get file last write time.
//    static void GetLastWriteTime(const char* filePath, std::filesystem::file_time_type& time);
//    static void GetLastWriteTime(const std::string& filePath, std::filesystem::file_time_type& time);
//    /// Modify last write time of given path.
//    static bool ModifyLastWriteTime(const char* dstPath, const std::filesystem::file_time_type& time);
//    static bool ModifyLastWriteTime(const std::string& dstPath, const std::filesystem::file_time_type& time);
//    /// Get space info of given path.
//    static std::filesystem::space_info GetSpaceInfo(const char* dstPath);
//    static std::filesystem::space_info GetSpaceInfo(const std::string& dstPath);
//    /// Whether directory or file is existent.
//    static bool IsExists(const char* path, const bool bDir = true);
//    static bool IsExists(const std::string& path, const bool bDir = true);
//    /// Get given file size.
//    static int GetFileSize(const char* filePath);
//    static int GetFileSize(const std::string& filePath);
//    /// Get parent directory of current path.
//    static std::string GetParentDirectory(const char* currentPath);
//    static std::string GetParentDirectory(const std::string& currentPath);
//    /// Create directory.(Support create cascaded directory)
//    static bool Mkdir(const char* dirPath);
//    static bool Mkdir(const std::string& dirPath);
//    /// Extract directory.
//    static std::string ExtractDirectory(const char* filePath);
//    static std::string ExtractDirectory(const std::string& filePath);
//    /// Get leaf.
//    static std::string GetLeaf(const char* dstPath);
//    static std::string GetLeaf(const std::string& dstPath);
//    /// List given suffix files of given directory and subdirectory.
//    static void ListFilesRecursive(const char* currentPath, const std::string& filter, std::vector<std::string>& filePathVec);
//    static void ListFilesRecursive(const std::string& currentPath, const std::string& filter, std::vector<std::string>& filePathVec);
//    /// List directory of given directory and subdirectory.
//    static void ListDirsRecursive(const char* currentPath, std::vector<std::string>& dirPathVec);
//    static void ListDirsRecursive(const std::string& currentPath, std::vector<std::string>& dirPathVec);
//    /// List given suffix files of given directory.
//    static void ListFiles(const char* currentPath, const std::string& filter, std::vector<std::string>& filePathVec);
//    static void ListFiles(const std::string& currentPath, const std::string& filter, std::vector<std::string>& filePathVec);
//    static void ListFiles(const std::string& currentPath, const std::string& givenFileName, const std::string& filter, std::vector<std::string>& filePathVec, const bool bSplitName = true);
//    /// List directory of given directory.
//    static void ListDirs(const char* currentPath, std::vector<std::string>& dirPathVec);
//    static void ListDirs(const std::string& currentPath, std::vector<std::string>& dirPathVec);
//    /// Copy file.
//    static bool CopyFile(const char* srcFilePath, const char* dstFilePath, const bool bCover = true);
//    static bool CopyFile(const std::string& srcFilePath, const std::string& dstFilePath, const bool bCover = true);
//    /// Move file to target directory.
//    static bool MoveFile(const char* srcPath, const char* dstPath);
//    static bool MoveFile(const std::string& srcPath, const std::string& dstPath);
//    /// Copy directory.
//    static bool CopyDirectory(const char* srcDirPath, const char* dstDirPath, const std::string& filter = DIRENT_SEARCH_SUFFIX);
//    static bool CopyDirectory(const std::string& srcDirPath, const std::string& dstDirPath, const std::string& filter = DIRENT_SEARCH_SUFFIX);
//    /// Move directory.
//    static bool MoveDirectory(const char* srcDirPath, const char* dstDirPath, const std::string& filter = DIRENT_SEARCH_SUFFIX);
//    static bool MoveDirectory(const std::string& srcDirPath, const std::string& dstDirPath, const std::string& filter = DIRENT_SEARCH_SUFFIX);
//    // Delete given directory.
//    static bool DeleteDirectory(const char* dirPath);
//    static bool DeleteDirectory(const std::string& dirPath);
//    /// Delete given file.
//    static bool DelFile(const char* filePath);
//    static bool DelFile(const std::string& filePath);
//    /// Delete given file of given directory.
//    static bool DelGivenFile(const std::string& dirPath, const std::string& givenFileName, const std::string& filter);
//    /// Rename file or directory.
//    static bool Rename(const char* srcPath, const char* dstPath);
//    static bool Rename(const std::string& srcPath, const std::string& dstPath);
//    /// Read the entire file.
//    static bool ReadAll(const char* filePath, std::string& fileContent);
//    static bool ReadAll(const std::string& filePath, std::string& fileContent);
//    static bool ReadAll(const char* filePath, char** fileContent, int& fileSize, bool bTxt = false);
//    static bool ReadAll(const std::string& filePath, char** fileContent, int& fileSize, bool bTxt = false);
//    /// Write file.
//    static bool WriteAll(const char* filePath, std::string& fileContent);
//    static bool WriteAll(const std::string& filePath, std::string& fileContent);
//    static bool WriteAll(const char* filePath, char* fileContent, int fileSize);
//    static bool WriteAll(const std::string& filePath, char* fileContent, int fileSize);
//};
