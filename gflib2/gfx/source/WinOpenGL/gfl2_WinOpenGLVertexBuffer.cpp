#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLVertexBuffer.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

WinOpenGLVertexBuffer::WinOpenGLVertexBuffer(u32 size, u32 vertex_num, u32 usage, Pool pool, const void* buffer) : VertexBuffer()
{
  (void)usage;
  (void)pool;

	m_VertexNum = vertex_num;
	//　頂点バッファの作成
	glGenBuffers(1, &m_BufferHandle);
	glNamedBufferDataEXT(m_BufferHandle, vertex_num * size, buffer, GL_STATIC_DRAW);
}

WinOpenGLVertexBuffer::~WinOpenGLVertexBuffer()
{
	glDeleteBuffers( 1, &m_BufferHandle );
}

void* WinOpenGLVertexBuffer::Lock()
{
	return glMapNamedBufferEXT(m_BufferHandle, GL_READ_WRITE);
}

void WinOpenGLVertexBuffer::UnLock()
{
	glUnmapNamedBufferEXT(m_BufferHandle);
}

void WinOpenGLVertexBuffer::Update( const void* pData, u32 size )
{
	glNamedBufferSubDataEXT(m_BufferHandle, 0, size, pData );
}



}}}
