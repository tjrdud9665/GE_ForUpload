#include "EnginePCH.h"
#include "TextBlock.h"
#include <Renderer/Renderer.h>
#include "CameraManager.h"
#include "GameFramework/Components/CameraComponent.h"

TextBlock::TextBlock()
	:QuadTexBuffer(INDEX_NONE)
	, FontScale(1.0f)
	, TextAlignemt(ETextAlignment::TAlign_Center)
	, TextTarget(INDEX_NONE)	
	, RenderedScale(1)
	, RenderedColor(SLinearColor(0,0,0,0))
	, FontSize(48)
	, RenderedTextSize(0)
{

}

TextBlock::TextBlock(const TextBlock& Other)
	:Super(Other)
	,Font(Other.Font)
	,FontColor(Other.FontColor)
	,Text(Other.Text)
	, QuadTexBuffer(INDEX_NONE)
	, FontScale(Other.FontScale)
	, TextAlignemt(Other.TextAlignemt)
	, TextTarget(INDEX_NONE)	
	, RenderedScale(1)
	, RenderedColor(SLinearColor(0, 0, 0, 0))
	, FontSize(Other.FontSize)
	, RenderedTextSize(0)
{
	//Create Buffer..
	CreateFontGeometry(g_Engine->Get<Renderer>());
	CreateTextBlockTarget(g_Engine->Get<Renderer>());

	
	
}

TextBlock::~TextBlock()
{

}

void TextBlock::Render(class Renderer* pRenderer)
{
	if(Font)
	{
	

		SVector2 parentSize = pRenderer->FrameRenderTargetDimensionsAsFloat2();

		SVector2 parentPos = SVector2(0, 0);
		SVector2 parentRenderedSize = SVector2(0, 0);
		PanelWidget* parent = Slot->GetParent();
	


		//SVector2 scrSize = pRenderer->FrameRenderTargetDimensionsAsFloat2() * 0.5f;
		
		//scrSize.y *= -1.0f;
		//SVector2 lt = SVector2(-parentSize.x + parentPos.x, parentSize.y + parentPos.y);
		SVector2 lt = GetLeftTop();
	
		SVector2 center = SVector2(lt.x + DesiredSize.x * 0.5f, -lt.y+ DesiredSize.y * 0.5f);

	


		//optimize - 텍스트를 TextTarget에 그리고 만약 글자가 변화가 없거나 글자크기의 변화글자색상의 변화가  없다면 TextTarget을 재구성할필요가없다.		
		if(RenderedText != Text || 
		   FontScale != RenderedScale  || 
		   RenderedColor != FontColor || 
		   RenderPos != center ||
		   RenderedTextSize != FontSize ||
		   RenderedAlignment != TextAlignemt.GetUnderlying())
		{
		/*	FRenderTargetID oldRt = pRenderer->GetActiveRenderTaregetID();*/
			const SPipelineState& old = pRenderer->GetPipelineState();
	
		
			pRenderer->BindRenderTarget(TextTarget);
			pRenderer->BeginRender(SClearCommand::Color(0, 0, 0, 0));
			pRenderer->Apply();
			DesiredSize = Font->RenderText(utf8_to_unicode(Text), pRenderer, parentSize, center, FontColor.ToVector3(), FontScale, TextAlignemt.Get(),FontSize, QuadTexBuffer);
			RenderedScale = FontScale;
			RenderedText = Text;
			RenderedColor = FontColor;
			RenderedTextSize= FontSize;
			RenderPos = center;
			RenderedAlignment = TextAlignemt.GetUnderlying();
			/*		pRenderer->BindRenderTarget(oldRt);
					pRenderer->Apply();*/
			pRenderer->SetPipelineState(old);
			pRenderer->Apply();
		}

		
		

		if (Slot)
		{			
			//FullScreneQuad기준
			SScissorsRect clip;
			if (Slot)
				clip = Slot->m_ClipSpace;

			SVector4 renderColor = SVector4(TintColor.ToVector3(), RenderOpacity);
			m_CachedManager->PushTexturedTri(pRenderer->GetRenderTargetTexture(TextTarget), renderColor,m_RendertedTextVertex.data(), 6, clip);
			//m_CachedManager->PushSlot(Slot, pRenderer->GetRenderTargetTexture(TextTarget));
		}



		pRenderer->Apply();
		
		
	}
	////글자를 TextTarget 에 그리고
	////Parent 의 Target 에 그리기
}

void TextBlock::Construct(const std::string& InName /*= ""*/)
{
	Super::Construct(InName);
	CreateFontGeometry(g_Engine->Get<Renderer>());
	CreateTextBlockTarget(g_Engine->Get<Renderer>());


	m_RendertedTextVertex.resize(6);
	const float& size = 1.0f;
	m_RendertedTextVertex[0].Position = SVector3(-size, -size, 0.0f);
	m_RendertedTextVertex[0].UV = SVector2(0.0f, 1.0f);

	m_RendertedTextVertex[1].Position = SVector3(-size, +size, 0.0f);
	m_RendertedTextVertex[1].UV = SVector2(0.0f, 0.0f);

	m_RendertedTextVertex[2].Position = SVector3(+size, +size, 0.0f);
	m_RendertedTextVertex[2].UV = SVector2(1.0f, 0.0f);

	//0
	m_RendertedTextVertex[3].Position = m_RendertedTextVertex[0].Position;
	m_RendertedTextVertex[3].UV = m_RendertedTextVertex[0].UV;
	//2
	m_RendertedTextVertex[4].Position = m_RendertedTextVertex[2].Position;
	m_RendertedTextVertex[4].UV = m_RendertedTextVertex[2].UV;
	//3
	m_RendertedTextVertex[5].Position = SVector3(+size, -size, 0.0f);
	m_RendertedTextVertex[5].UV = SVector2(1.0f, 1.0f);


}

void TextBlock::CreateFontGeometry(class Renderer* pRenderer)
{
	// create vertex buffer
	if (QuadTexBuffer == INDEX_NONE)
	{
		SBufferDesc bufDesc;
		bufDesc.ElementCount = 6;
		bufDesc.Stride = sizeof(SVector4); //Pos 2 UV 2
		bufDesc.BufferType = EBufferType::VERTEX_BUFFER;
		bufDesc.Usage = EBufferUsage::GPU_READ_CPU_WRITE;
		bufDesc.CPUAccess = ECPUAccess::CPU_W;
		QuadTexBuffer = pRenderer->CreateBuffer(bufDesc, nullptr, "TextRendererVB");
	}
}

void TextBlock::CreateTextBlockTarget(class Renderer* pRenderer)
{
	if (TextTarget == INDEX_NONE)
	{
		SVector2 res = pRenderer->FrameRenderTargetDimensionsAsFloat2();


		SRenderTargetDesc rtDesc_f4;
		rtDesc_f4.TextureDesc.Width = (int32)res.x;
		rtDesc_f4.TextureDesc.Height = (int32)res.y;
		rtDesc_f4.TextureDesc.mipCount = 1;
		rtDesc_f4.TextureDesc.arraySize = 1;
		rtDesc_f4.TextureDesc.Usage = ETextureUsage::RENDER_TARGET_RW;
		rtDesc_f4.TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		rtDesc_f4.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

		TextTarget = pRenderer->AddRenderTarget(rtDesc_f4);
	}
}

void TextBlock::SetText(const std::string& InText)
{
	Text = InText;
}

std::vector<SVertex_PUV> TextBlock::m_RendertedTextVertex;
