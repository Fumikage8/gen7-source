// ============================================================================
/*
 * @file gfl2_TemporaryBuffer.h
 * @brief テンポラリバッファクラスです。
 * @date 2015.03.12
 */
// ============================================================================
#if !defined( GFL2_TEMPORARYBUFFER_H_INCLUDED )
#define GFL2_TEMPORARYBUFFER_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)

class TemporaryBuffer
{
  GFL_FORBID_COPY_AND_ASSIGN( TemporaryBuffer );
private:
  static const u32 DEFAULT_ALIGN = 4;

public:
  TemporaryBuffer( gfl2::heap::HeapBase* pHeap );
  TemporaryBuffer( gfl2::heap::HeapBase* pHeap, u32 bufferSize );
  TemporaryBuffer( gfl2::heap::HeapBase* pHeap, const void* pData, u32 bufferSize );
  TemporaryBuffer( gfl2::heap::HeapBase* pHeap, u32 bufferSize, u32 align );
  virtual~TemporaryBuffer();

  void CreateBuffer( u32 bufferSize );
  void CreateBuffer( u32 bufferSize, u32 align );
  void ClearBuffer();
  u32 GetBufferSize() const;

  void ZeroClear();
  void CopyData( const void* pData, u32 dataSize );
  u8* GetData() const;
  u32 GetDataSize() const;
  void SetDataSize( u32 dataSize );

private:
  gfl2::heap::HeapBase*     m_pHeap;
  u8*                       m_pBuffer;
  u32                       m_BufferSize;
  u32                       m_DataSize;
};

GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_TEMPORARYBUFFER_H_INCLUDED
