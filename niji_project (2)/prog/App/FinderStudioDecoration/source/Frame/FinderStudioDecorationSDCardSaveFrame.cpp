//======================================================================
/**
 * @file    FinderStudioDecorationSDCardSaveFrame.cpp
 * @date    2017/01/31 15:40:21
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：SDカード書き込みフレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationSDCardSaveFrame.h"
#include "FinderStudioDecorationFrameDefine.h"

#include <App/FinderStudioStatic/include/FinderStudioAlbumWork.h>
#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioViewList.h>
#include <App/FinderStudioStatic/include/FinderStudioViewBase.h>
#include <App/FinderStudioStatic/include/FinderStudioImageDBUtil.h>
#include <App/FinderStudioStatic/include/FinderStudioUtil.h>
#include <App/FinderStudioDecoration/source/FinderStudioDecorationDecoItemManager.h>
#include <App/FinderStudioDecoration/source/View/FinderStudioDecorationViewDefine.h>

#include <GameSys/include/NijiTime.h>
#include <ExtSavedata/include/FinderStudioExtSaveData.h>
#include <util/include/gfl2_std_string.h>

#include <niji_conv_header/message/msg_fs_deco.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
SDCardSaveFrame::SDCardSaveFrame( void )
  : m_pExtSaveData( NULL )
  , m_pImageDBUtil( NULL )
  , m_pMessage( NULL )
  , m_ImageDBResultCode( ErrorConverter::IMGDB_RESULT_SUCCESS )
  , m_SDCardResultCode( ErrorConverter::SDCARD_RESULT_SUCCESS )
  , m_SaveType( SAVE_TYPE_NUM )
  , m_SavePhotoNum( 0 )
  , m_DataIndex( 0 )
  , m_pFilePath( NULL )
  , m_bBrokenData( false )
  , m_MsgWndID( SAVE_MSGWND_ID_NUM )
  , m_FrameCount(0)
{
  gfl2::std::MemClear( &m_SavePhotoData, sizeof( Static::IntegratedData ) );
  gfl2::std::MemClear( &m_DeleteFilePath, sizeof( wchar_t ) * Static::SAVE_FILEPATH_LENGTH );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
applib::frame::Result SDCardSaveFrame::InitFunc( void )
{
  GFL_PRINT( "SDCardSaveFrame Start\n" );

  /// 汎用メッセージウィンドウView
  m_pMsgMenuView = m_pViewList->GetMessageMenuView();
  m_pMsgMenuView->SetListener( this );
  m_pMsgMenuView->ShowMessage_Id( fs_deco_win_09, true, true, false );

  gfl2::heap::HeapBase* heap = Static::ImageDBUtil::GetImageDBHeap();
  m_pExtSaveData = GFL_NEW( heap ) Static::FinderStudioExtSaveDataAccessor( heap );
  m_pImageDBUtil = GFL_NEW( heap ) Static::ImageDBUtil();
  m_pMessage = GFL_NEW( heap ) ErrorConverter::Message( heap );

  // 保存タイプの設定
  m_SaveType = m_pDecoWork->save_type;

  // 拡張セーブデータの初期化
  _InitFunc_ExtSaveData();

  return applib::frame::RES_FINISH;
}

/**
 * @brief   初期化：拡張セーブデータ
 */
void SDCardSaveFrame::_InitFunc_ExtSaveData( void )
{
  // 写真データ
  gfl2::std::MemCopy( &m_pWork->photo_data, &m_SavePhotoData, sizeof( Static::IntegratedData ) );
  // ファイルパス
  m_pFilePath = reinterpret_cast<wchar_t*>( m_SavePhotoData.management_data.filepath );
  // 編集フラグ
  m_SavePhotoData.management_data.deco_flg = 1;
  // デコデータ
  _InitFunc_DecoData();
  // crc更新
  m_SavePhotoData.photo_data.deco_param.crc =
    Static::FinderStudioUtil::GetPhotoDataCrc( &m_SavePhotoData.photo_data );

  if( m_SaveType == SAVE_TYPE_OVER_WRITE )
  {
    // 上書き保存
    _InitFunc_OverWrite();
  }
  else {
    // 新規保存
    _InitFunc_NewWrite();
  }
}

/**
 * @brief   初期化：デコデータ
 */
