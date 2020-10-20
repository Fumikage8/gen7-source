//==============================================================================
/**
 * @file   NetAppQRTexture.cpp
 * @date   2015/10/16 Fri. 15:54:02
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <qr/include/gfl2_QRRGBImageInfo.h>
#include  <qr/source/ctr/gfl2_CTR_TextureConverter.h>

#include  "AppLib/include/Util/app_util_heap.h"
#include  "AppLib/include/Util/app_util_2d.h"
#include  "AppLib/include/Tool/app_tool_LytTexReplaceSystem.h"

#include  "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include  "NetStatic/NetAppLib/include/UI/NetAppQRTexture.h"



namespace NetAppLib {
namespace UI        {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppQRTexture::NetAppQRTexture(System::ApplicationWorkBase* pWorkBase)
  : m_pWorkBase(pWorkBase)
  , m_pTextureBuffer(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppQRTexture::~NetAppQRTexture()
{
  Release();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppQRTexture::Build(const gfl2::qr::QRRGBImageInfo* pQRImageInfo)
{
#if defined( GF_PLATFORM_CTR )
  if( m_pTextureBuffer == NULL )
  {
    app::util::Heap* pAppHeap = m_pWorkBase->GetAppHeap();

    u8* pRGBImageBuffer = pQRImageInfo->GetRGBImageBuffer();
    u16 imageWidth      = pQRImageInfo->GetWidth();
    u16 imageHeight     = pQRImageInfo->GetHeight();
    u16 realWidth       = static_cast<u16>( nn::math::CeilPwr2( static_cast<u32>( imageWidth ) ) );
    u16 realHeight      = static_cast<u16>( nn::math::CeilPwr2( static_cast<u32>( imageHeight ) ) );
    u32 imageBufferSize = realWidth * realHeight * 3;

    m_pTextureBuffer = static_cast<u8*>( GflHeapAllocMemoryAlign( pAppHeap->GetDeviceHeap(), imageBufferSize, 128 ) );
    u8* pReverseTextureBuffer = static_cast<u8*>( GflHeapAllocMemoryAlign( pAppHeap->GetDeviceHeap(), imageBufferSize, 128 ) );

    /*  変換元を準備（ConvertGLTextureToNativeで上下が逆になるようなので、ここでも反転）  */
    {
      const u32   qrLineBytes   = imageWidth * 3;                     /*  qr画像のラインサイズ    */
      const u32   texLineBytes  = realWidth * 3;                      /*  テクスチャラインサイズ  */
      const u32   strideBytes   = texLineBytes - qrLineBytes;         /*  余白部分のラインサイズ  */

      const u8*   pSrc  = reinterpret_cast<const u8*>( pRGBImageBuffer );

      /*  有効部分  */
      for( u16 line = 0; line < imageHeight; ++line )
      {
        u8* pDst = pReverseTextureBuffer + texLineBytes * ( realHeight - 1 - line );

        std::memcpy( pDst, pSrc, qrLineBytes );   /*  qr部分  */
        pDst += qrLineBytes;
        pSrc += qrLineBytes;

        std::memset( pDst, 0x44, strideBytes );   /*  右余白  */
        pDst += strideBytes;
      }

      /*  下余白  */
      std::memset( pReverseTextureBuffer, 0x0f, texLineBytes * ( realHeight - imageHeight ) );
    }

    gfl2::qr::ctr::ConvertGLTextureToNative( GL_RGB_NATIVE_DMP, static_cast<u32>( realWidth ), static_cast<u32>( realHeight ), pReverseTextureBuffer, m_pTextureBuffer );

    uptr virtualAddr = reinterpret_cast<uptr>( m_pTextureBuffer );
    
    u32 texObject = app::util::G2DUtil::CTRCalcLytTextureInfoTexObject( virtualAddr );
    
    m_TextureInfo.Set(
      texObject,
      nngxGetPhysicalAddr( virtualAddr ),
      nw::lyt::TexSize( imageWidth, imageHeight ),
      nw::lyt::TexSize( realWidth, realHeight ),
      nw::lyt::TEXFORMAT_RGB8,
      nw::lyt::TEXELROTATION_NONE );

    GflHeapFreeMemory( pReverseTextureBuffer );
  }
#endif // defined( GF_PLATFORM_CTR )
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppQRTexture::Release(void)
{
  GflHeapFreeMemory( m_pTextureBuffer );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppQRTexture::AssignTextureToPicturePane(gfl2::lyt::LytPicture* pPicture)
{
  app::tool::LytTexReplaceSystem::ReplaceTextureStatic( &m_TextureInfo, pPicture, 0, 0);
}






















} /*  namespace UI        */
} /*  namespace NetAppLib */
