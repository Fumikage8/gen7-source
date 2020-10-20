#ifndef GFLIB2_GRX_WINOpenGL_WINOpenGLCONSTANTBUFFER_H_INCLUDED
#define GFLIB2_GRX_WINOpenGL_WINOpenGLCONSTANTBUFFER_H_INCLUDED

#include <gfx/include/gfl2_Buffer.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

class WinOpenGLGL;

class WinOpenGLConstantBuffer : public ConstantBuffer
{
public:
	friend class WinOpenGLGL;

	WinOpenGLConstantBuffer(const ConstantBufferElement* pConstantBufferElements, u32 usage, Pool pool, const void* buffer);
	virtual ~WinOpenGLConstantBuffer();

	virtual void* Lock();
	virtual void UnLock();
	virtual void Update( const void* pData, u32 size );

	const GLuint GetHandle() const { return m_BufferHandle; }

private:

	u32						m_Size;
	GLuint				m_BufferHandle;
	
};

}}}

#endif