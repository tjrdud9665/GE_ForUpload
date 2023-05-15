#include "EnginePCH.h"
#include "BlackBoard.h"

Blackboard::Blackboard()
{

}

Blackboard::Blackboard(const Blackboard& Other)
	:Super(Other)
{
	//{
	//	FBoolKeyCIter iter = Other.m_Bools.begin();
	//	FBoolKeyCIter iter_end = Other.m_Bools.end();

	//	for(;iter != iter_end;iter++)
	//	{
	//		m_Bools.insert(std::make_pair((*iter).first, false));
	//	}
	//}

	//{
	//	FIntKeyCIter iter = Other.m_Ints.begin();
	//	FIntKeyCIter iter_end = Other.m_Ints.end();

	//	for (; iter != iter_end; iter++)
	//	{
	//		m_Ints.insert(std::make_pair((*iter).first, 0));
	//	}
	//}

	//{
	//	FFloatKeyCIter iter = Other.m_Floats.begin();
	//	FFloatKeyCIter iter_end = Other.m_Floats.end();

	//	for (; iter != iter_end; iter++)
	//	{
	//		m_Floats.insert(std::make_pair((*iter).first, 0.0f));
	//	}
	//}

	//{
	//	FVectorKeyCIter iter = Other.m_Vectors.begin();
	//	FVectorKeyCIter iter_end = Other.m_Vectors.end();

	//	for (; iter != iter_end; iter++)
	//	{
	//		m_Vectors.insert(std::make_pair((*iter).first, SVector3(0)));
	//	}
	//}

	//{
	//	FObjectKeyCIter iter = Other.m_Objects.begin();
	//	FObjectKeyCIter iter_end = Other.m_Objects.end();

	//	for (; iter != iter_end; iter++)
	//	{
	//		m_Objects.insert(std::make_pair((*iter).first, nullptr));
	//	}
	//}

}

Blackboard::~Blackboard()
{

}

bool Blackboard::Serialize(Archive& ar)
{
	bool bResult = Super::Serialize(ar);
	//Gathering Keys

	std::vector<std::string> boolKeys;
	std::vector<std::string> intKeys;
	std::vector<std::string> floatKeys;
	std::vector<std::string> vectorKeys;
	std::vector<std::string> objectKeys;
	std::vector<std::string> abilityKeys;


	if(ar.IsLoading())
	{
		ar.Serialize("boolKeys", boolKeys);		
		ar.Serialize("intKeys", intKeys);
		ar.Serialize("floatKeys", floatKeys);
		ar.Serialize("vectorKeys", vectorKeys);
		ar.Serialize("objectKeys", objectKeys);
		ar.Serialize("abilityKeys", abilityKeys);

		size_t boolKeySize = boolKeys.size();
		for(size_t i =0;i<boolKeySize;i++)
		{
			m_Bools.insert(std::make_pair(boolKeys[i],false));
		}

		size_t intKeysSize = intKeys.size();
		for (size_t i = 0; i < intKeysSize; i++)
		{
			m_Ints.insert(std::make_pair(intKeys[i], 0));
		}

		size_t floatKeysSize = floatKeys.size();
		for (size_t i = 0; i < floatKeysSize; i++)
		{
			m_Floats.insert(std::make_pair(floatKeys[i], 0.0f));
		}

		size_t vectorKeysSize = vectorKeys.size();
		for (size_t i = 0; i < vectorKeysSize; i++)
		{
			m_Vectors.insert(std::make_pair(vectorKeys[i], SVector3(0)));
		}

		size_t objectKeysSize = objectKeys.size();
		for (size_t i = 0; i < objectKeysSize; i++)
		{
			m_Objects.insert(std::make_pair(objectKeys[i], nullptr));
		}

		size_t abilityKeysSize = abilityKeys.size();
		for (size_t i = 0; i < abilityKeysSize; i++)
		{
			TSubclassOf<GameplayAbility> Ability;
			m_Abilities.insert(std::make_pair(abilityKeys[i], Ability));
		}

	}
	else
	{
		//Save
		
		boolKeys.resize(m_Bools.size());
		{
			FBoolKeyCIter iter = m_Bools.begin();
			FBoolKeyCIter iter_end = m_Bools.end();
			int32 i = 0;
			for (; iter != iter_end; iter++, i++)
			{
				boolKeys[i] = (*iter).first;
			}
		}
		
		intKeys.resize(m_Ints.size());
		{
			FIntKeyCIter iter = m_Ints.begin();
			FIntKeyCIter iter_end = m_Ints.end();
			int32 i = 0;
			for (; iter != iter_end; iter++, i++)
			{
				intKeys[i] = (*iter).first;
			}
		}

		
		floatKeys.resize(m_Floats.size());
		{
			FFloatKeyCIter iter = m_Floats.begin();
			FFloatKeyCIter iter_end = m_Floats.end();
			int32 i = 0;
			for (; iter != iter_end; iter++, i++)
			{
				floatKeys[i] = (*iter).first;
			}
		}

		
		vectorKeys.resize(m_Vectors.size());
		{
			FVectorKeyCIter iter = m_Vectors.begin();
			FVectorKeyCIter iter_end = m_Vectors.end();
			int32 i = 0;
			for (; iter != iter_end; iter++, i++)
			{
				vectorKeys[i] = (*iter).first;

			}
		}

		
		objectKeys.resize(m_Objects.size());
		{
			FObjectKeyCIter iter = m_Objects.begin();
			FObjectKeyCIter iter_end = m_Objects.end();
			int32 i = 0;
			for (; iter != iter_end; iter++, i++)
			{
				objectKeys[i] = (*iter).first;
			}
		}


		abilityKeys.resize(m_Abilities.size());
		{
			FAbilityKeyCIter iter = m_Abilities.begin();
			FAbilityKeyCIter iter_end = m_Abilities.end();
			int32 i = 0;
			for (; iter != iter_end; iter++, i++)
			{
				abilityKeys[i] = (*iter).first;
			}
		}

		ar.Serialize("boolKeys", boolKeys);
		ar.Serialize("intKeys", intKeys);
		ar.Serialize("floatKeys", floatKeys);
		ar.Serialize("vectorKeys", vectorKeys);
		ar.Serialize("objectKeys", objectKeys);
		ar.Serialize("abilityKeys", abilityKeys);
	}
	

	return bResult;
}
