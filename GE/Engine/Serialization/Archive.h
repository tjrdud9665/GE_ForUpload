#pragma once

#include "Json.h"

#include "GameplaySystem/GameplaySystem.h"



class Archive
{
public:
	Archive() : m_Loads(false){}
	Archive(bool Loads) : m_Loads(Loads){}
	virtual ~Archive();

private:
	const bool		m_Loads;

public:
	virtual void Serialize(const char* name, bool& val) = 0;

	virtual void Serialize(const char* name, uint8& val) = 0;

	virtual void Serialize(const char* name, int32& val) = 0;

	virtual void Serialize(const char* name, uint32& val) = 0;

	virtual void Serialize(const char* name, float& val) = 0;

	virtual void Serialize(const char* name, std::string& val) = 0;		

	virtual void Serialize(const char* name, Json& val) = 0;

	virtual void Serialize(const char* name, XEnum& val) = 0;

	virtual void Serialize(const char* name, XClass*& val) = 0;

	virtual void Serialize(const char* name, SVector2& val) = 0;

	virtual void Serialize(const char* name, SVector3& val) = 0;

	virtual void Serialize(const char* name, SVector4& val) = 0;

	virtual void Serialize(const char* name, SLinearColor& val) = 0;

	virtual void Serialize(const char* name, XSubclassOf& val) = 0 ;

	virtual void Serialize(const char* name, SCollisionProfile*& val) = 0;

	virtual void Serialize(const char* name, SCollisionChannel*& val) = 0;

	virtual void Serialize(const char* name, SActionKeyIfno& val) = 0;

	virtual void Serialize(const char* name, SKeyAxisValue& val) = 0;	

	virtual void Serialize(const char* name, class TextureAsset*& val) = 0;

	virtual void Serialize(const char* name, class Material*& val) = 0;

	virtual void Serialize(const char* name, class Skeleton*& val) = 0;

	virtual void Serialize(const char* name, class MeshAsset*& val) = 0;

	virtual void Serialize(const char* name, class SkeletalMeshAsset*& val) = 0;

	virtual void Serialize(const char* name, class SkeletalMeshSocket*& val) = 0;

	virtual void Serialize(const char* name, class AnimationSequence*& val) = 0;


	virtual void Serialize(const char* name, class AnimNotifyInfo*& val)	= 0;

	virtual void Serialize(const char* name, class AnimNotify*& val) = 0;


	virtual void Serialize(const char* name, SVertex& val) = 0;

	virtual void Serialize(const char* name, class GameObject*& val)= 0;

	virtual void Serialize(const char* name, class SceneComponent*& val) = 0;

	virtual void Serialize(const char* name, class ObjectComponent*& val) = 0;
	

	virtual void Serialize(const char* name, SGameplayAttributeData& val) = 0;

	virtual void Serialize(const char* name, SGameplayAttribute& val) = 0;

	virtual void Serialize(const char* name, AttributeModifer*& val) = 0;

	virtual void Serialize(const char* name, Blackboard*& val) = 0;

	virtual void Serialize(const char* name, SBlackboardKeySelector& val) = 0;

	virtual void Serialize(const char* name, BehaviorTree*& val) = 0;

	virtual void Serialize(const char* name, class Scene*& val) = 0;

	virtual void Serialize(const char* name, class Sound*& val) = 0;

	
	virtual void Serialize(const char* name, struct SAnchorData& val) = 0;

	virtual void Serialize(const char* name, struct SWidgetTransform& val) =0; 

	virtual void Serialize(const char* name, class FontAsset*& val) = 0;

	virtual void Serialize(const char* name, class ParticleAsset*& val) = 0;
	



	template<typename T>
	Archive& operator()(const char* name, T& val)
	{
		Serialize(name, val);		
		return *this;
	}


	template<typename T>
	void Serialize(const char* name, TSharedPtr<T>& val)
	{
		T* rawPtr = val.Get();
		Serialize(name, rawPtr);
		val = rawPtr;

	}

	template<typename T>
	void Serialize(const char* name, std::vector<T>& val)
	{
		BeginObject(name);
		if (IsLoading())
		{
			uint32 size;
			SerializeArraySize(size);
			
			if(val.size() < size)
				val.resize(size);

			for (uint32 i = 0; i < size; ++i)
			{
				BeginObject(i);
				{
					operator()("val", val[i]);
					//Data() = Data()[""]; // Temporal patch to avoid named properties
				}
				EndObject();
			}
		} else
		{
			uint32 size = (uint32)val.size();
			SerializeArraySize(size);

			for (uint32 i = 0; i < size; ++i)
			{
				BeginObject(i);
				{
					//Data()[""] = Data(); // Temporal patch to avoid named properties
					operator()("val", val[i]);
				}
				EndObject();
			}
		}
		EndObject();
	}





	template<>
	void Serialize(const char* name, std::vector<SCollisionChannel*>& val)
	{
		BeginObject(name);
		if (IsLoading())
		{
			uint32 size;
			SerializeArraySize(size);

			if (val.size() - (uint32)ECollision_Channel::Custom0 < size)
			{
				for (uint32 i = 0; i < size; i++)
				{
					val.push_back(new SCollisionChannel);
				}
			}

			for (uint32 i = (uint32)ECollision_Channel::Custom0; i < size + (uint32)ECollision_Channel::Custom0; ++i)
			{
				BeginObject(i - (uint32)ECollision_Channel::Custom0);
				{
					operator()("val", val[i]);
				}
				EndObject();
			}
		} else
		{
			uint32 size = (uint32)val.size() - (uint32)ECollision_Channel::Custom0;
			SerializeArraySize(size);

			for (uint32 i = 0; i < size; ++i)
			{
				BeginObject(i);
				{
					//Data()[""] = Data(); // Temporal patch to avoid named properties
					operator()("val", val[i + (uint32)ECollision_Channel::Custom0]);
				}
				EndObject();
			}
		}
		EndObject();
	}


public:
	// Starts an object by name
	virtual void BeginObject(const std::string& name) { BeginObject(name.c_str()); };
	virtual void BeginObject(const char* name) = 0;
	virtual bool HasObject(const char* name) = 0;
	virtual bool IsObjectValid() = 0;

