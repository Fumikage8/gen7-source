//======================================================================
/**
 * @file    FinderStudioCaptureSDCardLoadFrame.cpp
 * @date    2016/11/09 20:23:56
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：SDカード読込みフレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureSDCardLoadFrame.h"
#include "FinderStudioCaptureFrameDefine.h"

#include <App/FinderStudioStatic/include/FinderStudioViewList.h>
#include <App/FinderStudioStatic/include/FinderStudioMessageMenuView.h>
#include <App/FinderStudioStatic/include/FinderStudioImageDBUtil.h>

#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureViewDefine.h>

#include <AppLib/include/Util/app_util_heap.h>

#include <ExtSavedata/include/FinderStudioExtSaveData.h>

#include <niji_conv_header/message/msg_fs_photo.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
SDCardLoadFrame::SDCardLoadFrame( void )
  : m_pExtSaveData( NULL )
  , m_pMessage(NULL)
  , m_ImageDBResultCode( ErrorConverter::IMGDB_RESULT_SUCCESS )
  , m_SDCardResultCode( ErrorConverter::SDCARD_RESULT_SUCCESS )
  , m_WaitFrame(0)
  , m_LoadDataIndex(0)
  , m_SavePhotoNum( Static::SAVE_PHOTO_MAX )
  , m_MsgID( MSG_ID_NULL )
  , m_bDataFileNone( false )
{
#if defined( GF_PLATFORM_CTR )
  this->SetMainSeq( SEQ_WAIT_FRAME );
#else
  this->SetMainSeq( SEQ_END );
#endif
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
applib::frame::Result SDCardLoadFrame::InitFunc( void )
{
  GFL_PRINT( "SDCardLoadFrame Start\n" );

  Static::ViewList* viewList = m_pMainWork->viewList;

  // 汎用メッセージウィンドウView
  m_pMsgMenuView = viewList->GetMessageMenuView();
  m_pMsgMenuView->SetListener( this );

#if defined(GF_PLATFORM_CTR)
  gfl2::heap::HeapBase* heap = Static::ImageDBUtil::GetImageDBHeap();//m_pMainWork->heap->GetDeviceHeap()->GetLowerHandle();
  // ファインダースタジオ用拡張セーブデータ
  m_pExtSaveData = GFL_NEW( heap ) Static::FinderStudioExtSaveDataAccessor( heap );
  // メッセージクラスの生成
  m_pMessage = GFL_NEW( heap ) ErrorConverter::Message( heap );

  // メッセージウィンドウの表示
  _ShowMesasgeWindow( MSG_ID_LOAD_BEGIN );

  m_pMainWork->work->app.save_photo_count = Static::SAVE_PHOTO_MAX;
#endif

  return applib::frame::RES_FINISH;
}

/**
 * @brief   終了
 */
