#pragma once

#include <fstream>
#include "../Serialization/Json.h"


class FileSystem
{
public:
	static bool FileExists(const std::string& sPath) 
	{
		return FileExists(FromString(sPath));
	}

	static bool FolderExists(const std::string& sPath)
	{
		return FolderExists(FromString(sPath));
	}

	static bool LoadJsonFile(const std::string& sPath, Json& OutResult)
	{
		return LoadJsonFile(FromString(sPath),OUT OutResult);
	}
	static bool SaveJsonFile(const std::string& sPath, const Json& data, int32 indent = -1 )
	{
		return SaveJsonFile(FromString(sPath), data, indent);
	}

	static  bool LoadStringFile(const std::string& sPath, std::string& OutResult) 
	{
		return LoadStringFile(FromString(sPath),OUT OutResult);
	}
	static  bool SaveStringFile(const std::string& sPath, const std::string& data)
	{
		return SaveStringFile(FromString(sPath), data);
	}

	static std::string GetAssetsPathStr() 
	{ 
		return ToString(GetAssetsPath());
	}

	static bool IsAssetPath(const std::string& sPath) 
	{
		return IsAssetPath(FromString(sPath));
	}

public:
	/** Path API */
	static bool FileExists(const fs::path& InPath);
	static bool FolderExists(const fs::path& InPath);


	static bool LoadJsonFile(fs::path  InPath, Json& OutResult);
	static bool SaveJsonFile(fs::path  InPath, const Json& data, int32 indent = -1);

	static bool LoadStringFile(fs::path InPath, std::string& OutResult);
	static bool SaveStringFile(fs::path InPath, const std::string& InData);

	static fs::path GetAssetsPath();
	static void RelativeToAssetsPath(fs::path& OutPath);
	//static fs::path  GetConfigPath();

	static fs::path  FindMetaFile(fs::path  InPath);

	static fs::path  FindRawFile(fs::path  InPath);

	static bool IsAssetPath(fs::path InPath);
	static bool IsAssetFilePath(const fs::path& InPath)
	{
		return IsAssetPath(InPath) && InPath.has_extension() && InPath.extension() == ".meta";
	}

	static bool newDirectory(const fs::path& TargetDir,const std::string& InName);

	//TargetDir을 생성 없으면 없는 부분을 만듬
	static bool newDirectoryFromPath(const fs::path& TargetDir);

	static fs::path GetNewFileDirectory_Indexed(const fs::path& TargetDir, const std::string& InName);

	//이름이 곂치면 덮어쓰는 버전
	static fs::path GetNewFileDirectory(const fs::path& TargetDir, const std::string& InName);

	
	static int32 DirectoryFileCount(const fs::path& TargetDir,std::vector<fs::path>& OutFiles);


	static std::string GetRelativePath(const fs::path& InFullPath);


	//Convert
	static fs::path FromString(const std::string& sPath)
	{
		return { sPath.begin(), sPath.end() };
	}
	static std::string ToString(const fs::path& InPath)
	{
		std::string pathStr = InPath.string();
		std::replace(pathStr.begin(), pathStr.end(), '\\', '/');
		return pathStr;		
	}
	//if Is Child??
	


private:

	//백 슬래시를 슬래시로 변경하고 후행 슬래시도 제거합니다.
	static inline bool SanitizeAssetPath(fs::path& OutPath)
	{
		return OutPath.has_filename() && SanitizeDirectoryPath(OUT OutPath);
	}

	static inline bool SanitizeDirectoryPath(fs::path& OutPath)
	{

		if (OutPath.is_relative())
		{
			OutPath = GetAssetsPath() / OutPath;
			return true;
		}

		// Ensure path is inside Assets folder
		return !fs::relative(OutPath, GetAssetsPath()).empty();

	}


};

