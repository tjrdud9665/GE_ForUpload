#include "EnginePCH.h"
#include "AssetManager.h"
#include "Path/PathManager.h"
#include "FileSystem/FileSystem.h"
#include "Assets/Mesh/StaticMesh.h"
#include "Assets/TextureAsset/TextureAsset.h"
#include "Assets/Mesh/MeshAsset.h"
#include "Assets/Mesh/SkeletalMeshAsset.h"
#include "Assets/Sound/Sound.h"
#include "Importer/FBXLoader.h"
#include "ASsets/Font/FontAsset.h"

AssetManager::AssetManager()
{

}

AssetManager::~AssetManager()
{
	m_LoadedAsssets.clear();
}

void AssetManager::CreateMetaFromRawFile()
{
	const SPathInfo* resRootPath = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
	//const SPathInfo* MeshPath = g_Engine->Get<PathManager>()->FindPath(MESH_PATH);
	//const SPathInfo* TexturePath = g_Engine->Get<PathManager>()->FindPath(TEXTURE_PATH);
	//const SPathInfo* SoundPath = g_Engine->Get<PathManager>()->FindPath(SOUND_PATH);

	fs::path resPath = resRootPath->aPath;
	//fs::path soundPath = SoundPath->aPath;
	//fs::path meshPath = MeshPath->aPath;


	////ReadAllTextures~~
	std::vector<fs::path> vecResfiles;
	FileSystem::DirectoryFileCount(resPath, OUT vecResfiles);



	for (size_t i = 0; i < vecResfiles.size(); i++)
	{
		if(vecResfiles[i].extension() == ".hlsl" && vecResfiles[i].extension() == ".bin")
		{
			continue;
		}
		if (vecResfiles[i].extension() != ".meta" && !fs::is_directory(vecResfiles[i]))
		{
			std::string metaPath = vecResfiles[i].string();
			metaPath.append(".meta");
			AssetInfo assetMeta(metaPath);
			
			
			if(vecResfiles[i].extension() == ".FBX" ||
				vecResfiles[i].extension() == ".fbx" )
			{
				//읽은 데이터에 Bone 정보가 있으면 SkeletalMesh
				//없으면 그냥메쉬
				fs::path rawFilePath = FileSystem::FindRawFile(FileSystem::FromString(assetMeta.GetSPath()));
				std::string staticMeshName = rawFilePath.filename().string();

				MeshAsset* meshOrSkeletalMesh = nullptr;				

				if (!rawFilePath.empty())
				{					
					FBXLoader loader;
					//Load Animation , Mesh|SkeleltalMesh  , 
					loader.LoadFBX(rawFilePath.string().c_str(), meshOrSkeletalMesh);
					//AssetData* SameFromFile = FindAssetFromFileName(rawFilePath.filename().string());
					//if(!SameFromFile)
					//{
					//	
					//	//loader.Load(meshOrSkeletalMesh, rawFilePath.string().c_str());
					//	//내부에서 FBX에 Bone이 있는 경우와없는경우를 나눠서 해당타입으로 읽고 저장
					//	//
					//}

					//Loader 에서 LoadOrCreate 를 실행
				}				
				
				//LoadOrCreate(assetMeta, MeshAsset::StaticClass());
			}
			else if (vecResfiles[i].extension() == ".png" ||
					 vecResfiles[i].extension() == ".PNG" ||
					vecResfiles[i].extension() == ".tga" ||
					 vecResfiles[i].extension() == ".TGA" ||
					 vecResfiles[i].extension() == ".DDS" ||
					 vecResfiles[i].extension() == ".HDR" ||
					 vecResfiles[i].extension() == ".hdr" ||
					 vecResfiles[i].extension() == ".dds")
			{
				LoadOrCreate(assetMeta, TextureAsset::StaticClass());
			}
			else if (vecResfiles[i].extension() == ".mp3" ||
					 vecResfiles[i].extension() == ".wav")
			{
				LoadOrCreate(assetMeta, Sound::StaticClass());
			}
			else if (vecResfiles[i].extension() == ".otf" || 
					 vecResfiles[i].extension() == ".OTF")
			{
				LoadOrCreate(assetMeta, FontAsset::StaticClass());
			}
			else
			{
				Load(assetMeta);
			}				
			
		} 
	
	}


	FAssetMap::const_iterator iter = m_LoadedAsssets.cbegin();
	FAssetMap::const_iterator End = m_LoadedAsssets.cend();
	for (; iter != End; )
	{
		if ((*iter).second->IsValidate())
		{
			(*iter).second->Save();
			iter++;
		} else
		{
			iter = m_LoadedAsssets.erase(iter);
			End = m_LoadedAsssets.cend();
		}
	}
}

