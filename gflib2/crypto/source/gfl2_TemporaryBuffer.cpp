// ============================================================================
/*
 * @file gfl2_TemporaryBuffer.cpp
 * @brief テンポラリバッファクラスです。
 * @date 2015.03.12
 */
// ============================================================================
#include <crypto/include/gfl2_TemporaryBuffer.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)


TemporaryBuffer::TemporaryBuffer( gfl2::heap::HeapBase* pHeap ) :
  m_pHeap( pHeap ),
  m_pBuffer( NULL ),
  m_BufferSize( 0 ),
  m_DataSize( 0 )
{
}


TemporaryBuffer::TemporaryBuffer( gfl2::heap::HeapBase* pHeap, u32 bufferSize ) :
  m_pHeap( pHeap ),
  m_pBuffer( NULL ),
  m_BufferSize( 0 ),
  m_DataSize( 0 )
{
  CreateBuffer( bufferSize, DEFAULT_ALIGN );
}


TemporaryBuffer::TemporaryBuffer( gfl2::heap::HeapBase* pHeap, const void* pData, u32 bufferSize ) :
  m_pHeap( pHeap ),
  m_pBuffer( NULL ),
  m_BufferSize( 0 ),
  m_DataSize( 0 )
{
  CreateBuffer( bufferSize, DEFAULT_ALIGN );
  CopyData( pData, bufferSize );
}


TemporaryBuffer::TemporaryBuffer( gfl2::heap::HeapBase* pHeap, u32 bufferSize, u32 align ) :
  m_pHeap( pHeap ),
  m_pBuffer( NULL ),
  m_BufferSize( 0 ),
  m_DataSize( 0 )
{
  CreateBuffer( bufferSize, align );
}


TemporaryBuffer::~TemporaryBuffer()
{
  ClearBuffer();
}


void TemporaryBuffer::CreateBuffer( u32 bufferSize )
{
  CreateBuffer( bufferSize, DEFAULT_ALIGN );
}


void TemporaryBuffer::CreateBuffer( u32 bufferSize, u32 align )
{
  if( m_pBuffer == NULL )
  {
    m_pBuffer = reinterpret_cast<u8*>( GflHeapAllocMemoryAlign( m_pHeap, bufferSize, align ) );
    m_BufferSize = bufferSize;
  }
}


void TemporaryBuffer::ClearBuffer()
{
  if( m_pBuffer != NULL )
  {
    GflHeapFreeMemory( m_pBuffer );
    m_pBuffer = NULL;
  }
  m_BufferSize = 0;
  m_DataSize = 0;
}


u32 TemporaryBuffer::GetBufferSize() const
{
  return m_BufferSize;
}


void TemporaryBuffer::ZeroClear()
{
  memset( m_pBuffer, 0, m_BufferSize );
}


void TemporaryBuffer::CopyData( const void* pData, u32 dataSize )
{
  if( m_pBuffer != NULL )
  {
    if( dataSize <= m_BufferSize )
    {
      memcpy( m_pBuffer, pData, dataSize );
      m_DataSize = dataSize;
    }
  }
}


u8* TemporaryBuffer::GetData() const
{
  return m_pBuffer;
}


u32 TemporaryBuffer::GetDataSize() const
{
  return m_DataSize;
}


void TemporaryBuffer::SetDataSize( u32 dataSize )
{
  m_DataSize = dataSize;
}


GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)
