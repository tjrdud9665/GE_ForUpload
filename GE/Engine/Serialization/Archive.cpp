#include "EnginePCH.h"
#include "Archive.h"
#include "../GameFramework/Components/SceneComponent.h"
#include "Assets/TextureAsset/TextureAsset.h"
#include  "Assets/Material/Material.h"
#include "Path/PathManager.h"
#include "AssetManager.h"
#include "Assets/Mesh/Skeleton.h"
#include "Assets/Mesh/SkeletalMeshSocket.h"
#include "Assets/Mesh/SkeletalMeshAsset.h"
#include "Assets/Animation/AnimationSequence.h"
#include "GameFramework/GameObject/GameObject.h"
#include <ObjectManager.h>
#include <CollisionManager.h>
#include <Assets/Animation/AnimNotify.h>
#include <Assets/Scene/Scene.h>
#include <Assets/AI/BehaviorTree.h>
#include <Assets/Sound/Sound.h>
#include <Assets/Font/FontAsset.h>
#include <Assets/Particle/ParticleAsset.h>


Archive::~Archive()
{

}


JsonArchive::~JsonArchive()
{
	while (!depthData.empty())
	{
		SAFE_DELETE(depthData.top());
		depthData.pop();
	}



}
void JsonArchive::Serialize(const char* name, bool& val)
{
	if (IsLoading())
	{
		const Json& field = Data()[name];
		if (field.is_boolean())
		{
			val = field.get<bool>();
		}
	} else
	{
		Data()[name] = val;
	}
}

void JsonArchive::Serialize(const char* name, uint8& val)
{
	if (IsLoading())
	{
		const Json& field = Data()[name];
		if (field.is_number_unsigned())
		{
			val = field.get<uint8>();
		}
	} else
	{
		Data()[name] = val;
	}
}

void JsonArchive::Serialize(const char* name, int32& val)
{
	if (IsLoading())
	{
		const Json& field = Data()[name];
		if (field.is_number())
		{
			val = field.get<int32>();
		}
	} else
	{
		Data()[name] = val;
	}
}

void JsonArchive::Serialize(const char* name, uint32& val)
{
	if (IsLoading())
	{
		const Json& field = Data()[name];
		if (field.is_number_unsigned())
		{
			val = field.get<uint32>();
		}
	} else
	{
		Data()[name] = val;
	}
}

void JsonArchive::Serialize(const char* name, float& val)
{
	if (IsLoading())
	{
		const Json& field = Data()[name];
		if (field.is_number())
		{
			val = field.get<float>();
		}
	} else
	{
		Data()[name] = val;
	}
}

void JsonArchive::Serialize(const char* name, std::string& val)
{
	if (IsLoading())
	{
		const Json& field = Data()[name];
		if (field.is_string())
		{
			val = field.get<std::string>();
		}
	} else
	{
		Data()[name] = val;
	}
}

void JsonArchive::Serialize(const char* name, Json& val)
{
	if (IsLoading())
	{
		val = Data()[name];
	} else
	{
		Data()[name] = val;
	}
}


void JsonArchive::Serialize(const char* name, XClass*& val)
{
	if (IsLoading())
	{
		const Json& field = Data()[name];
		if (field.is_string())
		{
			std::string className = field.get<std::string>();
			val = XObject::StaticClass()->FindChild(className);

		}
	} else
	{
		Data()[name] = val->GetName();
	}

}

void JsonArchive::Serialize(const char* name, class SceneComponent*& val)
{
	if (IsLoading())
	{
		BeginObject(name);
		if(val)
			val->Serialize(*this);
		EndObject();
	} else
	{
		BeginObject(name);
		if(val)
			val->Serialize(*this);
		EndObject();
	}
}

void JsonArchive::Serialize(const char* name, SVertex& val)
{
	BeginObject(name);
	{
		Serialize("position", val.Position);
		Serialize("normal", val.Normal);
		Serialize("tangent", val.Tangent);
		Serialize("uv", val.UV);

		Serialize("blendindex", val.BlendIndex);
		Serialize("blendweight", val.BlendWeight);

	}
	EndObject();
}

void JsonArchive::Serialize(const char* name, SVector2& val)
{
	BeginObject(name);
	{
		Serialize("x", val.x);
		Serialize("y", val.y);		
	}
	EndObject();
}