void AssetManager::CreateDefaultEngineDirectory()
{

	std::vector<SPathInfo*> EnginePaths;
	int32 size = g_Engine->Get<PathManager>()->GetAllEnginePath(OUT EnginePaths);

	for (int32 i = 0; i < size; i++)
	{
		fs::path registeredPath = FileSystem::FromString(EnginePaths[i]->aPath);
		if (!FileSystem::FolderExists(registeredPath))
			fs::create_directory(registeredPath);
	}

}

void AssetManager::LoadAllMetaFiles()
{

	const SPathInfo* ResourcePath = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
	fs::path resPath = ResourcePath->aPath;

	const SPathInfo* sceneRootPath = g_Engine->Get<PathManager>()->FindPath(SCENE_PATH);
	fs::path scenePath = sceneRootPath->aPath;


	std::vector<fs::path> vecAllResfiles;
	FileSystem::DirectoryFileCount(resPath, OUT vecAllResfiles);


	//Don't Load Scene File
	



	for (size_t i = 0; i < vecAllResfiles.size(); i++)
	{		
		fs::path rel = vecAllResfiles[i].lexically_relative(scenePath);
		if(rel.string().find("..") == std::string::npos)
		{
			continue;
		}

		if (!fs::is_directory(vecAllResfiles[i]) && vecAllResfiles[i].extension() == ".meta")
		{
			std::string metaPath = vecAllResfiles[i].string();
			Load(AssetInfo(metaPath));
		}
	}

}

void AssetManager::Initialize()
{
	CreateDefaultEngineDirectory();

	if (g_Engine->IsEditMode())
	{	
		LoadAllMetaFiles();
		CreateMetaFromRawFile();
	}	

}

AssetData* AssetManager::Load(const AssetInfo& Ininfo)
{
	if (Ininfo.GetAssetNameFromFile()== ".meta")
		return nullptr;
	AssetData* loadedAsset = GetLoadedAsset(Ininfo);

	Json data;
	bool jsonLoad = false;

	if (loadedAsset)
	{
		if (loadedAsset->IsLoaded())
		{
			data = loadedAsset->GetData();
			jsonLoad = true;
		} else
		{
			jsonLoad = FileSystem::LoadJsonFile(Ininfo.GetSPath(), OUT data);

		}

		loadedAsset->OnLoad(Ininfo, data);
		return loadedAsset;
	} 
	else
	{
		jsonLoad = FileSystem::LoadJsonFile(Ininfo.GetSPath(), OUT data);
	}




	if (Ininfo.IsNull() || !FileSystem::IsAssetPath(Ininfo.GetSPath()))
	{
		return nullptr;
	}



	std::string str = Ininfo.GetSPath();

	if (jsonLoad)
	{
		const auto type{ data["asset_type"] };
		if (!type.is_string())
		{
			return nullptr;
		}

		// Get asset type from json
		XClass* assetClass = AssetData::StaticClass()->FindChild(std::string{ type });
		if (!assetClass)
		{
			return nullptr;
		}

		AssetData* newAsset = Globals::CreateX<AssetData>(assetClass, GetSelf(), Ininfo.GetAssetNameFromFile());
		if (newAsset)
		{
			if (newAsset->OnLoad(Ininfo, data))
			{
				//m_AssetInfos.push_back(Ininfo);
				m_LoadedAsssets.insert(std::make_pair(Ininfo.GetSPath(), newAsset));
				return newAsset;
			}
		}

	}


	return nullptr;
}

