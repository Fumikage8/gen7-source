#include <FontRenderer/include/gfl2_FontRenderer.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <stdarg.h>

#ifndef _WIN64
// @attention: 2015.01.07 glProgramUniform1iをどうしても使わざるを得ない状況のため追加.
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLPixelShader.h>
#endif
//#define _GEOMETRY_SHADER_AVAILABLE_		// ジオメトリシェーダ有効化
		
namespace gfl2 { namespace fontrenderer{
FontRenderer::Impl* FontRenderer::s_pImpl = NULL;

class FontAnalyzer
{
public:
  FontAnalyzer();
	~FontAnalyzer();
	c16 analyzeAsciiCtrlCode(
		f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
		const c16** wstr, const f32 w, const f32 h,
		const f32 screenW, const f32 screenH);
	c16 analyzeTag(
		f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
		const c16** wstr, const f32 w, const f32 h,
		const f32 screenW, const f32 screenH);
	c16 analyzeEscapeSequence(
		f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
		const c16** wstr, const f32 w, const f32 h,
		const f32 screenW, const f32 screenH);
	void clear();
  void setDefaultFontColor(const math::Vector4& color);

private:
	enum TAG_STATUS
	{
		NONE = -1,
		LEFT,
		RIGHT,
		CENTER,
		FONTCOL,
		BR,
		NUMBER_OF_TAG_STATUS
	};

	enum ESCAPE_SEQUENCE
	{
		LT,
		GT,
		NUMBER_OF_ESCAPE_SEQUENCE
	};

  math::Vector4 m_defaultFontColor;

	static const c16* s_elements[TAG_STATUS::NUMBER_OF_TAG_STATUS];
	static const s32 s_stackSize = 16;
	u32 m_stackCount;
	TAG_STATUS m_statusStack[s_stackSize];

	static const c16* s_escapeSequences[ESCAPE_SEQUENCE::NUMBER_OF_ESCAPE_SEQUENCE];
	static const c16 s_escapeConversions[ESCAPE_SEQUENCE::NUMBER_OF_ESCAPE_SEQUENCE];

	const c16* extractFromTag(c16* outElem, c16* outAttr, const c16** in);
	s16 strC16LengthWithoutTags(const c16* in, const c16* tagName);

	void setLeft(
		f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
		const c16** wstr, const c16* attr, const f32 w, const f32 h,
		const f32 screenW, const f32 screenH);
	void setRight(
		f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
		const c16** wstr, const c16* attr, const f32 w, const f32 h,
		const f32 screenW, const f32 screenH);
	void setCenter(
		f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
		const c16** wstr, const c16* attr, const f32 w, const f32 h,
		const f32 screenW, const f32 screenH);
	void setColor(
		f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
		const c16** wstr, const c16* attr, const f32 w, const f32 h,
		const f32 screenW, const f32 screenH);
	void setBr(
		f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
		const c16** wstr, const c16* attr, const f32 w, const f32 h,
		const f32 screenW, const f32 screenH);

	void resetPosition(
		f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
		const c16** wstr, const c16* attr, const f32 w, const f32 h,
		const f32 screenW, const f32 screenH);
	void resetColor(
		f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
		const c16** wstr, const c16* attr, const f32 w, const f32 h,
		const f32 screenW, const f32 screenH);

	void fault(
		f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
		const c16** wstr, const c16* attr, const f32 w, const f32 h,
		const f32 screenW, const f32 screenH);