void JsonArchive::Serialize(const char* name, SVector3& val)
{
	BeginObject(name);
	{
		Serialize("x", val.x);
		Serialize("y", val.y);
		Serialize("z", val.z);
	}
	EndObject();
}

void JsonArchive::Serialize(const char* name, SVector4& val)
{
	BeginObject(name);
	{
		Serialize("x", val.x);
		Serialize("y", val.y);
		Serialize("z", val.z);
		Serialize("w", val.w);
	}
	EndObject();
}

void JsonArchive::Serialize(const char* name, SLinearColor& val)
{
	BeginObject(name);
	{
		Serialize("r", val.r);
		Serialize("g", val.g);
		Serialize("b", val.b);
		Serialize("a", val.a);
	}
	EndObject();
}

void JsonArchive::Serialize(const char* name, class TextureAsset*& val)
{
	if (IsLoading())
	{
		BeginObject(name);
		const SPathInfo* texPathInfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
		//const SPathInfo* resRootPath = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);

		std::string FullPath = texPathInfo->aPath;

		std::string texFileName;
		std::string relativePath;

		Serialize("m_Name", OUT texFileName);

		if (!texFileName.empty())
		{
		
			Serialize("m_RelativePath", OUT relativePath);

			FullPath.append(relativePath);
			FullPath.append(texFileName);
			FullPath.append(".meta");

			val = (TextureAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(FullPath), TextureAsset::StaticClass());
		}


		EndObject();
	}
	else
	{

		BeginObject(name);
		if (val)
			val->Serialize(*this);
		EndObject();
	}
}

void JsonArchive::Serialize(const char* name, class Material*& val)
{
	if (IsLoading())
	{
		BeginObject(name);
		const SPathInfo* materialPathInfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);

		std::string FullPath = materialPathInfo->aPath;

		std::string matFileName;
		std::string relativePath;

		Serialize("m_Name", OUT matFileName);

		if (!matFileName.empty())
		{
			
			Serialize("m_RelativePath", OUT relativePath);

			FullPath.append(relativePath);
			FullPath.append(matFileName);
			FullPath.append(".meta");

			val = (Material*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(FullPath), Material::StaticClass());
		}
		EndObject();
	} 
	else
	{

		BeginObject(name);
		if (val)
		{
			if (val->IsValidate())
			{
				std::string name = val->GetName();
				std::string relPath = val->GetRelativePath();

				Serialize("m_Name", name);
				Serialize("m_RelativePath", relPath);
				//val->Serialize(*this);
			}
		}
		EndObject();
	}
}

void JsonArchive::Serialize(const char* name, class Skeleton*& val)
{
	if (IsLoading())
	{
		BeginObject(name);
		const SPathInfo* skeletonPathInfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);

		std::string FullPath = skeletonPathInfo->aPath;

		std::string skelFileName;
		std::string relativePath;

		Serialize("m_Name", OUT skelFileName);

		if (!skelFileName.empty())
		{

			Serialize("m_RelativePath", OUT relativePath);

			FullPath.append(relativePath);
			FullPath.append(skelFileName);
			FullPath.append(".meta");

			val = (Skeleton*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(FullPath), Skeleton::StaticClass());
		}

		EndObject();
	} 
	else
	{
		BeginObject(name);
		if (val)
		{
			if (val->IsValidate())
			{
				std::string name = val->GetName();
				std::string relPath = val->GetRelativePath();

				Serialize("m_Name", name);
				Serialize("m_RelativePath", relPath);
				//val->Serialize(*this);
			}
		}
		EndObject();
	}

}

void JsonArchive::Serialize(const char* name, class SkeletalMeshSocket*& val)
{
	if (IsLoading())
	{
		if(!val)
			val = Globals::CreateX<SkeletalMeshSocket>(nullptr, "MeshSocket");		
		
		val->Serialize(*this);
	}
	else
	{
		if (val)
			val->Serialize(*this);
	}
}