void SDCardSaveFrame::_InitFunc_DecoData( void )
{
  DecoItemManager* decoMan = m_pDecoWork->deco_man;
  Static::PhotoData* data = &m_SavePhotoData.photo_data;

  for( u8 i = 0; i < Static::DECO_ITEM_MAX; ++i )
  {
    data->deco_param.deco_param[i] = decoMan->GetStampData( i );

    FUKUSHIMA_PRINT( "StampData[%d]\n", i );
    FUKUSHIMA_PRINT( " frame_id=[%d]\n", data->deco_param.deco_param[i].frame_id );
    FUKUSHIMA_PRINT( " stamp_id=[%d]\n", data->deco_param.deco_param[i].id );
    FUKUSHIMA_PRINT( " stamp_x=[%d]\n", data->deco_param.deco_param[i].x );
    FUKUSHIMA_PRINT( " stamp_y=[%d]\n", data->deco_param.deco_param[i].y );
  }
}

/**
 * @brief   初期化：上書き保存
 */
void SDCardSaveFrame::_InitFunc_OverWrite( void )
{
  // ファイルインデックス
  m_DataIndex = m_pWork->photo_data_index;
  // 削除対象のファイルパス
  gfl2::std::MemCopy( m_pFilePath, m_DeleteFilePath, ( sizeof( wchar_t ) * Static::SAVE_FILEPATH_LENGTH ) );
}

/**
 * @brief   初期化：新規保存
 */
void SDCardSaveFrame::_InitFunc_NewWrite( void )
{
  // ファイルインデックス
  m_DataIndex = Static::FinderStudioExtSaveDataAccessor::SAVE_DATA_MAX;
  // 日時
  _SetDate();
  // 編集フラグ
  m_SavePhotoData.management_data.favorite_flg = 0;
  // 共有フラグ
  m_SavePhotoData.management_data.joint_flg = 0;
}

/**
 * @brief   終了
 */
