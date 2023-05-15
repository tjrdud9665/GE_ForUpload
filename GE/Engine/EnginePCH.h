#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define NOMINMAX
#define	DIRECTINPUT_VERSION	0x0800


#define BT_THREADSAFE   1
#define BT_USE_PPL    1


#include <windows.h>
#undef GetObject
#include <tchar.h>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <exception>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <deque>
#include <string>
#include <queue>
#include <type_traits>
#include <typeinfo>
#include <typeindex>
#include <stdarg.h>
#include <filesystem>
#include <stack>
#include <random>
#include <tuple>



#include <d3d11.h>
#include <d3dcompiler.h>
#include <dinput.h>
#include <dwrite.h>
#include <dwrite_3.h>
#include <d2d1.h>
#include <process.h>


#include <DirectXMath.h>
#include <dxgidebug.h>
#include <DirectXPackedVector.h>

//#include "DirectTex/DirectXTex.h"
#include "fmod.hpp"

using namespace DirectX;
using namespace std;
namespace fs = std::filesystem;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "fmod64_vc.lib")



#ifdef _DEBUG
#pragma comment(lib, "DirectXTex_Debug.lib")

//Recast 
#pragma comment(lib, "Detour-d.lib")
#pragma comment(lib, "DetourCrowd-d.lib")
#pragma comment(lib, "DebugUtils-d.lib")
#pragma comment(lib, "DetourTileCache-d.lib")
#pragma comment(lib, "Recast-d.lib")

//Physics Bullet
#pragma comment(lib, "BulletCollision_debug.lib")
#pragma comment(lib, "BulletDynamics_debug.lib")
#pragma comment(lib, "BulletSoftBody_debug.lib")
#pragma comment(lib, "DebugUtils-d.lib")
#pragma comment(lib, "LinearMath_debug.lib")

#pragma comment(lib, "freetype_debug.lib")

#else
//DX TEX
#pragma comment(lib, "DirectXTex.lib")

//Recast 
#pragma comment(lib, "Detour.lib")
#pragma comment(lib, "DetourCrowd.lib")
#pragma comment(lib, "DebugUtils.lib")
#pragma comment(lib, "DetourTileCache.lib")
#pragma comment(lib, "Recast.lib")


//Physics Bullet
#pragma comment(lib, "BulletCollision.lib")
#pragma comment(lib, "BulletDynamics.lib")
#pragma comment(lib, "BulletSoftBody.lib")
#pragma comment(lib, "DebugUtils.lib")
#pragma comment(lib, "LinearMath.lib")

#pragma comment(lib, "freetype.lib")

#endif 

#include "Types.h"
#include "EngineMacros.h"
#include "Reflection/EnumTypes.h"
#include "Reflection/TEnum.h"

#include "Flags.h"
#include "Math/MathUtils.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/LinearColor.h"
#include "Math/Transform.h"
#include "Math/Matrix.h"
#include "Math/FrustumPlaneset.h"
#include "DirectXTex.h"

#include "Renderer/D3DTexture.h"
#include "Structs.h"
#include "Renderer/D3DBuffer.h"




#include "ReflectionTypes.h"
#include "XObject/XObject.h"
#include "SharedPtr/TSharedPtr.h"

#include "Globals.h"






#include "Engine.h"