void JsonArchive::Serialize(const char* name, class SkeletalMeshAsset*& val)
{
	if (IsLoading())
	{
		BeginObject(name);
		const SPathInfo* skeletonPathInfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
		//const SPathInfo* resRootPath = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);

		std::string FullPath = skeletonPathInfo->aPath;

		std::string skelFileName;
		std::string relativePath;

		Serialize("m_Name", OUT skelFileName);

		if (!skelFileName.empty())
		{

			Serialize("m_RelativePath", OUT relativePath);

			FullPath.append(relativePath);
			FullPath.append(skelFileName);
			FullPath.append(".meta");

			val = (SkeletalMeshAsset*)g_Engine->Get<AssetManager>()->Load(AssetInfo(FullPath));
				//LoadOrCreate(AssetInfo(FullPath), SkeletalMeshAsset::StaticClass());
		}

		EndObject();
	} else
	{
		BeginObject(name);
		if (val)
		{
			if (val->IsValidate())
			{
				std::string name = val->GetName();
				std::string relPath = val->GetRelativePath();

				Serialize("m_Name", name);
				Serialize("m_RelativePath", relPath);
				//val->Serialize(*this);
			}
		}
		EndObject();
	}
}

void JsonArchive::Serialize(const char* name, class AnimationSequence*& val)
{
	if (IsLoading())
	{
		BeginObject(name);
		const SPathInfo* skeletonPathInfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
		//const SPathInfo* resRootPath = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);

		std::string FullPath = skeletonPathInfo->aPath;

		std::string skelFileName;
		std::string relativePath;

		Serialize("m_Name", OUT skelFileName);

		if (!skelFileName.empty())
		{

			Serialize("m_RelativePath", OUT relativePath);

			FullPath.append(relativePath);
			FullPath.append(skelFileName);
			FullPath.append(".meta");

			val = (AnimationSequence*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(FullPath), AnimationSequence::StaticClass());
		}

		EndObject();
	} else
	{
		BeginObject(name);
		if (val)
		{
			if (val->IsValidate())
			{
				std::string name = val->GetName();
				std::string relPath = val->GetRelativePath();

				Serialize("m_Name", name);
				Serialize("m_RelativePath", relPath);
				//val->Serialize(*this);
			}
		}
		EndObject();
	}
}

void JsonArchive::Serialize(const char* name, class MeshAsset*& val)
{
	if (IsLoading())
	{
		BeginObject(name);
		const SPathInfo* skeletonPathInfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
		//const SPathInfo* resRootPath = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);

		std::string FullPath = skeletonPathInfo->aPath;

		std::string skelFileName;
		std::string relativePath;

		Serialize("m_Name", OUT skelFileName);

		if (!skelFileName.empty())
		{

			Serialize("m_RelativePath", OUT relativePath);

			FullPath.append(relativePath);
			FullPath.append(skelFileName);
			FullPath.append(".meta");

			val = (MeshAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(FullPath), MeshAsset::StaticClass());
		}

		EndObject();
	} else
	{
		BeginObject(name);
		if (val)
		{
			if (val->IsValidate())
			{
				std::string name = val->GetName();
				std::string relPath = val->GetRelativePath();

				Serialize("m_Name", name);
				Serialize("m_RelativePath", relPath);
				//val->Serialize(*this);
			}
		}
		EndObject();
	}
}

void JsonArchive::Serialize(const char* name, class GameObject*& val)
{
	if (IsLoading())
	{
		BeginObject(name);

		const SPathInfo* objectPathInfo = g_Engine->Get<PathManager>()->FindPath(GAMEOBJECT_PATH);

		std::string FullPath = objectPathInfo->aPath;

		std::string objectFileName;
		std::string relativePath;

		Serialize("m_Name", OUT objectFileName);
		GameObject* cdo = nullptr;
		if (!objectFileName.empty())
		{
			//Sprite의 Relative를 
			Serialize("m_RelativePath", OUT relativePath);

			FullPath.append(relativePath);
			FullPath.append(objectFileName);
			FullPath.append(".meta");


			cdo = (GameObject*)g_Engine->Get<AssetManager>()->Load(AssetInfo(FullPath));
		}


		XClass* loadClass;
		Serialize("class_type", loadClass);

		TSubclassOf<GameObject> customCdo;
		if (cdo)
		{
			std::string customCDOName;
			cdo->GetRelativePathWithName(OUT customCDOName);
			customCdo = cdo->GetClass()->GetCustomCDO(customCDOName);
			val = ObjectManager::CreateGameObject<GameObject>(customCdo);
		} else
		{
			val = ObjectManager::CreateGameObject<GameObject>((TClass<GameObject>*)loadClass);
		}


		val->Serialize(*this);

		EndObject();
	} else
	{
		BeginObject(name);

		if (val)
		{
			if (val->IsValidate())
			{
				std::string name = val->GetName();
				std::string relPath = val->GetRelativePath();

				size_t pos = name.rfind("_");
				if (pos != std::string::npos)
				{
					name = name.substr(0, pos);
				}
				val->Serialize(*this);
				Serialize("m_Name", name);
				//val->Serialize(*this);
				XClass* saveClass = val->GetClass();
				Serialize("class_type", saveClass);

			}
		}

		EndObject();
	}
}

