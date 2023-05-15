#include "EnginePCH.h"
#include "DirectionalLightComponent.h"
#include <Renderer/Renderer.h>
#include <Physics/PhysicsSystem.h>
#include <Physics/PhysicsDebugDraw.h>

#include <CameraManager.h>
#include "CameraComponent.h"


DirectionalLightComponent::DirectionalLightComponent()
{
	m_CameraOffset = SVector3(0.0f);
	m_Brightness = 20.0f;
}

DirectionalLightComponent::DirectionalLightComponent(const DirectionalLightComponent& Other)
	:Super(Other)	
{

}

DirectionalLightComponent::~DirectionalLightComponent()
{

}

void DirectionalLightComponent::OnUpdateTransform()
{
	SVector3 offset = SVector3(0.0);
	CameraManager* mgr = g_Engine->Get<CameraManager>();
	UpdateViewTransform(mgr);
	
	//
}

void DirectionalLightComponent::PostUpdate(float DeltaSeconds)
{
	Super::PostUpdate(DeltaSeconds);

	if (!IsPlaying() && g_Engine->IsEditMode())
	{		
		//DrawLine TO Target		
		g_Engine->Get<PhysicsSystem>()->GetDebugDraw()->drawArrow(GetWorldLocation(), GetWorldLocation() + SVector3::ForwardVector.TransformNormal(m_ComponentToWorld.ToMatrixNoScale()) * 800, 0, 100, SLinearColor::Red, 20);			
	}
	CameraManager* mgr = g_Engine->Get<CameraManager>();
	
	if (mgr)
	{
		
		SVector3 camPos = mgr->GetCurrentCamera()->GetWorldLocation();
		if((m_CameraOffset - camPos).Size() > 2048.0f)
			UpdateViewTransform(mgr);
		
	}

}

Matrix DirectionalLightComponent::CalculateProjectionMatrix(SVector2 viewPortSize)
{
	if (viewPortSize.x < 1.0f) 
		return XMMatrixIdentity();
	return XMMatrixOrthographicLH(viewPortSize.x, viewPortSize.y, Near, Far);
}

void DirectionalLightComponent::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	D3D11_VIEWPORT viewport = g_Engine->Get<Renderer>()->GetCurrentViewport();

	m_ProjectionMatrix = CalculateProjectionMatrix(SVector2(4096, 4096));

}


void DirectionalLightComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void DirectionalLightComponent::UpdateViewTransform(class CameraManager* Mgr)
{
	SVector3 offset = SVector3(0.0);	
	if (Mgr)
	{
		offset = Mgr->GetCurrentCamera()->GetWorldLocation();
		m_CameraOffset = offset;
	}

	m_ViewMatrix[0] = CalculateDirectionalLightViewMatrix(this, offset);
}

void DirectionalLightComponent::GatherLighting(SSceneLighting& OutSceneLighting, std::vector<SPointLight>& PointLights, std::vector<SPointLight>& PointLightCasters, std::vector<SSpotLight>& SpotLights)
{
	SDirectionalLight& DirectionalLight = OutSceneLighting.directional;

	DirectionalLight.brightness = m_Brightness;
	DirectionalLight.color = m_LightColor.ToVector3();
	//SVector3 Rotator = GetWorldRotation();
	//SVector3 direction = SVector3::ForwardVector;
	//direction = direction.TransformNormal(Matrix::StaticRotation(Rotator));
	DirectionalLight.lightDirection = SVector3::ForwardVector.TransformNormal(m_ComponentToWorld.ToMatrixNoScale());
	


	DirectionalLight.depthBias = m_DepthBias;
	DirectionalLight.shadowing = m_CastingShadow;
	DirectionalLight.enabled = true;
	OutSceneLighting.shadowViewDirectional = GetViewMatrix() * GetProjectionMatrix();
}
