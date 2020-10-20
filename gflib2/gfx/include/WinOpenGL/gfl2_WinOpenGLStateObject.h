#ifndef GFLIB2_GRX_WINOpenGL_WINOpenGLSTATEOBJECT_H_INCLUDED
#define GFLIB2_GRX_WINOpenGL_WINOpenGLSTATEOBJECT_H_INCLUDED

#include <gfx/include/gfl2_StateObject.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLSurface.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTexture.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

class WinOpenGLGL;

class WinOpenGLBlendStateObject : public BlendStateObject
{
	friend class WinOpenGLGL;
public:
	virtual void UpdateState();
	virtual ~WinOpenGLBlendStateObject();
	void Bind( const WinOpenGLSurface** ppSurface, u32 max );

protected:
	
	WinOpenGLBlendStateObject();

	StateData			m_FixData;

};

class WinOpenGLRasterizerStateObject : public RasterizerStateObject
{
	friend class WinOpenGLGL;
public:
	virtual void UpdateState();
	virtual ~WinOpenGLRasterizerStateObject();
	void Bind();

protected:
	
	WinOpenGLRasterizerStateObject();

	StateData						m_FixData;

};

class WinOpenGLDepthStencilStateObject : public DepthStencilStateObject
{
	friend class WinOpenGLGL;
public:
	virtual void UpdateState();
	virtual ~WinOpenGLDepthStencilStateObject();

	void Bind();

protected:
	
	WinOpenGLDepthStencilStateObject();

	StateData						m_FixData;

};

class WinOpenGLSamplerStateObject : public SamplerStateObject
{
	friend class WinOpenGLGL;
public:
	virtual void UpdateState();
	virtual ~WinOpenGLSamplerStateObject();

	void Bind( u32 texNo, const WinOpenGLTexture *pTexture ) const;
	GLuint GetHandle() const { return m_SamplerHandle; }

protected:
	
	WinOpenGLSamplerStateObject();

	StateData					m_FixData;

  GLuint m_SamplerHandle;
};


}}}

#endif