void JsonArchive::Serialize(const char* name, XEnum& val)
{
	int32 EnumVal = val.GetUnderlying();
	if (IsLoading())
	{

		Serialize(name, EnumVal);
		val.Set(EnumVal);

	} else
	{
		Serialize(name, EnumVal);
	}
}

void JsonArchive::Serialize(const char* name, SCollisionProfile*& val)
{
	//커스텀 프리셋 가능하게 수정요망
	if (IsLoading())
	{
		BeginObject(name);

		std::string profileName;
		Serialize("ProfileName", profileName);


		if (name != profileName)
		{
			SCollisionProfile* findProfile = g_Engine->Get<CollisionManager>()->FindProfile(profileName);
			if (findProfile)
			{
				val = findProfile;
				EndObject();
				return;
			}
			if (profileName.empty())
			{
				EndObject();
				return;
			}

			if (val)
			{
				if (val->Defaults)
				{
					val = nullptr;
					EndObject();
					return;
				}
			}
		}

		bool Enabled = false;
		std::string channelName;
		std::string Desc;
		if (val->Channel)
		{
			channelName = val->Channel->Name;
		}



		Serialize("ProfileName", val->Name);
		Serialize("Enabled", val->Enable);
		Serialize("ChannelName", channelName);
		Serialize("Descrition", val->Descrition);
		Serialize("Defaults", val->Defaults);

		std::vector<int32> interactions;
		Archive::Serialize<int32>("Interactions", interactions);

		if (val->vecCollisionInteraction.size() < interactions.size())
			val->vecCollisionInteraction.resize(interactions.size());

		for (size_t i = 0; i < interactions.size(); i++)
		{
			val->vecCollisionInteraction[i] = ((ECollision_Interaction)interactions[i]);
		}
		val->Channel = g_Engine->Get<CollisionManager>()->GetCollisionChannel(channelName);
		if (!val->Channel)
		{
			val->Channel = g_Engine->Get<CollisionManager>()->GetCollisionChannel("Default");
		}


		EndObject();
	} else
	{
		BeginObject(name);
		std::string profileName = val->Name;

		Serialize("ProfileName", val->Name);

		if (name != profileName)
		{
			SCollisionProfile* findProfile = g_Engine->Get<CollisionManager>()->FindProfile(profileName);
			if (findProfile)
			{
				val = findProfile;
				EndObject();
				return;
			}
			if (profileName.empty())
			{
				EndObject();
				return;
			}

			if (val)
			{
				if (val->Defaults)
				{
					val = nullptr;
					EndObject();
					return;
				}

			}
		}




		Serialize("Enabled", val->Enable);
		Serialize("ChannelName", val->Channel->Name);
		Serialize("Descrition", val->Descrition);
		Serialize("Defaults", val->Defaults);

		std::vector<int32> interactions;
		for (size_t i = 0; i < val->vecCollisionInteraction.size(); i++)
		{
			interactions.push_back((int32)val->vecCollisionInteraction[i]);
		}

		Archive::Serialize<int32>("Interactions", interactions);

		EndObject();
	}
}

void JsonArchive::Serialize(const char* name, SCollisionChannel*& val)
{
	if (IsLoading())
	{
		BeginObject(name);



		int32 interaction = 0;
		int32 channel = 0;
		Serialize("Name", val->Name);
		Serialize("Interaction", interaction);
		Serialize("Channel", channel);

		val->Interaction = (ECollision_Interaction)interaction;
		val->Channel = (ECollision_Channel)channel;


		EndObject();
	} else
	{
		BeginObject(name);
		//Save Only Custom..
		if ((int32)val->Channel >= (int32)ECollision_Channel::Custom0)
		{
			std::string Name = val->Name;
			Serialize("Name", Name);
			Serialize("Interaction", (int32&)val->Interaction);
			Serialize("Channel", (int32&)val->Channel);

		}


		EndObject();
	}
}

