#include <gfx/include/CTR/gfl2_CTRTypes.h>
#include <gfx/include/CTR/gfl2_CTRGeometryShader.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace gfx { namespace ctr {

CTRGeometryShader::CTRGeometryShader( const void* code, u32 size ) :
  Shader(),
  m_hGsProgram(0)
{
  // 対応しません
}

CTRGeometryShader::~CTRGeometryShader()
{
}

void CTRGeometryShader::Bind() const
{
	
}

}}}
