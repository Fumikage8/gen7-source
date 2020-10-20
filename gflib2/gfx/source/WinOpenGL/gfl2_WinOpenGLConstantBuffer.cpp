#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLConstantBuffer.h>
#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace gfx { namespace winOpenGL {

WinOpenGLConstantBuffer::WinOpenGLConstantBuffer(const ConstantBufferElement* pConstantBufferElements, u32 usage, Pool pool, const void* buffer) : ConstantBuffer()
{
  (void)usage;
  (void)pool;

	m_Size = 0;

	u32					elementSize[ConstantType::NumberOf] = {
		sizeof(math::Vector4),//Vector4
		sizeof(math::Matrix44),//Matrix
		sizeof(b32),//Bool
		sizeof(s32),//Int
		sizeof(f32),//Float
		sizeof(b32)*4,//BoolArray
		sizeof(s32)*4,//IntArray
		sizeof(f32)*4//FloatArray
	};

	while( pConstantBufferElements->Type != 0xFF)
	{
		//パディングは考慮していない。
		m_Size += (elementSize[ pConstantBufferElements->Type ] * pConstantBufferElements->Size);
		++pConstantBufferElements;
	}

	glGenBuffers( 1, &m_BufferHandle );
  glNamedBufferDataEXT(m_BufferHandle, m_Size, buffer, GL_STATIC_DRAW);
}

WinOpenGLConstantBuffer::~WinOpenGLConstantBuffer()
{
	glDeleteBuffers( 1, &m_BufferHandle );
}

void* WinOpenGLConstantBuffer::Lock()
{
	return NULL;
}

void WinOpenGLConstantBuffer::UnLock()
{
	
}

void WinOpenGLConstantBuffer::Update( const void* pData, u32 size )
{
  glNamedBufferSubDataEXT(m_BufferHandle, 0, size, pData);
}



}}}
