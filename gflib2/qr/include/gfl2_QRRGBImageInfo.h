// ============================================================================
/*
 * @file gfl2_QRRGBImageInfo.h
 * @brief QRコードのRGB形式のイメージ情報です。
 * @date 2015.03.05
 */
// ============================================================================
#if !defined( GFL2_QRRGBIMAGEINFO_H_INCLUDE )
#define GFL2_QRRGBIMAGEINFO_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)


class QRRGBImageInfo
{
  GFL_FORBID_COPY_AND_ASSIGN( QRRGBImageInfo );
public:
  QRRGBImageInfo( gfl2::heap::HeapBase* pDeviceHeap );
  virtual~QRRGBImageInfo();

  // --------------------------------------------------------------------------
  /**
   * @brief  RGBイメージデータを格納するバッファを削除します。
   */
  // --------------------------------------------------------------------------
  void ClearBuffer();
  void ClearBMPBuffer();

  // --------------------------------------------------------------------------
  /**
   * @brief  RGBイメージデータを格納するバッファを作成します。
   * @param  bufferSize バッファのサイズです。byteサイズを指定します。
   */
  // --------------------------------------------------------------------------
  void CreateBuffer( u32 bufferSize );
  void CreateBMPBuffer( u32 bufferSize );

  // --------------------------------------------------------------------------
  /**
   * @brief  RGBイメージデータを取得します。
   * @return RGBイメージデータ
   */
  // --------------------------------------------------------------------------
  u8* GetRGBImageBuffer() const;
  u8* GetBMPImageBuffer() const;

  // --------------------------------------------------------------------------
  /**
   * @brief  RGBイメージデータのバッファサイズを取得します。
   * @return RGBイメージデータのバッファサイズ
   */
  // --------------------------------------------------------------------------
  u32 GetRGBImageBufferSize() const;
  u32 GetBMPImageBufferSize() const;

  // --------------------------------------------------------------------------
  /**
   * @brief  イメージの横幅を設定します。
   * @param  width イメージの横幅
   */
  // --------------------------------------------------------------------------
  void SetWidth( u16 width );

  // --------------------------------------------------------------------------
  /**
   * @brief  イメージの横幅を取得します。
   * @return イメージの横幅
   */
  // --------------------------------------------------------------------------
  u16 GetWidth() const;

  // --------------------------------------------------------------------------
  /**
   * @brief  イメージの縦幅を設定します。
   * @param  height イメージの縦幅
   */
  // --------------------------------------------------------------------------
  void SetHeight( u16 height );

  // --------------------------------------------------------------------------
  /**
   * @brief  イメージの縦幅を取得します。
   * @return イメージの縦幅
   */
  // --------------------------------------------------------------------------
  u16 GetHeight() const;

private:
  gfl2::heap::HeapBase*     m_pDeviceHeap;
  u8*                       m_pBuffer;
  u8*                       m_pBMPBuffer;
  u32                       m_BufferSize;
  u32                       m_BMPBufferSize;
  u16                       m_Width;
  u16                       m_Height;


};


GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_QRRGBIMAGEINFO_H_INCLUDE
