#include "EnginePCH.h"
#include "CameraComponent.h"
#include "../../D3D/D3DManager.h"
#include <Physics/PhysicsSystem.h>
#include <Renderer/Renderer.h>


CameraComponent::CameraComponent()
	:ViewDistance(10000.0f)
	, FieldOfView(90.0f)
	, ProjectionType(EProjectionType::Perspective)
	, PrevProjectionType(EProjectionType::INVALID)
{

}

CameraComponent::CameraComponent(const CameraComponent& Other)
	:Super(Other)
	, ViewDistance(Other.ViewDistance)
	, FieldOfView(Other.FieldOfView)
	, ProjectionType(Other.ProjectionType)
	, PrevProjectionType(EProjectionType::INVALID)
	, m_AspectRatio(Other.m_AspectRatio)
{

}

CameraComponent::~CameraComponent()
{

}

void CameraComponent::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	m_AspectRatio = g_Engine->Get<D3DManager>()->GetAspectRatio();

	m_RelativeLocation = SVector3(0, 0.0f, 0.0f);
	
	
}

void CameraComponent::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);

	Matrix mat = m_ComponentToWorld.ToMatrixNoScale();
	mat.Inverse();
	m_matView = mat;


	if(PrevProjectionType != ProjectionType)
	{
		if (ProjectionType == EProjectionType::Perspective)
		{
			m_matProj = DirectX::XMMatrixPerspectiveFovLH(SMathUtils::DegreesToRadians(FieldOfView), m_AspectRatio, 0.1f, ViewDistance);						
		}
		else
		{
			SVector2 res =g_Engine->Get<Renderer>()->FrameRenderTargetDimensionsAsFloat2();

			m_matProj = DirectX::XMMatrixOrthographicOffCenterLH(0.f, res.x, 0.f, res.y, 0.f, ViewDistance);
			
		}


	}

}


void CameraComponent::Render(class Renderer* pRenderer)
{
	//Set Update Scene
}

void CameraComponent::unproject(const D3D11_VIEWPORT& viewport, float x, float y, float depth, SVector3* dst) const
{
	SVector3 screen((x - viewport.TopLeftX) / viewport.Width , ((viewport.Height - y) - viewport.TopLeftY) / viewport.Height, depth);

	// Map to range -1 to 1.
	screen.x = screen.x * 2.0f - 1.0f;
	screen.y = screen.y * 2.0f - 1.0f;
	screen.z = screen.z * 2.0f - 1.0f;

	
	Matrix vp_inv = (m_matView * m_matProj);
	vp_inv.Inverse();
	screen = screen.TransformCoord(vp_inv);	
	
	
	// Divide by our W coordinate.
	if (depth != 0.0f)
	{
		screen.x /= depth;
		screen.y /= depth;
		screen.z /= depth;
	}

	(*dst) = SVector3(screen.x, screen.y, screen.z);
}

SVector3 CameraComponent::ScreenToWorldCoordinates(const SVector2& position_screen, const float z) const
{
	const D3D11_VIEWPORT& viewport = g_Engine->Get<Renderer>()->GetCurrentViewport();

	
	const Matrix projection = m_matProj;


	// Convert screen space position to clip space position
	SVector3 position_clip;
	position_clip.x = (position_screen.x / viewport.Width) * 2.0f - 1.0f;
	position_clip.y = (position_screen.y / viewport.Height) * -2.0f + 1.0f;
	position_clip.z = ::clamp(z, 0.0f, 1.0f);

	// Compute world space position
	Matrix view_projection_inverted = (m_matView * projection);
	view_projection_inverted.Inverse();
	
	SVector3 position_world = SVector3(position_clip);	
	position_world.TransformCoord(view_projection_inverted);
	

	return SVector3(position_world) / 1;
}

SHitResult CameraComponent::pickRay(const D3D11_VIEWPORT& viewport, float x, float y, SVector3& OutRayOrigin, SVector3& OutRayDireciton, float MaxDistance ) const
{
	SVector3 nearPoint;	
	unproject(viewport, x, y, 0.0f, &nearPoint);
	
	SVector3 farPoint;
	unproject(viewport, x, y, 1.0f, &farPoint);

	// Set the direction of the ray.
	SVector3 direction = farPoint - nearPoint;		
	direction.Normalize();
	OutRayDireciton = direction;
	OutRayOrigin = GetWorldLocation();
	

	SHitResult hit = g_Engine->Get<PhysicsSystem>()->rayTest(OutRayOrigin, OutRayDireciton, MaxDistance);	

	return hit;

}

void CameraComponent::SetProjectionType(EProjectionType InProjectionType)
{
	if(PrevProjectionType != EProjectionType::INVALID)
		PrevProjectionType = ProjectionType;
	ProjectionType = InProjectionType;

	
}

void CameraComponent::SetViewDistance(float Dist)
{
	ViewDistance = Dist;
}

void CameraComponent::SetTransformFromMatrix(const Matrix& InMatrix)
{
	Matrix matrix = InMatrix;
	SVector3 scale3D;
	SVector4 rotQuat;
	SVector3 translation;
	matrix.DecomposeMatrix(OUT scale3D, OUT rotQuat, OUT translation);	
	
	SVector3 EnulerAngle = rotQuat.ToEulerAngles();		
	SetWorldRotation(EnulerAngle);
	SetWorldLocation(translation);

}
