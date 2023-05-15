#include "EnginePCH.h"
#include "FileSystem.h"
#include "../Path/PathManager.h"


bool FileSystem::FileExists(const fs::path& InPath)
{
	return fs::exists(InPath) && fs::is_regular_file(InPath);
}

bool FileSystem::FolderExists(const fs::path& InPath)
{
	return fs::exists(InPath) && fs::is_directory(InPath);
}

bool FileSystem::LoadJsonFile(fs::path InPath, Json& OutResult)
{	
	if (!SanitizeAssetPath(OUT InPath) || !FileExists(InPath))
		return false;
	

	std::ifstream file(InPath);

	OutResult = {};
	
	file >> OutResult;	
	return true;
}

bool FileSystem::SaveJsonFile(fs::path InPath, const Json& data, int32 indent /*= -1*/)
{
	if (!SanitizeAssetPath(OUT InPath))
		return false;

	std::ofstream file(InPath);
	if (indent >= 0)
		file << std::setw(indent) << data;
	else
		file << data;
	
	return true;
}

bool FileSystem::LoadStringFile(fs::path InPath, std::string& OutResult)
{
	if (!SanitizeAssetPath(OUT InPath) || !FileExists(InPath))
		return false;

	std::ifstream file(InPath);

	// Clean string and reserve it
	OutResult = {};
	file.seekg(0, std::ios::end);
	OutResult.reserve(int32(file.tellg()));
	file.seekg(0, std::ios::beg);	

	
	return !OutResult.empty();

}

bool FileSystem::SaveStringFile(fs::path InPath, const std::string& InData)
{
	if (!SanitizeAssetPath(OUT InPath))
		return false;

	std::ofstream file(InPath);
	file.write(InData.data(), InData.size());

	return true;
}

fs::path FileSystem::GetAssetsPath()
{	
	//TODO : Path 시작위치 확인
	//fs::path AssetPath= fs::current_path().parent_path().parent_path();

	const SPathInfo* pathInfo =g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);

	fs::path AssetPath = pathInfo->aPath;

	AssetPath = AssetPath.parent_path();
	

	return AssetPath;
}

void FileSystem::RelativeToAssetsPath(fs::path& OutPath)
{
	if (OutPath.is_absolute())
	{
		OutPath = fs::relative(OutPath, GetAssetsPath());
	}
}

fs::path FileSystem::FindMetaFile(fs::path InPath)
{
	if (!SanitizeAssetPath(OUT InPath))
		return InPath;

	if (InPath.extension() != ".meta")
		InPath += ".meta";

	return InPath;
}

fs::path FileSystem::FindRawFile(fs::path InPath)
{
	if (!SanitizeAssetPath(OUT InPath))
		return {};

	const fs::path stem = InPath.stem();
	if (!stem.has_extension() || InPath.extension() != ".meta")
		return {};

	return InPath.parent_path() / stem;
}

bool FileSystem::IsAssetPath(fs::path InPath)
{
	if (InPath.is_relative())
		return true;

	const fs::path assets = GetAssetsPath();
	const fs::path rootPath = InPath.root_path();

	

	// Iterate parent directories to find Assets path
	for (; InPath != rootPath; InPath = InPath.parent_path())
	{
		if (InPath == assets)
		{
			return true;
		}
	}

	return false;
}

bool FileSystem::newDirectory(const fs::path& TargetDir, const std::string& InName)
{
	
	fs::path dir = TargetDir;
	dir.append(InName);
	int32 cnt = 0;

	while(FileSystem::FolderExists(dir))
	{
		dir = TargetDir;
		dir.append(InName + std::to_string(cnt));

		cnt++;
		
	}
	fs::create_directory(dir);

	return true;
}

bool FileSystem::newDirectoryFromPath(const fs::path& TargetDir)
{
	fs::path dir = TargetDir;		

	if(FileSystem::FolderExists(dir))
	{
		return false;
	}
	fs::create_directories(dir);


	return true;
}

fs::path FileSystem::GetNewFileDirectory_Indexed(const fs::path& TargetDir, const std::string& InName)
{
	fs::path dir = TargetDir;
	dir.append(InName);	
	dir.replace_extension(".meta");
	int32 cnt = 1;

	while (FileSystem::FileExists(dir))
	{
		std::string FileName = InName + "_" + std::to_string(cnt);
		dir = TargetDir;	
		dir.append(FileName);
		dir.replace_extension(".meta");
		cnt++;

	}	
	
	return dir;
}



fs::path FileSystem::GetNewFileDirectory(const fs::path& TargetDir, const std::string& InName)
{
	fs::path dir = TargetDir;
	dir.append(InName);
	dir.replace_extension(".meta");

	return dir;
}

int32 FileSystem::DirectoryFileCount(const fs::path& TargetDir, std::vector<fs::path>& OutFiles)
{

	if (!FileSystem::FolderExists(TargetDir))
		return 0;

	int32 Count = 0;
	for (auto& directoryEntry : std::filesystem::directory_iterator(TargetDir))
	{
		if(FileExists(directoryEntry.path()))
		{
			OutFiles.push_back(directoryEntry.path());
			Count++;
		}
		else if(FolderExists(directoryEntry.path()))
		{
			Count += DirectoryFileCount(directoryEntry.path(), OUT OutFiles);
		}
		
	}

	return Count;
}

//ResourceRoot에 lexical 을 걸어서 쓰자..
std::string FileSystem::GetRelativePath(const fs::path& InFullPath)
{
	const SPathInfo* resRoot = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
	fs::path enginePath = resRoot->wPath;
	enginePath = InFullPath.lexically_relative(enginePath);

	enginePath.replace_filename("");

	return enginePath.string();

	//EnginePath가있을떄까지 계속위로올리고
	//EnginePath/rPath/rPathzxc/
	// /rPath/rPathzxc/<<이부분만따로 저장
//InEnginePath.replace_filename("");

	//std::vector<SPathInfo*> EnginePath;
	//g_Engine->Get<PathManager>()->GetAllEnginePath(OUT EnginePath);

	//fs::path InEnginePath;
	//int32 MinPathLength = INT_MAX;
	//
	//for(size_t i=0;i<EnginePath.size();i++)
	//{
	//	fs::path enginePath = EnginePath[i]->wPath;
	//	enginePath  = InFullPath.lexically_relative(enginePath);

	//	int32 pathSize = (int32)enginePath.string().size();
	//	if (pathSize < MinPathLength)
	//	{
	//		InEnginePath = enginePath;
	//		MinPathLength = pathSize;
	//	}
	//}
	//
	//fs::path relateivePath = m_Path;
	//fs::path parent = relateivePath.parent_path();
	//FileSystem::GetAssetsPath()
		//EnginePath가있을떄까지 계속위로올리고
		//EnginePath/rPath/rPathzxc/
		// /rPath/rPathzxc/<<이부분만따로 저장
	//InEnginePath.replace_filename("");


	//return InEnginePath.string();
}
