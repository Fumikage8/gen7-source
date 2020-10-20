#ifndef GFLIB2_GRX_WINOpenGL_WINOpenGLVERTEXBUFFER_H_INCLUDED
#define GFLIB2_GRX_WINOpenGL_WINOpenGLVERTEXBUFFER_H_INCLUDED

#include <gfx/include/gfl2_Buffer.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

class WinOpenGLGL;

class WinOpenGLVertexBuffer : public VertexBuffer
{
public:
	friend class WinOpenGLGL;

	WinOpenGLVertexBuffer(u32 size, u32 vertex_num, u32 usage, Pool pool, const void* buffer);
	virtual ~WinOpenGLVertexBuffer();

	virtual void* Lock();
	virtual void UnLock();
	virtual void Update( const void* pData, u32 size );

	const GLuint GetHandle() const { return m_BufferHandle; }

private:
	u32					m_VertexNum;
	GLuint			m_BufferHandle;
	
};

}}}

#endif