#include "EnginePCH.h"
#include "PointLightComponent.h"
#include "Physics/PhysicsSystem.h"
#include "Physics/PhysicsDebugDraw.h"

 PointLightComponent::PointLightComponent()
	 :Range(1500.0f)
{

}

 PointLightComponent::PointLightComponent(const PointLightComponent& Other)
	 :Super(Other)
	 , Range(Other.Range)
 {

 }

 PointLightComponent::~PointLightComponent()
 {

 }



 void PointLightComponent::Construct(const std::string& InName /* = "" */)
 {
	 Super::Construct(InName);

	 m_ProjectionMatrix = CalculateProjectionMatrix(SVector2(2048, 2048));
 }

 void PointLightComponent::PostUpdate(float DeltaSeconds)
 {
	 Super::PostUpdate(DeltaSeconds);
	 	
	 if (!IsPlaying() && g_Engine->IsEditMode())
	 {
		 //DrawLine TO Target
		 g_Engine->Get<PhysicsSystem>()->GetDebugDraw()->drawSphere(ToBtVector3(GetWorldLocation()), Range, btVector3(1, 0, 0));
	 }	 


 }

 void PointLightComponent::OnUpdateTransform()
 {
	 for (int i = 0; i < 6; ++i)
		 m_ViewMatrix[i] = CalculatePointLightViewMatrix(static_cast<D3DTexture::CubemapUtility::ECubeMapLookDirections>(i), GetWorldLocation());

 }

 void PointLightComponent::GatherLighting(SSceneLighting& OutSceneLighting, std::vector<SPointLight>& PointLights, std::vector<SPointLight>& PointLightCasters, std::vector<SSpotLight>& SpotLights)
 {
	

	 SPointLight pointLight;
	 pointLight.brightness = m_Brightness;
	 pointLight.color = m_LightColor.ToVector3();
	 pointLight.position = GetWorldLocation();
	 pointLight.depthBias = m_DepthBias;
	 pointLight.range = Range;
	 

	 PointLights[OutSceneLighting.numPointLights++] = pointLight;	
	 if (m_CastingShadow)
	 {		 	 
		 PointLightCasters[OutSceneLighting.numPointCasters++] = pointLight;
	 }
	 
 }

 const Matrix& PointLightComponent::GetViewMatrix(D3DTexture::CubemapUtility::ECubeMapLookDirections lookDir /*= DEFAULT_POINT_LIGHT_LOOK_DIRECTION*/) const
 {
	 return m_ViewMatrix[lookDir];
 }

 Matrix PointLightComponent::CalculateProjectionMatrix(SVector2 viewPortSize)
 {
	 constexpr float ASPECT_RATIO = 1.0f; // cubemap aspect ratio
	 return XMMatrixPerspectiveFovLH(XM_PIDIV2, ASPECT_RATIO, Near, Far);
 }

