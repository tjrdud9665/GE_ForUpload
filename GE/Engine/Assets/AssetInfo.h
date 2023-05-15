#pragma once
#include "../FileSystem/FileSystem.h"

//��� Asset �� .meta �����̸� json ���� �����ϱ⿡ ū �������� ��� .bin ������ �����Ͽ� �ּҸ� json�� �����ؼ� �ε�ÿ� �е��� �����Ѵ�.

//m_Path �� Ǯ�佺
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
	

	/* File �� ���� ���� ������ ���ɴϴ�.
	* ex testScene.meta -> testScene
	*   Texture.png.meta ->Texture.png  �ؽ�ó�� Ȯ���ڸ��� �����մϴ�.
	*/

	//RelativePath �� �ִ°��
	// Sprite/Test/zxc.meta
	// Sprite/Test2/zxc.meta
	//�� ������ �Ǿ� �ε������ �ش������� ã���� �����ã���� �˾��Ѵ�.

	const std::string GetAssetNameFromFile() const
	{
		fs::path assetPath = FileSystem::FromString(m_Path);
		return assetPath.filename().replace_extension("").string();
	}

protected:
	std::string	m_Path;

	std::string	m_RelativePath;

};

