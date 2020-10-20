#ifndef GFLIB2_GRX_CTR_CTRVERTEXSHADER_H_INCLUDED
#define GFLIB2_GRX_CTR_CTRVERTEXSHADER_H_INCLUDED

#include <gfx/include/gfl2_Shader.h>
#include <gfx/include/CTR/gfl2_CTRTypes.h>
#include <gfx/include/CTR/gfl2_CTRVertexBuffer.h>

namespace gfl2 { namespace gfx { namespace ctr {

class CTRVertexShader : public Shader
{
public:

  struct AttachType
  {
    enum
    {
      Geometry = 0,
      Pixel,
      Max
    };
  };

	CTRVertexShader( const void* code, u32 size, const VertexElement* pVertexElements );
	virtual ~CTRVertexShader();

	virtual void Bind() const;
	virtual void AttachProgram( AttachType type, GLuint handle );
	virtual void BindVertyexAttribute( const u32 index ) const;

  void BindVertexBuffer(CTRVertexBuffer* pVertexBuffer);

  nn::gr::Vertex* GetVertex()
  {
    return m_pVertex;
  }
  nn::gr::ShaderLite* GetHandle()
  {
    return &m_Shader;
  }

  u8 GetUniformSymbolOffset() const
  {
    return m_UniformSymbolOffset;
  }

#if 0
	const GLuint GetProgramHandle() const { return m_hProgram; }
	const GLuint GetHandle() const { return m_hVsProgram; }
#endif

private:

  nn::gr::ShaderLite m_Shader;
  const void* m_pShaderBinary;
  u32* m_pShaderBuffer;

  u8 m_UniformSymbolOffset;
  u32 : 24; // padding

  nn::gr::Vertex::InterleaveInfo m_InterleaveInfo;
  nn::gr::BindSymbolVSInput m_InputSymbolList[nn::gr::Vertex::VERTEX_ATTRIBUTE_MAX];

  nn::gr::Vertex* m_pVertex;
};

}}}

#endif