void JsonArchive::Serialize(const char* name, SActionKeyIfno& val)
{
	BeginObject(name);
	Serialize("actionName", val.ActionName);
	Serialize("shift", val.Shift);
	Serialize("alt", val.Alt);
	Serialize("ctrl", val.Ctrl);
	EndObject();
}

void JsonArchive::Serialize(const char* name, SKeyAxisValue& val)
{
	BeginObject(name);
	Serialize("key", val.Key);
	Serialize("scale", val.scale);
	Serialize("shift", val.Shift);
	Serialize("alt", val.Alt);
	Serialize("ctrl", val.Ctrl);
	EndObject();
}

void JsonArchive::Serialize(const char* name, XSubclassOf& val)
{
	if (IsLoading())
	{
		BeginObject(name);
		Serialize("class", (XClass*&)val.Class);
		std::string rootPathKey;
		Serialize("rootPathKey", rootPathKey);		
		Serialize("customCDOName", val.CustomCDOName);
		if (!val.CustomCDOName.empty())
		{
			const SPathInfo* ResRoot = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
			const SPathInfo* CDORootPath = g_Engine->Get<PathManager>()->FindPath(rootPathKey);
			fs::path cdoPath = CDORootPath->aPath;
			fs::path resRootPath = ResRoot->aPath;
			
			std::string removal = cdoPath.lexically_relative(resRootPath).string();
			std::string cdoWithPath = val.CustomCDOName;
			size_t pos = cdoWithPath.find(removal);			
			if (pos != std::string::npos)
			{
				cdoWithPath = cdoWithPath.substr(removal.length());
			}

			std::string FullPath = CDORootPath->aPath;
			FullPath.append(cdoWithPath);
			FullPath.append(".meta");

			//GetCustomCDOPathKey
			g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(FullPath), val.Class);
		}
		EndObject();
	} else
	{
		BeginObject(name);
		Serialize("class", (XClass*&)val.Class);

		std::string rootPathKey = val.Class->GetCustomCDOPathKey();

		Serialize("rootPathKey", rootPathKey);
		Serialize("customCDOName", val.CustomCDOName);

		AssetData* customCDO = (AssetData*)val.Class->GetCustomCDO(val.CustomCDOName);
		if (customCDO)
		{
			std::string pathWithName;
			customCDO->GetRelativePathWithName(OUT pathWithName);
			Serialize("loadCDO_path", pathWithName);
		}

		EndObject();
	}


	//커스텀CDO의 RelativePAth를 저장 및 로드..
}

void JsonArchive::Serialize(const char* name, AnimNotifyInfo*& val)
{
	if (IsLoading())
	{
		BeginObject(name);
		if (!val)
		{
			val = Globals::CreateX<AnimNotifyInfo>(nullptr, name);
		}
		val->Serialize(*this);
		EndObject();
	} else
	{
		BeginObject(name);
		val->Serialize(*this);
		EndObject();
	}
}

void JsonArchive::Serialize(const char* name, AnimNotify*& val)
{
	if (IsLoading())
	{
		BeginObject(name);
		val->Serialize(*this);
		EndObject();
	} else
	{
		BeginObject(name);
		val->Serialize(*this);
		EndObject();
	}
}

void JsonArchive::Serialize(const char* name, class ObjectComponent*& val)
{
	if (IsLoading())
	{
		BeginObject(name);
		val->Serialize(*this);
		EndObject();
	} else
	{
		BeginObject(name);
		val->Serialize(*this);
		EndObject();
	}
}

void JsonArchive::Serialize(const char* name, SGameplayAttributeData& val)
{
	BeginObject(name);
	Serialize("base_value", val.BaseValue);
	Serialize("current_value", val.CurrentValue);
	EndObject();
}

void JsonArchive::Serialize(const char* name, SGameplayAttribute& val)
{

	//프로퍼티를 저장하고 불러올수 있어야함..
	if (IsLoading())
	{
		BeginObject(name);
		XClass* containerType = nullptr;
		Serialize("container_type", containerType);
		std::string propName;
		Serialize("property_name", propName);
		if (containerType)
		{
			FPropertyMap props;
			containerType->GetOwnProperties(OUT props);
			FPropertyMap::iterator finditer = props.find(propName);
			if (finditer != props.end())
			{
				val.Attribute = finditer->second;
			}

		}
		EndObject();

	} else
	{
		BeginObject(name);
		//어떤 AttributeSet의 Attribute 인가>?
		if (val.Attribute)
		{
			XClass* containerType = val.Attribute->GetContainerType();
			std::string attributeName = val.Attribute->GetName();
			Serialize("container_type", containerType);
			Serialize("property_name", attributeName);

		}
		EndObject();
	}
}

