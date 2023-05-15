#pragma once

//Base Of TEnum

//Property에서 하나의 타입으로 모든 enum값에 대한 핸들링.

class XEnum
{
public:
	using value_type = int32;

	XEnum()
		:val(INDEX_NONE)
	{
	}

	XEnum(const XEnum& Other)		
		:val(Other.val)
	{

	}
	virtual ~XEnum() {}

	

	virtual void Set(int32 _val)
	{
		val = _val;
	}
	int32 GetUnderlying()
	{
		return val;
	}

	virtual const char* ToString()
	{
		return nullptr;
	}

	virtual const int32* Values()
	{
		return nullptr;
	}

	virtual const char* const* Names() 
	{
		return nullptr;
	}

	virtual const size_t Count() 
	{
		return 0;
	}


protected:
	int32 val;	
};

