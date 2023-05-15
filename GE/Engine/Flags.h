#pragma once

ENUM(EAxis, X = 0, Y, Z, MAX);

ENUM(EProjectionType, Orthographic = 0, Perspective , INVALID);

ENUM(EColliderShape, ColliderShape_Box = 0, ColliderShape_Sphere, ColliderShape_StaticPlane, ColliderShape_Cylinder, ColliderShape_Capsule, ColliderShape_Cone, ColliderShape_Mesh);

ENUM(EHorizontalAlignment, HAlign_Fill = 0, HAlign_Left, HAlign_Center, HAlign_Right);

ENUM(EVerticalAlignment, VAlign_Fill = 0, VAlign_Top, VAlign_Center, VAlign_Bottom);

ENUM(EOrientation, Orient_Horizontal = 0, Orient_Vertical);


ENUM(ETextAlignment, TAlign_Left= 0, TAlign_Center, TAlign_RIght);

ENUM(EProgressBarDir, RightToLeft = 0, LeftToRight, TopToBottom, BottomToTop);


enum EGeometry
{
	//TRIANGLE = 0,
	QUAD = 0,
	FULLSCREENQUAD,
	CUBE,
	CYLINDER,
	SPHERE,
	GRID,
	CONE,
	LIGHT_CUE_CONE,
	//BONE,

	MESH_TYPE_COUNT
};

/*=========Renderings=========*/
enum class EBufferUsage
{	
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ff476259(v=vs.85).aspx
	GPU_READ				= D3D11_USAGE_IMMUTABLE,	//GPU Read only
	GPU_READ_WRITE			= D3D11_USAGE_DEFAULT,		//GPU RW
	GPU_READ_CPU_WRITE		= D3D11_USAGE_DYNAMIC,		//GPU(ReadOnly) ,CPU (WrtieOnly)  /Map ,UnMap
	GPU_READ_CPU_READ		= D3D11_USAGE_STAGING,		//GPU to CPU

	BUFFER_USAGE_COUNT
};

enum class EBufferType 
{
	VERTEX_BUFFER = D3D11_BIND_VERTEX_BUFFER,
	INDEX_BUFFER = D3D11_BIND_INDEX_BUFFER,
	

	// Shader Reflection 에서 잡아서할거임
	// CONSTANT_BUFFER, // this can fit here			
	RWSTRUCTED_BUFFER			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,
	//STRUCTED_DYNAMIC_BUFFER	= D3D11_BIND_UNORDERED_ACCESS,

	BUFFER_TYPE_UNKNOWN,
	BUFFER_TYPE_COUNT
};

enum  EShaderStage 
{	// used to map **SetShaderConstant(); function in Renderer::Apply()
	VS = 0,
	GS,
	DS,
	HS,
	PS,
	CS,

	COUNT
};

enum EShaderStageFlags : uint16
{
	SHADER_STAGE_NONE = 0x00000000,
	SHADER_STAGE_VS = 0x00000001,
	SHADER_STAGE_GS = 0x00000002,
	SHADER_STAGE_DS = 0x00000004,
	SHADER_STAGE_HS = 0x00000008,
	SHADER_STAGE_PS = 0x00000010,
	SHADER_STAGE_ALL_GRAPHICS = 0X0000001F,
	SHADER_STAGE_CS = 0x00000020,

	SHADER_STAGE_COUNT = 6
};


enum class EPrimitiveTopology
{
	POINT_LIST = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
	TRIANGLE_LIST = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	LINE_LIST = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,

	TOPOLOGY_COUNT
};

enum ETextureUsage : uint16
{
	RESOURCE = D3D11_BIND_SHADER_RESOURCE,		
	RENDER_TARGET = D3D11_BIND_RENDER_TARGET,	
	RENDER_TARGET_RW = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
	DEPTH_TARGET = D3D11_BIND_DEPTH_STENCIL,
	COMPUTE_RW_TEXTURE = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,

	TEXTURE_USAGE_COUNT
};

enum ECPUAccess : uint32
{
	NONE = 0,
	CPU_R = D3D11_CPU_ACCESS_READ,
	CPU_W = D3D11_CPU_ACCESS_WRITE,
	CPU_RW = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE
};



enum EDefaultRasterizerState
{
	// FILL
	CULL_NONE = 0,
	CULL_FRONT,
	CULL_BACK,
	CULL_NONE_Rect,
	// WIREFRAME
	WIREFRAME,

	RASTERIZER_STATE_COUNT
};


enum EDefaultBlendState
{
	DISABLED,
	ADDITIVE_COLOR,
	ALPHA_BLEND,

	BLEND_STATE_COUNT
};

enum EDefaultDepthStencilState
{
	DEPTH_STENCIL_WRITE,
	DEPTH_STENCIL_DISABLED,
	DEPTH_WRITE,
	STENCIL_WRITE,
	DEPTH_TEST_ONLY,
	// add more as needed

	DEPTH_STENCIL_STATE_COUNT
};

enum EDefaultSamplerState
{
	POINT_SAMPLER,
	POINT_SAMPLER_WRAP_UVW,
	LINEAR_FILTER_SAMPLER_WRAP_UVW,
	LINEAR_FILTER_SAMPLER,

	//WRAP_SAMPLER, //

	ANISOTROPIC_1_CLAMPED_SAMPLER,
	ANISOTROPIC_2_CLAMPED_SAMPLER,
	ANISOTROPIC_4_CLAMPED_SAMPLER,
	ANISOTROPIC_16_CLAMPED_SAMPLER,

	ANISOTROPIC_1_WRAPPED_SAMPLER,
	ANISOTROPIC_2_WRAPPED_SAMPLER,
	ANISOTROPIC_4_WRAPPED_SAMPLER,
	ANISOTROPIC_16_WRAPPED_SAMPLER,

	DEFAULT_SAMPLER_COUNT
};


enum EShaders : uint32// built-in shaders
{
	COMPUTE_ANIMATION =0,
	FORWARD_PHONG ,
	UNLIT,
	TEXTURE_COORDINATES,
	NORMAL,
	TANGENT,
	BINORMAL,
	LINE,
	SOLID,
	TBN,
	DEBUG,
	SKYBOX,
	SKYBOX_EQUIRECTANGULAR,
	//FORWARD_BRDF,
	SHADOWMAP_DEPTH,
	SHADOWMAP_DEPTH_INSTANCED,
	TEXT,
	PARTICLE_UPDATE,
	PARTICLE_RENDER,


	SHADER_COUNT
};

enum ELightType : uint32
{
	EDirectionalLight,
	EPointLight,
	ESpotLight,
	
};

enum class ECollisionReportMode
{
	None, /**< No collision events will be triggered. */
	Report, /**< Collision events will be triggered when object enters and/or leaves collision. */
	/**
	 * Collision events will be triggered when object enters and/or leaves collision, but also every frame the object
	 * remains in collision.
	 */
	 ReportPersistent,
};

enum class ECollision_Channel : uint32
{
	Default,
	Mouse,

	Custom0,
	Custom1,
	Custom2,
	Custom3,
	Custom4,
	Custom5,
	Custom6,
	Custom7,
	Custom8,
	Custom9,
	Custom10,
	Custom11,
	Custom12,
	Custom13,
	Custom14,
	Custom15,
	Custom16,
	Max
};


enum class ECollision_Interaction
{
	Ignore,
	Overlap,
	Block
};
//Overlap
//Block


enum class EBlackBoardValType
{
	Bool = 0,
	Int,
	Float,
	Vector,
	Object,
	Ability,
	MAX
};