applib::frame::Result SDCardSaveFrame::EndFunc( void )
{
  GFL_PRINT( "SDCardSaveFrame Finish\n" );

  GFL_DELETE( m_pMessage );
  GFL_DELETE( m_pImageDBUtil );
  GFL_DELETE( m_pExtSaveData );

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
applib::frame::Result SDCardSaveFrame::UpdateFunc( void )
{
  applib::frame::Result( SDCardSaveFrame::*UPDATE_FUNC[] )( void ) = {
    &SDCardSaveFrame::_UpdateFrameCount,        //!< SEQ_FRAME_COUNT
    &SDCardSaveFrame::_UpdateSaveImageDB,       //!< SEQ_SAVE_IMAGEDB
    &SDCardSaveFrame::_UpdateSaveExtSaveData,   //!< SEQ_SAVE_EXTSAVEDATA
    &SDCardSaveFrame::_UpdateSaveResult,        //!< SEQ_SAVE_RESULT
    &SDCardSaveFrame::_UpdateCheckRegion,       //!< SEQ_CHECK_REGION
    &SDCardSaveFrame::_UpdateErrorMessage,      //!< SEQ_ERROR_MESSAGE
  };

  switch( m_MainSeq )
  {
    case SEQ_FATAL_ERROR:   //!< フェイタルエラー
    {
      m_pWork->event_result = Static::EVE_RES_FATAL_ERROR;
      this->GetFrameManager()->SetResult( Frame::RES_ERROR );
      return applib::frame::RES_FINISH;
    } // no break;

    case SEQ_FORCED_END:    //!< 強制終了
    {
      m_pWork->event_result = Static::EVE_RES_ERROR;
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
      return ( this->*UPDATE_FUNC[m_MainSeq] )( );
    }
  }
}

/**
 * @brief   更新：フレームカウント
 */
applib::frame::Result SDCardSaveFrame::_UpdateFrameCount( void )
{
  m_FrameCount++;

  if( m_FrameCount >= 120 )
  {
    this->SetMainSeq( SEQ_SAVE_IMAGEDB );
  }

  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   更新：ImageDBチェック
 */
applib::frame::Result SDCardSaveFrame::_UpdateSaveImageDB( void )
{
  // ImageDBへの保存処理
  if( _SaveImageDB() )
  {
    // 結果に応じた分岐
    if( m_ImageDBResultCode == ErrorConverter::IMGDB_RESULT_SUCCESS )
    {
      this->SetMainSeq( SEQ_SAVE_EXTSAVEDATA );
    }
    else if( m_ImageDBResultCode == ErrorConverter::IMGDB_RESULT_FATAL_ERROR )
    {
      this->SetMainSeq( SEQ_FATAL_ERROR );
    }
    else {
      this->SetMainSeq( SEQ_ERROR_MESSAGE );
    }
  }

  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   更新：ExtSaveDataチェック
 */
applib::frame::Result SDCardSaveFrame::_UpdateSaveExtSaveData( void )
{
  // 拡張セーブデータへの保存処理
  if( _SaveExtSavedata() )
  {
    if( m_bBrokenData )
    {
      this->SetMainSeq( SEQ_ERROR_MESSAGE );
    }
    else if( m_SDCardResultCode == ErrorConverter::SDCARD_RESULT_SUCCESS )
    {
      this->SetMainSeq( SEQ_SAVE_RESULT );
    }
    else if( m_SDCardResultCode == ErrorConverter::SDCARD_RESULT_FATAL_ERROR )
    {
      this->SetMainSeq( SEQ_FATAL_ERROR );
    }
    else if( m_SDCardResultCode == ErrorConverter::SDCARD_RESULT_NOT_SPACE )
    {
      this->SetMainSeq( SEQ_ERROR_MESSAGE );
    }
  }

  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   更新：保存結果
 */
applib::frame::Result SDCardSaveFrame::_UpdateSaveResult( void )
{
  if( m_SubSeq == 0 )
  {
    _HideMsgWnd();
    _ShowMsgWnd( SAVE_MSGWND_ID_SAVE_SUCCESS );

    m_SubSeq = 1;
  }
  if( m_SubSeq == 2 )
  {
    this->SetMainSeq( SEQ_CHECK_REGION );
  }

  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   更新：残数チェック
 */
applib::frame::Result SDCardSaveFrame::_UpdateCheckRegion( void )
{
  if( m_SubSeq == 0 )
  {
    // 残り保存枚数
    m_SavePhotoNum = Static::SAVE_PHOTO_MAX - m_pExtSaveData->GetDataCount();
    this->SetSubSeq( 1 );
  }
  if( m_SubSeq == 1 )
  {
    if( m_SavePhotoNum >= Static::ATTENTION_PHOTO_NUM )
    {
      this->SetMainSeq( SEQ_END );
    }
    else {
      this->SetMainSeq( SEQ_ERROR_MESSAGE );
    }
  }

  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   更新：エラーメッセージ
 */
applib::frame::Result SDCardSaveFrame::_UpdateErrorMessage( void )
{
  if( m_SubSeq == 0 )
  {
    _HideMsgWnd();
    _ShowErrorMessage();
    this->SetSubSeq( 1 );
  }
  if( m_SubSeq == 2 )
  {
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    保存：非公開
///
//////////////////////////////////////////////////////////////////////////
/** 
 * @brief   保存：ImageDB
 */
bool SDCardSaveFrame::_SaveImageDB( void )
{
  if( m_SaveType == SAVE_TYPE_OVER_WRITE )
  {
    // 上書き保存
    return _SaveImageDB_OverWrite();
  }
  else {
    // 新規保存
    return _SaveImageDB_NewWrite();
  }
}

/**
 * @brief   保存：ImageDB上書き
 */
bool SDCardSaveFrame::_SaveImageDB_OverWrite( void )
{
  enum {
    OW_SEQ_BEGIN,
    OW_SEQ_SAVE_START,
    OW_SEQ_SAVE_WAIT,
    OW_SEQ_SAVE_END,
    OW_SEQ_DELETE_START,
    OW_SEQ_DELETE_END,
    OW_SEQ_END,
  };

  if( m_SubSeq == OW_SEQ_BEGIN )
  {
    FUKUSHIMA_PRINT( "ImageDB OverWrite Start\n" );

    this->SetSubSeq( OW_SEQ_SAVE_START );
  }
  if( m_SubSeq == OW_SEQ_SAVE_START )
  {
    gfl2::heap::HeapBase* heap = Static::ImageDBUtil::GetImageDBHeap();

    // 保存開始
    m_pImageDBUtil->StartSaveImage( heap, m_pFilePath, m_pDecoWork->photo_data );

    this->SetSubSeq( OW_SEQ_SAVE_WAIT );
  }
  if( m_SubSeq == OW_SEQ_SAVE_WAIT )
  {
    /// 保存処理
    if( m_pImageDBUtil->IsFinishSaveImage() )
    {
      /// リザルトの取得
      m_ImageDBResultCode = m_pImageDBUtil->GetResult();

      this->SetSubSeq( OW_SEQ_SAVE_END );
    }
  }
  if( m_SubSeq == OW_SEQ_SAVE_END )
  {
    /// 失敗
    if( m_ImageDBResultCode != ErrorConverter::IMGDB_RESULT_SUCCESS )
    {
      GFL_PRINT( "ImageDB Write Failed!! : result=[%d]\n", m_ImageDBResultCode );
      return true;
    }
    /// 成功
    else {
      this->SetSubSeq( OW_SEQ_DELETE_START );
    }
  }
  if( m_SubSeq == OW_SEQ_DELETE_START )
  {
    gfl2::heap::HeapBase* heap = Static::ImageDBUtil::GetImageDBHeap();

    // 写真を削除
    m_pImageDBUtil->DeleteImage( heap, m_DeleteFilePath );
    // リザルトの取得
    m_ImageDBResultCode = m_pImageDBUtil->GetResult();

    this->SetSubSeq( OW_SEQ_DELETE_END );
  }
  if( m_SubSeq == OW_SEQ_DELETE_END )
  {
    if( m_ImageDBResultCode != ErrorConverter::IMGDB_RESULT_SUCCESS )
    {
      GFL_PRINT( "ImageDB Delete Failed!! : result=[%d]\n", m_ImageDBResultCode );
      return true;
    }
    else {
      this->SetSubSeq( OW_SEQ_END );
    }
  }
  if( m_SubSeq == OW_SEQ_END )
  {
    FUKUSHIMA_PRINT( "ImageDB OverWrite End\n" );
    return true;
  }

  return false;
}

/**
 * @brief   保存：ImageDB新規
 */
bool SDCardSaveFrame::_SaveImageDB_NewWrite( void )
{
  enum {
    NEW_SAVE_SEQ_START,
    NEW_SAVE_SEQ_WAIT,
    NEW_SAVE_SEQ_END,
  };

  if( m_SubSeq == NEW_SAVE_SEQ_START )
  {
    FUKUSHIMA_PRINT( "ImageDB NewWrite Start\n" );

    gfl2::heap::HeapBase* heap = Static::ImageDBUtil::GetImageDBHeap();

    // 保存開始
    m_pImageDBUtil->StartSaveImage( heap, m_pFilePath, m_pDecoWork->photo_data );

    m_SubSeq = NEW_SAVE_SEQ_WAIT;
  }
  if( m_SubSeq == NEW_SAVE_SEQ_WAIT )
  {
    if( m_pImageDBUtil->IsFinishSaveImage() )
    {
      m_ImageDBResultCode = m_pImageDBUtil->GetResult();
      m_SubSeq = NEW_SAVE_SEQ_END;
    }
  }
  if( m_SubSeq == NEW_SAVE_SEQ_END )
  {
    FUKUSHIMA_PRINT( "ImageDB NewWrite End\n" );
    return true;
  }

  return false;
}

/**
 * @brief   保存：ExtSaveData
 */
bool SDCardSaveFrame::_SaveExtSavedata( void )
{
  enum {
    EXT_SEQ_SAVE,
    EXT_SEQ_END,
  };

  if( m_SubSeq == EXT_SEQ_SAVE )
  {
    Static::FinderStudioExtSaveDataAccessor::Result result = m_pExtSaveData->WriteFile( &m_SavePhotoData, m_DataIndex );

    if( result != Static::FinderStudioExtSaveDataAccessor::RESULT_CONTINUE )
    {
      FUKUSHIMA_PRINT( "FinderStudioExtSaveData::Result=[%d]\n", result );

      // 独自リザルトコードへ変換
      m_SDCardResultCode = ErrorConverter::ConvertSDCardErrorFromExtSaveResult( m_pExtSaveData->GetResult() );

      if( result == Static::FinderStudioExtSaveDataAccessor::RESULT_BROKEN )
      {
        m_bBrokenData = true;
      }

      return true;
    }
  }

  return false;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    保存データ関連：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   日時を設定
 */
void SDCardSaveFrame::_SetDate( void )
{
  // 現在時間
  GameSys::DeviceDate deviceDate;
  gfl2::system::Date* systemDate = deviceDate.GetDate();
  gfl2::system::Date::GetNow( systemDate );

  Static::Date* date = &m_SavePhotoData.management_data.date;
  {
    date->year   = static_cast<u16>( systemDate->GetYear() );
    date->month  = static_cast<u8>( systemDate->GetMonth() );
    date->day    = static_cast<u8>( systemDate->GetDay() );
    date->hour   = static_cast<u8>( systemDate->GetHour() );
    date->minute = static_cast<u8>( systemDate->GetMinute() );
    date->second = static_cast<u8>( systemDate->GetSecond() );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    メッセージウィンドウ：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   メッセージウィンドウを表示
 *
 * @param   id    表示するメッセージウィンドウID
 */
void SDCardSaveFrame::_ShowMsgWnd( SaveMsgWnd id )
{
  if( id == SAVE_MSGWND_ID_SAVE_SUCCESS )
  {
    // メッセージウィンドウを表示
    gfl2::str::StrBuf* strBuf = m_pMessage->GetFinishMessage( ErrorConverter::Message::MSG_TYPE_SAVE_B );
    m_pMsgMenuView->ShowMessage_Str( strBuf, false, false, false );
  }
  else if( id == SAVE_MSGWND_ID_IMDDB_ERROR )
  {
    gfl2::str::StrBuf* strBuf = m_pMessage->GetImgDBErrorMessage( ErrorConverter::Message::MSG_TYPE_SAVE_B, m_ImageDBResultCode );
    m_pMsgMenuView->ShowMessage_Str( strBuf, false, false, false );
  }
  else if( id == SAVE_MSGWND_ID_EXTSAVE_BROKEN )
  {
    gfl2::str::StrBuf* strBuf = m_pMessage->GetExtSaveErrorMessage( ErrorConverter::SDCARD_RESULT_BROKEN_DATA );
    m_pMsgMenuView->ShowMessage_Str( strBuf, false, false, false );
  }
  else if( id == SAVE_MSGWND_ID_EXTSAVE_ERROR )
  {
    gfl2::str::StrBuf* strBuf = m_pMessage->GetExtSaveErrorMessage( m_SDCardResultCode );
    m_pMsgMenuView->ShowMessage_Str( strBuf, false, false, false );
  }
  else if( id == SAVE_MSGWND_ID_REGION )
  {
    gfl2::str::StrBuf* strBuf = m_pMessage->GetRegionMessage( ErrorConverter::Message::MSG_TYPE_SAVE_B, m_SavePhotoNum );
    m_pMsgMenuView->ShowMessage_Str( strBuf, false, false, false );
  }

  m_MsgWndID = id;
}

/**
 * @brief   メッセージウィンドウを閉じる
 */
void SDCardSaveFrame::_HideMsgWnd( void )
{
  m_pMsgMenuView->HideMessage();
}


/**
 * @brief   リザルトコードに対応したエラーメッセージを表示
 */
void SDCardSaveFrame::_ShowErrorMessage( void )
{
  // ImageDBエラー
  if( m_ImageDBResultCode != ErrorConverter::IMGDB_RESULT_SUCCESS )
  {
    _ShowMsgWnd( SAVE_MSGWND_ID_IMDDB_ERROR );
  }
  // 拡張セーブデータ 破損
  else if( m_bBrokenData )
  {
    _ShowMsgWnd( SAVE_MSGWND_ID_EXTSAVE_BROKEN );
  }
  // 拡張セーブデータ エラー
  else if( m_SDCardResultCode != ErrorConverter::SDCARD_RESULT_SUCCESS )
  {
    _ShowMsgWnd( SAVE_MSGWND_ID_EXTSAVE_ERROR );
  }
  // 保存可能枚数
  else if( m_SavePhotoNum < Static::ATTENTION_PHOTO_NUM )
  {
    _ShowMsgWnd( SAVE_MSGWND_ID_REGION );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー関数：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   MessageMenuViewListener：イベント取得
 */
void SDCardSaveFrame::OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event )
{
  if( event == App::Tool::MessageMenuView::IEventHandler::EVENT_OnEOM )
  {
    if( m_MsgWndID == SAVE_MSGWND_ID_SAVE_SUCCESS )
    {
      this->SetSubSeq( 2 );
    }
    else if( m_MsgWndID == SAVE_MSGWND_ID_IMDDB_ERROR )
    {
      // 強制終了へ
      this->SetMainSeq( SEQ_FORCED_END );
    }
    else if( m_MsgWndID == SAVE_MSGWND_ID_EXTSAVE_BROKEN )
    {
      this->SetMainSeq( SEQ_CHECK_REGION ); //!< 残数チェックへ
    }
    else if( m_MsgWndID == SAVE_MSGWND_ID_EXTSAVE_ERROR )
    {
      // 強制終了へ
      this->SetMainSeq( SEQ_FORCED_END );
    }
    else if( m_MsgWndID == SAVE_MSGWND_ID_REGION )
    {
      if( m_SavePhotoNum == 0 )
      {
        // 強制終了へ
        this->SetMainSeq( SEQ_FORCED_END );
      }
      else {
        this->SetMainSeq( SEQ_END );
      }
    }

    m_MsgWndID = SAVE_MSGWND_ID_NUM;
  }
}

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