AssetData* AssetManager::LoadOrCreate(const AssetInfo& Ininfo, XClass* AssetType)
{		

	AssetData* loadedAsset = Load(Ininfo);
	if (loadedAsset)
	{
		return loadedAsset;
	}

	if (!AssetType)
		return nullptr;

	//loadedAsset = GetLoadedAsset(Ininfo);

	//if (loadedAsset)
	//	return loadedAsset;



	if (AssetType->IsChildOf<AssetData>())
	{

		AssetData* newAsset = Globals::CreateX<AssetData>(AssetType, GetSelf(), Ininfo.GetAssetNameFromFile());
		//AssetData* newAsset = (AssetData*)AssetType->CreateInstance(GetSelf());
		if (newAsset->OnCreate(Ininfo))
		{			
			m_LoadedAsssets.insert(std::make_pair(Ininfo.GetSPath(), newAsset));
			return newAsset;
		}
	}

	return nullptr;
}

AssetData* AssetManager::GetLoadedAsset(const AssetInfo& key) const
{
	FAssetMap::const_iterator iter = m_LoadedAsssets.find(key.GetSPath());

	if (iter != m_LoadedAsssets.end())
		return (*iter).second;


	//FindFail.
	return nullptr;
}

AssetData* AssetManager::GetLoadedAsset(const std::string& InName) const
{
	FAssetMap::const_iterator iter = m_LoadedAsssets.find(InName);

	if (iter != m_LoadedAsssets.end())
		return (*iter).second;

	return nullptr;
}

AssetData* AssetManager::FindAssetFromName(const std::string& InName, XClass* AssetType) const
{
	FAssetMap::const_iterator iter = m_LoadedAsssets.begin();
	FAssetMap::const_iterator iterEnd = m_LoadedAsssets.end();

	for(; iter != iterEnd;iter++)
	{
		fs::path assetPath = (*iter).first;
		if(assetPath.filename().replace_extension("").string() == InName)
		{
			if (AssetType->IsA((*iter).second->GetClass()) || AssetType->IsParentOf((*iter).second->GetClass()))
			{
				return (*iter).second;
			}			
		}
	}

	return nullptr;
}

AssetData* AssetManager::FindAssetFromFileName(const std::string& InFileName) const
{
	FAssetMap::const_iterator iter = m_LoadedAsssets.begin();
	FAssetMap::const_iterator iterEnd = m_LoadedAsssets.end();

	for (; iter != iterEnd; iter++)
	{
		if((*iter).second->GetFromFile() == InFileName)
		{
			return (*iter).second;
		}
	}

	return nullptr;
}

AssetData* AssetManager::FindAssetFromFileNameAndAssetType(const std::string& InFileName, XClass* AssetType) const
{
	FAssetMap::const_iterator iter = m_LoadedAsssets.begin();
	FAssetMap::const_iterator iterEnd = m_LoadedAsssets.end();

	for (; iter != iterEnd; iter++)
	{
		if ((*iter).second->GetFromFile() == InFileName 
			&& (AssetType->IsA((*iter).second->GetClass()) || AssetType->IsParentOf((*iter).second->GetClass())))
		{

			return (*iter).second;
		}
	}

	return nullptr;
}

size_t AssetManager::GetAssets(std::vector<AssetData*>& OutAssets, XClass* AssetType)
{
	if (!AssetType)
		return 0;

	FAssetMap::iterator iter = m_LoadedAsssets.begin();
	FAssetMap::iterator iterEnd = m_LoadedAsssets.end();
	size_t outSize = 0;

	for(;iter != iterEnd;iter++)
	{
		AssetData* asset = (*iter).second;

		if (asset->GetClass()->IsA(AssetType))
		{
			OutAssets.push_back(asset);
			outSize++;
		}
	}


	return outSize;
}

