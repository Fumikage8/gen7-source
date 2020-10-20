//======================================================================
/**
 * @file    FinderStudioCaptureSDCardSaveFrame.cpp
 * @date    2016/11/28 17:27:36
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：SDカード書き込みフレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureSDCardSaveFrame.h"
#include "FinderStudioCaptureFrameDefine.h"

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioViewList.h>
#include <App/FinderStudioStatic/include/FinderStudioMessageMenuView.h>
#include <App/FinderStudioStatic/include/FinderStudioImageDBUtil.h>

#include <App/FinderStudioCapture/source/View/FinderStudioCaptureViewDefine.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureUpperView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCaptureSelectButtonView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCapturePhotoListView.h>
#include <App/FinderStudioCapture/source/View/FinderStudioCapturePrepareCommonView.h>

#include <App/AppEvent/include/FieldMenu/FieldMenuGameData.h>
#include <ExtSavedata/include/FinderStudioExtSaveData.h>
#include <GameSys/include/NijiTime.h>
#include <GameSys/include/GameData.h>
#include <PokeTool/include/NakayoshiManager.h>
#include <Savedata/include/MyStatus.h>
#include <Savedata/include/Record.h>
#include <System/include/gflUse.h>
#include <System/include/HomeSleepAndPowerSystem.h>

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
SDCardSaveFrame::SDCardSaveFrame( void )
  : m_pUpperView( NULL )
  , m_pPhotoListView( NULL )
  , m_pSelectButtonView( NULL )
  , m_pPrepareCommonView( NULL )
  , m_SavePhotoNum( 0 )
  , m_SaveRegionNum( 0 )
  , m_SaveCount( 0 )
  , m_pImageDBUtil( NULL )
  , m_ImageDBResultCode( ErrorConverter::IMGDB_RESULT_NUM )
  , m_pExtSaveData( NULL )
  , m_ExtSaveResultCode( ErrorConverter::SDCARD_RESULT_NUM )
  , m_bBrokenData( false )
  , m_pMessage( NULL )
  , m_MsgWndID( MSGWND_ID_NULL )
  , m_InitSeq( INIT_SEQ_BEGIN )
  , m_CurtainSeq( CURTAIN_SEQ_BEGIN )
  , m_ImageDBSeq( IMAGEDB_SEQ_BEGIN )
  , m_ExtSaveSeq( EXTSAVE_SEQ_BEGIN )
  , m_SaveFinSeq( SAVE_FIN_SEQ_BEGIN )
  , m_RegionSeq( REGION_SEQ_BEGIN )
  , m_EvaluationSeq( EVAL_SEQ_BEGIN )
{
  for( u32 i = 0; i < PHOTO_MAX; ++i )
  {
    m_pPhotoData[i] = NULL;
    gfl2::std::MemClear( &m_PhotoParam[i], sizeof( Viewer::PhotoParam ) );
  }
  
  gfl2::std::MemClear( &m_IntegratedData, sizeof( Static::IntegratedData ) );
  gfl2::std::MemClear( &m_ManagementData, sizeof( Static::ManagementData ) );
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

  Static::ViewList* viewList = m_pMainWork->viewList;
  {
    /// 上画面View
    m_pUpperView = reinterpret_cast<UpperView*>( viewList->GetView( VIEW_ID_UPPER ) );
    /// 写真選択View
    m_pPhotoListView = reinterpret_cast<PhotoListView*>( viewList->GetView( VIEW_ID_PHOTO_LIST ) );
    /// 選択ボタンView
    m_pSelectButtonView = reinterpret_cast<SelectButtonView*>( viewList->GetView( VIEW_ID_SELECT_BUTTON ) );
    /// 共通View：１行メッセージ用
    m_pPrepareCommonView = reinterpret_cast<PrepareCommonView*>( viewList->GetView( VIEW_ID_PREPARE_COMMON ) );
    /// 汎用メッセージウィンドウView
    m_pMsgMenuView = viewList->GetMessageMenuView();
    m_pMsgMenuView->SetListener( this );
  }

  gfl2::heap::HeapBase* heap = m_pMainWork->heap->GetDeviceHeap();
  {
    // ファインダースタジオ用拡張セーブデータ
    m_pExtSaveData = GFL_NEW( heap ) Static::FinderStudioExtSaveDataAccessor( heap );
    // ImageDBUtilクラスの生成
    m_pImageDBUtil = GFL_NEW( heap ) Static::ImageDBUtil();
    // メッセージクラスの生成
    m_pMessage = GFL_NEW( heap ) ErrorConverter::Message( heap );
  }

#if defined(GF_PLATFORM_CTR)
  // 保存する写真があるかどうか
  if( _IsSavePhotoDataExist() )
  {
    // ホーム／スリープ禁止
    _SetDisableHomeSleep();
    // ある
    this->SetMainSeq( MAIN_SEQ_SAVE );
  }
  else {
    // ない
    this->SetMainSeq( MAIN_SEQ_CONFIRM );
  }
#else
  this->SetMainSeq( MAIN_SEQ_CONFIRM );
#endif

  return applib::frame::RES_FINISH;
}

/**
 * @brief   終了
 */