	// 関数ポインタの定義を毎回書くのは面倒なのでtypedef
	typedef void (FontAnalyzer::*CONFIG_FUNC)(
		f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
		const c16** wstr, const c16* attr, const f32 w, const f32 h,
		const f32 screenW, const f32 screenH);
	CONFIG_FUNC m_pSetFunc[TAG_STATUS::NUMBER_OF_TAG_STATUS];
	CONFIG_FUNC m_pResetFunc[TAG_STATUS::NUMBER_OF_TAG_STATUS];
};

// タグのエレメント名
const c16* FontAnalyzer::s_elements[FontAnalyzer::TAG_STATUS::NUMBER_OF_TAG_STATUS] = {
	L"left",
	L"right",
	L"center",
	L"fontcol",
	L"br"
};

// エスケープシーケンスと, それに対応する変換後の文字
const c16* FontAnalyzer::s_escapeSequences[ESCAPE_SEQUENCE::NUMBER_OF_ESCAPE_SEQUENCE] =
{
	L"&lt;",
	L"&gt;",
};
const c16 FontAnalyzer::s_escapeConversions[ESCAPE_SEQUENCE::NUMBER_OF_ESCAPE_SEQUENCE] =
{
	L'<',
	L'>',
};

FontAnalyzer::FontAnalyzer() : 
	m_stackCount(0),
  m_defaultFontColor(math::Vector4(0.0f, 0.0f, 0.0f, 1.0f))
{
	memset(m_statusStack, TAG_STATUS::NONE, sizeof(m_statusStack));
	m_pSetFunc[TAG_STATUS::LEFT] = &FontAnalyzer::setLeft;
	m_pSetFunc[TAG_STATUS::RIGHT] =&FontAnalyzer:: setRight;
	m_pSetFunc[TAG_STATUS::CENTER] = &FontAnalyzer::setCenter;
	m_pSetFunc[TAG_STATUS::FONTCOL] = &FontAnalyzer::setColor;
	m_pSetFunc[TAG_STATUS::BR] = &FontAnalyzer::setBr;

	m_pResetFunc[TAG_STATUS::LEFT] = &FontAnalyzer::resetPosition;
	m_pResetFunc[TAG_STATUS::RIGHT] =&FontAnalyzer:: resetPosition;
	m_pResetFunc[TAG_STATUS::CENTER] = &FontAnalyzer::resetPosition;
	m_pResetFunc[TAG_STATUS::FONTCOL] = &FontAnalyzer::resetColor;
	m_pResetFunc[TAG_STATUS::BR] = &FontAnalyzer::fault;
}

FontAnalyzer::~FontAnalyzer()
{
}

c16 FontAnalyzer::analyzeAsciiCtrlCode(
		f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
		const c16** wstr, const f32 w, const f32 h,
		const f32 screenW, const f32 screenH)
{
	if(**wstr == L'\n' || **wstr == L'\r')
	{
		setBr(outX, outY, outDiffX, outBaseX, outColor, wstr, NULL, w, h, screenW, screenH);
		++(*wstr);
	}

	return (**wstr);
}

c16 FontAnalyzer::analyzeTag(
	f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
	const c16** wstr, const f32 w, const f32 h, const f32 screenW, const f32 screenH)
{
	c16 elemName[64];
	c16 attrName[64];
	memset(elemName, 0, sizeof(elemName));
	memset(attrName, 0, sizeof(attrName));
	++(*wstr);	// '<'の部分を飛ばす
	(*wstr) = extractFromTag(elemName, attrName, wstr);
			
	if((m_stackCount > 0) && (elemName[0] == L'/'))
	{
		TAG_STATUS status = m_statusStack[m_stackCount - 1];
		// 直前のタグと名前が一致していなければ受け取った文字列がおかしい
		assert((wcscmp(&elemName[1], s_elements[status]) == 0)
			&& "描画対象の文字列のタグ構造が入れ子構造になっていません");
		(this->*m_pResetFunc[status])(outX, outY, outDiffX, outBaseX, outColor, wstr, attrName, w, h, screenW, screenH);
		m_statusStack[m_stackCount - 1] = TAG_STATUS::NONE;
		--m_stackCount;
		return (**wstr);
	}

	for(s8 i = 0; i < TAG_STATUS::NUMBER_OF_TAG_STATUS; ++i)
	{
		if(wcscmp(elemName, s_elements[i]) == 0)
		{
			(this->*m_pSetFunc[i])(outX, outY, outDiffX, outBaseX, outColor, wstr, attrName, w, h, screenW, screenH);
			if(i != TAG_STATUS::BR)
			{
				m_statusStack[m_stackCount] = static_cast<TAG_STATUS>(i);
				++m_stackCount;
				return (**wstr);
			}
		}
	}

	return (**wstr);
}

c16 FontAnalyzer::analyzeEscapeSequence(
		f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
		const c16** wstr, const f32 w, const f32 h,
		const f32 screenW, const f32 screenH)
{
	for(s8 i = 0; i < ESCAPE_SEQUENCE::NUMBER_OF_ESCAPE_SEQUENCE; ++i)
	{
		if(wcsstr(*wstr, s_escapeSequences[i]))
		{
			(*wstr) += wcslen(s_escapeSequences[i]) - 1;	// -1するのは次の文字を描画するため
			return s_escapeConversions[i];
		}
	}

	return (**wstr);
}

void FontAnalyzer::resetPosition(
		f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
		const c16** wstr, const c16* attr, const f32 w, const f32 h,
		const f32 screenW, const f32 screenH)
{
	(*outBaseX) = (*outX);
}

void FontAnalyzer::resetColor(
	f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
	const c16** wstr, const c16* attr, const f32 w, const f32 h,
	const f32 screenW, const f32 screenH)
{
  outColor->x = m_defaultFontColor.x;
	outColor->y = m_defaultFontColor.y;
	outColor->z = m_defaultFontColor.z;
	outColor->w = m_defaultFontColor.w;
}

void FontAnalyzer::fault(
	f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
	const c16** wstr, const c16* attr, const f32 w, const f32 h,
	const f32 screenW, const f32 screenH)
{
	assert(0 && "FontAnalyzer::fault()");	// 来るはずがない場所に来たら止める
}

void FontAnalyzer::setLeft(
	f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
	const c16** wstr, const c16* attr, const f32 w, const f32 h, const f32 screenW, const f32 screenH)
{
	(*outBaseX) = 0.0f;
}

void FontAnalyzer::setRight(
	f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
	const c16** wstr, const c16* attr, const f32 w, const f32 h, const f32 screenW, const f32 screenH)
{
	// 次の'<'が来る位置を割り出して, 描画する文字列の長さを算出
	// @attention: wcschrのNULLチェック
	s16 length = (s16)strC16LengthWithoutTags((*wstr), L"right");
	(*outBaseX) = screenW - w * length;
}

void FontAnalyzer::setCenter(
	f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
	const c16** wstr, const c16* attr, const f32 w, const f32 h, const f32 screenW, const f32 screenH)
{
	// 次の'<'が来る位置を割り出して, 描画する文字列の長さを算出
	// @attention: wcschrのNULLチェック
	s16 length = strC16LengthWithoutTags((*wstr), L"center");
	(*outBaseX) = screenW / 2.0f - w * (f32)length / 2.0f;
}

void FontAnalyzer::setColor(
	f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
	const c16** wstr, const c16* attr, const f32 w, const f32 h, const f32 screenW, const f32 screenH)
{
	// "0xRRGGBBAA"という形式で入っているので, その通りに取り出す
	// @attention: 範囲外の文字が来たときのエラーチェック('a', 'G'など)
	outColor->x = (float)(((attr[2] - ((attr[2] & 0x40) ? 55 : 48)) << 4) + (attr[3] - ((attr[3] & 0x40) ? 55 : 48))) / 255.0f;
  outColor->y = (float)(((attr[4] - ((attr[4] & 0x40) ? 55 : 48)) << 4) + (attr[5] - ((attr[5] & 0x40) ? 55 : 48))) / 255.0f;
	outColor->z = (float)(((attr[6] - ((attr[6] & 0x40) ? 55 : 48)) << 4) + (attr[7] - ((attr[7] & 0x40) ? 55 : 48))) / 255.0f;
	outColor->w = (float)(((attr[8] - ((attr[8] & 0x40) ? 55 : 48)) << 4) + (attr[9] - ((attr[9] & 0x40) ? 55 : 48))) / 255.0f;
}

void FontAnalyzer::setBr(
	f32* outX, f32* outY, u16* outDiffX, f32* outBaseX, math::Vector4* outColor,
	const c16** wstr, const c16* attr, const f32 w, const f32 h, const f32 screenW, const f32 screenH)
{
	(*outDiffX) = 0;
	(*outY) += h;
}

void FontAnalyzer::clear()
{
	m_stackCount = 0;
	memset(m_statusStack, TAG_STATUS::NONE, sizeof(m_statusStack));
}

void FontAnalyzer::setDefaultFontColor(const math::Vector4& color)
{
  m_defaultFontColor = color;
}

const c16* FontAnalyzer::extractFromTag(c16* outElem, c16* outAttr, const c16** in)
{
	c16* out = outElem;
	while(**in != L'>')
	{
		if(**in == L'=')
		{
			// '='の分だけ読み飛ばす
			++(*in);
			out = outAttr;
		}
		*out = **in;
		++out;
		++(*in);
	}

	// '>'の分だけ読み飛ばす
	++(*in);
	return (*in);
}

s16 FontAnalyzer::strC16LengthWithoutTags(const c16* in, const c16* tagName)
{
	// </tagname>の文字列を作る
	c16 tag[64] = L"</";
	c16* p = &tag[2];
	const c16* q = tagName;
	while(*p++ = *q++);
	*(--p) = L'>';
	*(++p) = L'\0';

	s16 length = 0;
	const c16* end = wcsstr(in, tag);
	while(in != end)
	{
		if(*in == L'<')
		{
			while(*in != L'>')
			{
				++in;
			}
			// '>'の分だけ飛ばす
			++in;
			continue;
		}
		++length;
		++in;
	}
	return length;
}

class FontRenderer::Impl
{
public:
	Impl(s16 screenWidth, s16 screenHeight);
	~Impl();
	b32 update();
	u16 draw(const c16* wstr, f32 x, f32 y, f32 w, f32 h);
	void reset();
	b32 begin();
	void end();
	void setScreenSize(s16 screenWidth, s16 screenHeight);
  void setDefaultFontColor(const math::Vector4& color);

