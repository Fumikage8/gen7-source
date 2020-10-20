#include <gfx/include/CTR/gfl2_CTRTypes.h>
#include <gfx/include/CTR/gfl2_CTRPixelShader.h>
#include <gfx/include/gfl2_GFGL.h>

#include <string.h>

namespace gfl2 { namespace gfx { namespace ctr {

CTRPixelShader::CTRPixelShader( const void* code, u32 size ) : Shader()
{
  // コマンドバッファをピクセルシェーダとして使用します
  m_pCommandBuffer = reinterpret_cast<u32*>(gfl2::gfx::GLMemory::GLMemory::Malloc(size, 16));

  // メモリコピー
  if (m_pCommandBuffer != NULL)
  {
    memcpy(m_pCommandBuffer, code, size);
  }
  m_CommandSize = size;
}

CTRPixelShader::~CTRPixelShader()
{
  // メモリ開放
  if (m_pCommandBuffer != NULL)
  {
    gfl2::gfx::GLMemory::Free(m_pCommandBuffer);
  }
}

void CTRPixelShader::Bind() const
{
	
}

}}}
