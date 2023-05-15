#pragma once

#include "../AssetData.h"
#include "../GameplayAbility/GameplayAbility.h"




class Blackboard : public AssetData
{
	CLASS(Blackboard, AssetData)
	friend class AIController;
private:
	Blackboard();
	Blackboard(const Blackboard& Other);
	virtual ~Blackboard();
public:
	using FBoolKeyIter = std::unordered_map<std::string, bool>::iterator;
	using FIntKeyIter = std::unordered_map<std::string, int32>::iterator;
	using FFloatKeyIter = std::unordered_map<std::string, float>::iterator;
	using FVectorKeyIter = std::unordered_map<std::string, SVector3>::iterator;
	using FObjectKeyIter = std::unordered_map<std::string, TSharedPtr<XObject>>::iterator;
	using FAbilityKeyIter = std::unordered_map<std::string, TSubclassOf<GameplayAbility>>::iterator;

	using FBoolKeyCIter = std::unordered_map<std::string, bool>::const_iterator;
	using FIntKeyCIter = std::unordered_map<std::string, int32>::const_iterator;
	using FFloatKeyCIter = std::unordered_map<std::string, float>::const_iterator;
	using FVectorKeyCIter = std::unordered_map<std::string, SVector3>::const_iterator;
	using FObjectKeyCIter = std::unordered_map<std::string, TSharedPtr<XObject>>::const_iterator;
	using FAbilityKeyCIter = std::unordered_map<std::string, TSubclassOf<GameplayAbility>>::const_iterator;

	void AddKey(const std::string& InKey, EBlackBoardValType ValueType)
	{
		switch (ValueType)
		{
		case EBlackBoardValType::Bool:
			m_Bools.insert(std::make_pair(InKey, false));
			break;
		case EBlackBoardValType::Float:
			m_Floats.insert(std::make_pair(InKey, 0.0f));
			break;
		case EBlackBoardValType::Int:
			m_Ints.insert(std::make_pair(InKey, 0));
			break;
		case EBlackBoardValType::Vector:
			m_Vectors.insert(std::make_pair(InKey, SVector3::ZeroVector));
			break;
		case EBlackBoardValType::Object:
			m_Objects.insert(std::make_pair(InKey, nullptr));
			break;
		case EBlackBoardValType::Ability:
			TSubclassOf<GameplayAbility> Ability;
			m_Abilities.insert(std::make_pair(InKey, Ability));
			break;
		}
	}
	void RemoveKey(const std::string& InKey, EBlackBoardValType ValueType)
	{
		switch (ValueType)
		{
		case EBlackBoardValType::Bool:
		{
			auto finditer = m_Bools.find(InKey);
			if (finditer != m_Bools.end())
				m_Bools.erase(finditer);
		}
		break;
		case EBlackBoardValType::Float:
		{
			auto finditer = m_Floats.find(InKey);
			if (finditer != m_Floats.end())
				m_Floats.erase(finditer);
		}
		break;
		case EBlackBoardValType::Int:
		{
			auto finditer = m_Ints.find(InKey);
			if (finditer != m_Ints.end())
				m_Ints.erase(finditer);
		}
		break;
		case EBlackBoardValType::Vector:
		{
			auto finditer = m_Vectors.find(InKey);
			if (finditer != m_Vectors.end())
				m_Vectors.erase(finditer);
		}
		break;
		case EBlackBoardValType::Object:
		{
			auto finditer = m_Objects.find(InKey);
			if (finditer != m_Objects.end())
				m_Objects.erase(finditer);
		}
		break;
		case EBlackBoardValType::Ability:
		{
			auto finditer = m_Abilities.find(InKey);
			if (finditer != m_Abilities.end())
				m_Abilities.erase(finditer);
		}
		break;
		}
	}




	bool SetBool(const std::string& InKey, bool value)
	{
		m_Bools[InKey] = value;
		return false;
	}


	bool GetBool(const std::string& InKey, bool& OutValue)
	{
		if (m_Bools.find(InKey) == m_Bools.end())
		{
			return false;
		}

		OutValue = m_Bools[InKey];
		return true;
	}

	bool HasBool(const std::string& InKey) const
	{
		return m_Bools.find(InKey) != m_Bools.end();
	}

	void SetInt(const std::string& InKey, int32 value)
	{
		m_Ints[InKey] = value;
	}
	bool GetInt(const std::string& InKey, int32& OutValue)
	{
		if (m_Ints.find(InKey) == m_Ints.end())
		{
			return false;
		}
		OutValue = m_Ints[InKey];
		return true;
	}

	bool HasInt(const std::string& InKey) const
	{
		return m_Ints.find(InKey) != m_Ints.end();
	}

	void SetFloat(const std::string& InKey, float  value)
	{
		m_Floats[InKey] = value;
	}
	bool GetFloat(const std::string& InKey, float& OutValue)
	{
		if (m_Floats.find(InKey) == m_Floats.end())
		{
			return false;
		}
		OutValue = m_Floats[InKey];
		return true;
	}

	bool HasFloat(const std::string& InKey) const
	{
		return m_Floats.find(InKey) != m_Floats.end();
	}

	void SetVector(const std::string& InKey, const SVector3& value)
	{
		m_Vectors[InKey] = value;
	}
	bool GetVector(const std::string& InKey, SVector3& OutValue)
	{
		if (m_Vectors.find(InKey) == m_Vectors.end())
		{
			return false;
		}
		OutValue = m_Vectors[InKey];
		return true;
	}

