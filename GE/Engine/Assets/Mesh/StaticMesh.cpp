#include "EnginePCH.h"
#include "StaticMesh.h"
#include "../../FileSystem/FileSystem.h"
#include "../../Importer/FBXLoader.h"

//StaticMesh::StaticMesh()
//{
//
//}
//
//StaticMesh::StaticMesh(const StaticMesh& Other)
//	:Super(Other)
//{
//
//}
//
//StaticMesh::~StaticMesh()
//{
//
//}
//
//bool StaticMesh::PostLoad()
//{
//	JsonArchive ar(GetData());
//	Serialize(ar);
//	bool bResult = Super::PostLoad();
//
//	//Already Loaded.
//	if(m_vecContainer.size())
//	{
//		return true;
//	}
//
//	fs::path rawFilePath = FileSystem::FindRawFile(FileSystem::FromString(GetInfo().GetSPath()));
//	std::string staticMeshName = rawFilePath.filename().string();
//
//	if (!rawFilePath.empty())
//	{
//		LoadMeshFullPath(staticMeshName, rawFilePath.string().c_str());
//
//	}
//
//	return true;	
//}
//
//bool StaticMesh::LoadMeshFullPath(const std::string& InName, const char* FullPath)
//{
//	char    Ext[_MAX_EXT] = {};
//
//	_splitpath_s(FullPath, 0, 0, 0, 0, 0, 0, Ext, _MAX_EXT);
//	_strupr_s(Ext);
//
//	if (strcmp(Ext, ".FBX") == 0)
//	{
//		FBXLoader  Loader;
//
//		if (!Loader.LoadFBX(FullPath))
//			return false;
//
//		return ConvertFBX(&Loader, FullPath);
//	}
//
//	return true;
//}
