#pragma once
#include "AssetInfo.h"
#include "../XObject/XObject.h"

class AssetData :public XObject
{
	CLASS(AssetData,XObject)
	friend class AssetManager;
		
protected:
	AssetData();
	AssetData(const AssetData& Other);
	virtual ~AssetData();

	bool OnCreate(const AssetInfo& InInfo);

	virtual bool OnLoad(const AssetInfo& InInfo, Json& data);

protected:		
	virtual bool PostLoad()			override;
	virtual void OnUnload()			override;

public:
	virtual bool IsLoaded() const;


public:
	const AssetInfo& GetInfo() const { return m_Info; }
	const std::string& GetRawPath() const { return m_Info.GetSPath(); }

	void ReSet(const std::string& InPathID) { return m_Info.ReSet(InPathID); }

	void SetInfo(const AssetInfo& InInfo)
	{
		m_Info = InInfo;
	}
	bool SaveToPath(const std::string& path);

	virtual bool Save();

	virtual void  GetRelativePathWithName(std::string& OutRelPathWithName) const override;

	virtual const std::string& GetRelativePath();

	virtual const Json& GetData() const;

	virtual void SetData(const Json& InData);

	virtual bool Serialize(Archive& ar) override;


	//Fucking FBX
	virtual void SetFromFile(const std::string& InFromFile);

	const std::string& GetFromFile() const
	{
		return m_FromFile;
	}

private:
	AssetInfo		m_Info;

	PROP(std::string, m_RelativePath)
	std::string m_RelativePath;

	bool					m_Loaded;
private:
	//For Serialize
	PROP(Json, m_jsonData, Transient)
	Json m_jsonData;


	//For FBX ReLoad or OneLoad
protected:
	// Want to From File Path -> One FBX File Make Multiple Assets..
	PROP(std::string, m_FromFile)
	std::string				m_FromFile;



};