void JsonArchive::Serialize(const char* name, AttributeModifer*& val)
{
	if (IsLoading())
	{
		if(!val)
		{
			val = Globals::CreateX<AttributeModifer>();
		}
		val->Serialize(*this);
	}
	else
	{
		if (val)
		{
			val->Serialize(*this);
		}
	}
}

void JsonArchive::Serialize(const char* name, BehaviorTree*& val)
{
	if (IsLoading())
	{
		BeginObject(name);
		const SPathInfo* aiPathInfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);

		std::string FullPath = aiPathInfo->aPath;

		std::string btreeFileName;
		std::string relativePath;

		Serialize("m_Name", OUT btreeFileName);

		if (!btreeFileName.empty())
		{

			Serialize("m_RelativePath", OUT relativePath);

			FullPath.append(relativePath);
			FullPath.append(btreeFileName);
			FullPath.append(".meta");

			val = (BehaviorTree*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(FullPath), BehaviorTree::StaticClass());
		}

		EndObject();
	} else
	{
		BeginObject(name);
		if (val)
		{
			if (val->IsValidate())
			{
				std::string name = val->GetName();
				std::string relPath = val->GetRelativePath();

				Serialize("m_Name", name);
				Serialize("m_RelativePath", relPath);
				//val->Serialize(*this);
			}
		}
		EndObject();
	}
}

void JsonArchive::Serialize(const char* name, Scene*& val)
{
	if (IsLoading())
	{
		BeginObject(name);
		const SPathInfo* scenePathInfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);

		std::string FullPath = scenePathInfo->aPath;

		std::string sceneFileName;
		std::string relativePath;

		Serialize("m_Name", OUT sceneFileName);

		Serialize("m_RelativePath", OUT relativePath);

		FullPath.append(relativePath);
		FullPath.append(sceneFileName);
		FullPath.append(".meta");

		val = (Scene*)g_Engine->Get<AssetManager>()->Load(AssetInfo(FullPath));

		EndObject();
	}
	else
	{
		BeginObject(name);
		if (val)
		{
			if (val->IsValidate())
			{
				std::string name = val->GetName();
				std::string relPath = val->GetRelativePath();

				Serialize("m_Name", name);
				Serialize("m_RelativePath", relPath);
				//val->Serialize(this);
			}
		}
		EndObject();
	}
}

void JsonArchive::Serialize(const char* name, Sound*& val)
{
	if (IsLoading())
	{
		BeginObject(name);
		const SPathInfo* soundPathInfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);

		std::string FullPath = soundPathInfo->aPath;

		std::string soundFileName;
		std::string relativePath;

		Serialize("m_Name", OUT soundFileName);
		//Sprite의 Relative를 
		Serialize("m_RelativePath", OUT relativePath);

		FullPath.append(relativePath);
		FullPath.append(soundFileName);
		FullPath.append(".meta");

		val = (Sound*)g_Engine->Get<AssetManager>()->Load(AssetInfo(FullPath));

		EndObject();
	}
	else
	{
		BeginObject(name);
		if (val)
		{
			if (val->IsValidate())
			{
				std::string name = val->GetName();
				std::string relPath = val->GetRelativePath();

				Serialize("m_Name", name);
				Serialize("m_RelativePath", relPath);
				//val->Serialize(*this);
			}
		}
		EndObject();
	}
}

void JsonArchive::Serialize(const char* name, Blackboard*& val)
{
	if (IsLoading())
	{
		BeginObject(name);
		const SPathInfo* aiPathInfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);

		std::string FullPath = aiPathInfo->aPath;

		std::string bbFileName;
		std::string relativePath;

		Serialize("m_Name", OUT bbFileName);

		if (!bbFileName.empty())
		{

			Serialize("m_RelativePath", OUT relativePath);

			FullPath.append(relativePath);
			FullPath.append(bbFileName);
			FullPath.append(".meta");

			val = (Blackboard*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(FullPath), Blackboard::StaticClass());
		}

		EndObject();
	} else
	{
		BeginObject(name);
		if (val)
		{
			if (val->IsValidate())
			{
				std::string name = val->GetName();
				std::string relPath = val->GetRelativePath();

				Serialize("m_Name", name);
				Serialize("m_RelativePath", relPath);

			}
		}
		EndObject();
	}
}