applib::frame::Result SDCardSaveFrame::EndFunc( void )
{
  GFL_PRINT( "SDCardSaveFrame Finish\n" );

  /// ホームスリープが禁止されているか？
  if( !_IsEnableHomeSleep() )
  {
    /// ホーム／スリープ禁止を解除
    _SetEnableHomeSleep();
  }

  // リスナーを解除
  m_pMsgMenuView->RemoveListener();

  GFL_SAFE_DELETE( m_pMessage );
  GFL_SAFE_DELETE( m_pImageDBUtil );
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
applib::frame::Result SDCardSaveFrame::UpdateFunc( void )
{
  switch( m_MainSeq )
  {
    /// セーブシーケンス
    case MAIN_SEQ_SAVE:
    {
      _UpdateSave();
    } break;

    /// 継続選択
    case MAIN_SEQ_CONFIRM:
    {
      _UpdateConfirm();
    } break;

    /// カーテンオープン
    case MAIN_SEQ_CURTAIN_OPEN:
    {
      _UpdateCurtainOpen();
    } break;

    /// フェイタルエラー
    case MAIN_SEQ_FATAL_ERROR:
    {
      m_pMainWork->work->app.fatal_error = true;
      this->GetFrameManager()->SetResult( Frame::RES_FINISH );
      return applib::frame::RES_FINISH;
    } // no break;

    /// 強制終了
    case MAIN_SEQ_FORCED_END:
    {
      this->GetFrameManager()->SetResult( Frame::RES_FINISH );
      return applib::frame::RES_FINISH;
    } // no break;

    /// 終了
    case MAIN_SEQ_END:
    {
      return applib::frame::RES_FINISH;
    } // no break;
  }


  return applib::frame::RES_CONTINUE;
}

/**
 * @brief   更新：セーブシーケンス
 */
void SDCardSaveFrame::_UpdateSave( void )
{
  switch( m_SubSeq )
  {
    case SAVE_SEQ_INIT:
    {
      // 保存処理の初期化
      _SaveInit();

      this->SetSubSeq( SAVE_SEQ_CURTAIN_OPEN );
    } break;

    case SAVE_SEQ_CURTAIN_OPEN:
    {
      if( _CurtainOpen() )
      {
        this->SetSubSeq( SAVE_SEQ_IMAGEDB );
      }
    } // no break;

    case SAVE_SEQ_IMAGEDB:
    {
      // ImageDBへの保存処理
      _SaveImageDB();
    } break;

    case SAVE_SEQ_EXTSAVE:
    {
      // 拡張セーブデータの保存処理
      _SaveExtSave();
    } break;

    case SAVE_SEQ_FINISH:
    {
      // 保存処理の終了
      _SaveFinish();
    } break;

    case SAVE_SEQ_REGION:
    {
      // 残数チェック
      _SaveRegion();
    } break;

    case SAVE_SEQ_EVALUATION:
    {
      // 評価
      _SaveEvaluation();
    } break;

    case SAVE_SEQ_CURTAIN_CLOSE:
    {
      // カーテンを閉じる
      if( _CurtainClose() )
      {
        this->SetMainSeq( MAIN_SEQ_CONFIRM );
      }
    } break;
  }
}

/**
 * @brief   更新：継続選択シーケンス
 */
void SDCardSaveFrame::_UpdateConfirm( void )
{
  if( m_SubSeq == CONFIRM_SEQ_SHOW )
  {
    // 上画面の写真を非表示
    m_pUpperView->InvisiblePhotoImage();
    // Viewの表示
    _FinishConfirm_ShowView();
    m_SubSeq = CONFIRM_SEQ_MAIN;
  }
  if( m_SubSeq == CONFIRM_SEQ_MAIN )
  {
    // Viewの実行
    _FinishConfirm_Proc();
  }
}

/**
 * @brief   更新：カーテンを開く
 */
void SDCardSaveFrame::_UpdateCurtainOpen( void )
{
  if( m_SubSeq == 0 )
  {
    // Viewerの描画を開始
    m_pListener->ShowViewer();

    // シーケンスを初期化
    _InitCurtainSequence();
    this->SetSubSeq( 1 );
  }
  if( m_SubSeq == 1 )
  {
    if( _CurtainOpen() )
    {
      this->SetSubSeq( 2 );
    }
  }
  if( m_SubSeq == 2 )
  {
    this->SetMainSeq( MAIN_SEQ_END );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    保存処理の初期化：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   保存処理の初期化
 */
void SDCardSaveFrame::_SaveInit( void )
{
  /// 写真データ
  _SaveInit_PhotoData();
  /// 絞込み
  _SaveInit_SortData();
  /// 日時
  _SaveInit_Date();
  /// レコード
  _SaveInit_Record();
  /// 上画面表示
  _SaveInit_UpperView();
  /// メッセージウィンドウの表示
  _SaveInit_MsgWnd();

  /// ImageDB保存シーケンスの初期化
  _InitImageDBSequence();
  /// 拡張セーブデータ保存シーケンスの初期化
  _InitExtSaveSequence();
  /// カーテン演出の初期化
  _InitCurtainSequence();
}

/**
 * @brief   保存処理の初期化：写真データ
 */
void SDCardSaveFrame::_SaveInit_PhotoData( void )
{
#if defined(GF_PLATFORM_CTR)
  ViewerWork work = _GetViewerWork();

  for( u32 i = 0; i < PHOTO_MAX; ++i )
  {
    PhotoListView::PhotoState state = m_pPhotoListView->GetPhotoState( i );

    // 保存対象の写真をチェック
    if( state == PhotoListView::PHOTO_STATE_SELECT )
    {
      m_pPhotoData[m_SavePhotoNum] = work.photo_tex[i];
      m_PhotoParam[m_SavePhotoNum] = work.photo_param[i];
      m_SavePhotoNum++;
    }
  }
#endif
}

/**
 * @brief   保存処理の初期化：絞込みデータ
 */
void SDCardSaveFrame::_SaveInit_SortData( void )
{
  ViewerWork work = _GetViewerWork();

  // 種族No
  m_ManagementData.mons_no = work.simple_param.monsNo;
  // 編集フラグ
  m_ManagementData.deco_flg = 0;
  // お気に入りフラグ
  m_ManagementData.favorite_flg = 0;
  // 撮影モード
  m_ManagementData.cap_mode = work.capture_mode;
  // 背景インデックス
  m_ManagementData.bg_index = work.background_id;
  // 共有フラグ
  m_ManagementData.joint_flg = 0;
}

/**
 * @brief   保存処理の初期化：日時設定
 */
void SDCardSaveFrame::_SaveInit_Date( void )
{
  // 現在時間
  GameSys::DeviceDate deviceDate;
  gfl2::system::Date* systemDate = deviceDate.GetDate();
  gfl2::system::Date::GetNow( systemDate );

  Static::Date* date = &m_ManagementData.date;
  {
    date->year   = static_cast<u16>( systemDate->GetYear() );
    date->month  = static_cast<u8>( systemDate->GetMonth() );
    date->day    = static_cast<u8>( systemDate->GetDay() );
    date->hour   = static_cast<u8>( systemDate->GetHour() );
    date->minute = static_cast<u8>( systemDate->GetMinute() );
    date->second = static_cast<u8>( systemDate->GetSecond() );
  }

  FUKUSHIMA_PRINT( "year=[%d] month=[%d] day=[%d]\n",
    static_cast<u16>( systemDate->GetYear() ),
    static_cast<u8>( systemDate->GetMonth() ),
    static_cast<u8>( systemDate->GetDay() ) );

  FUKUSHIMA_PRINT( "year=[%d] month=[%d] day=[%d]\n",
    date->year, date->month, date->day );
}

/**
 * @brief   保存処理の初期化：レコード
 */
void SDCardSaveFrame::_SaveInit_Record( void )
{
  // レコードの更新：ファインダースタジオで保存した回数
  Savedata::IncRecord( Savedata::Record::RECID_FINDERSTUDIO_FILM_CNT );
  // ロトムのお題
  FieldMenu::PlayData::SetRotomRequestAchieved( Savedata::FieldMenu::ROTOM_REQ_FINDER_STUDIO );
  // システムフラグを更新
  {
    GameSys::GameData* gameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
    Field::EventWork* eventWork = gameData->GetEventWork();
    eventWork->SetEventFlag( SYS_FLAG_FS_PHOTO_SAVEIN );
  }
}

/**
 * @brief   保存処理の初期化：上画面表示
 */
void SDCardSaveFrame::_SaveInit_UpperView( void )
{
  // 先頭インデックスのテクスチャを取得
  void* texture = m_pPhotoData[0];
  // 上画面に写真を表示
  m_pUpperView->RequestPhotoImage( texture );
}

/**
 * @brief   保存処理の初期化：メッセージウィンドウの表示
 */
void SDCardSaveFrame::_SaveInit_MsgWnd( void )
{
  // メッセージウィンドウを表示
  _ShowMessageWindow( MSGWND_ID_SAVE_START );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    保存処理 ImageDB：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   保存処理 ImageDB
 */
void SDCardSaveFrame::_SaveImageDB( void )
{
  // ImageDBへの保存処理
  if( _SaveImageDB_Proc() )
  {
    // 結果に応じた分岐
    if( m_ImageDBResultCode == ErrorConverter::IMGDB_RESULT_SUCCESS )
    {
      // 拡張セーブデータの保存へ
      this->SetSubSeq( SAVE_SEQ_EXTSAVE );
    }
    else if( m_ImageDBResultCode == ErrorConverter::IMGDB_RESULT_FATAL_ERROR )
    {
      // フェイタルエラー
      this->SetMainSeq( MAIN_SEQ_FATAL_ERROR );
    }
    else {
      // メッセージウィンドウを表示
      _SaveImageDB_MsgWnd();
      this->SetSubSeq( SAVE_SEQ_IMAGEDB_ERROR );
    }
  }
}

/**
 * @brief   保存処理 ImageDB：実行部分
 */
bool SDCardSaveFrame::_SaveImageDB_Proc( void )
{
  if( m_ImageDBSeq == IMAGEDB_SEQ_START )
  {
    gfl2::heap::HeapBase* heap = Static::ImageDBUtil::GetImageDBHeap();
    wchar_t* filepath = reinterpret_cast<wchar_t*>( m_ManagementData.filepath );

    // 保存開始
    m_pImageDBUtil->StartSaveImage( heap, filepath, m_pPhotoData[m_SaveCount] );

    m_ImageDBSeq = IMAGEDB_SEQ_WAIT;
  }
  if( m_ImageDBSeq == IMAGEDB_SEQ_WAIT )
  {
    // 保存完了まで待機
    if( m_pImageDBUtil->IsFinishSaveImage() )
    {
      // 結果を取得
      m_ImageDBResultCode = m_pImageDBUtil->GetResult();
      m_ImageDBSeq = IMAGEDB_SEQ_END;
    }
  }
  if( m_ImageDBSeq == IMAGEDB_SEQ_END )
  {
    return true;
  }

  return false;
}

/**
 * @brief   保存処理 ImageDB：メッセージウィンドウ
 */
void SDCardSaveFrame::_SaveImageDB_MsgWnd( void )
{
  // メッセージウィンドウを閉じる
  _HideMessageWindow();

  switch( m_ImageDBResultCode )
  {
    case ErrorConverter::IMGDB_RESULT_NOT_SPACE:
    {
      _ShowMessageWindow( MSGWND_ID_IMGDB_ERROR_STORAGE );
    } break;

    case ErrorConverter::IMGDB_RESULT_OVER_IMAGE:
    {
      _ShowMessageWindow( MSGWND_ID_IMGDB_ERROR_OVER_NUM );
    } break;

    case ErrorConverter::IMGDB_RESULT_OTHER_ERROR:
    {
      _ShowMessageWindow( MSGWND_ID_IMGDB_ERROR_OTHER );
    } break;
  }
}

/**
 * @brief   保存処理 ImageDB：メッセージウィンドウイベント
 */
void SDCardSaveFrame::_SaveImageDB_MsgWndEvent( MsgWndID id )
{
  this->SetMainSeq( MAIN_SEQ_FORCED_END );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    保存処理 ExtSave：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   保存処理 ExtSave
 */
void SDCardSaveFrame::_SaveExtSave( void )
{
  // ExtSaveへの保存処理
  if( _SaveExtSave_Proc() )
  {
    // 結果に応じた分岐
    if( m_ExtSaveResultCode == ErrorConverter::SDCARD_RESULT_SUCCESS )
    {
      // 保存終了へ
      this->SetSubSeq( SAVE_SEQ_FINISH );
    }
    else if( m_ExtSaveResultCode == ErrorConverter::SDCARD_RESULT_FATAL_ERROR )
    {
      // フェイタルエラー
      this->SetMainSeq( MAIN_SEQ_FATAL_ERROR );
    }
    else {
      // メッセージウィンドウを表示
      _SaveExtSave_MsgWnd();
      this->SetSubSeq( SAVE_SEQ_EXTSAVE_ERROR );
    }
  }
}

/**
 * @brief   保存処理 ExtSave：実行
 */
bool SDCardSaveFrame::_SaveExtSave_Proc( void )
{
  if( m_ExtSaveSeq == EXTSAVE_SEQ_INIT )
  {
    gfl2::std::MemClear( &m_IntegratedData, sizeof(Static::IntegratedData) );
    m_IntegratedData.management_data = m_ManagementData;
    m_IntegratedData.photo_data.photo_param = m_PhotoParam[m_SaveCount];

    for( u8 i = 0; i < Static::DECO_ITEM_MAX; ++i )
    {
      m_IntegratedData.photo_data.deco_param.deco_param[i] = Static::STAMP_NULL_DATA;
    }

    m_ExtSaveSeq = EXTSAVE_SEQ_WAIT;
  }
  if( m_ExtSaveSeq == EXTSAVE_SEQ_WAIT )
  {
    Static::FinderStudioExtSaveDataAccessor::Result result = m_pExtSaveData->WriteFile( &m_IntegratedData );

    if( result != Static::FinderStudioExtSaveDataAccessor::RESULT_CONTINUE )
    {
      FUKUSHIMA_PRINT( "FinderStudioExtSaveData::Result=[%d]\n", result );

      // 独自リザルトコードへ変換
      m_ExtSaveResultCode = ErrorConverter::ConvertSDCardErrorFromExtSaveResult( m_pExtSaveData->GetResult() );

      return true;
    }
  }

  return false;
}

/**
 * @brief   保存処理 ExtSave：メッセージウィンドウ
 */
void SDCardSaveFrame::_SaveExtSave_MsgWnd( void )
{
  // メッセージウィンドウを閉じる
  _HideMessageWindow();

  switch( m_ExtSaveResultCode )
  {
    case ErrorConverter::SDCARD_RESULT_BROKEN_DATA:
    {
      _ShowMessageWindow( MSGWND_ID_EXTSAVE_ERROR_BROKEN );
    } break;

    case ErrorConverter::SDCARD_RESULT_NOT_SPACE:
    {
      _ShowMessageWindow( MSGWND_ID_EXTSAVE_ERROR_STORAGE );
    } break;
  }
}

/**
 * @brief   保存処理 ExtSave：メッセージウィンドウイベント
 */
void SDCardSaveFrame::_SaveExtSave_MsgWndEvent( MsgWndID id )
{
  if( id == MSGWND_ID_EXTSAVE_ERROR_BROKEN )
  {
    this->SetSubSeq( SAVE_SEQ_EVALUATION );
  }
  else {
    this->SetMainSeq( MAIN_SEQ_FORCED_END );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    保存処理 終了：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   保存処理 終了
 */
void SDCardSaveFrame::_SaveFinish( void )
{
  if( m_SaveFinSeq == SAVE_FIN_SEQ_CHECK )
  {
    // 保存が全て完了したかどうかをチェック
    if( _SaveFinish_Check() )
    {
      // ホーム／スリープ禁止解除
      _SetEnableHomeSleep();

      m_SaveFinSeq = SAVE_FIN_SEQ_MSGWND;
    }
    else {
      // 全ての保存が完了していないのでImageDBの保存へ
      _InitImageDBSequence();
      _InitExtSaveSequence();
      this->SetSubSeq( SAVE_SEQ_IMAGEDB );
    }
  }
  if( m_SaveFinSeq == SAVE_FIN_SEQ_MSGWND )
  {
    // 完了した場合は保存完了メッセージを表示
    _SaveFinish_MsgWnd();
    m_SaveFinSeq = SAVE_FIN_SEQ_MSGWND_WAIT;
  }
}

/**
 * @brief   保存処理 終了：チェック
 */
bool SDCardSaveFrame::_SaveFinish_Check( void )
{
  m_SaveCount++;

  // 保存枚数が保存対象枚数に達したか
  if( m_SaveCount >= m_SavePhotoNum )
  {
    return true;
  }
  else {
    return false;
  }
}

/**
 * @brief   保存処理 終了：メッセージウィンドウ
 */
void SDCardSaveFrame::_SaveFinish_MsgWnd( void )
{
  // メッセージウィンドウを閉じる
  _HideMessageWindow();
  _ShowMessageWindow( MSGWND_ID_SAVE_SUCCESS );
}

/**
 * @brief   保存処理 終了：メッセージウィンドウイベント
 */
void SDCardSaveFrame::_SaveFinish_MsgWndEvent( MsgWndID id )
{
  if( id == MSGWND_ID_SAVE_SUCCESS )
  {
    this->SetSubSeq( SAVE_SEQ_EVALUATION );
  }
  else {
    GFL_ASSERT(0);
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    残数チェック：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   残数チェック
 */
void SDCardSaveFrame::_SaveRegion( void )
{
  if( m_RegionSeq == REGION_SEQ_CHECK )
  {
    // 残数チェック
    if( _SaveRegion_Check() )
    {
      // メッセージウィンドウを閉じる
      _HideMessageWindow();

      // カーテンを閉じる
      this->SetSubSeq( SAVE_SEQ_CURTAIN_CLOSE );
    }
    else {
      // １０枚未満
      m_RegionSeq = REGION_SEQ_MSGWND;
    }
  }
  if( m_RegionSeq == REGION_SEQ_MSGWND )
  {
    // 注意喚起メッセージの表示
    _SaveRegion_MsgWnd();
    m_RegionSeq = REGION_SEQ_MSGWND_WAIT;
  }
}

/**
 * @brief   残数チェック：チェック
 */
bool SDCardSaveFrame::_SaveRegion_Check( void )
{
  // 管理領域残数チェック
  m_SaveRegionNum = Static::SAVE_PHOTO_MAX - m_pExtSaveData->GetDataCount();
  // @fix MMCat[510]：保存枚数を更新
  m_pMainWork->work->app.save_photo_count = m_SaveRegionNum;

  // 残数が安全圏内
  if( m_SaveRegionNum >= Static::ATTENTION_PHOTO_NUM )
  {
    return true;
  }
  else {
    return false;
  }
}

/**
 * @brief   残数チェック：メッセージウィンドウ
 */
void SDCardSaveFrame::_SaveRegion_MsgWnd( void )
{
  // メッセージウィンドウを閉じる
  _HideMessageWindow();

  if( m_SaveRegionNum == 0 )
  {
    // 残数０
    _ShowMessageWindow( MSGWND_ID_REGION_FULL );
  }
  else {
    // 残数１０未満
    _ShowMessageWindow( MSGWND_ID_REGION_CAUTION );
  }
}

/**
 * @brief   残数チェック：メッセージウィンドウイベント
 */
void SDCardSaveFrame::_SaveRegion_MsgWndEvent( MsgWndID id )
{
  if( id == MSGWND_ID_REGION_CAUTION )
  {
    // メッセージウィンドウを閉じる
    _HideMessageWindow();

    this->SetSubSeq( SAVE_SEQ_CURTAIN_CLOSE );
  }
  else if( id == MSGWND_ID_REGION_FULL )
  {
    this->SetMainSeq( MAIN_SEQ_FORCED_END );
  }
  else {
    GFL_ASSERT( 0 );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    評価：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   評価
 */
void SDCardSaveFrame::_SaveEvaluation( void )
{
  if( m_EvaluationSeq == EVAL_SEQ_MSGWND )
  {
    // メッセージウィンドウを表示
    _SaveEvaluation_MsgWnd();
    m_EvaluationSeq = EVAL_SEQ_MSGWND_WAIT;
  }
  if( m_EvaluationSeq == EVAL_SEQ_VOICE )
  {
    // 泣き声再生
    _SaveEvaluation_Voice();
    m_EvaluationSeq = EVAL_SEQ_VOICE_WAIT;
  }
  if( m_EvaluationSeq == EVAL_SEQ_VOICE_WAIT )
  {
    // 泣き声の再生完了待ち
    if( _SaveEvaluation_VoiceWait() )
    {
      m_EvaluationSeq = EVAL_SEQ_FRIENDSHIP;
    }
  }
  if( m_EvaluationSeq == EVAL_SEQ_FRIENDSHIP )
  {
    // 仲良し度アップ
    _SaveEvaluation_Friendship();
    // カーテン演出シーケンスの初期化
    _InitCurtainSequence();
    this->SetSubSeq( SAVE_SEQ_REGION );
  }
}

/**
 * @brief   評価：メッセージウィンドウ
 */
void SDCardSaveFrame::_SaveEvaluation_MsgWnd( void )
{
  const MsgWndID c_EvalMsgWndID[] = {
    MSGWND_ID_EVALUATION_A,
    MSGWND_ID_EVALUATION_B,
    MSGWND_ID_EVALUATION_C,
    MSGWND_ID_EVALUATION_D,
    MSGWND_ID_EVALUATION_E,
  };

  u32 rand = System::GflUse::GetPublicRand( 100 ) % GFL_NELEMS( c_EvalMsgWndID );
  GFL_ASSERT( rand < GFL_NELEMS(c_EvalMsgWndID) );

  // メッセージウィンドウの表示
  _ShowMessageWindow( c_EvalMsgWndID[rand] );
}

/**
 * @brief   評価：メッセージウィンドウイベント
 */
void SDCardSaveFrame::_SaveEvaluation_MsgWndEvent( void )
{
  m_EvaluationSeq = EVAL_SEQ_VOICE;
}

/**
 * @brief   評価：ボイス
 */
void SDCardSaveFrame::_SaveEvaluation_Voice( void )
{
  ViewerWork work = _GetViewerWork();

  MonsNo monsNo = work.simple_param.monsNo;
  FormNo formNo = work.simple_param.formNo;

  Sound::PlayVoice( 0, monsNo, formNo );
}

/**
 * @brief   評価：ボイス待機
 */
bool SDCardSaveFrame::_SaveEvaluation_VoiceWait( void )
{
  return GFL_BOOL_CAST( Sound::IsVoiceFinished( 0 ) );
}

/**
 * @brief   評価：仲良し度アップ
 */
void SDCardSaveFrame::_SaveEvaluation_Friendship( void )
{
  const u8 ADD_FRIENDSHIP_POINT = 10;

  pml::pokepara::PokemonParam* pp = _GetPokemonParam();
  {
    GameSys::GameData* gameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
    Savedata::MyStatus* myStatus = gameData->GetPlayerStatus();

    // 仲良し度の更新
    PokeTool::nakayoshi::UpdateFriendship( pp, myStatus, ADD_FRIENDSHIP_POINT );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    終了確認：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   終了確認：Viewの表示
 */
void SDCardSaveFrame::_FinishConfirm_ShowView( void )
{
  // 「撮影をつづける？」を表示
  m_pPrepareCommonView->SetViewType( PrepareCommonView::VIEW_TYPE_FINISH_MAIN );
  m_pPrepareCommonView->Show();

  // 「撮影をつづける？」に設定
  m_pSelectButtonView->SetSelectButtonType( SelectButtonView::BTN_TYPE_FINISH_MAIN );
  // ボタン選択画面を表示
  m_pSelectButtonView->Show();
}

/**
 * @brief   終了確認：メイン処理
 */
void SDCardSaveFrame::_FinishConfirm_Proc( void )
{
  u32 result = m_pSelectButtonView->GetResult();
  m_pSelectButtonView->SetResult( SelectButtonView::RES_CONTINUE );

  if( result != SelectButtonView::RES_CONTINUE )
  {
    // ボタン選択Viewを非表示
    m_pSelectButtonView->Hide();

    // リザルト処理
    _FinishConfirm_Result( result );
  }
}

/**
 * @brief   終了確認：リザルト
 */
void SDCardSaveFrame::_FinishConfirm_Result( u32 result )
{
  if( result == SelectButtonView::RES_SELECT_BTN_00 )
  {
    // 共通Viewを非表示
    m_pPrepareCommonView->Hide();

    // つづける
    this->GetFrameManager()->SetResult( Frame::RES_NEXT );
    this->SetMainSeq( MAIN_SEQ_CURTAIN_OPEN );
  }
  else if( result == SelectButtonView::RES_SELECT_BTN_01 )
  {
    // 準備へ
    this->GetFrameManager()->SetResult( Frame::RES_PREV );
    this->SetMainSeq( MAIN_SEQ_CURTAIN_OPEN );
  }
  else {
    // やめる
    this->GetFrameManager()->SetResult( Frame::RES_FINISH );
    this->SetMainSeq( MAIN_SEQ_END );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    カーテン演出：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   カーテン：開く
 *
 * @retval  "true" なら開き終わった
 * @retval  "false"なら演出中
 */
bool SDCardSaveFrame::_CurtainOpen( void )
{
  if( m_CurtainSeq == CURTAIN_SEQ_START )
  {
    m_pUpperView->CurtainOpen();
    m_CurtainSeq = CURTAIN_SEQ_WAIT;
  }
  if( m_CurtainSeq == CURTAIN_SEQ_WAIT )
  {
    if( m_pUpperView->IsCurtainOpen() )
    {
      m_CurtainSeq = CURTAIN_SEQ_END;
    }
  }
  if( m_CurtainSeq == CURTAIN_SEQ_END )
  {
    return true;
  }

  return false;
}

/**
 * @brief   カーテン：閉じる
 *
 * @retval  "true" なら閉じ終わった
 * @retval  "false"なら演出中
 */
bool SDCardSaveFrame::_CurtainClose( void )
{
  if( m_CurtainSeq == CURTAIN_SEQ_START )
  {
    m_pUpperView->CurtainClose();
    m_CurtainSeq = CURTAIN_SEQ_WAIT;
  }
  if( m_CurtainSeq == CURTAIN_SEQ_WAIT )
  {
    if( m_pUpperView->IsCurtainClose() )
    {
      m_CurtainSeq = CURTAIN_SEQ_END;
    }
  }
  if( m_CurtainSeq == CURTAIN_SEQ_END )
  {
    return true;
  }

  return false;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    メッセージウィンドウ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   メッセージウィンドウ：表示
 */
void SDCardSaveFrame::_ShowMessageWindow( MsgWndID id )
{
  m_MsgWndID = id;

  switch( m_MsgWndID )
  {
    case MSGWND_ID_SAVE_START:
    {
      gfl2::str::StrBuf* strBuf = m_pMessage->GetBeginMessage( MESSAGE_TYPE );
      m_pMsgMenuView->ShowMessage_Str( strBuf, true, true, false );
    } break;

    case MSGWND_ID_SAVE_SUCCESS:
    {
      gfl2::str::StrBuf* strBuf = m_pMessage->GetFinishMessage( MESSAGE_TYPE );
      m_pMsgMenuView->ShowMessage_Str( strBuf, false, false, false );
    } break;

    case MSGWND_ID_IMGDB_ERROR_STORAGE:
    case MSGWND_ID_IMGDB_ERROR_OVER_NUM:
    case MSGWND_ID_IMGDB_ERROR_OTHER:
    {
      gfl2::str::StrBuf* strBuf = m_pMessage->GetImgDBErrorMessage( MESSAGE_TYPE, m_ImageDBResultCode );
      m_pMsgMenuView->ShowMessage_Str( strBuf, false, false, false );
    } break;

    case MSGWND_ID_EXTSAVE_ERROR_BROKEN:
    {
      gfl2::str::StrBuf* strBuf = m_pMessage->GetExtSaveErrorMessage( ErrorConverter::SDCARD_RESULT_BROKEN_DATA );
      m_pMsgMenuView->ShowMessage_Str( strBuf, false, false, false );
    } break;

    case MSGWND_ID_EXTSAVE_ERROR_STORAGE:
    {
      gfl2::str::StrBuf* strBuf = m_pMessage->GetExtSaveErrorMessage( m_ExtSaveResultCode );
      m_pMsgMenuView->ShowMessage_Str( strBuf, false, false, false );
    } break;

    case MSGWND_ID_REGION_CAUTION:
    case MSGWND_ID_REGION_FULL:
    {
      gfl2::str::StrBuf* strBuf = m_pMessage->GetRegionMessage( MESSAGE_TYPE, m_SaveRegionNum );
      m_pMsgMenuView->ShowMessage_Str( strBuf, false, false, false );
    } break;

    case MSGWND_ID_EVALUATION_A:
    case MSGWND_ID_EVALUATION_B:
    case MSGWND_ID_EVALUATION_C:
    case MSGWND_ID_EVALUATION_D:
    case MSGWND_ID_EVALUATION_E:
    {
      const u32 c_EvaluationMsgID[] = {
        fs_photo_result_01,
        fs_photo_result_02,
        fs_photo_result_03,
        fs_photo_result_04,
        fs_photo_result_05,
      };

      u32 evalID = m_MsgWndID - MSGWND_ID_EVALUATION_A;
      GFL_ASSERT( evalID < GFL_NELEMS(c_EvaluationMsgID) );

      gfl2::str::StrBuf* strBuf = m_pMsgMenuView->GetStrBuf();
      // ポケモン名登録
      m_pMsgMenuView->GetMsgManager()->SetRegisterPokeNickName( 0, _GetPokemonParam() );
      // 文字列の取得
      m_pMsgMenuView->GetMsgManager()->GetStringExpand( strBuf, c_EvaluationMsgID[evalID] );

      // メッセージウィンドウの表示
      m_pMsgMenuView->ShowMessage_Str( strBuf, false, false, false );
    } break;
  }
}

/**
 * @brief   メッセージウィンドウ：非表示
 */
void SDCardSaveFrame::_HideMessageWindow( void )
{
  m_pMsgMenuView->HideMessage();
}

/**
 * @brief   メッセージウィンドウ：イベント
 */
void SDCardSaveFrame::_MessageWindowEvent( void )
{
  switch( m_MsgWndID )
  {
    case MSGWND_ID_SAVE_SUCCESS:
    {
      _SaveFinish_MsgWndEvent( m_MsgWndID );
    } break;

    case MSGWND_ID_IMGDB_ERROR_STORAGE:
    case MSGWND_ID_IMGDB_ERROR_OVER_NUM:
    case MSGWND_ID_IMGDB_ERROR_OTHER:
    {
      _SaveImageDB_MsgWndEvent( m_MsgWndID );
    } break;

    case MSGWND_ID_EXTSAVE_ERROR_BROKEN:
    case MSGWND_ID_EXTSAVE_ERROR_STORAGE:
    {
      _SaveExtSave_MsgWndEvent( m_MsgWndID );
    } break;

    case MSGWND_ID_REGION_CAUTION:
    case MSGWND_ID_REGION_FULL:
    {
      _SaveRegion_MsgWndEvent( m_MsgWndID );
    } break;

    case MSGWND_ID_EVALUATION_A:
    case MSGWND_ID_EVALUATION_B:
    case MSGWND_ID_EVALUATION_C:
    case MSGWND_ID_EVALUATION_D:
    case MSGWND_ID_EVALUATION_E:
    {
      _SaveEvaluation_MsgWndEvent();
    } break;
  }

  m_MsgWndID = MSGWND_ID_NULL;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック関数
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   保存対象の写真があるかどうか
 *
 * @retval  "true"なら有り
 * @retval  "false"なら無し
 */
bool SDCardSaveFrame::_IsSavePhotoDataExist( void ) const
{
  return ( m_pPhotoListView->GetSavePhotoNum() > 0 );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ホーム／スリープ禁止：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ホーム／スリープ禁止解除
 */
void SDCardSaveFrame::_SetEnableHomeSleep( void )
{
  System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_IMAGEDB_ACC );
}

/**
 * @brief   ホーム／スリープ禁止
 */
void SDCardSaveFrame::_SetDisableHomeSleep( void )
{
  System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_IMAGEDB_ACC );
}

/**
 * @brief   ホーム／スリープ禁止チェック
 *
 * @retval  "true"なら許可
 * @retval  "false"なら禁止
 */
bool SDCardSaveFrame::_IsEnableHomeSleep( void )
{
  return System::HomeSleepAndPowerSystem::IsEnableSleepConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_IMAGEDB_ACC );
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
    // メッセージウィンドウイベント
    _MessageWindowEvent();
  }
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