	static const u32 s_fontNum = 512;	// !<@ brief フォントの表示可能最大数

private:
	static const u32 s_textureNum = 5;	            // @attention: 可変にすべき

	// 定数バッファのデータ定義①　変更しないデータ
	struct cbNeverChanges {
		math::Matrix InvViewport;   // 逆ビューポート行列
	};
	s16 m_screenWidth;            //!< @brief スクリーンの横幅
	s16 m_screenHeight;           //!< @brief スクリーンの縦幅

  math::Vector4 m_fontColor;     //!< @brief フォントの描画色

	VertexBuffer				*m_pVertexBuffer;
#ifndef _GEOMETRY_SHADER_AVAILABLE_	
	IndexBuffer					*m_pIndexBuffer;
#endif
	Shader							*m_pVertexShader;	// 頂点シェーダ
#ifdef _GEOMETRY_SHADER_AVAILABLE_	
	Shader							*m_pGeometryShader;	// ジオメトリシェーダ
#endif
	Shader							*m_pPixelShader;		// ピクセル・シェーダ
	ConstantBuffer			*m_pConstantBuffer[1];
	
	BlendStateObject				*m_pBlendStateObject;		// ブレンド・ステート・オブジェクト
	RasterizerStateObject		*m_RasterizerStateObject;	// ラスタライザ・ステート・オブジェクト
	DepthStencilStateObject	*m_DepthStencilStateObject;	// 深度/ステンシル・ステート・オブジェクト
	SamplerStateObject			*m_SamplerStateObject[s_textureNum];	// サンプラー・ステート・オブジェクト
	// 定数バッファのデータ
	cbNeverChanges						m_cbNeverChanges;			// 逆ビューポート行列
	Texture										*m_pTexture[s_textureNum];					// テクスチャ
	FontAnalyzer							m_analyzer;
};

FontRenderer::Impl::Impl(s16 screenWidth, s16 screenHeight) :
	m_screenWidth(screenWidth),
	m_screenHeight(screenHeight),
  m_fontColor(math::Vector4(0.0f, 0.0f, 0.0f, 1.0f))
	{
    m_analyzer.setDefaultFontColor(m_fontColor);

		VertexElement element[] = {
			{ 0, StreamUsage::Position,	0, VertexType::Float, 4 },
			{ 0, StreamUsage::TexCoord,	0, VertexType::Float, 4 },
			{ 0, StreamUsage::Color,	0, VertexType::Float, 4 },
			VertexElement::VERTEX_ELEMENT_END()
		};

#ifdef _GEOMETRY_SHADER_AVAILABLE_	
    GF_ALIGNED(0x80) static unsigned char  s_font_geometry_sh[] = {
			#include "../resource/font_geometry.sh.txt"
			0x00
		};
    m_pVertexShader = GFGL::CreateVertexShaderFromSource(reinterpret_cast<c8*>(s_font_geometry_sh), "VS", element, NULL);
    m_pGeometryShader = GFGL::CreateGeometryShaderFromSource(reinterpret_cast<c8*>(s_font_geometry_sh), "GS", NULL);
    m_pPixelShader = GFGL::CreatePixelShaderFromSource(reinterpret_cast<c8*>(s_font_geometry_sh), "PS", NULL);
#else
    GF_ALIGNED(0x80) static unsigned char s_font_gfvsh[] = {
			#include "../resource/font.gfvsh.txt"
			0x00
		};

    GF_ALIGNED(0x80) static unsigned char s_font_gffsh[] = {
			#include "../resource/font.gffsh.txt"
			0x00
		};
    m_pVertexShader = GFGL::CreateVertexShaderFromSource(reinterpret_cast<c8*>(s_font_gfvsh), "VS", element, NULL);
    m_pPixelShader = GFGL::CreatePixelShaderFromSource(reinterpret_cast<c8*>(s_font_gffsh), "PS", NULL);
#endif

	{
		ConstantBufferElement element[] = {
			{ ConstantType::Matrix, 1 },
			ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
		};

		m_pConstantBuffer[0] = GFGL::CreateConstantBuffer( element, Usage::WriteOnly, Pool::Default );
	}

#ifdef _GEOMETRY_SHADER_AVAILABLE_	
	{
		VertexFormat* vertexDataArray = (VertexFormat*)GLMemory::Malloc(sizeof(VertexFormat) * s_fontNum);
		memset(vertexDataArray, 0, sizeof(VertexFormat) * s_fontNum);
		m_pVertexBuffer = GFGL::CreateVertexBuffer( sizeof(VertexFormat), s_fontNum, Usage::WriteOnly, Pool::Default, vertexDataArray);
		GLMemory::Free(vertexDataArray);
	}
#else
	{
		VertexFormat* vertexDataArray = (VertexFormat*)GLMemory::Malloc(sizeof(VertexFormat) * s_fontNum * 4);
		memset(vertexDataArray, 0, sizeof(VertexFormat) * s_fontNum * 4);
		m_pVertexBuffer = GFGL::CreateVertexBuffer( sizeof(VertexFormat), s_fontNum * 4, Usage::WriteOnly, Pool::Default, vertexDataArray);
		GLMemory::Free(vertexDataArray);
	}

	{
		u16 idxVertexIdlLength = 6 * s_fontNum - 2;
		u16* idxVertexIdl = (u16*)GLMemory::Malloc(sizeof(u16) * idxVertexIdlLength);
		u16 cntVtx = 0;
		for(u16 cntFont = 0; cntFont < s_fontNum; ++cntFont)
		{
			idxVertexIdl[cntVtx++] = 4 * cntFont;
			idxVertexIdl[cntVtx++] = 4 * cntFont + 1;
			idxVertexIdl[cntVtx++] = 4 * cntFont + 2;
			idxVertexIdl[cntVtx++] = 4 * cntFont + 3;

			if(cntFont < (s_fontNum - 1))
			{
				idxVertexIdl[cntVtx++] = 4 * cntFont + 3;
				idxVertexIdl[cntVtx++] = 4 * cntFont + 4;
			}
		}

		m_pIndexBuffer = GFGL::CreateIndexBuffer( sizeof(u16) * idxVertexIdlLength, Usage::WriteOnly, Pool::Default, IndexType::Index16, idxVertexIdl);
		GLMemory::Free(idxVertexIdl);
	}
#endif

	m_pBlendStateObject = GFGL::CreateBlendStateObject();
	m_pBlendStateObject->SetBlendEnable(true);
	m_pBlendStateObject->SetBlendOp(BlendOp::Add);
	m_pBlendStateObject->SetBlendFunc(
		BlendFunc::SrcAlpha,
		BlendFunc::InverseSrcAlpha,
		BlendFunc::One,
		BlendFunc::Zero);
	m_pBlendStateObject->UpdateState();

	m_RasterizerStateObject = GFGL::CreateRasterizerStateObject();
	m_RasterizerStateObject->SetCullMode( CullMode::None );
	m_RasterizerStateObject->UpdateState();

	m_DepthStencilStateObject = GFGL::CreateDepthStencilStateObject();
	m_DepthStencilStateObject->SetDepthTestEnable(false);
	m_DepthStencilStateObject->UpdateState();

	for(int i = 0; i < s_textureNum; ++i)
	{
		m_SamplerStateObject[i] = GFGL::CreateSamplerStateObject();
		m_SamplerStateObject[i]->UpdateState();
	}
	// **********************************************************

  GF_ALIGNED(0x80) static const unsigned char  s_font_texture_00_tga[] = {
		#include "../resource/font_texture_00.tga.txt"
    0x00
  };
  GF_ALIGNED(0x80) static const unsigned char  s_font_texture_01_tga[] = {
		#include "../resource/font_texture_01.tga.txt"
    0x00
  };
  GF_ALIGNED(0x80) static const unsigned char  s_font_texture_02_tga[] = {
		#include "../resource/font_texture_02.tga.txt"
    0x00
  };
  GF_ALIGNED(0x80) static const unsigned char  s_font_texture_03_tga[] = {
		#include "../resource/font_texture_03.tga.txt"
    0x00
  };
  GF_ALIGNED(0x80) static const unsigned char  s_font_texture_04_tga[] = {
		#include "../resource/font_texture_04.tga.txt"
    0x00
  };

	ImageData image[s_textureNum];
  image[0].ReadFromMemory(s_font_texture_00_tga);
  image[1].ReadFromMemory(s_font_texture_01_tga);
  image[2].ReadFromMemory(s_font_texture_02_tga);
  image[3].ReadFromMemory(s_font_texture_03_tga);
  image[4].ReadFromMemory(s_font_texture_04_tga);
	for(int i = 0; i < s_textureNum; ++i)
	{
		m_pTexture[i] = GFGL::CreateTextureFromImage( image[i], Pool::Default );
	}
}

FontRenderer::Impl::~Impl()
{
	reset();
}

//! @name 描画命令関連
//! @attention  フォント描画メソッドは必ず begin() 〜 end() の間で呼ばなければいけない
b32 FontRenderer::Impl::begin()
{
	return true;
}

//! @brief フォント描画処理を終了する
void FontRenderer::Impl::end()
{
}

b32 FontRenderer::Impl::update()
{
	{// 定数バッファ[0]を更新
		// 解像度が変わったときにココを変えれば上手くいくはず
		math::Matrix matInvViewport;
		matInvViewport = math::Matrix::GetIdentity();
		matInvViewport.SetElem(0, 0, 2.0f / (float)m_screenWidth);
		matInvViewport.SetElem(1, 1, -2.0f / (float)m_screenHeight);
		matInvViewport.SetElem(0, 3, -1.0f);
		matInvViewport.SetElem(1, 3, 1.0f);
		m_cbNeverChanges.InvViewport = matInvViewport;

		m_pConstantBuffer[0]->Update(&m_cbNeverChanges, sizeof(m_cbNeverChanges));
	}

	return true;
}

u16 FontRenderer::Impl::draw(const c16* wstr, f32 x, f32 y, f32 w, f32 h)
{
#ifdef _GEOMETRY_SHADER_AVAILABLE_	
	VertexFormat vertexDataArray[s_fontNum];
#else
	VertexFormat vertexDataArray[s_fontNum * 4];
#endif

	u16 cntFont = 0;
	u16 dispFontLength = (u16)wcslen(wstr);
  if (dispFontLength == 0)
  {
    return cntFont;
  }
  assert((dispFontLength <= FontRenderer::Impl::s_fontNum) && "FontRenderer::Impl::draw(). 文字列が512文字を超えています.");
	// ココの決めうちはどうにかしたい.
	// 画像データの枠の本数とかで引っ張って来られそうな気がする.
	const s16 numFontX = 32;
	const s16 numFontY = 32;
	f32 offsetUVWidth = 1.0f / (f32)numFontX;
	f32 offsetUVHeight = 1.0f / (f32)numFontY;
	u16 diffX = 0;		// 次の文字への差分
	f32 baseX = x;		// 基準となるX座標
	math::Vector4 color = m_fontColor;

	while(*wstr)
	{
		if(cntFont >= s_fontNum)
		{
			break;
		}

		c16 fontCode = *wstr;
		if(fontCode < 0x20)
		{// ASCIIの制御コードの範囲だったら改行などの制御を行う
			m_analyzer.analyzeAsciiCtrlCode(&x, &y, &diffX, &baseX, &color,
				&wstr, w, h, m_screenWidth, m_screenHeight);
			continue;
		}

		if(fontCode == L'<')
		{// 開始タグを発見したらタグの解釈を行う
			m_analyzer.analyzeTag(&x, &y, &diffX, &baseX, &color,
				&wstr, w, h, m_screenWidth, m_screenHeight);
			continue;
		}

		if(fontCode == L'&')
		{// "&lt;", "&gt;"のいずれかに該当していないかどうかを調べる
			fontCode = m_analyzer.analyzeEscapeSequence(&x, &y, &diffX, &baseX, &color,
				&wstr, w, h, m_screenWidth, m_screenHeight);
		}

		if(fontCode < 0x7F)
		{// ASCIIの範囲だったら0x0020(半角スペース分)だけ差し引く
			fontCode -= 0x20;
		}

		f32 fontU = (fontCode % numFontX) / (f32)numFontX;
		f32 fontV = (fontCode / numFontX) / (f32)numFontY;

#ifdef _GEOMETRY_SHADER_AVAILABLE_	
		vertexDataArray[cntFont] = VertexFormat(
			math::Vector4(baseX + diffX * w, y, w, h),
			math::Vector4(fontU, fontV, offsetUVWidth, offsetUVHeight),
			color
		);
#else
		vertexDataArray[4 * cntFont + 0] = VertexFormat(
			math::Vector4(baseX + (diffX + 0) * w, y, 0.0f),
			math::Vector4(fontU, fontV, 0.0f),
			color
		);
		vertexDataArray[4 * cntFont + 1] = VertexFormat(
			math::Vector4(baseX + (diffX + 1) * w, y, 0.0f),
			math::Vector4(fontU + offsetUVWidth, fontV, 0.0f),
			color
		);
		vertexDataArray[4 * cntFont + 2] = VertexFormat(
			math::Vector4(baseX + (diffX + 0) * w, y + h, 0.0f),
			math::Vector4(fontU, fontV + offsetUVHeight, 0.0f),
			color
		);
		vertexDataArray[4 * cntFont + 3] = VertexFormat(
			math::Vector4(baseX + (diffX + 1) * w, y + h, 0.0f),
			math::Vector4(fontU + offsetUVWidth, fontV + offsetUVHeight, 0.0f),
			color
		);
#endif

		++diffX;
		++wstr;
		++cntFont;
	}

	m_pVertexBuffer->Update(vertexDataArray, sizeof(vertexDataArray));

	// ***************************************
	GFGL::SetVertexShader( m_pVertexShader );
	GFGL::SetVertexShaderConstant( 0, m_pConstantBuffer[0] );
#ifdef _GEOMETRY_SHADER_AVAILABLE_	
	GFGL::SetGeometryShader( m_pGeometryShader );
	GFGL::SetGeometryShaderConstant( 0, m_pConstantBuffer[0] );
#endif
	GFGL::SetPixelShader( m_pPixelShader );
#ifndef _WIN64
  for (int i = 0; i < s_textureNum; ++i)
  {
    // @attention: 2014.12.19 定数バッファで1つ使っているので1 + ってなってる. 2014.12.22以降に取り決めが決まる予定.
    glProgramUniform1i(static_cast<gfl2::gfx::winOpenGL::WinOpenGLPixelShader*>(m_pPixelShader)->GetHandle(), 1 + i, i);
  }
#endif

	GFGL::SetPixelShaderConstant( 0, m_pConstantBuffer[0] );

#ifndef _GEOMETRY_SHADER_AVAILABLE_	
	GFGL::SetIndices( m_pIndexBuffer );
#endif
	GFGL::SetStreamSource( 0, m_pVertexBuffer );

	GFGL::SetBlendState( m_pBlendStateObject );
	GFGL::SetRasterizerState( m_RasterizerStateObject );
	GFGL::SetDepthStencilState( m_DepthStencilStateObject );
	for(int i = 0; i< s_textureNum; ++i)
	{
		GFGL::SetSamplerState( i, m_SamplerStateObject[i] );
	}
	for(int i = 0; i < s_textureNum; ++i)
	{
		GFGL::SetTexture(i, m_pTexture[i]);
	}
	// ***************************************

#ifdef _GEOMETRY_SHADER_AVAILABLE_	
	GFGL::DrawPrimitive(PrimitiveType::Points, 0, s_fontNum);
#else
	GFGL::DrawIndexedPrimitive( PrimitiveType::TriangleStrip, 0, 6 * cntFont - 2);
#endif

	return cntFont;
}

void FontRenderer::Impl::setScreenSize(s16 screenWidth, s16 screenHeight)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
}

void FontRenderer::Impl::reset()
{
	for(int i = 0; i < s_textureNum; ++i)
	{
		GFX_SAFE_DELETE( m_pTexture[i] );
	}
	for(int i = 0; i < s_textureNum; ++i)
	{
		GFX_SAFE_DELETE( m_SamplerStateObject[i] );
	}
	GFX_SAFE_DELETE( m_DepthStencilStateObject );
	GFX_SAFE_DELETE( m_RasterizerStateObject );
	GFX_SAFE_DELETE( m_pBlendStateObject );

#ifndef _GEOMETRY_SHADER_AVAILABLE_	
	GFX_SAFE_DELETE( m_pIndexBuffer );
#endif
	GFX_SAFE_DELETE( m_pVertexBuffer );
	GFX_SAFE_DELETE( m_pConstantBuffer[0] );
	GFX_SAFE_DELETE( m_pVertexShader );
#ifdef _GEOMETRY_SHADER_AVAILABLE_	
	GFX_SAFE_DELETE( m_pGeometryShader );
#endif
	GFX_SAFE_DELETE( m_pPixelShader );
}

void FontRenderer::Impl::setDefaultFontColor(const math::Vector4& color)
{
  m_fontColor = color;
  m_analyzer.setDefaultFontColor(m_fontColor);
}

// 呼び出し側の実装
void FontRenderer::Create(s16 screenWidth, s16 screenHeight)
{
	// @attention: newをheapに変えること
	assert(!s_pImpl && "FontRenderer::Create() is called twice!");
	s_pImpl = new Impl(screenWidth, screenHeight);
}

void FontRenderer::Destroy()
{
	assert(s_pImpl && "FontRenderer::Destroy() is called twice!");
	// @attention: deleteをheapに変えること
	delete s_pImpl;
	s_pImpl = NULL;
}

b32 FontRenderer::Update()
{
	return s_pImpl->update();
}

u16 FontRenderer::Draw(const c16* wstr, f32 x, f32 y, f32 w, f32 h)
{
	return s_pImpl->draw(wstr, x, y, w, h);
}

u16 FontRenderer::DrawEx( f32 x, f32 y, f32 w, f32 h, const c16* wstr, ... )
{
  va_list	argp;
  wchar_t textBuffer[ FontRenderer::Impl::s_fontNum ];
  va_start(argp, wstr);
  s32 size = vswprintf( textBuffer, FontRenderer::Impl::s_fontNum, wstr, argp );
  va_end(argp);

  // タグ解釈で配列外参照が行われる可能性があるため、バッファサイズが足りない場合は処理を打ち切ります
  assert(size >= 0);
   

  FontString::ConvertUTF16( textBuffer, textBuffer ); 
  return Draw( textBuffer, x, y, w, h );
}

void FontRenderer::Reset()
{
	s_pImpl->reset();
}

b32 FontRenderer::Begin()
{
	return s_pImpl->begin();
}

void FontRenderer::End()
{
	s_pImpl->end();
}

void FontRenderer::SetScreenSize(s16 screenWidth, s16 screenHeight)
{
	s_pImpl->setScreenSize(screenWidth, screenHeight);
}

void FontRenderer::SetDefaultFontColor(const math::Vector4& color)
{
  s_pImpl->setDefaultFontColor(color);
}

}}