	bool HasVector(const std::string& InKey) const
	{
		return m_Vectors.find(InKey) != m_Vectors.end();
	}




	void SetObj(const std::string& InKey, XObject* value)
	{
		m_Objects[InKey] = value;
	}


	bool GetObj(const std::string& InKey, XObject*& OutValue)
	{
		if (m_Objects.find(InKey) == m_Objects.end())
		{
			return false;
		}

		OutValue = m_Objects[InKey];
		return true;
	}

	bool HasObj(const std::string& InKey) const
	{
		return m_Objects.find(InKey) != m_Objects.end();
	}




	void SetAbility(const std::string& InKey, TSubclassOf<GameplayAbility> value)
	{
		m_Abilities[InKey] = value;
	}


	bool GetAbility(const std::string& InKey, TSubclassOf<GameplayAbility>& OutValue)
	{
		if (m_Abilities.find(InKey) == m_Abilities.end())
		{
			return false;
		}

		OutValue = m_Abilities[InKey];
		return true;
	}

	bool HasAbility(const std::string& InKey) const
	{
		return m_Abilities.find(InKey) != m_Abilities.end();
	}




	void CheckObjectValidation()
	{
		std::unordered_map<std::string, TSharedPtr<XObject>>::iterator iter = m_Objects.begin();
		std::unordered_map<std::string, TSharedPtr<XObject>>::iterator iterEnd = m_Objects.end();

		for (; iter != iterEnd; iter++)
		{
			if ((*iter).second->IsValidate())
			{
				(*iter).second = nullptr;
			}
		}
	}


	size_t GetBoolValues(std::unordered_map<std::string, bool>& OutBools)
	{
		OutBools = m_Bools;
		return m_Bools.size();
	}

	size_t GetIntValues(std::unordered_map<std::string, int32>& OutInts)
	{
		OutInts = m_Ints;
		return m_Ints.size();
	}

	size_t GetFloatValues(std::unordered_map<std::string, float>& OutFloats)
	{
		OutFloats = m_Floats;
		return m_Floats.size();
	}

	size_t GetVectorValues(std::unordered_map<std::string, SVector3>& OutVectors)
	{
		OutVectors = m_Vectors;
		return m_Vectors.size();
	}

	size_t GetObjectValues(std::unordered_map<std::string, TSharedPtr<XObject>>& OutObjects)
	{
		OutObjects = m_Objects;
		return m_Objects.size();
	}

	size_t GetAbilityValues(std::unordered_map<std::string, TSubclassOf<GameplayAbility>>& OutAbilities)
	{
		OutAbilities = m_Abilities;
		return m_Abilities.size();
	}

	size_t GetValueKeys(EBlackBoardValType type , std::vector<std::string>& outKeys)
	{
		switch(type)
		{
		case EBlackBoardValType::Bool:
		{
			FBoolKeyCIter iter = m_Bools.begin();
			FBoolKeyCIter iter_end = m_Bools.end();
			int32 i = 0;
			outKeys.resize(m_Bools.size());
			for(;iter != iter_end;iter++,i++)
			{
				outKeys[i] = (*iter).first;
			}
			break;
		}
		case EBlackBoardValType::Int:
		{
			FIntKeyCIter iter = m_Ints.begin();
			FIntKeyCIter iter_end = m_Ints.end();
			int32 i = 0;
			outKeys.resize(m_Ints.size());
			for (; iter != iter_end; iter++, i++)
			{
				outKeys[i] = (*iter).first;
			}
			break;
		}
		case EBlackBoardValType::Float:
		{
			FFloatKeyCIter iter = m_Floats.begin();
			FFloatKeyCIter iter_end = m_Floats.end();
			int32 i = 0;
			outKeys.resize(m_Floats.size());
			for (; iter != iter_end; iter++, i++)
			{
				outKeys[i] = (*iter).first;
			}
			break;
		}
		case EBlackBoardValType::Vector:
		{
			FVectorKeyCIter iter = m_Vectors.begin();
			FVectorKeyCIter iter_end = m_Vectors.end();
			int32 i = 0;
			outKeys.resize(m_Vectors.size());
			for (; iter != iter_end; iter++, i++)
			{
				outKeys[i] = (*iter).first;
			}
			break;
		}

		case EBlackBoardValType::Object:
		{
			FObjectKeyCIter iter = m_Objects.begin();
			FObjectKeyCIter iter_end = m_Objects.end();
			int32 i = 0;
			outKeys.resize(m_Objects.size());
			for (; iter != iter_end; iter++, i++)
			{
				outKeys[i] = (*iter).first;
			}
			break;
		}

		case EBlackBoardValType::Ability:
		{
			FAbilityKeyCIter iter = m_Abilities.begin();
			FAbilityKeyCIter iter_end = m_Abilities.end();
			int32 i = 0;
			outKeys.resize(m_Abilities.size());
			for (; iter != iter_end; iter++, i++)
			{
				outKeys[i] = (*iter).first;
			}
			break;
		}


		}

		return outKeys.size();
	}

public:
	virtual bool Serialize(Archive& ar) override;


protected:
	std::unordered_map<std::string, bool> m_Bools;

	std::unordered_map<std::string, int32> m_Ints;

	std::unordered_map<std::string, float> m_Floats;

	std::unordered_map<std::string, SVector3> m_Vectors;

	std::unordered_map<std::string, TSharedPtr<XObject>>	m_Objects;

	std::unordered_map<std::string, TSubclassOf<GameplayAbility>>	m_Abilities;

	//std::unordered_map<std::string, std::string> m_Strings;


};

