#include <gfx/include/WinDX11/gfl2_WinDX11Types.h>
#include <gfx/include/WinDX11/gfl2_WinDX11VertexShader.h>
#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_GFGLUtil.h>

namespace gfl2 { namespace gfx { namespace windx11 {

WinDX11VertexShader::WinDX11VertexShader( const void* code, u32 size, const VertexElement* pVertexElements ) : Shader()
{
	HRESULT								hr;

	hr = GFGL::GetDevice()->CreateVertexShader( code, size, NULL, &m_pVertexShader);

	if (FAILED(hr))
		assert(0);

	// 入力要素
	static const s32					ELEMENT_MAX = D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;
	D3D11_INPUT_ELEMENT_DESC	layout[ELEMENT_MAX];

	//  頂点宣言を合成
	static const DXGI_FORMAT s_FormatTable[ VertexType::NumberOf ][ 4 ] = {
		{   DXGI_FORMAT_UNKNOWN,		DXGI_FORMAT_R16G16_SNORM,	DXGI_FORMAT_UNKNOWN,					DXGI_FORMAT_R16G16B16A16_SNORM     },		//  ShortNormalized = 0,    //!< 16bit符号付整数を[-1...1]に正規化した値
		{   DXGI_FORMAT_R32_FLOAT,	DXGI_FORMAT_R32G32_FLOAT,	DXGI_FORMAT_R32G32B32_FLOAT,	DXGI_FORMAT_R32G32B32A32_FLOAT      },	//  Float,                  //!< 32bit浮動少数値
		{   DXGI_FORMAT_UNKNOWN,		DXGI_FORMAT_R16G16_FLOAT,	DXGI_FORMAT_UNKNOWN,					DXGI_FORMAT_R16G16B16A16_FLOAT   },			//  Half,                   //!< 16bit浮動少数値
		{   DXGI_FORMAT_UNKNOWN,		DXGI_FORMAT_UNKNOWN,			DXGI_FORMAT_UNKNOWN,					DXGI_FORMAT_R8G8B8A8_UNORM     },				//  UByteNormalized,        //!< 8bit符号なし整数を[0...1]に正規化した値
		{   DXGI_FORMAT_UNKNOWN,		DXGI_FORMAT_R16G16_SINT,	DXGI_FORMAT_UNKNOWN,					DXGI_FORMAT_R16G16B16A16_SINT      },		//  Short,                  //!< 16bit符号付整数
		{   DXGI_FORMAT_UNKNOWN,		DXGI_FORMAT_UNKNOWN,			DXGI_FORMAT_UNKNOWN,					DXGI_FORMAT_UNKNOWN      },							//  CompressNormalized,     //!< 32bit(11:11:10)に圧縮した整数を[-1...1]に正規化した値(PS3のみ)
		{   DXGI_FORMAT_UNKNOWN,		DXGI_FORMAT_UNKNOWN,			DXGI_FORMAT_UNKNOWN,					DXGI_FORMAT_R8G8B8A8_UINT      },				//  UByte,                  //!< 8bit符号なし整数値
		{   DXGI_FORMAT_UNKNOWN,		DXGI_FORMAT_UNKNOWN,			DXGI_FORMAT_UNKNOWN,					DXGI_FORMAT_UNKNOWN      },							//  ByteNormalized6,
		{   DXGI_FORMAT_UNKNOWN,		DXGI_FORMAT_UNKNOWN,			DXGI_FORMAT_UNKNOWN,					DXGI_FORMAT_UNKNOWN      },							//  ShortNormalized12,
	};

	static const char* s_pSemanticTable[StreamUsage::NumberOf]= {
		"POSITION",
		"COLOR",
		"TEXCOORD",
		"NORMAL",
		"TANGENT",
		"BINORMAL",
		"BLENDWEIGHT",
		"BLENDINDICES"
	};

	s32				elementCnt = 0;
	s32				dataOffSet = 0;
	s32				beforeInputSlotNo = -1;
	while( pVertexElements->Stream != 0xFF)
	{
		if ( beforeInputSlotNo != pVertexElements->Stream )
			dataOffSet = 0;

		layout[elementCnt].SemanticName = s_pSemanticTable[pVertexElements->Usage];
		layout[elementCnt].SemanticIndex = pVertexElements->UsageIndex;
		layout[elementCnt].Format = s_FormatTable[pVertexElements->Type][pVertexElements->Size-1];
		layout[elementCnt].InputSlot = pVertexElements->Stream;
		layout[elementCnt].AlignedByteOffset = dataOffSet;
		layout[elementCnt].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		layout[elementCnt].InstanceDataStepRate = 0;

		dataOffSet += GFGLUtil::GetCoordStride( static_cast<VertexType>(pVertexElements->Type), pVertexElements->Size );

		beforeInputSlotNo = pVertexElements->Stream;
		++pVertexElements;
		++elementCnt;
	}
	assert( elementCnt < ELEMENT_MAX );

	// 入力レイアウト・オブジェクトの作成
	hr = GFGL::GetDevice()->CreateInputLayout( layout, elementCnt, code, size, &m_pInputLayout);

	if (FAILED(hr))
		assert(0);
}

WinDX11VertexShader::~WinDX11VertexShader()
{
	m_pVertexShader.reset();
	m_pInputLayout.reset();
}

void WinDX11VertexShader::Bind() const
{
	GFGL::GetDeviceContext()->IASetInputLayout(m_pInputLayout.get());
	GFGL::GetDeviceContext()->VSSetShader( m_pVertexShader.get(), NULL, 0);
}

}}}
