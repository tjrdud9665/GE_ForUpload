#include "EnginePCH.h"
#include "AssetData.h"

AssetData::AssetData()
	:m_Loaded(false)
{

}

AssetData::AssetData(const AssetData& Other)
	: Super(Other)
	, m_RelativePath(Other.m_RelativePath)
	, m_Loaded(Other.m_Loaded)
{

}

AssetData::~AssetData()
{
	OnUnload();
}

bool AssetData::OnCreate(const AssetInfo& InInfo)
{
	PreLoad();
	m_Info = InInfo;
	m_RelativePath = m_Info.m_RelativePath;
	//Remove  .meta
	fs::path assetPath = InInfo.GetSPath();
	m_FromFile = assetPath.filename().replace_extension("").string();
	return PostLoad();
}

bool AssetData::OnLoad(const AssetInfo& InInfo, Json& data)
{
	PreLoad();
	m_Info = InInfo;
	m_RelativePath = m_Info.m_RelativePath;


	if (data != GetData())
	{
		JsonArchive ar(data);
		Serialize(ar);
		m_Loaded = true;
		m_jsonData = data;
	}



	return PostLoad();
}

bool AssetData::PostLoad()
{
	m_Loaded = true;
	return true;
}

void AssetData::OnUnload()
{
	m_Loaded = false;
}

bool AssetData::IsLoaded() const
{
	return m_Loaded;
}

bool AssetData::SaveToPath(const std::string& sPath)
{
	if (sPath.empty()) //|| !FileSystem::IsAssetPath(sPath))
		return false;

	m_jsonData.clear();
	JsonArchive ar{};
	Serialize(ar);
	m_jsonData = ar.GetData();


	return FileSystem::SaveJsonFile(sPath, ar.GetData(), ar.GetIndent());
}

bool AssetData::Save()
{
	fs::path toSavePath = m_Info.GetSPath();
	if(!FileSystem::FolderExists(toSavePath.remove_filename()))
	{
		fs::create_directories(toSavePath);
	}



	return SaveToPath(m_Info.GetSPath());
}

void AssetData::GetRelativePathWithName(std::string& OutRelPathWithName) const
{
	OutRelPathWithName = (m_RelativePath + GetName());
}

const std::string& AssetData::GetRelativePath()
{
	return m_RelativePath;
}

const Json& AssetData::GetData() const
{
	return m_jsonData;
}

void AssetData::SetData(const Json& InData)
{
	m_jsonData = InData;
	m_Loaded = true;
}

bool AssetData::Serialize(Archive& ar)
{
	std::string className = GetClass()->GetName();
	ar("asset_type", className);
	bool bResult = Super::Serialize(ar);
	return bResult;
}

void AssetData::SetFromFile(const std::string& InFromFile)
{
	m_FromFile = InFromFile;
}