	// Starts an object by index (Array)
	virtual void BeginObject(uint32 index) = 0;
	virtual void EndObject() = 0;

	inline bool IsLoading()
	{
		return m_Loads;
	
	}
	inline bool IsSaving()
	{
		return !m_Loads;	
	}

	virtual void SerializeArraySize(uint32& Size) = 0;
};

class JsonArchive : public Archive
{

	Json baseData;
	std::stack<Json*> depthData;

	const bool bBeautify;

public:


	//Save Constructor
	JsonArchive(const bool bBeautify = true)
		: Archive(false)
		, baseData()
		, depthData{}
		, bBeautify{ bBeautify }
	{}

	// Load constructor
	JsonArchive(const Json& data)
		: Archive(true)
		, baseData(data)
		, depthData{}
		, bBeautify{ false }
	{}

	virtual ~JsonArchive();

	std::string GetDataString() const { return baseData.dump(GetIndent()); }
	const Json& GetData() const { return baseData; }

	int32 GetIndent() const { return bBeautify ? 2 : -1; }	

private:

	virtual void Serialize(const char* name, bool& val) override;

	virtual void Serialize(const char* name, uint8& val) override;

	virtual void Serialize(const char* name, int32& val) override;

	virtual void Serialize(const char* name, uint32& val) override;

	virtual void Serialize(const char* name, float& val) override;

	virtual void Serialize(const char* name, std::string& val) override;

	virtual void Serialize(const char* name, Json& val) override;

	virtual void Serialize(const char* name, XEnum& val) override;

	virtual void Serialize(const char* name, XSubclassOf& val) override;

	virtual void Serialize(const char* name, SActionKeyIfno& val) override;

	virtual void Serialize(const char* name, SKeyAxisValue& val) override;


	virtual void Serialize(const char* name, SCollisionProfile*& val) override;

	virtual void Serialize(const char* name, SCollisionChannel*& val) override;	

	virtual void Serialize(const char* name, class TextureAsset*& val) override;

	virtual void Serialize(const char* name, class Material*& val) override;

	virtual void Serialize(const char* name, class Skeleton*& val) override;

	virtual void Serialize(const char* name, class SkeletalMeshSocket*& val) override;

	virtual void Serialize(const char* name, class MeshAsset*& val) override;

	virtual void Serialize(const char* name, class SkeletalMeshAsset*& val) override;

	virtual void Serialize(const char* name, class AnimationSequence*& val) override;

	virtual void Serialize(const char* name, AnimNotifyInfo*& val) override;

	virtual void Serialize(const char* name, AnimNotify*& val) override;

	virtual void Serialize(const char* name, class GameObject*& val) override;

	virtual void Serialize(const char* name, XClass*& val)	override;

	virtual void Serialize(const char* name, SVertex& val) override;

	virtual void Serialize(const char* name, SVector2& val) override;

	virtual void Serialize(const char* name, SVector3& val) override;

	virtual void Serialize(const char* name, SVector4& val) override;

	virtual void Serialize(const char* name, SLinearColor& val) override;

	virtual void Serialize(const char* name, class SceneComponent*& val) override;

	virtual void Serialize(const char* name, class ObjectComponent*& val) override;


	virtual void Serialize(const char* name, SGameplayAttributeData& val) override;

	virtual void Serialize(const char* name, SGameplayAttribute& val) override;

	virtual void Serialize(const char* name, AttributeModifer*& val) override;

	virtual void Serialize(const char* name, Blackboard*& val) override;

	virtual void Serialize(const char* name, SBlackboardKeySelector& val) override;

	virtual void Serialize(const char* name, BehaviorTree*& val) override;

	virtual void Serialize(const char* name, class Scene*& val) override;
	virtual void Serialize(const char* name, struct SAnchorData& val) override;

	virtual void Serialize(const char* name, class Sound*& val) override;


	virtual void Serialize(const char* name, struct SWidgetTransform& val) override;

	virtual void Serialize(const char* name, class FontAsset*& val) override;

	virtual void Serialize(const char* name, class ParticleAsset*& val) override;


	inline Json& Data()
	{
		if (!depthData.empty())
			return *depthData.top();
		return baseData;
	}

	virtual void BeginObject(const char* name) override
	{
		depthData.push(&Data()[name]);
	}

	virtual bool HasObject(const char* name) override
	{
		return Data().find(name) != Data().end();
	}

	virtual void BeginObject(uint32 index) override
	{
		depthData.push(&Data()[index]);
	}

	virtual bool IsObjectValid() override
	{
		return !Data().is_null();
	}
		
	virtual void EndObject() override
	{
		depthData.pop();
	}

	virtual void SerializeArraySize(uint32& size) override
	{
		Json& data = Data();
		if (IsLoading())
		{
			size = data.is_null() ? 0 : (uint32)data.size();
		} else
		{
			if (!data.is_array())
				data = Json::array();
			Json::array_t* ptr = data.get_ptr<Json::array_t*>();
			ptr->reserve((size_t)size);
		}
	};
};
