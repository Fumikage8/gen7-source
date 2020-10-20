#include <gfx/include/CTR/gfl2_CTRTypes.h>
#include <gfx/include/CTR/gfl2_CTRVertexShader.h>
#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_GFGLUtil.h>

namespace gfl2 { namespace gfx { namespace ctr {

CTRVertexShader::CTRVertexShader( const void* code, u32 size, const VertexElement* pVertexElements ) :
  Shader(),
  m_pShaderBinary(NULL),
  m_pShaderBuffer(NULL),
  m_UniformSymbolOffset(0),
  m_pVertex(NULL)
{
	static const s32 s_FormatTable[][VertexType::NumberOf] =
  {
    // 要素１つ
    {
      -1,	                            //!< 16bit符号付整数を[-1...1]に正規化した値
      PICA_DATA_SIZE_1_FLOAT,					//!< 32bit浮動少数値
      -1,									            //!< 16bit浮動少数値
      -1,			                        //!< 8bit符号なし整数を[0...1]に正規化した値
      PICA_DATA_SIZE_1_SHORT,					//!< 16bit符号付整数
      -1,		                          //!< 32bit(11:11:10)に圧縮した整数を[-1...1]に正規化した値(PS3のみ)
      PICA_DATA_SIZE_1_UNSIGNED_BYTE,	//!< 8bit符号なし整数値
      -1,			                        //!< 8bit固定小数値(小数部6bit)
      -1,		                          //!< 16bit固定小数値(小数部12bit)
      PICA_DATA_SIZE_1_BYTE,          //!< 8bit符号付整数値
    },
    // 要素２つ
    {
      -1,	                            //!< 16bit符号付整数を[-1...1]に正規化した値
      PICA_DATA_SIZE_2_FLOAT,					//!< 32bit浮動少数値
      -1,									            //!< 16bit浮動少数値
      -1,			                        //!< 8bit符号なし整数を[0...1]に正規化した値
      PICA_DATA_SIZE_2_SHORT,					//!< 16bit符号付整数
      -1,		                          //!< 32bit(11:11:10)に圧縮した整数を[-1...1]に正規化した値(PS3のみ)
      PICA_DATA_SIZE_2_UNSIGNED_BYTE,	//!< 8bit符号なし整数値
      -1,			                        //!< 8bit固定小数値(小数部6bit)
      -1,		                          //!< 16bit固定小数値(小数部12bit)
      PICA_DATA_SIZE_2_BYTE,          //!< 8bit符号付整数値
    },
    // 要素３つ
    {
      -1,	                            //!< 16bit符号付整数を[-1...1]に正規化した値
      PICA_DATA_SIZE_3_FLOAT,					//!< 32bit浮動少数値
      -1,									            //!< 16bit浮動少数値
      -1,			                        //!< 8bit符号なし整数を[0...1]に正規化した値
      PICA_DATA_SIZE_3_SHORT,					//!< 16bit符号付整数
      -1,		                          //!< 32bit(11:11:10)に圧縮した整数を[-1...1]に正規化した値(PS3のみ)
      PICA_DATA_SIZE_3_UNSIGNED_BYTE,	//!< 8bit符号なし整数値
      -1,			                        //!< 8bit固定小数値(小数部6bit)
      -1,		                          //!< 16bit固定小数値(小数部12bit)
      PICA_DATA_SIZE_3_BYTE,          //!< 8bit符号付整数値
    },
    // 要素４つ
    {
      -1,	                            //!< 16bit符号付整数を[-1...1]に正規化した値
      PICA_DATA_SIZE_4_FLOAT,					//!< 32bit浮動少数値
      -1,									            //!< 16bit浮動少数値
      -1,			                        //!< 8bit符号なし整数を[0...1]に正規化した値
      PICA_DATA_SIZE_4_SHORT,					//!< 16bit符号付整数
      -1,		                          //!< 32bit(11:11:10)に圧縮した整数を[-1...1]に正規化した値(PS3のみ)
      PICA_DATA_SIZE_4_UNSIGNED_BYTE,	//!< 8bit符号なし整数値
      -1,			                        //!< 8bit固定小数値(小数部6bit)
      -1,		                          //!< 16bit固定小数値(小数部12bit)
      PICA_DATA_SIZE_4_BYTE,          //!< 8bit符号付整数値
    },
  };

  // シェーダーのセットアップ
  m_pShaderBinary = code;
  u32 bufferSize = m_Shader.GetRequiredBufferSize(m_pShaderBinary);

  // メモリ確保
  m_pShaderBuffer = reinterpret_cast<u32*>(gfl2::gfx::GLMemory::Malloc(bufferSize, 128));
  m_Shader.SetupBinary(m_pShaderBinary, 0, -1, m_pShaderBuffer);

  // インプットエレメントの解決
  u8 index = 0;

  // インターリーブ形式のみ
  while( pVertexElements->Stream != 0xFF)
  {
    // バインドシンボル取得
    nn::gr::BindSymbolVSInput* pSymbol = &m_InputSymbolList[index];
    bool isSuccess = m_Shader.SearchBindSymbol(pSymbol, index);

    GFL_ASSERT(isSuccess);

    u8 formatIndex = pVertexElements->Size - 1;
    u8 formatType = pVertexElements->Type;

    // インターリーブ情報
    m_InterleaveInfo.symbol[index] = pSymbol;
    s32 dataType = s_FormatTable[formatIndex][formatType];
    GFL_ASSERT(dataType >= 0);
    m_InterleaveInfo.dataType[index] = static_cast<PicaDataVertexAttrType>(dataType);

    ++pVertexElements;
    ++index;
  }
  m_InterleaveInfo.dataNum = index;

  // ユニフォーム変数用のオフセット取得
  m_UniformSymbolOffset = m_Shader.SearchBindSymbolNum(nn::gr::BindSymbol::SHADER_TYPE_VERTEX, nn::gr::BindSymbol::SYMBOL_TYPE_INPUT);
}

CTRVertexShader::~CTRVertexShader()
{
  // メモリ開放
  if (m_pShaderBuffer != NULL)
  {
    gfl2::gfx::GLMemory::Free(m_pShaderBuffer);
  }
}

void CTRVertexShader::Bind() const
{
}

void CTRVertexShader::AttachProgram( AttachType type, GLuint handle )
{
}

void CTRVertexShader::BindVertyexAttribute( const u32 index ) const
{
}

void CTRVertexShader::BindVertexBuffer(CTRVertexBuffer* pVertexBuffer)
{
  if (m_pVertex == NULL)
  {
    nn::gr::Vertex* pVertex = pVertexBuffer->GetHandle();
    pVertex->EnableInterleavedArray(m_InterleaveInfo, pVertexBuffer->GetPhysicalAddr());

    m_pVertex = pVertex;
  }
}

}}}
