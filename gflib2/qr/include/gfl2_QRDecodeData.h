// ============================================================================
/*
 * @file gfl2_QRDecodeData.h
 * @brief QRコードのデコードデータです。
 * @date 2015.03.05
 */
// ============================================================================
#if !defined( GFL2_QRDECODEDATA_H_INCLUDE )
#define GFL2_QRDECODEDATA_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)


class QRDecodeData
{
  GFL_FORBID_COPY_AND_ASSIGN( QRDecodeData );
public:
  QRDecodeData( gfl2::heap::HeapBase* pHeap );
  virtual~QRDecodeData();

  // --------------------------------------------------------------------------
  /**
   * @brief  デコードデータを格納するバッファを削除します。
   */
  // --------------------------------------------------------------------------
  void ClearBuffer();

  // --------------------------------------------------------------------------
  /**
   * @brief  デコードデータを格納するバッファを作成します。
   * @param  bufferSize バッファのサイズです。byteサイズを指定します。
   */
  // --------------------------------------------------------------------------
  void CreateBuffer( u32 bufferSize );

  // --------------------------------------------------------------------------
  /**
   * @brief  デコードデータをバッファにコピーします。
   * @param  pData デコード結果のデータ
   * @param  dataSize デコードデータのサイズ
   */
  // --------------------------------------------------------------------------
  void CopyData( const u8* pData, u32 dataSize );

  // --------------------------------------------------------------------------
  /**
   * @brief  デコードデータを取得します。
   * @return デコードデータ
   */
  // --------------------------------------------------------------------------
  u8* GetData() const;

  // --------------------------------------------------------------------------
  /**
   * @brief  デコードデータのサイズを取得します。
   * @return デコードデータのサイズ
   */
  // --------------------------------------------------------------------------
  u32 GetDataSize() const;

private:
  gfl2::heap::HeapBase*     m_pHeap;
  u8*                       m_pBuffer;
  u32                       m_BufferSize;
  u32                       m_DataSize;

};


GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_QRDECODEDATA_H_INCLUDE
