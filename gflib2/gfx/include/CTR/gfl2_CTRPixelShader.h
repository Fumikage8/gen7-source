#ifndef GFLIB2_GRX_CTR_CTRPIXELSHADER_H_INCLUDED
#define GFLIB2_GRX_CTR_CTRPIXELSHADER_H_INCLUDED

#include <gfx/include/gfl2_Shader.h>
#include <gfx/include/CTR/gfl2_CTRTypes.h>

namespace gfl2 { namespace gfx { namespace ctr {

class CTRPixelShader : public Shader
{
public:

	CTRPixelShader( const void* code, u32 size );
	virtual ~CTRPixelShader();

	virtual void Bind() const;

  const u32* GetCommandBuffer() const
  {
    return m_pCommandBuffer;
  }
  u32 GetCommandSize() const
  {
    return m_CommandSize;
  }

private:

  u32* m_pCommandBuffer;
  u32 m_CommandSize;
};

}}}

#endif