applib::frame::Result SDCardLoadFrame::EndFunc( void )
{
  GFL_PRINT( "SDCardLoadFrame Finish\n" );

  m_pMsgMenuView->HideMessage();
  m_pMsgMenuView->RemoveListener();

  GFL_SAFE_DELETE( m_pMessage );
  GFL_SAFE_DELETE( m_pExtSaveData );

  return applib::frame::RES_FINISH;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新
 */
applib::frame::Result SDCardLoadFrame::UpdateFunc( void )
{
  // シーケンス更新関数のリスト
  void ( SDCardLoadFrame::*UPDATE_FUNC_TABLE[] )( void ) = {
    &SDCardLoadFrame::_UpdateWaitFrame,
    &SDCardLoadFrame::_UpdateImageDB,
    &SDCardLoadFrame::_UpdateExtSavedata,
    &SDCardLoadFrame::_UpdateCheckRegion,
  };

  switch( m_MainSeq )
  {
    case SEQ_FATAL_ERROR:   //!< フェイタルエラー
    {
      m_pMainWork->work->app.fatal_error = true;  //!< フェイタルエラー
      this->GetFrameManager()->SetResult( Frame::RES_ERROR );
      return applib::frame::RES_FINISH;
    } // no break;

    case SEQ_FORCED_END:    //!< 強制終了
    {
      this->GetFrameManager()->SetResult( Frame::RES_FINISH );
      return applib::frame::RES_FINISH;
    } // no break;

    case SEQ_END:           //!< 通常終了：次のシーケンスへ
    {
      this->GetFrameManager()->SetResult( Frame::RES_NEXT );
      return applib::frame::RES_FINISH;
    } // no break;

    default:
    {
      // シーケンスの更新処理
      ( this->*UPDATE_FUNC_TABLE[m_MainSeq] )( );
    }
  }

  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   更新：待機フレーム
 */
void SDCardLoadFrame::_UpdateWaitFrame( void )
{
  const u32 WAIT_FRAME_MAX = 15;

  m_WaitFrame++;

  if( m_WaitFrame == WAIT_FRAME_MAX )
  {
    this->SetMainSeq( SEQ_CHECK_IMGDB );
  }
}

/**
 * @brief   更新：ImageDBチェック
 */
void SDCardLoadFrame::_UpdateImageDB( void )
{
#if defined( GF_PLATFORM_CTR )
  if( m_SubSeq == 0 )
  {
    // ImageDBチェック
    _CheckImageDB( &m_ImageDBResultCode );

    // 結果に応じた分岐
    if( m_ImageDBResultCode == ErrorConverter::IMGDB_RESULT_SUCCESS )
    {
      this->SetMainSeq( SEQ_CHECK_EXTSAVEDATA );
    }
    else if( m_ImageDBResultCode == ErrorConverter::IMGDB_RESULT_FATAL_ERROR )
    {
      this->SetMainSeq( SEQ_FATAL_ERROR );
    }
    else {
      _ShowMesasgeWindow( MSG_ID_IMGDB_ERROR );
      this->SetSubSeq( 1 );
    }
  }
#else
  this->SetMainSeq( SEQ_CHECK_EXTSAVEDATA );
#endif
}

/**
 * @brief   更新：拡張セーブデータのチェック
 */
void SDCardLoadFrame::_UpdateExtSavedata( void )
{
#if defined(GF_PLATFORM_CTR)
  if( m_SubSeq == 0 )
  {
    if( _CheckExtSavedata( &m_SDCardResultCode ) )
    {
      if( m_bDataFileNone )
      {
        this->SetMainSeq( SEQ_END );
      }
      else if( m_SDCardResultCode == ErrorConverter::SDCARD_RESULT_SUCCESS )
      {
        this->SetMainSeq( SEQ_CHECK_REGION );
      }
      else if( m_SDCardResultCode == ErrorConverter::SDCARD_RESULT_FATAL_ERROR )
      {
        this->SetMainSeq( SEQ_FATAL_ERROR );
      }
      else if( m_SDCardResultCode == ErrorConverter::SDCARD_RESULT_BROKEN_DATA )
      {
        _ShowMesasgeWindow( MSG_ID_EXTSAVE_ERROR_BROKEN_DATA );
        this->SetSubSeq( 1 );
      }
      else {
        _ShowMesasgeWindow( MSG_ID_EXTSAVE_ERROR_NOT_SPACE );
        this->SetSubSeq( 1 );
      }
    }
  }
#else
  this->SetMainSeq( SEQ_CHECK_REGION );
#endif
}

/**
 * @brief   更新：撮影可能枚数のチェック
 */
void SDCardLoadFrame::_UpdateCheckRegion( void )
{
#if defined(GF_PLATFORM_CTR)
  if( m_SubSeq == 0 )
  {
    m_SavePhotoNum = Static::SAVE_PHOTO_MAX - m_pExtSaveData->GetDataCount();
    m_pMainWork->work->app.save_photo_count = m_SavePhotoNum;

    if( m_SavePhotoNum >= Static::ATTENTION_PHOTO_NUM )
    {
      this->SetMainSeq( SEQ_END );
    }
    else {
      _ShowMesasgeWindow( MSG_ID_REGION_CAUTION );
      this->SetSubSeq( 1 );
    }
  }
#else
  this->SetMainSeq( SEQ_END );
#endif
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ImageDBのチェック
 *
 * @param[out]  code    リザルトの格納先
 */
bool SDCardLoadFrame::_CheckImageDB( ErrorConverter::ImageDBResultCode* code )
{
  FUKUSHIMA_PRINT( "SDCardLoadFrame::_CheckImageDB\n" );

#if defined(GF_PLATFORM_CTR)
  gfl2::heap::HeapBase* heap = Static::ImageDBUtil::GetImageDBHeap();
  gfl2::imagedb::ImageDBAccessor::imgDbResult result = imgdb::ResultSuccess;

  // ImageDBのチェック
  Static::ImageDBUtil::CheckSDCard( &result, heap->GetLowerHandle() );

  // 独自リザルトコードへ変換
  *code = ErrorConverter::ConvertImageDBErrorFromImgDBResult( result );
#endif
  return true;
}

/**
 * @brief   拡張セーブデータのチェック
 *
 * @param[out]  code    リザルトの格納先
 */
bool SDCardLoadFrame::_CheckExtSavedata( ErrorConverter::SDCardResultCode* code )
{
  Static::FinderStudioExtSaveDataAccessor::Result result = m_pExtSaveData->ReadFile();

  if( result != Static::FinderStudioExtSaveDataAccessor::RESULT_CONTINUE )
  {
    if( result == Static::FinderStudioExtSaveDataAccessor::RESULT_SUCCESS )
    {
      *code = ErrorConverter::SDCARD_RESULT_SUCCESS;
    }
    else if( result == Static::FinderStudioExtSaveDataAccessor::RESULT_NOT_EXIST )
    {
      m_bDataFileNone = true;
      *code = ErrorConverter::SDCARD_RESULT_SUCCESS;
    }
    else if( result == Static::FinderStudioExtSaveDataAccessor::RESULT_BROKEN )
    {
      *code = ErrorConverter::SDCARD_RESULT_BROKEN_DATA;
    }
    else {
      // 独自リザルトコードへ変換
      *code = ErrorConverter::ConvertSDCardErrorFromExtSaveResult( m_pExtSaveData->GetResult() );
    }

    return true;
  }

  return false;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    メッセージウィンドウ：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   メッセージウィンドウ：表示
 *
 * @param[in]   id    メッセージID
 */
void SDCardLoadFrame::_ShowMesasgeWindow( MsgID id )
{
  const ErrorConverter::Message::MsgType type = ErrorConverter::Message::MSG_TYPE_LOAD_A;

  if( id != MSG_ID_LOAD_BEGIN )
  {
    _HideMessageWindow();
  }

  switch( id )
  {
    /// 読込み開始
    case MSG_ID_LOAD_BEGIN:
    {
      gfl2::str::StrBuf* strBuf = m_pMessage->GetBeginMessage( type );
      m_pMsgMenuView->ShowMessage_Str( strBuf, true, true, false );
    } break;

    /// ImageDBエラー
    case MSG_ID_IMGDB_ERROR:
    {
      gfl2::str::StrBuf* strBuf = m_pMessage->GetImgDBErrorMessage( type, m_ImageDBResultCode );
      m_pMsgMenuView->ShowMessage_Str( strBuf, false, false, false );
    } break;

    /// 拡張セーブデータ 破損
    case MSG_ID_EXTSAVE_ERROR_BROKEN_DATA:
    {
      gfl2::str::StrBuf* strBuf = m_pMessage->GetExtSaveErrorMessage( ErrorConverter::SDCARD_RESULT_BROKEN_DATA );
      m_pMsgMenuView->ShowMessage_Str( strBuf, false, false, false );
    } break;

    /// 拡張セーブデータ エラー
    case MSG_ID_EXTSAVE_ERROR_NOT_SPACE:
    {
      gfl2::str::StrBuf* strBuf = m_pMessage->GetExtSaveErrorMessage( m_SDCardResultCode );
      m_pMsgMenuView->ShowMessage_Str( strBuf, false, false, false );
    } break;

    /// 保存枚数
    case MSG_ID_REGION_CAUTION:
    {
      gfl2::str::StrBuf* strBuf = m_pMessage->GetRegionMessage( type, m_SavePhotoNum );
      m_pMsgMenuView->ShowMessage_Str( strBuf, false, false, false );
    } break;
  }

  m_MsgID = id;
}

/**
 * @brief   メッセージウィンドウ：非表示
 */
void SDCardLoadFrame::_HideMessageWindow( void )
{
  m_pMsgMenuView->HideMessage();
}

/**
 * @brief   メッセージウィンドウ：結果処理
 */
void SDCardLoadFrame::_ResultMessageWindow( void )
{
  switch( m_MsgID )
  {
    case MSG_ID_IMGDB_ERROR:
    case MSG_ID_EXTSAVE_ERROR_NOT_SPACE:
    {
      this->SetMainSeq( SEQ_FORCED_END );
    } break;

    case MSG_ID_EXTSAVE_ERROR_BROKEN_DATA:
    {
      this->SetMainSeq( SEQ_CHECK_REGION );
    } break;

    case MSG_ID_REGION_CAUTION:
    {
      if( m_SavePhotoNum == Static::SAVE_PHOTO_MIN )
      {
        this->SetMainSeq( SEQ_FORCED_END );
      }
      else {
        this->SetMainSeq( SEQ_END );
      }
    } break;
  }

  m_MsgID = MSG_ID_NULL;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー関数：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   MessageMenuViewListener：イベント取得
 */
void SDCardLoadFrame::OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event )
{
  if( event == App::Tool::MessageMenuView::IEventHandler::EVENT_OnEOM )
  {
    _ResultMessageWindow();
  }
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
