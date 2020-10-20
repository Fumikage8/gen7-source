#if PM_DEBUG

//======================================================================
/**
 * @file    ImageDBTestSavePicture.cpp
 * @date    2016/10/24 11:40:39
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if defined(GF_PLATFORM_CTR)

#include "ImageDBTestSavePicture.h"

#include <Test/ImageDBTest/source/debug/ImageDBTestTick.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>

#include <util/include/gfl2_std_string.h>
#include <debug/include/gfl2_DebugTick.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )

namespace {
  static u32 DELAY_TIME = 4;

  enum {
    SEQ_NONE,
    SEQ_WAIT,
    SEQ_GPU_WAIT_1,
    SEQ_GPU_WAIT_2,
    SEQ_READY,
    SEQ_NUM,
  };
}

//------------------------------------------------------------------------------
// @brief   コンストラクタ・デストラクタ
//------------------------------------------------------------------------------
ImageDBSave::ImageDBSave( gfl2::heap::HeapBase* heap )
  : m_pHeap( heap )
  , m_pColorBuffer( NULL )
  , m_OnlySeq( SEQ_NONE )
  , m_SaveSeq( SEQ_NONE )
{
  m_pColorBuffer = ImageDBUtil::CreateColorBuffer( m_pHeap->GetLowerHandle() );
}

ImageDBSave::~ImageDBSave( void )
{
  GflHeapSafeFreeMemory( m_pColorBuffer );
}



//------------------------------------------------------------------------------
// @brief   更新処理
//------------------------------------------------------------------------------
void ImageDBSave::Update( void )
{
  Update_Wait();
  Update_Only();
  Update_Save();
}

void ImageDBSave::Update_Wait( void )
{
  if( m_OnlySeq == SEQ_WAIT || m_SaveSeq == SEQ_WAIT )
  {
    m_DelayTime--;

    if( m_DelayTime == 0 )
    {
      if( m_OnlySeq == SEQ_WAIT ) m_OnlySeq++;
      if( m_SaveSeq == SEQ_WAIT ) m_SaveSeq++;
    }
  }
  if( m_OnlySeq == SEQ_GPU_WAIT_1 || m_SaveSeq == SEQ_GPU_WAIT_1 )
  {
    if( m_OnlySeq == SEQ_GPU_WAIT_1 ) m_OnlySeq++;
    if( m_SaveSeq == SEQ_GPU_WAIT_1 ) m_SaveSeq++;
  }
  if( m_OnlySeq == SEQ_GPU_WAIT_2 || m_SaveSeq == SEQ_GPU_WAIT_2 )
  {
    if( m_OnlySeq == SEQ_GPU_WAIT_2 ) m_OnlySeq++;
    if( m_SaveSeq == SEQ_GPU_WAIT_2 ) m_SaveSeq++;
  }
}

void ImageDBSave::Update_Only( void )
{
  if( m_OnlySeq == SEQ_READY )
  {
    m_OnlySeq = 0;

    DebugTick::EndTick( TICK_ID_PHOTO_SHOOT );
    DebugTick::DumpTick( TICK_ID_PHOTO_SHOOT, "PhotoShoot" );
  }
}

void ImageDBSave::Update_Save( void )
{
  if( m_SaveSeq == SEQ_READY )
  {
    // SDカードへの保存
    SaveForSDCard();
    m_SaveSeq = 0;

    DebugTick::EndTick( TICK_ID_IMGDB_SAVE );
    DebugTick::DumpTick( TICK_ID_IMGDB_SAVE, "ImageDBSave" );
  }
}



//------------------------------------------------------------------------------
// @brief   SDカードへ保存
//------------------------------------------------------------------------------
void ImageDBSave::SaveForSDCard( void )
{
  ImageDBAccessor::ResultCode resCode = ImageDBAccessor::E_RET_SUCCESS;
  ImageDBAccessor::imgDbResult imgRes = imgdb::ResultSuccess;
  wchar_t filepath[ImageDBUtil::PHOTO_FILENAME_SIZE];  // @todo 仮のファイルパス格納先。本来はセーブデータのはず。

  // SDカードへの保存
  resCode = ImageDBUtil::SaveColorBuffer(
                              &imgRes,
                              m_pHeap->GetLowerHandle(),
                              m_pColorBuffer,
                              filepath );

  if( resCode == ImageDBAccessor::E_RET_SUCCESS )
  {
    GFL_PRINT( "Saved FilePath [%ls]\n", filepath );
  }
  else {
    GFL_PRINT( "SDCard Save Error!! ResultCode[%d] imgDBResult[%d]\n", resCode, imgRes );
    GFL_ASSERT( 0 );  // @todo 何かしらのエラー
  }
}



//------------------------------------------------------------------------------
// @brief   リクエスト
//------------------------------------------------------------------------------
void ImageDBSave::RequestPhotoShoot_Only( app::util::AppRenderingManager* renderMan )
{
  if( m_OnlySeq == SEQ_NONE && m_SaveSeq == SEQ_NONE )
  {
    DebugTick::StartTick( TICK_ID_PHOTO_SHOOT );

    gfl2::std::MemClear( m_pColorBuffer, ImageDBUtil::COLOR_BUFFER_SIZE );

    // カラーバッファの出力先をセット
    renderMan->RequestTransferColorBuffer_Liner( m_pColorBuffer );

    m_DelayTime = DELAY_TIME;
    m_OnlySeq++;
  }
}

void ImageDBSave::RequestPhotoShoot_Save( app::util::AppRenderingManager* renderMan )
{
  if( m_OnlySeq == SEQ_NONE && m_SaveSeq == SEQ_NONE )
  {
    DebugTick::StartTick( TICK_ID_IMGDB_SAVE );

    gfl2::std::MemClear( m_pColorBuffer, ImageDBUtil::COLOR_BUFFER_SIZE );

    // カラーバッファの出力先をセット
    renderMan->RequestTransferColorBuffer_Liner( m_pColorBuffer );

    m_DelayTime = DELAY_TIME;
    m_SaveSeq++;
  }
}


GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // defined(GF_PLATFORM_CTR)

#endif // #if PM_DEBUG
