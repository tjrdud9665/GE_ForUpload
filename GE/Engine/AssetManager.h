#pragma once
#include "XObject/XObject.h"
#include "Assets/AssetData.h"


class AssetManager :public XObject
{
	friend class Globals;
	friend class FBXLoader;
	using FAssetMap = std::unordered_map<std::string, TSharedPtr<AssetData>>;

private:
	AssetManager();
	virtual ~AssetManager();



public:

	//template<typename AssetType, typename = std::enable_if_t<std::is_base_of<AssetData, AssetType>::value>>
	//AssetType* Create(const AssetInfo& Ininfo)
	//{

	//}

	/* RawFile을 읽어 Meta 파일 형태로 만듭니다.*/
	void CreateMetaFromRawFile();

	void CreateDefaultEngineDirectory();

	void LoadAllMetaFiles();


	void Initialize();

	AssetData* Load(const AssetInfo& Ininfo);	
	AssetData* LoadOrCreate(const AssetInfo& Ininfo, XClass* AssetType);

	/* AssetId Maybe Path */
	AssetData* GetLoadedAsset(const AssetInfo& key) const;

	AssetData* GetLoadedAsset(const std::string& InName) const;

	AssetData* FindAssetFromName(const std::string& InName,XClass* AssetType) const;

	AssetData* FindAssetFromFileName(const std::string& InFileName) const;

	AssetData* FindAssetFromFileNameAndAssetType(const std::string& InFileName, XClass* AssetType) const;


	/* AssetType과 일치하는것을 반환 */
	size_t GetAssets(std::vector<AssetData*>& OutAssets, XClass* AssetType);

	/* AssetType을 상속받은것 + 일치하는것을 반환 */
	size_t GetAssetsFromInheritedType(std::vector<AssetData*>& OutAssets, XClass* AssetType);

	void Save(AssetData* ToSaveAsset);

	void ReName(const fs::path& InTarget, const std::string& InName);

	void Copy(const fs::path& InTarget, const std::string& InCopyName);

	void Remove(const fs::path& InTarget);


private:
	FAssetMap					m_LoadedAsssets;

};

