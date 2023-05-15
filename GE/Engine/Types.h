#pragma once

using uchar8 = unsigned char;
using char8 = char;
using char16 = wchar_t;

using byte = unsigned char;
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

template<typename T>
inline type_index typeidx()
{
	return type_index(typeid(T));
}

//파이프라인 스테이트를 ID로만 구성합니다.
//포멧   F()ID
//타입 int32
/* ====Renderings==== */
using FShaderID				= int32;
using FBufferID				= int32;


using FIAPair = std::pair<FBufferID, FBufferID>;


//Tex Smp
using FSamplerID			= int32;
using FTextureID			= int32;

//RenderState
using FRasterizerStateID	= int32;	
using FBlendStateID			= int32;
using FDepthStencilStateID	= int32;
using FRenderTargetID		= int32;
using FDepthTargetID		= int32;

using FRenderTargetIDs = std::vector<FRenderTargetID>;

using FMaterialID			= int32;



