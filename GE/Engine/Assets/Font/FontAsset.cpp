#include "EnginePCH.h"
#include "FontAsset.h"

#include "CameraManager.h"
#include "GameFramework/Components/CameraComponent.h"



FontAsset::FontAsset()
	: m_Loaded(false)
{
	DefulatFontSize = 48;
}

FontAsset::FontAsset(const FontAsset& Other)
	:Super(Other)
	, DefulatFontSize(Other.DefulatFontSize)
	, m_Loaded(Other.m_Loaded)
{
	m_cachedRenderer = g_Engine->Get<Renderer>();
}

FontAsset::~FontAsset()
{
	// cleanup
	FT_Done_Face(m_face);
	if (m_ftLib)
	{
		FT_Done_FreeType(m_ftLib);
		m_ftLib = nullptr;
	}
}

void FontAsset::Construct(const std::string& InName /* = "" */)
{
	Super::Construct(InName);

	m_cachedRenderer = g_Engine->Get<Renderer>();

}

bool FontAsset::PostLoad()
{
	bool bResult= Super::PostLoad();

	if (m_Loaded)
	{
		return true;
	}

	FT_Error err;

	// init font lib
	if(!m_ftLib)
	{		
		err = FT_Init_FreeType(&m_ftLib);
		if (err != 0)
		{
			return false;
		}
	}


	// load font
	fs::path fontPath = GetRawPath();

	fontPath = fontPath.replace_extension("");	
	//FT_Face face;
	err = FT_New_Face(m_ftLib, fontPath.string().c_str(), 0, &m_face);
	if (err != 0)
	{
		//Log::Error("Could not load Font (Err=%d): %s", err, fontDir);
		return false;
	}

	// set font width-height
	const int fontSize = DefulatFontSize;
	FT_Set_Pixel_Sizes(m_face, 0, fontSize);



	//static const ImWchar ranges[] =
	//{
	//	0x0020, 0x00FF, // Basic Latin + Latin Supplement
	//	0x3131, 0x3163, // Korean alphabets
	//	0xAC00, 0xD7A3, // Korean characters
	//};
	

	for (wchar_t c = 0x0020; c < 0x00FF; ++c)
	{
		// load character glyph
		err = FT_Load_Char(m_face, c, FT_LOAD_RENDER);
		if (err != 0)
		{
			//Log::Error("Couldn't load character glyph (%d): %c", c, c);
		}

		int w = (m_face)->glyph->bitmap.width;
		int h = (m_face)->glyph->bitmap.rows;
		const int l = (m_face)->glyph->bitmap_left;
		const int t = (m_face)->glyph->bitmap_top;

		w = w == 0 ? 1 : w;
		h = h == 0 ? 1 : h;
		//if (w == 0 || h == 0)
		//	continue;

		STextureDesc texDesc;
		texDesc.Format = DXGI_FORMAT_R8_UNORM;
		texDesc.Width = w;
		texDesc.Height = h;
		texDesc.pData = (m_face)->glyph->bitmap.buffer;
		texDesc.dataPitch = (m_face)->glyph->bitmap.pitch;
		texDesc.dataSlicePitch = 0;
		texDesc.FileName = std::string("Char:");

		FTextureID charTexture = m_cachedRenderer->CreateTexture2D(texDesc);
		SFontGlyph fontglyph =
		{
			charTexture,
			SVector2(w, h),
			SVector2(l, t),
			(m_face)->glyph->advance.x
		};
		m_wFontGlyphPerSize[DefulatFontSize].insert(std::pair<wchar_t, SFontGlyph>(c, fontglyph));
	}

	for (wchar_t c = 0xAC00; c < 0xD7A3; ++c)
	{
		// load character glyph
		err = FT_Load_Char(m_face, c, FT_LOAD_RENDER);
		if (err != 0)
		{
			//Log::Error("Couldn't load character glyph (%d): %c", c, c);
		}

		int w = (m_face)->glyph->bitmap.width;
		int h = (m_face)->glyph->bitmap.rows;
		const int l = (m_face)->glyph->bitmap_left;
		const int t = (m_face)->glyph->bitmap_top;

		w = w == 0 ? 1 : w;
		h = h == 0 ? 1 : h;
		//if (w == 0 || h == 0)
		//	continue;

		STextureDesc texDesc;
		texDesc.Format = DXGI_FORMAT_R8_UNORM;
		texDesc.Width = w;
		texDesc.Height = h;
		texDesc.pData = (m_face)->glyph->bitmap.buffer;
		texDesc.dataPitch = (m_face)->glyph->bitmap.pitch;
		texDesc.dataSlicePitch = 0;
		texDesc.FileName = std::string("Char:");

		FTextureID charTexture = m_cachedRenderer->CreateTexture2D(texDesc);
		SFontGlyph fontglyph =
		{
			charTexture,
			SVector2(w, h),
			SVector2(l, t),
			(m_face)->glyph->advance.x
		};
		
		m_wFontGlyphPerSize[DefulatFontSize].insert(std::pair<wchar_t, SFontGlyph>(c, fontglyph));
	}

	for (wchar_t c = 0x3131; c < 0x3163; ++c)
	{
		// load character glyph
		err = FT_Load_Char(m_face, c, FT_LOAD_RENDER);
		if (err != 0)
		{
			//Log::Error("Couldn't load character glyph (%d): %c", c, c);
		}

		int w = (m_face)->glyph->bitmap.width;
		int h = (m_face)->glyph->bitmap.rows;
		const int l = (m_face)->glyph->bitmap_left;
		const int t = (m_face)->glyph->bitmap_top;

		w = w == 0 ? 1 : w;
		h = h == 0 ? 1 : h;
		//if (w == 0 || h == 0)
		//	continue;

		STextureDesc texDesc;
		texDesc.Format = DXGI_FORMAT_R8_UNORM;
		texDesc.Width = w;
		texDesc.Height = h;
		texDesc.pData = (m_face)->glyph->bitmap.buffer;
		texDesc.dataPitch = (m_face)->glyph->bitmap.pitch;
		texDesc.dataSlicePitch = 0;
		texDesc.FileName = std::string("Char:") ;

		FTextureID charTexture = m_cachedRenderer->CreateTexture2D(texDesc);
		SFontGlyph fontglyph =
		{
			charTexture,
			SVector2(w, h),
			SVector2(l, t),
			(m_face)->glyph->advance.x
		};
		m_wFontGlyphPerSize[DefulatFontSize].insert(std::pair<wchar_t, SFontGlyph>(c, fontglyph));
	}

	m_Loaded = true;

	
	return bResult;
}


