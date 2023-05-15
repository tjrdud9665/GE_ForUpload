#pragma once

#define SMALL_NUMBER		(1.e-8f)
#define KINDA_SMALL_NUMBER	(1.e-4f)
#define PI 					(3.1415926535897932f)
#define INV_PI				(0.31830988618f)
#define HALF_PI				(1.57079632679f)	

#define RANGE(c) std::begin(c), std::end(c)
#define RRANGE(c) std::rbegin(c), std::rend(c)


//Invalid Index
#define INDEX_NONE -1

//OutParam
#define OUT 

#define __unique_string_inner2(str,c) str #c
#define __unique_string_inner(str,c) __unique_string_inner2(str,c)
#define UNIQUE_STRING(base) __unique_string_inner( base, __COUNTER__ )


//Deletes..
#define	SAFE_DELETE(p)	if(p)	{ delete p; p = nullptr; }
#define	SAFE_DELETE_ARRAY(p)	if(p)	{ delete[] p; p = nullptr; }
#define	SAFE_RELEASE(p)	if(p)	{ p->Release(); p = nullptr; }



//Path
#define	ROOT_PATH						"RootPath"
#define SETTING_PATH					"SettingPath"
#define RESOURCE_ROOT_PATH				"ResRootPath"

//Res 
#define	TEXTURE_PATH					"TexturePath"

#define	MATERIAL_PATH					"MaterialPath"
#define SKELETON_PATH					"SkeletonPath"

#define	SOUND_PATH						"SoundPath"
#define	FONT_PATH						"FontPath"
#define SHADER_PATH						"ShaderPath"
#define ANIMATION_PATH					"AnimatonPath"
#define PARTICLE_PATH					"ParticlePath"
#define MESH_PATH						"MeshPath"


//UI
#define WIDGET_PATH						"WidgetPath"
//GameFramework
#define SCENEINFO_PATH					"SceneInfoPath"
#define GAMEOBJECT_PATH					"GameObjectPath"
#define SCENE_PATH						"ScenePath"
#define ABILITYSYSTEM_PATH				"GameplayAbilityPath"

#define	AI_PATH							"AIPath"

//Editor Only EditorOnly Images.
#define EDIT_RES_PATH		"EditorContentPath"