size_t AssetManager::GetAssetsFromInheritedType(std::vector<AssetData*>& OutAssets, XClass* AssetType)
{
	if (!AssetType)
		return 0;

	FAssetMap::iterator iter = m_LoadedAsssets.begin();
	FAssetMap::iterator iterEnd = m_LoadedAsssets.end();
	size_t outSize = 0;

	for (; iter != iterEnd; iter++)
	{
		AssetData* asset = (*iter).second;

		if (asset->GetClass()->IsA(AssetType) ||
			asset->GetClass()->IsChildOf(AssetType))
		{
			OutAssets.push_back(asset);
			outSize++;
		}
	}


	return outSize;
}

void AssetManager::Save(AssetData* ToSaveAsset)
{
	ToSaveAsset->Save();
}

void AssetManager::ReName(const fs::path& InTarget, const std::string& InName)
{
	AssetData* loadedAsset = GetLoadedAsset(AssetInfo(InTarget.string()));
	if (loadedAsset)
	{
		std::string oldName;
		loadedAsset->GetRelativePathWithName(OUT oldName);
		loadedAsset->SetName(InName);
		fs::path renamePath = InTarget.parent_path();
		renamePath /= InName;
		renamePath.replace_extension(".meta");

		AssetInfo targetInfo = AssetInfo(InTarget.string());
		AssetInfo newInfo = AssetInfo(renamePath.string());

		fs::remove(InTarget);

		loadedAsset->SetInfo(newInfo);
		loadedAsset->SetName(InName);
		loadedAsset->Save();

		std::string RelPathWithName;
		loadedAsset->GetRelativePathWithName(OUT RelPathWithName);
		loadedAsset->GetClass()->ReNameCDO(oldName, RelPathWithName);

		//만약 loadedAsset 의 유일 REfCnt인데 같인 이름인 경우 를 막는다.		
		loadedAsset->AddRef();
		m_LoadedAsssets.erase(InTarget.string());
		m_LoadedAsssets.insert(std::make_pair(newInfo.GetSPath(), loadedAsset));
		loadedAsset->Release();
		

	}
}

void AssetManager::Copy(const fs::path& InTarget, const std::string& InCopyName)
{
	AssetData* loadedAsset = GetLoadedAsset(AssetInfo(InTarget.string()));
	if (loadedAsset)
	{

		fs::path renamePath = InTarget.parent_path();
		renamePath /= InCopyName;
		renamePath.replace_extension(".meta");

		AssetInfo targetInfo = AssetInfo(InTarget.string());
		AssetInfo newInfo = AssetInfo(renamePath.string());

		AssetInfo prevInfo = loadedAsset->GetInfo();
		std::string prevName = loadedAsset->GetName();

		loadedAsset->SetInfo(newInfo);
		loadedAsset->SetName(InCopyName);

		loadedAsset->Save();

		loadedAsset->SetInfo(prevInfo);
		loadedAsset->SetName(prevName);


		Load(newInfo);

	}
}

void AssetManager::Remove(const fs::path& InTarget)
{
	AssetInfo RemoveTarget = AssetInfo(InTarget.string());

	AssetData* Loaded = GetLoadedAsset(AssetInfo(InTarget.string()));

	//std::remove(m_AssetInfos.begin(), m_AssetInfos.end(), RemoveTarget);
	//m_AssetInfos.erase(std::remove(m_AssetInfos.begin(), m_AssetInfos.end(), RemoveTarget));	

	FAssetMap::iterator iter = m_LoadedAsssets.find(InTarget.string());
	if (iter != m_LoadedAsssets.end())
	{
		iter->second->Kill();
		m_LoadedAsssets.erase(iter);
	}

}