void FontAsset::RegisterTextCache(const wchar_t& c,int32 FontSize)
{
	FT_Error err;	

	FT_Set_Pixel_Sizes(m_face, 0, FontSize);

	err = FT_Load_Char(m_face, c, FT_LOAD_RENDER);

	if (err != 0)
	{
		//Log::Error("Couldn't load character glyph (%d): %c", c, c);
	}

	int w = (m_face)->glyph->bitmap.width;
	int h = (m_face)->glyph->bitmap.rows;
	const int l = (m_face)->glyph->bitmap_left;
	const int t = (m_face)->glyph->bitmap_top;

	w = w == 0 ? 1 : w;
	h = h == 0 ? 1 : h;
	//if (w == 0 || h == 0)
	//	continue;

	STextureDesc texDesc;
	texDesc.Format = DXGI_FORMAT_R8_UNORM;
	texDesc.Width = w;
	texDesc.Height = h;
	texDesc.pData = (m_face)->glyph->bitmap.buffer;
	texDesc.dataPitch = (m_face)->glyph->bitmap.pitch;
	texDesc.dataSlicePitch = 0;
	texDesc.FileName = "wchar";

	FTextureID charTexture = m_cachedRenderer->CreateTexture2D(texDesc);
	SFontGlyph fontglyph =
	{
		charTexture,
		SVector2(w, h),
		SVector2(l, t),
		(m_face)->glyph->advance.x
	};


	

	m_wFontGlyphPerSize[FontSize].insert(std::pair<wchar_t, SFontGlyph>(c, fontglyph));
}

