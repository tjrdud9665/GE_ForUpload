#pragma once

#include "XEnum.h"

template<typename E>
class TEnum : public XEnum
{
public:
	TEnum()
		:__enumStruct(E::_enumerated(INDEX_NONE))
	{
		__names = nullptr;
		Initialized = false;
	}
	TEnum(int32 enumval)
		:__enumStruct(E::_enumerated(enumval))
	{
		val = enumval;
		if (!Initialized)
		{
			__names = __enumStruct._names();
			Initialized = true;
		}
	}
	TEnum(E enumval)
		:__enumStruct(enumval)
	{
		if (!Initialized)
		{
			__names = __enumStruct._names();
			Initialized = true;
		}
	}

	E Get() const
	{
		return __enumStruct;
	}

	virtual void Set(int32 _val) override
	{
		val = _val;
		__enumStruct = E::_enumerated(_val);
	}

	virtual ~TEnum()
	{	
		if (Initialized)
		{
			for (size_t i = 0; i < __enumStruct._count; i++)
			{
				delete __names[i];
			}
			__names = nullptr;
			Initialized = false;
	}
		
		
	}

	virtual const char* ToString() override
	{
		return __enumStruct._to_string();
	}
	virtual const int32* Values() override
	{
		return __enumStruct._values();

	}

	virtual const char* const* Names() override
	{
		if (!Initialized)
		{
			__names = __enumStruct._names();
			Initialized = true;
		}

		return __names;
	}
	
	virtual const size_t Count() override
	{
		return __enumStruct._count;
	}

private:
	E __enumStruct;
	static const char* const* __names;
	static bool Initialized;
};

template<typename E>
__declspec(selectany) const char* const* TEnum<E>::__names;

template<typename E>
__declspec(selectany) bool TEnum<E>::Initialized = false;
