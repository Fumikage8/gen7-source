#include <clr/include/gfx/gfl2_Buffer_CLR.h>
#include <math/include/gfl2_Matrix.h>

namespace gfl2 { namespace clr { namespace gfx {

VertexBuffer::!VertexBuffer()
{
	if ( m_pNative )
	{
		delete m_pNative;
		m_pNative = 0;
	}
}

void VertexBuffer::Update( array<u8>^ pData, u32 size )
{
	pin_ptr<void>		pSrc = &pData[0];
	m_pNative->Update( pSrc, size );
}

IndexBuffer::!IndexBuffer()
{
	if ( m_pNative )
	{
		delete m_pNative;
		m_pNative = 0;
	}
}

void IndexBuffer::Update( array<u8>^ pData, u32 size )
{
	pin_ptr<void>		pSrc = &pData[0];
	m_pNative->Update( pSrc, size );
}

ConstantBuffer::!ConstantBuffer()
{
	if ( m_pNative )
	{
		delete m_pNative;
		m_pNative = 0;
	}
}

void ConstantBuffer::Update( array<u8>^ pData, u32 size )
{
	pin_ptr<void>		pSrc = &pData[0];
	void*						pVoid = pSrc ;
	math::Matrix		*pMat = (math::Matrix*)pVoid;
	
	m_pNative->Update( pSrc, size );
}


}}}