//void FontAsset::RenderText(const std::string& InText, class Renderer* pRenderer, const SVector2& InPostion , const SVector3& InColor, const FBufferID& fontBuffer)
//{
//	//Updates..	
//	FShaderID oldShader = pRenderer->GetActiveShader();
//	FDepthStencilStateID oldDepthState = pRenderer->GetDepthStencilState();	
//
//	const SVector2 windowSizeXY = pRenderer->FrameRenderTargetDimensionsAsFloat2();
//	CameraManager* camMgr = g_Engine->Get<CameraManager>();
//
//	Matrix matP = camMgr->GetUICamera()->GetProjMatrix();
//
//
//	pRenderer->SetShader(EShaders::TEXT);
//	pRenderer->SetConstant4x4f("projection", matP);
//	pRenderer->SetConstant3f("color", InColor);
//	pRenderer->SetSamplerState("samText", EDefaultSamplerState::LINEAR_FILTER_SAMPLER);
//	pRenderer->SetDepthStencilState(EDefaultDepthStencilState::DEPTH_STENCIL_DISABLED);
//	pRenderer->SetBlendState(EDefaultBlendState::ALPHA_BLEND);
//
//
//
//	// offset with half window size, so that (0,0) is top left corner
//	float x = InPostion.x - windowSizeXY.x / 2;
//	const float y = -InPostion.y + windowSizeXY.y / 2;
//
//	for (const char& c : InText)
//	{
//		const SFontGlyph& ch = m_FontGlyph.at(c);
//
//		const float xpos = x + ch.bearing.x * 1;
//		const float ypos = y - (ch.size.y - ch.bearing.y) * 1;
//
//		const float w = ch.size.x * 1;
//		const float h = ch.size.y * 1;
//	
//
//
//		float vertices[6][4] =
//		{
//			{ xpos,     ypos + h,   0.0, 0.0 },
//			{ xpos,     ypos,       0.0, 1.0 },
//			{ xpos + w, ypos,       1.0, 1.0 },
//
//			{ xpos,     ypos + h,   0.0, 0.0 },
//			{ xpos + w, ypos,       1.0, 1.0 },
//			{ xpos + w, ypos + h,   1.0, 0.0 }
//		};
//		pRenderer->UpdateVertexBuffer(fontBuffer, vertices);
//
//		pRenderer->SetTexture("textMap", ch.tex);
//
//		pRenderer->SetVertexBuffer(fontBuffer);
//		pRenderer->Apply();
//		pRenderer->Draw(6, EPrimitiveTopology::TRIANGLE_LIST);
//		x += (ch.advance >> 6) * 1;
//		// Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
//	}
//	pRenderer->SetDepthStencilState(oldDepthState);
//	pRenderer->SetShader(oldShader);
//	pRenderer->Apply();
//}
/*InPostion - > Center */
SVector2 FontAsset::RenderText(const std::wstring& InText, class Renderer* pRenderer, const SVector2& InRes, const SVector2& InPostion , const SVector3& InColor, float InScale, ETextAlignment alignment, int32 FontSize,const FBufferID& fontBuffer)
{
	FShaderID oldShader = pRenderer->GetActiveShader();
	FDepthStencilStateID oldDepthState = pRenderer->GetDepthStencilState();
	FBufferID	oldVtxBuffer = pRenderer->GetActiveVertexBufferID();


	const SVector2 windowSizeXY = InRes.GetAbs()*2.0f;
	

	Matrix matP =  DirectX::XMMatrixOrthographicLH(windowSizeXY.x, windowSizeXY.y, 0.1f, 1000.0f);

		


	pRenderer->SetShader(EShaders::TEXT);
	pRenderer->SetConstant4x4f("projection", matP);
	pRenderer->SetConstant3f("color", InColor);
	pRenderer->SetSamplerState("samText", EDefaultSamplerState::LINEAR_FILTER_SAMPLER);

	pRenderer->SetDepthStencilState(EDefaultDepthStencilState::DEPTH_STENCIL_DISABLED);
	pRenderer->SetBlendState(EDefaultBlendState::ALPHA_BLEND);

	float offset = 0;
	float FontWidget = GetFontWidth(InText, InScale, FontSize);
	if(alignment == ETextAlignment::TAlign_Left)
	{
		offset = -FontWidget *0.5f;
	}
	else if (alignment == ETextAlignment::TAlign_RIght)
	{
		offset = +FontWidget * 0.5f;
	}
	

	float x = InPostion.x - windowSizeXY.x / 2 + offset;
	const float y = -InPostion.y + windowSizeXY.y / 2 ;	
	
	const FFontGlyphMap& glyphMap = m_wFontGlyphPerSize[FontSize];

	for (const wchar_t& c : InText)
	{
		const SFontGlyph& ch = glyphMap.at(c);

		const float xpos = x + ch.bearing.x * InScale;
		const float ypos = y - (ch.size.y*0.5f - ch.bearing.y) * InScale - FontSize * InScale;		
		
		const float w = ch.size.x * InScale;
		const float h = ch.size.y * InScale;
		

		std::vector<SVector2> vtx;
		vtx.resize(6);

		//vtx[0] = XMVector2TransformCoord(SVector2(xpos, ypos + h*0.5f).Convert(), matRot.m);
		//vtx[1] = XMVector2TransformCoord(SVector2(xpos, ypos- h * 0.5f).Convert(), matRot.m);
		//vtx[2] = XMVector2TransformCoord(SVector2(xpos + w, ypos - h * 0.5f).Convert(), matRot.m);
		//vtx[3] = XMVector2TransformCoord(SVector2(xpos, ypos + h *0.5f).Convert(), matRot.m);
		//vtx[4] = XMVector2TransformCoord(SVector2(xpos + w, ypos - h * 0.5f).Convert(), matRot.m);
		//vtx[5] = XMVector2TransformCoord(SVector2(xpos + w, ypos + h * 0.5f).Convert(), matRot.m);

		vtx[0] = (SVector2(xpos, ypos + h * 0.5f));
		vtx[1] = (SVector2(xpos, ypos - h * 0.5f));
		vtx[2] = (SVector2(xpos + w, ypos - h * 0.5f));
		vtx[3] = (SVector2(xpos, ypos + h * 0.5f));
		vtx[4] = (SVector2(xpos + w, ypos - h * 0.5f));
		vtx[5] = (SVector2(xpos + w, ypos + h * 0.5f));

		//	  0 3	+-----+ 5	
		//			|	  |		
		//			|	  |		
		//	  1		+-----+ 2 4		


		float vertices[6][4] =
		{
			{ vtx[0].x, vtx[0].y,   0.0, 0.0 },
			{ vtx[1].x, vtx[1].y,    0.0, 1.0 },
			{ vtx[2].x, vtx[2].y,    1.0, 1.0 },
			{ vtx[3].x, vtx[3].y,    0.0, 0.0 },
			{ vtx[4].x, vtx[4].y,     1.0, 1.0 },
			{ vtx[5].x, vtx[5].y,    1.0, 0.0 }
		};
		pRenderer->UpdateVertexBuffer(fontBuffer, vertices);

		pRenderer->SetTexture("textMap", ch.tex);
		
		pRenderer->SetVertexBuffer(fontBuffer);

		pRenderer->Apply();
		pRenderer->Draw(6, EPrimitiveTopology::TRIANGLE_LIST);
		x += (ch.advance >> 6) * InScale;
		// Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	pRenderer->SetDepthStencilState(oldDepthState);
	pRenderer->SetShader(oldShader);
	pRenderer->SetVertexBuffer(oldVtxBuffer);
	pRenderer->Apply();

	return SVector2(FontWidget, FontSize * InScale);
}



float FontAsset::GetFontWidth(const std::wstring& InText, float InScale, int32 FontSize)
{
	float ret = 0;
	
	FFontSizeMap::iterator sizeFinder = m_wFontGlyphPerSize.find(FontSize);

	if(sizeFinder == m_wFontGlyphPerSize.end())
	{
		m_wFontGlyphPerSize.insert(std::make_pair(FontSize, FFontGlyphMap()));		
	}

	const FFontGlyphMap& glyphMap = m_wFontGlyphPerSize[FontSize];

	for (const wchar_t& c : InText)
	{
		if (glyphMap.find(c) == glyphMap.end())
			RegisterTextCache(c,FontSize);
		const SFontGlyph& glyph = glyphMap.at(c);
		ret +=(glyph.advance >> 6) * InScale;
	}
	return ret;
}

FT_Library FontAsset::m_ftLib =nullptr;

