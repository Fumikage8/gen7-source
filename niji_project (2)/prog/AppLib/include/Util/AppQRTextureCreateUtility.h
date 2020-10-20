// ============================================================================
/*
 * @file AppQRTextureCreateUtility.h
 * @brief QRテクスチャ生成ユーティリティです
 * @date 2015.11.10
 * @author endo_akira
 */
// ============================================================================
#if !defined( APPQRTEXTURECREATEUTILITY_H_INCLUDE )
#define APPQRTEXTURECREATEUTILITY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <qr/include/gfl2_QREncodeData.h>
#include <qr/include/gfl2_IQREventListener.h>
#include <Layout/include/gfl2_LytRes.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
class IQREncoder;
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


class AppQRTextureCreateUtility : public gfl2::qr::IQREventListener
{
  GFL_FORBID_COPY_AND_ASSIGN( AppQRTextureCreateUtility );

public:

  AppQRTextureCreateUtility( gfl2::heap::HeapBase* pHeap, u32 encodeDataBufferSize );
  AppQRTextureCreateUtility( gfl2::heap::HeapBase* pHeap, u32 encodeDataBufferSize, int threadPriority );
  virtual~AppQRTextureCreateUtility();

  // IQREventListener
  virtual void OnEncodeSuccess( const gfl2::qr::QRRGBImageInfo* pQRRGBImageInfo, const u8* pTextureBuffer );
  virtual void OnEncodeFailure();
  virtual void OnDecodeSuccess( const gfl2::qr::QRDecodeData* pQRDecodeData );
  virtual void OnDecodeFailure();

  bool IsBusy();

  void CreateStart( const u8* pData, u32 dataSize, u32 version = 16, u32 cellSize = 2 );
  void Update();
  gfl2::lyt::LytResTextureInfo* GetLytResTextureInfo();

private:

  enum State
  {
    STATE_NULL,
    STATE_CREATING
  };

  gfl2::qr::QREncodeData                  m_QREncodeData;
  gfl2::qr::IQREncoder*                   m_pQREncoder;
  gfl2::lyt::LytResTextureInfo            m_TextureInfo;
  State                                   m_eState;
  bool                                    m_bCreate;

};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

#endif // APPQRTEXTURECREATEUTILITY_H_INCLUDE

