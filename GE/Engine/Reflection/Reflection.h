#pragma once

#include "ReflectionMacro.h"
#include "XClass.h"
#include "ReflectionTag.h"
#include <type_traits>

// Wrap some types so we can use them at compile time
template<uint32 N> struct MetaCounter : MetaCounter<N - 1> { static constexpr uint32 value = N; };
template<> struct MetaCounter<0> { static constexpr uint32 value = 0; };


/** Defines a Class */
#define CLASS(type, parent, tags) \
public:\
using Super = parent;\
\
virtual inline type* Clone() override {\
	return new type(*this);\
}\
virtual inline void SerializeReflection(Archive& ar) override {\
	Super::SerializeReflection(ar);\
	__meta_SerializeProperty(ar, MetaCounter<0>{});\
}\
static void __meta_Registry() {\
	StaticClass()->__Registry<Super>(#type);\
BASECLASS(type, tags)

/** Defines a class with no parent */
#define ORPHAN_CLASS(type, tags) \
public:\
virtual inline void SerializeReflection(Archive& ar) {\
	__meta_SerializeProperty(ar, MetaCounter<0>{});\
}\
public:\
static void __meta_Registry() {\
	static_assert(std::is_convertible<__meta_type, XObject >::value, "Type does not inherit XObject!");\
	StaticClass()->__Registry(#type);\
BASECLASS(type, tags)

#define BASECLASS(type, inTags)\
	TYPETAGS(type, inTags)\
}\
friend class Globals;\
private:\
using __meta_type = type;\
friend TClass<__meta_type>;\
\
public:\
static TClass<__meta_type>* StaticClass() {\
	return TClass<__meta_type>::GetStatic();\
}\
\
static inline void __meta_RegistryProperties() {\
	__meta_RegistryProperty(MetaCounter<0>{});\
}\
\
private:\
static constexpr MetaCounter<0> __meta_Counter(MetaCounter<0>);\
template<uint32 N> static void __meta_RegistryProperty(MetaCounter<N>) {}\
template<uint32 N>  void __meta_SerializeProperty(Archive&, MetaCounter<N>) {}



#define TYPETAGS(type, inTags)\
	constexpr ReflectionTags tags = ReflectionTagsInitializer<inTags>::value;\
	static_assert(!(tags & DetailsEdit), "Only properties can use DetailsEdit"); \
	static_assert(!(tags & DetailsView), "Only properties can use DetailsView"); \
	StaticClass()->__RegistryTags(tags);\

#define PROP(type, name, tags, groupname, ...) __PROPERTY_IMPL(type, name, CAT(__meta_id_, name), tags ,groupname, ##__VA_ARGS__)

#define __PROPERTY_IMPL(type, name, id_name, inTags, ...)\
static constexpr uint32 id_name = decltype(__meta_Counter(MetaCounter<255>{}))::value;\
static constexpr MetaCounter<id_name + 1> __meta_Counter(MetaCounter<id_name + 1>);\
\
static void __meta_RegistryProperty(MetaCounter<id_name>) {\
	static_assert(IsReflectableType<type>(), "Register reflectable Types");\
\
	constexpr ReflectionTags tags = ReflectionTagsInitializer<inTags>::value;\
	static_assert(!(tags & Abstract), "Properties can't be Abstract");\
\
	StaticClass()->__RegistryProperty<type>(#name, [](XObject* baseInstance)\
	{\
		if(__meta_type* instance = dynamic_cast<__meta_type*>(baseInstance))\
			return &(instance->name);\
		return (type*)nullptr;\
	}, tags,##__VA_ARGS__);\
\
	/* Registry next property if any */\
	__meta_RegistryProperty(MetaCounter<id_name + 1>{});\
};\
void __meta_SerializeProperty(Archive& ar, MetaCounter<id_name>) {\
\
	constexpr ReflectionTags tags = ReflectionTagsInitializer<inTags>::value;\
	\
	if constexpr(!(tags & Transient))\
	{/* Don't serialize property if Transient */\
		ar(#name , name);\
	}\
	/* Serialize next property if any */\
	__meta_SerializeProperty(ar, MetaCounter<id_name + 1>{});\
};