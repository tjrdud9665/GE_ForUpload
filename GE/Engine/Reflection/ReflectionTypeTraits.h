#pragma once
#include <type_traits>
#include <sstream>
#include "../SharedPtr/TSharedPtr.h"
#include "Reflection/XSubClass.h"
#include "TSubClass.h"

template<typename T>
struct HasItemType
{
private:
	template<typename V> static void impl(decltype(typename V::value_type(), int()));
	template<typename V> static bool impl(char);
public:
	static const bool value = std::is_void<decltype(impl<T>(0))>::value;
};




template<typename T>
inline constexpr bool IsArrayType()
{
	// Check if we are dealing with a array
	if constexpr (HasItemType<T>::value)
		return std::is_same<vector<typename T::value_type>, T>::value;


	return false;
}

template<typename E>
inline constexpr bool IsEnumType()
{	
	
	if constexpr (std::is_base_of<XEnum, E>::value)
		return std::is_base_of<XEnum, E>::value;	
	
	return false;
}



template<typename T>
inline constexpr bool IsSharedType()
{	
	if constexpr (HasItemType<T>::value)
		return std::is_same<TSharedPtr<typename T::value_type>, T>::value;

	return false;
}

//TSubClassof
template<typename C>
inline constexpr bool IsClassType()
{	
	if constexpr (std::is_base_of<XSubclassOf, C>::value)
		return std::is_base_of<XSubclassOf, C>::value;
	
	return false;
}


template<typename T>
inline constexpr bool IsReflectableType()
{
	if constexpr (IsArrayType<T>())
	{
		return IsReflectableType<typename T::value_type>();
	}
	else if constexpr (IsSharedType<T>())
	{
		return IsReflectableType<typename T::value_type>();
	}
	else if constexpr (IsEnumType<T>())
	{
		return true;
	}
	else if constexpr (IsClassType<T>())
	{
		return true;
	}




	return false;
}

template<typename T>
inline std::string GetReflectableName()
{
	if constexpr (IsArrayType<T>())
	{
		if constexpr (IsReflectableType<typename T::value_type>())
		{
			stringstream ss;
			ss << "vector<" << GetReflectableName<typename T::value_type>() <<">";
			return ss.str();
		}
		return std::string("vector<Invalid>");	
	}
	else if constexpr (IsSharedType<T>())
	{
		stringstream ss;
		ss << "TSharedPtr<" << GetReflectableName<typename T::value_type>() << ">";
		return ss.str();
	}
	else if constexpr (IsEnumType<T>())
	{
		stringstream ss;
		ss << "TEnum<" << GetReflectableName<typename T::value_type>() << ">";
		return ss.str();
	} 
	else if constexpr (IsClassType<T>())
	{
		stringstream ss;
		ss << "TSubclassOf<" << GetReflectableName<typename T::value_type>() << ">";
		return ss.str();
	}
	else if constexpr (IsClassType<T>())
	{
		stringstream ss;
		ss << "TSubclassOf<" << GetReflectableName<typename T::value_type>() << ">";
		return ss.str();
	}

	return std::string("Invalid");
}


#define DECLARE_REFLECTION_TYPE(Type)\
template<>\
inline constexpr bool IsReflectableType<Type>() { return true; }\
template<>\
inline std::string GetReflectableName<Type>() { return (#Type); }

#define DECLARE_SHARED_REFLECTION_TYPE(Type)\
DECLARE_REFLECTION_TYPE(Type*)\
DECLARE_REFLECTION_TYPE(TSharedPtr<Type>)


	
