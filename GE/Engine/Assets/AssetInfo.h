#pragma once
#include "../FileSystem/FileSystem.h"

//모든 Asset 은 .meta 형식이며 json 으로 저장하기에 큰 데이터의 경우 .bin 등으로 저장하여 주소를 json에 저장해서 로드시에 읽도록 유도한다.

//m_Path 는 풀페스
class AssetInfo
{
	friend class AssetData;
public:
	AssetInfo() {}
	AssetInfo(const std::string& PathID, bool MakeRelativePath = true)
		:m_Path(PathID)
	{	
	//Check Extension Must.meta
		fs::path assetPath = m_Path;

		if (assetPath.extension() != ".meta")
			m_Path += ".meta";
		

		if (MakeRelativePath)//To Heavy..
			m_RelativePath = FileSystem::GetRelativePath(m_Path);

	}

	virtual ~AssetInfo()
	{

	}

	bool operator ==(const AssetInfo& Other)
	{
		return m_Path == Other.m_Path;
	}



public:
	void ReSet(const std::string& PathID)
	{
		m_Path = PathID;
		m_RelativePath = FileSystem::GetRelativePath(m_Path);
	}

	inline const std::string& GetSPath() const
	{
		return m_Path;
	}

	inline const std::string& GetRelativePath() const
	{
		return m_RelativePath;
	}

	const bool IsNull() const
	{
		return m_Path.empty() || !FileSystem::IsAssetPath(GetSPath());
	}
	

	/* File 로 부터 에셋 네임을 따옵니다.
	* ex testScene.meta -> testScene
	*   Texture.png.meta ->Texture.png  텍스처는 확장자명을 포함합니다.
	*/

	//RelativePath 가 있는경우
	// Sprite/Test/zxc.meta
	// Sprite/Test2/zxc.meta
	//는 구분이 되어 로드되지만 해당파일을 찾을떄 어떤것을찾는지 알아한다.

	const std::string GetAssetNameFromFile() const
	{
		fs::path assetPath = FileSystem::FromString(m_Path);
		return assetPath.filename().replace_extension("").string();
	}

protected:
	std::string	m_Path;

	std::string	m_RelativePath;

};