void JsonArchive::Serialize(const char* name, SBlackboardKeySelector& val)
{
	if(IsLoading())
	{
		BeginObject(name);
		{

			Serialize("SeletedKeyName", val.SeletedKeyName);
			int32 selType = 0;
			Serialize("SeletedKeyType", selType);
			val.SeletedKeyType = (EBlackBoardValType)selType;

			BeginObject("allowTypes");
			std::vector<int32> types;	
			uint32 size;
			SerializeArraySize(size);
			val.AllowedTypes.resize(size);

			if (types.size() < size)
				types.resize(size);

			for (uint32 i = 0; i < size; ++i)
			{
				BeginObject(i);
				{
					operator()("val", types[i]);
					val.AllowedTypes[i] = (EBlackBoardValType)types[i];				
				}
				EndObject();
			}
			EndObject();
		}
		EndObject();
	}
	else
	{
		BeginObject(name);
		{

			Serialize("SeletedKeyName", val.SeletedKeyName);
			int32 selType = (int32)val.SeletedKeyType;
			Serialize("SeletedKeyType", selType);

			BeginObject("allowTypes");
			uint32 size = (uint32)val.AllowedTypes.size();
			SerializeArraySize(size);

			for (uint32 i = 0; i < size; ++i)
			{
				BeginObject(i);
				{
					//Data()[""] = Data(); // Temporal patch to avoid named properties
					int32 t  = (int32)val.AllowedTypes[i];
					operator()("val", t);
				}
				EndObject();
			}

			EndObject();

		}
		EndObject();
	}

}

void JsonArchive::Serialize(const char* name, struct SAnchorData& val)
{
	BeginObject(name);
	{

		Serialize("Alignment", val.Alignment);
		Serialize("Anchors.Max", val.Anchors.Maximum);
		Serialize("Anchors.Min", val.Anchors.Minimum);

		Serialize("Offsets.Left", val.Offsets.Left);
		Serialize("Offsets.Right", val.Offsets.Right);
		Serialize("Offsets.Bottom", val.Offsets.Bottom);
		Serialize("Offsets.Top", val.Offsets.Top);

	}
	EndObject();

}

void JsonArchive::Serialize(const char* name, struct SWidgetTransform& val)
{
	BeginObject(name);
	{
		Serialize("Translation", val.Translation);
		Serialize("Scale", val.Scale);
		Serialize("Angle", val.Angle);	
	}
	EndObject();
}

void JsonArchive::Serialize(const char* name, class FontAsset*& val)
{
	if (IsLoading())
	{
		BeginObject(name);
		const SPathInfo* texPathInfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
		//const SPathInfo* resRootPath = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);

		std::string FullPath = texPathInfo->aPath;

		std::string texFileName;
		std::string relativePath;

		Serialize("m_Name", OUT texFileName);

		if (!texFileName.empty())
		{

			Serialize("m_RelativePath", OUT relativePath);

			FullPath.append(relativePath);
			FullPath.append(texFileName);
			FullPath.append(".meta");

			val = (FontAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(FullPath), FontAsset::StaticClass());
		}


		EndObject();
	} else
	{

		BeginObject(name);
		if (val)
			val->Serialize(*this);
		EndObject();
	}

}

void JsonArchive::Serialize(const char* name, class ParticleAsset*& val)
{
	if (IsLoading())
	{
		BeginObject(name);
		const SPathInfo* texPathInfo = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);
		//const SPathInfo* resRootPath = g_Engine->Get<PathManager>()->FindPath(RESOURCE_ROOT_PATH);

		std::string FullPath = texPathInfo->aPath;

		std::string texFileName;
		std::string relativePath;

		Serialize("m_Name", OUT texFileName);

		if (!texFileName.empty())
		{

			Serialize("m_RelativePath", OUT relativePath);

			FullPath.append(relativePath);
			FullPath.append(texFileName);
			FullPath.append(".meta");

			val = (ParticleAsset*)g_Engine->Get<AssetManager>()->LoadOrCreate(AssetInfo(FullPath), ParticleAsset::StaticClass());
		}


		EndObject();
	} else
	{

		BeginObject(name);
		if (val)
			val->Serialize(*this);
		EndObject();
	}

}

