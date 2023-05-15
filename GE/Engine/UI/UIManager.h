#pragma once

#include "../XObject/XObject.h"


/*
*  SceneViewport 및 UI 를 관리
*/
class UIManager :public XObject
{
	friend class Globals;
	friend class Engine;
	friend class SceneViewport;	


	//using FPanelPosContainer = std::array<SVector3, 6>;
	//using FPanelUvContainer = std::array<SVector2, 6>;


private:
	UIManager();
	virtual ~UIManager();

public:
	void Initialize(class Renderer* pRenderer);

	void ClearGeometry();

	void BeginUI();
	void EndUI();
	void RenderUI();



	void PushClipSpace(const SScissorsRect& InClipSpace);

	void PushSlot(class PanelSlot* slot, FTextureID texID);

	void PushTexturedTri(FTextureID texID,const SVector4& InColor,const SVertex_PUV* triangles, const uint32 count,const SScissorsRect& InClipRect = SScissorsRect());	
	

	FTextureID GetUITargetTexture() const;



private:
	
	std::vector<uint32> m_TriangleBufferPtrs;
	//Make Array - > Optimize~~~
	std::vector<FTextureID> m_TriangleBufferTextures;
	std::vector<SVector4>	m_RenderColor;

	std::vector<SScissorsRect>	m_ClipRects;
	

	//시간없다..
	//std::vector<Matrix>	 m_TriangleTransform;


	FBufferID					m_VtxID;

	class Renderer* m_CachedRenderer;

public:
	FRenderTargetID				m_PanelTarget;

};

