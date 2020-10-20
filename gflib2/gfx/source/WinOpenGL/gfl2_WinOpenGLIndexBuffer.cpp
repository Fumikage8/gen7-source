#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLIndexBuffer.h>
#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_GFGLUtil.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

WinOpenGLIndexBuffer::WinOpenGLIndexBuffer(u32 size, u32 usage, Pool pool, IndexType type, const void* buffer) : IndexBuffer()
{
  (void)usage;
  (void)pool;

	m_IndexType = type;
	m_IndexNum = size / GFGLUtil::GetIndexStride(type);

	//　インデックスバッファの作成
	glGenBuffers(1, &m_BufferHandle);
	glNamedBufferDataEXT(m_BufferHandle, size, buffer, GL_STATIC_DRAW);
}

WinOpenGLIndexBuffer::~WinOpenGLIndexBuffer()
{
	glDeleteBuffers(1, &m_BufferHandle);
}

void* WinOpenGLIndexBuffer::Lock()
{
	return glMapNamedBufferEXT(m_BufferHandle, GL_READ_WRITE);
}

void WinOpenGLIndexBuffer::UnLock()
{
	glUnmapNamedBufferEXT(m_BufferHandle);
}

void WinOpenGLIndexBuffer::Update( const void* pData, u32 size )
{
  glNamedBufferSubDataEXT(m_BufferHandle, 0, size, pData);
}



}}}
