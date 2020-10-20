// ============================================================================
/*
 * @file AppQRTextureCreateUtility.cpp
 * @brief QRテクスチャ生成ユーティリティです
 * @date 2015.11.10
 * @author endo_akira
 */
// ============================================================================
#include "AppLib/include/Util/AppQRTextureCreateUtility.h"
#include "AppLib/include/Util/app_util_2d.h"
#include <qr/include/gfl2_IQREncoder.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


AppQRTextureCreateUtility::AppQRTextureCreateUtility( gfl2::heap::HeapBase* pHeap, u32 encodeDataBufferSize ) :
  m_QREncodeData( pHeap, encodeDataBufferSize ),
  m_pQREncoder( NULL ),
  m_TextureInfo(),
  m_eState( STATE_NULL ),
  m_bCreate( false )
{
  m_pQREncoder = gfl2::qr::IQREncoder::CreateQREncoder( pHeap, pHeap, 17 );
}


AppQRTextureCreateUtility::AppQRTextureCreateUtility( gfl2::heap::HeapBase* pHeap, u32 encodeDataBufferSize, int threadPriority ) :
  m_QREncodeData( pHeap, encodeDataBufferSize ),
  m_pQREncoder( NULL ),
  m_TextureInfo(),
  m_eState( STATE_NULL ),
  m_bCreate( false )
{
  m_pQREncoder = gfl2::qr::IQREncoder::CreateQREncoder( pHeap, pHeap, threadPriority );
}


AppQRTextureCreateUtility::~AppQRTextureCreateUtility()
{
  GFL_SAFE_DELETE( m_pQREncoder );
}


bool AppQRTextureCreateUtility::IsBusy()
{
  return m_pQREncoder->IsBusy();
}


void AppQRTextureCreateUtility::CreateStart( const u8* pData, u32 dataSize, u32 version, u32 cellSize )
{
  if( m_eState == STATE_NULL )
  {
    m_QREncodeData.CopyData( pData, dataSize );
    m_QREncodeData.SetVersion( version );
    m_QREncodeData.SetCellSize( cellSize );
    m_pQREncoder->EncodeAsync( this, &m_QREncodeData );
    m_eState = STATE_CREATING;
    m_bCreate = false;
  }
}


void AppQRTextureCreateUtility::Update()
{
  m_pQREncoder->Update();
}


gfl2::lyt::LytResTextureInfo* AppQRTextureCreateUtility::GetLytResTextureInfo()
{
  if( m_bCreate )
  {
    return &m_TextureInfo;
  }
  return NULL;
}


void AppQRTextureCreateUtility::OnEncodeSuccess( const gfl2::qr::QRRGBImageInfo* pQRRGBImageInfo, const u8* pTextureBuffer )
{
#ifdef GF_PLATFORM_CTR

  u16 imageWidth      = pQRRGBImageInfo->GetWidth();
  u16 imageHeight     = pQRRGBImageInfo->GetHeight();
  u16 realWidth       = static_cast<u16>( nn::math::CeilPwr2( static_cast<u32>( imageWidth ) ) );
  u16 realHeight      = static_cast<u16>( nn::math::CeilPwr2( static_cast<u32>( imageHeight ) ) );

  uptr virtualAddr = reinterpret_cast<uptr>( pTextureBuffer );
    
  u32 texObject = app::util::G2DUtil::CTRCalcLytTextureInfoTexObject( virtualAddr );
    
  m_TextureInfo.Set(
    texObject,
    nngxGetPhysicalAddr( virtualAddr ),
    nw::lyt::TexSize( imageWidth, imageHeight ),
    nw::lyt::TexSize( realWidth, realHeight ),
    nw::lyt::TEXFORMAT_RGB8,
    nw::lyt::TEXELROTATION_NONE );

#endif // GF_PLATFORM_CTR

  m_eState = STATE_NULL;
  m_bCreate = true;
}


void AppQRTextureCreateUtility::OnEncodeFailure()
{
  m_eState = STATE_NULL;
}


void AppQRTextureCreateUtility::OnDecodeSuccess( const gfl2::qr::QRDecodeData* pQRDecodeData )
{
}


void AppQRTextureCreateUtility::OnDecodeFailure()
{
}


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

