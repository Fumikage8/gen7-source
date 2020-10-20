#if PM_DEBUG

//======================================================================
/**
 * @file    ImageDBTestLoadPicture.cpp
 * @date    2016/10/25 12:12:15
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if defined(GF_PLATFORM_CTR)

#include "ImageDBTestLoadPicture.h"

#include <Test/ImageDBTest/source/debug/ImageDBTestTick.h>

#include <util/include/gfl2_std_string.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )

namespace {
  static const u8 DELAY_TIME = 4;
}

ImageDBLoad::ImageDBLoad( gfl2::heap::HeapBase* heap )
  : m_pHeap(heap)
  , m_pColorBuffer(NULL)
  , m_pTmpLoadBuff( NULL )
  , m_pTmpConvBuffer( NULL )
  , m_pPicture( NULL )
  , m_DelayTime( 0 )
  , m_LoadSnapSeq( 0 )
  , m_LoadThumbSeq( 0 )
  , m_ThumbPaneIndex( 0 )
{
  m_pColorBuffer = ImageDBUtil::CreateColorBuffer( m_pHeap );

  for( u32 i = 0; i < 16; ++i )
  {
    m_pThumBuffer[i] = ImageDBUtil::CreatePhotoThumbnail( m_pHeap );
  }
}

ImageDBLoad::~ImageDBLoad( void )
{
  for( u32 i = 0; i < 16; ++i )
  {
    ImageDBUtil::DeletePhotoThumbnail( m_pThumBuffer[i] );
  }
  ImageDBUtil::DeleteColorBuffer( m_pColorBuffer );
}



void ImageDBLoad::Update( void )
{
  Update_Wait();
  Update_Replace();
}

void ImageDBLoad::Update_Wait( void )
{
  if( m_LoadSnapSeq == 1 || m_LoadThumbSeq == 1 )
  {
    m_DelayTime--;

    if( m_DelayTime == 0 )
    {
      if( m_LoadSnapSeq == 1 )  m_LoadSnapSeq++;
      if( m_LoadThumbSeq == 1 ) m_LoadThumbSeq++;
    }
  }
}

void ImageDBLoad::Update_Replace( void )
{
  if( m_LoadSnapSeq == 2 || m_LoadThumbSeq == 2 )
  {
    // ピクチャペインを非表示
    if( m_pPicture ) m_pPicture->SetVisible( false );

    if( m_LoadSnapSeq == 2 )  m_LoadSnapSeq++;
    if( m_LoadThumbSeq == 2 ) m_LoadThumbSeq++;
  }
  if( m_LoadSnapSeq == 3 )
  {
    // コピー
    gfl2::std::MemCopy( m_pTmpConvBuffer, m_pColorBuffer, ImageDBUtil::COLOR_BUFFER_SIZE );
    // テンポラリバッファの破棄
    ImageDBUtil::DeleteColorBuffer( m_pTmpConvBuffer );
    ImageDBUtil::DeleteColorBuffer( m_pTmpLoadBuff );
    // ピクチャペインへ反映
    ImageDBUtil::ReplaceTexture_LoadSnap( m_pColorBuffer, m_pPicture );
    m_LoadSnapSeq++;
  }
  if( m_LoadThumbSeq == 3 )
  {
    // コピー
    gfl2::std::MemCopy( m_pTmpConvBuffer, m_pThumBuffer[m_ThumbPaneIndex], sizeof( FinderPhotoThumbnail ) );
    // テンポラリバッファの破棄
    ImageDBUtil::DeletePhotoThumbnail( m_pTmpConvBuffer );
    ImageDBUtil::DeletePhotoThumbnail( m_pTmpLoadBuff );
    // ピクチャペインへ反映
    ImageDBUtil::ReplaceTexture_LoadThumb(
      static_cast<FinderPhotoThumbnail*>( m_pThumBuffer[m_ThumbPaneIndex] )->data, m_pPicture );
    m_LoadThumbSeq++;
  }
  if( m_LoadSnapSeq == 4 || m_LoadThumbSeq == 4 )
  {
    // ピクチャペインを非表示
    if( m_pPicture ) m_pPicture->SetVisible( true );

    if( m_LoadSnapSeq == 4 )
    {
      m_pListener->OnEndLoadSnap();

      DebugTick::EndTick( TICK_ID_IMGDB_LOAD );
      DebugTick::DumpTick( TICK_ID_IMGDB_LOAD, "ImageDBLoad" );
    }
    else if( m_LoadThumbSeq == 4 )
    {
      m_pListener->OnEndLoadThumbnail();

      DebugTick::EndTick( TICK_ID_IMGDB_LOAD_THUMB );
      DebugTick::DumpTick( TICK_ID_IMGDB_LOAD_THUMB, "ImageDBLoadThumb" );
    }

    m_LoadSnapSeq  = 0;
    m_LoadThumbSeq = 0;
  }
}



void ImageDBLoad::RequestLoadPhoto_Snap( const wchar_t* filepath, gfl2::lyt::LytPicture* picture )
{
  if( m_LoadSnapSeq == 0 || m_LoadThumbSeq == 0 )
  {
    DebugTick::StartTick( TICK_ID_IMGDB_LOAD );

    // テンポラリバッファの生成
    m_pTmpLoadBuff   = ImageDBUtil::CreateColorBuffer( m_pHeap );
    m_pTmpConvBuffer = ImageDBUtil::CreateColorBuffer( m_pHeap );
    // ピクチャペインの設定
    m_pPicture = picture;

    // 読込み
    ImageDBUtil::LoadSnapImage(
      m_pHeap, filepath, m_pTmpLoadBuff, m_pTmpConvBuffer );

    m_DelayTime = DELAY_TIME;
    m_LoadSnapSeq++;
  }
}

void ImageDBLoad::RequestLoadPhoto_Thumb( const wchar_t* filepath, gfl2::lyt::LytPicture* picture, u32 paneIndex )
{
  if( m_LoadSnapSeq == 0 || m_LoadThumbSeq == 0 )
  {
    DebugTick::StartTick( TICK_ID_IMGDB_LOAD_THUMB );

    // テンポラリバッファの生成
    m_pTmpLoadBuff = ImageDBUtil::CreatePhotoThumbnail( m_pHeap );
    m_pTmpConvBuffer = ImageDBUtil::CreatePhotoThumbnail( m_pHeap );

    // ピクチャペインの設定
    m_pPicture = picture;
    m_ThumbPaneIndex = paneIndex;

    // 読込み
    ImageDBUtil::LoadThumbImage(
      m_pHeap, filepath,
      static_cast<FinderPhotoThumbnail*>( m_pTmpLoadBuff ),
      static_cast<FinderPhotoThumbnail*>( m_pTmpConvBuffer ) );

    m_DelayTime = DELAY_TIME;
    m_LoadThumbSeq++;
  }
}


GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // defined(GF_PLATFORM_CTR)

#endif // #if PM_DEBUG
