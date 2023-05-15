#include "EnginePCH.h"
#include "PathManager.h"
#include "../FileSystem/FileSystem.h"

PathManager::PathManager()
{

}

PathManager::~PathManager()
{
	for(auto each : m_mapPath)
	{
		SAFE_DELETE((each).second);
	}
}

bool PathManager::Initialize()
{
	SPathInfo* rootPath = new SPathInfo;

	rootPath->wPath = GetExePath();
	rootPath->aPath =WStringToString(rootPath->wPath);

	m_mapPath.insert(std::make_pair(ROOT_PATH, rootPath));


	
	AddPath(RESOURCE_ROOT_PATH, TEXT("\\Resources\\"));
	AddPath(SETTING_PATH, TEXT("\\Settings\\"));	

	AddPath(TEXTURE_PATH, TEXT("\\Resources\\Texture\\"));
	AddPath(SHADER_PATH, TEXT("\\Resources\\Shader\\"));
	AddPath(SOUND_PATH, TEXT("\\Resources\\Sound\\"));
	AddPath(FONT_PATH, TEXT("\\Resources\\Font\\"));

	AddPath(MESH_PATH, TEXT("\\Resources\\Mesh\\"));
	AddPath(MATERIAL_PATH, TEXT("\\Resources\\Material\\"));
	AddPath(SKELETON_PATH, TEXT("\\Resources\\Skeleton\\"));

	AddPath(ANIMATION_PATH, TEXT("\\Resources\\Animation\\"));

	AddPath(SCENE_PATH, TEXT("\\Resources\\Scene\\"));	

	AddPath(GAMEOBJECT_PATH, TEXT("\\Resources\\GameObject\\"));	

	AddPath(ABILITYSYSTEM_PATH, TEXT("\\Resources\\GameplayAbility\\"));

	AddPath(SCENEINFO_PATH, TEXT("\\Resources\\SceneInfo\\"));	


	AddPath(AI_PATH, TEXT("\\Resources\\AI\\"));


	AddPath(WIDGET_PATH, TEXT("\\Resources\\Widget\\"));	
	AddPath(PARTICLE_PATH, TEXT("\\Resources\\Particle\\"));


	//AddPath(SPRITE_PATH, TEXT("\\Resources\\Sprite\\"));
	//AddPath(FLIPBOOK_PATH, TEXT("\\Resources\\FlipBook\\"));
	
	
	
	
	
	//Editor Only
	if(g_Engine->IsEditMode())
		AddPath(EDIT_RES_PATH, TEXT("\\EditorRes\\"));
	


	return true;
}

bool PathManager::AddPath(const std::string& InName, const std::wstring& InPath, const std::string& BasePathName /*= ROOT_PATH*/)
{
	if (FindPath(InName))
		return false;

	const SPathInfo* basePath = FindPath(BasePathName);


	

	SPathInfo* pathInfo = new SPathInfo;


	if (basePath)
	{
		pathInfo->wPath = basePath->wPath;
		pathInfo->aPath = basePath->aPath;
	}


	pathInfo->wPath += InPath;
	pathInfo->aPath += WStringToString(InPath);

	m_mapPath.insert(std::make_pair(InName, pathInfo));

	return true;
}

const SPathInfo* PathManager::FindPath(const std::string& InName) const
{
	FPathMap::const_iterator iter = m_mapPath.find(InName);

	if (iter != m_mapPath.cend())
		return iter->second;

	return nullptr;
}



bool PathManager::IsEnginePath(const fs::path& InPath) const
{	
	FPathMap::const_iterator iter = m_mapPath.cbegin();
	FPathMap::const_iterator End = m_mapPath.cend();

	for(;iter!= End;iter++)
	{
		std::string compare = FileSystem::FromString(iter->second->aPath).parent_path().string();
		
		
		if(InPath.string() == compare)
		{
			return true;
		}		
	}
	return false;

}

int32 PathManager::GetAllEnginePath(std::vector<SPathInfo*>& OutPath)
{
	OutPath.clear();

	FPathMap::const_iterator iter = m_mapPath.cbegin();
	FPathMap::const_iterator End = m_mapPath.cend();

	for (; iter != End; iter++)
	{
		OutPath.push_back(iter->second);
	}

	return (int32)OutPath.size();
}

void PathManager::MakeFullPath(const std::string& InPathKey, const std::string& InPath, std::string& OutFullPath)
{
	const SPathInfo* pathInfo = FindPath(InPathKey);

	OutFullPath = pathInfo->aPath + InPath;
	
}

std::wstring PathManager::GetExePath()
{
	WCHAR buffer[MAX_PATH + 1] = { 0, };
	if (GetModuleFileNameW(nullptr, buffer, MAX_PATH))
	{
		if (LPWSTR p = wcsrchr(buffer, L'\\'))
		{
			*p = L'\0';
		}

		return buffer;
	}

	return L"./";
}
