#ifndef GFLIB2_GRX_WINOpenGL_WINOpenGLINDEXBUFFER_H_INCLUDED
#define GFLIB2_GRX_WINOpenGL_WINOpenGLINDEXBUFFER_H_INCLUDED

#include <gfx/include/gfl2_Buffer.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

class WinOpenGLGL;

class WinOpenGLIndexBuffer : public IndexBuffer
{
public:
	friend class WinOpenGLGL;

	WinOpenGLIndexBuffer(u32 size, u32 usage, Pool pool, IndexType type, const void* buffer);
	virtual ~WinOpenGLIndexBuffer();

	virtual void* Lock();
	virtual void UnLock();
	virtual void Update( const void* pData, u32 size );

	const GLuint GetHandle() const { return m_BufferHandle; }

private:

	GLuint		m_BufferHandle;
	
};

}}}

#endif