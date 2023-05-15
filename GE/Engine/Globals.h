#pragma once

#include <codecvt> // for std::codecvt_utf8
#include <locale>  // for std::wstring_convert



static std::wstring utf8_to_unicode(const std::string& utf8_str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.from_bytes(utf8_str);
}

extern class Engine* g_Engine;
extern float	g_DeltaSeconds;

static inline std::wstring StringToWString(const std::string& string)
{
	std::wstring result;
	//result.assign(string.begin(), string.end());	
	int32	ConvertLength = (int32)::MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, nullptr, 0);
	wchar_t* wcChar = new wchar_t[ConvertLength];
	::MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, wcChar, ConvertLength);

	result = wcChar;
	SAFE_DELETE_ARRAY(wcChar);

	return result;
}

static inline std::string WStringToString(const std::wstring& wstring)
{
	std::string result;
	//result.assign(wstring.begin(), wstring.end());
	int32 Length = (int32)WideCharToMultiByte(CP_ACP, 0, wstring.c_str(), -1, nullptr, 0, nullptr, nullptr);
	char* mbChar = new char[Length];	
	::WideCharToMultiByte(CP_ACP, 0, wstring.c_str(), -1, mbChar, Length, nullptr, nullptr);

	result = mbChar;

	SAFE_DELETE_ARRAY(mbChar);

	return result;

}


class Globals
{

public:
	template<typename Type, typename = std::enable_if_t<std::is_base_of<XObject, Type>::value>>
	static Type* GetCDO()
	{
		XClass* _class = Type::StaticClass();
		XObject* cdo = _class->GetDefaultObject();				

		return (Type*)cdo;
	}

	//Do Not CDO ,Clone
	template<typename Type, typename = std::enable_if_t<std::is_base_of<XObject, Type>::value>>
	static Type* CreateEngineObject()
	{
		XObject* ptr = new Type();	
		if (!g_Engine)
			g_Engine = (Engine*)ptr;

		ptr->PreConstruct(ptr, Type::StaticClass(), g_Engine);
		ptr->Construct();
		return (Type*)ptr;
	}

	
	//오브젝트를 생성하는 함수
	template<typename Type, typename = std::enable_if_t<std::is_base_of<XObject, Type>::value>>
	static Type* CreateX(class XObject* Owner = nullptr, const std::string& InName = "")
	{
		XClass* _class = Type::StaticClass();
		XObject* cdo = _class->GetDefaultObject();				
		XObject* ptr = cdo->Clone();
		
		//오너가없는경우 스스로를 오너를 사용
		if (!Owner)
			Owner = ptr;

		ptr->PreConstruct(ptr, Type::StaticClass(), Owner);
		ptr->Construct(InName);
		return (Type*)ptr;
	}

	template<typename Type, typename = std::enable_if_t<std::is_base_of<XObject, Type>::value>>
	static Type* CreateX(XClass* ObjectClass, class XObject* Owner, const std::string& InName = "")
	{
		if (ObjectClass)
		{
			return (Type*)ObjectClass->CreateInstance(Owner,InName);
		}
		return nullptr;
	}

	template<typename Type, typename = std::enable_if_t<std::is_base_of<XObject, Type>::value>>
	static Type* CreateX(TSubclassOf<Type> ObjectClass, class XObject* Owner, const std::string& InName = "")
	{
		if (ObjectClass)
		{
			return (Type*)ObjectClass->CreateInstance(Owner, InName, ObjectClass.CustomCDOName);
		}
		return nullptr;
	}

	static void DeleteEngineObject(class XObject* ptr)
	{
		SAFE_DELETE(ptr);
	}


};




static void(__cdecl ID3D11DeviceContext::* SetShaderResources[EShaderStage::COUNT])
(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews) =
{
	&ID3D11DeviceContext::VSSetShaderResources,
	&ID3D11DeviceContext::GSSetShaderResources,
	&ID3D11DeviceContext::DSSetShaderResources,
	&ID3D11DeviceContext::HSSetShaderResources,
	&ID3D11DeviceContext::PSSetShaderResources,
	&ID3D11DeviceContext::CSSetShaderResources,
};
