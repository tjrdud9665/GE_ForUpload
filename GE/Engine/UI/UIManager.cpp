#include "EnginePCH.h"
#include "UIManager.h"
#include <Renderer/Renderer.h>

//!Optimize  
//! 버텍스를 구조화버퍼에넣고 Texture 를 SRVArray로 만들어서 DrawInstanced로 교체
//! 

static const int32 max_canvas_vtx_buffer_bytes = 1048576; // 1 MiB
static const int32 max_canvas_vtx_buffer_vertices = max_canvas_vtx_buffer_bytes / sizeof(SVertex_PUV);

UIManager::UIManager()
	:m_VtxID(INDEX_NONE)
	,m_PanelTarget(INDEX_NONE)
{

}

UIManager::~UIManager()
{

}

void UIManager::Initialize(class Renderer* pRenderer)
{
	m_CachedRenderer = pRenderer;


	SBufferDesc bufferDesc = {};
	std::vector<uchar8> vertexBuffer(max_canvas_vtx_buffer_bytes, 0);

	const std::string VBName = "UIVertices";
	bufferDesc.BufferType = EBufferType::VERTEX_BUFFER;
	bufferDesc.Usage = EBufferUsage::GPU_READ_CPU_WRITE;
	bufferDesc.ElementCount = max_canvas_vtx_buffer_vertices;
	bufferDesc.Stride = sizeof(SVertex_PUV);
	bufferDesc.CPUAccess = ECPUAccess::CPU_W;	
	m_VtxID = pRenderer->CreateBuffer(bufferDesc, vertexBuffer.data(), VBName.c_str());

	m_TriangleBufferPtrs.clear();
	m_TriangleBufferPtrs.push_back(0);
	m_TriangleBufferTextures.clear();
	m_TriangleBufferTextures.push_back(INDEX_NONE);
	m_RenderColor.push_back(SVector4(1, 1, 1, 1));
	m_ClipRects.clear();
	m_ClipRects.push_back(SScissorsRect());

	if (m_PanelTarget == INDEX_NONE)
	{
		SVector2 res = g_Engine->Get<Renderer>()->FrameRenderTargetDimensionsAsFloat2();


		SRenderTargetDesc rtDesc_f4;
		rtDesc_f4.TextureDesc.Width = (int32)res.x;
		rtDesc_f4.TextureDesc.Height = (int32)res.y;
		rtDesc_f4.TextureDesc.mipCount = 1;
		rtDesc_f4.TextureDesc.arraySize = 1;
		rtDesc_f4.TextureDesc.Usage = ETextureUsage::RENDER_TARGET_RW;
		rtDesc_f4.TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		rtDesc_f4.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

		m_PanelTarget = m_CachedRenderer->AddRenderTarget(rtDesc_f4);
	}

}

void UIManager::ClearGeometry()
{
	m_TriangleBufferPtrs.clear();	
	m_TriangleBufferPtrs.push_back(0);

	m_TriangleBufferTextures.clear();
	m_TriangleBufferTextures.push_back(INDEX_NONE);

	m_RenderColor.clear();
	m_RenderColor.push_back(SVector4(1, 1, 1, 1));

	m_ClipRects.clear();
	m_ClipRects.push_back(SScissorsRect());
}


void UIManager::BeginUI()
{
	m_CachedRenderer->SetShader(EShaders::UNLIT,false,true);
	m_CachedRenderer->BindRenderTarget(m_PanelTarget);		
	m_CachedRenderer->SetVertexBuffer(m_VtxID);
	m_CachedRenderer->UnbindDepthTarget();
	m_CachedRenderer->BeginRender(SClearCommand::Color(0, 0, 0, 0));
	m_CachedRenderer->Apply();
}

void UIManager::EndUI()
{
	m_TriangleBufferPtrs.clear();
	m_TriangleBufferPtrs.push_back(0);

	m_TriangleBufferTextures.clear();
	m_TriangleBufferTextures.push_back(INDEX_NONE);

	m_RenderColor.clear();
	m_RenderColor.push_back(SVector4(1, 1, 1, 1));

	m_ClipRects.clear();
	m_ClipRects.push_back(SScissorsRect());
	m_CachedRenderer->UnbindRenderTargets();
	m_CachedRenderer->Apply();
	
}

void UIManager::RenderUI()
{
	uint32 prev = 0;
	for (uint32 i = 0; i < m_TriangleBufferPtrs.size(); ++i)
	{
		if (m_TriangleBufferPtrs[i] - prev > 0)
		{
			if (m_TriangleBufferTextures[i] != INDEX_NONE)
			{				
				STextureInfo tex;
				tex.alpha = m_RenderColor[i].w;
				tex.diffuse = m_RenderColor[i];
				tex.isDiffuseMap = 1.0f;
				m_CachedRenderer->SetConstantStruct("texInfo", &tex);
				m_CachedRenderer->SetTexture("texDiffuseMap", m_TriangleBufferTextures[i]);
				
				SScissorsRect clips;
				if (m_ClipRects.size() <= i)
				{
					clips = m_ClipRects.back();
				} else
					clips = m_ClipRects[i];

				m_CachedRenderer->SetScissorsRect(clips.left, clips.right, clips.top, clips.bottom);
				m_CachedRenderer->Apply();
			}			
			else
			{
				STextureInfo tex;
				tex.alpha = m_RenderColor[i].w;
				tex.diffuse = m_RenderColor[i];
				tex.isDiffuseMap = 0.0f;
				m_CachedRenderer->SetConstantStruct("texInfo", &tex);
				m_CachedRenderer->Apply();
			}

			
			m_CachedRenderer->DrawVtxOffset(m_TriangleBufferPtrs[i] - prev, prev);
			
		}
		prev = m_TriangleBufferPtrs[i];
	}
}

void UIManager::PushClipSpace(const SScissorsRect& InClipSpace)
{
	if (m_ClipRects.back() != InClipSpace)
	{
		m_ClipRects.push_back(InClipSpace);
	}	
}

void UIManager::PushSlot(class PanelSlot* slot, FTextureID texID)
{
	SVector4 contentColor = SVector4(slot->Content->TintColor.ToVector3(), slot->Content->RenderOpacity);				

	
	PushTexturedTri(texID, contentColor, slot->m_Vertices.data(), 6, slot->m_ClipSpace);
}

void UIManager::PushTexturedTri(FTextureID texID, const SVector4& InColor, const SVertex_PUV* triangles, const uint32 count, const SScissorsRect& InClipRect)
{
	if (count % 3 || count < 3)
		assert(false);

	if (m_TriangleBufferTextures.back() != texID)
	{
		m_TriangleBufferPtrs.push_back(m_TriangleBufferPtrs.back());
		m_TriangleBufferTextures.push_back(texID);		
		m_RenderColor.push_back(InColor);
		m_ClipRects.push_back(InClipRect);
	
	}
	
	
	m_CachedRenderer->UpdateVertexBuffer_Append<SVertex_PUV>(m_VtxID, triangles, count * sizeof(SVertex_PUV), m_TriangleBufferPtrs.back());	
	m_TriangleBufferPtrs.back() += count;
	
}

FTextureID UIManager::GetUITargetTexture() const
{
	return m_CachedRenderer->GetRenderTargetTexture(m_PanelTarget);
}

