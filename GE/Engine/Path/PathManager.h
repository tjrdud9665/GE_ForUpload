#pragma once


//Path Only wstring
struct SPathInfo
{
	std::string		aPath;
	std::wstring	wPath;

	bool operator==(const SPathInfo& rhs) const
	{
		return aPath == rhs.aPath;
	}

	bool operator!=(const SPathInfo& rhs) const
	{
		return aPath != rhs.aPath;
	}
};

class PathManager : public XObject
{
	friend class Globals;
	using FPathMap = std::unordered_map<std::string, SPathInfo*>;

private:
	PathManager();
	~PathManager();
	

public:
	bool Initialize();
	
	bool AddPath(const std::string& InName, const std::wstring& InPath, const std::string& BasePathName = ROOT_PATH);

	const SPathInfo* FindPath(const std::string& InName) const;


	//TEXTURE_PATH,SCENE_PATH와 같이 등록된패스인지 확인
	bool IsEnginePath(const fs::path& InPath) const;

	int32 GetAllEnginePath(std::vector<SPathInfo*>& OutPath);


	void MakeFullPath(const std::string& InPathKey, const std::string& InPath, std::string& OutFullPath);


private:
	std::wstring GetExePath();

private:
	FPathMap	m_mapPath;

};